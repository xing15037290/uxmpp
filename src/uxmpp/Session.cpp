/*
 *  Copyright (C) 2013-2015 Ultramarin Design AB <dan@ultramarin.se>
 *
 *  This file is part of uxmpp.
 *
 *  uxmpp is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <uxmpp/Logger.hpp>
#include <uxmpp/Semaphore.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/io/BsdResolver.hpp>
#include <uxmpp/StreamXmlObj.hpp>
#include <uxmpp/utils.hpp>
#include <uxmpp/IqStanza.hpp>
#include <uxmpp/xml/names.hpp>
#include <arpa/inet.h>

UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;
using namespace uxmpp::io;


static const string log_unit  {"Session"};
static string XmlDiscoQueryNs {"http://jabber.org/protocol/disco#info"};


static std::vector<IpHostAddr> get_server_address_list (const SessionConfig& cfg);


static const bool valid_session_state_matrix [5/*old state*/][5/*new state*/] = {
    // closed, connecting, negotiating, bound,  closing  new /old
    //-------------------------------------------------------------------------------------/
    {  true,     true,       false,     false,   false, },//  closed
    {  true,     false,      true,      false,   true,  },//  connecting
    {  true,     false,      true,      true,    true,  },//  negotiating
    {  true,     false,      false,     false,   true,  },//  bound
    {  true,     false,      false,     false,   false, },//  closing
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static XmlObject* get_child_node (XmlObject& xml_obj, const string& name)
{
    for (auto& node : xml_obj.get_nodes())
        if (node.get_tag_name() == name)
            return &node;
    return nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Session::Session ()
    :
    XmppModule ("core"),
    xs         (XmlObject(xml::tag_stream, xml::namespace_stream, false, false)),
    sess_id    {""},
    sess_from  {""},
    state      {SessionState::closed}
{
    register_module (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Session::~Session ()
{
    stop ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::run (const SessionConfig& config)
{
    uxmpp_log_debug (log_unit, "Starting XMPP session");

    if (!change_state(SessionState::connecting)) {
        uxmpp_log_warning (log_unit, "Unable to connect stream in state ", to_string(state));
        return;
    }

    // Initialize session data
    //
    cfg       = config;
    sess_id   = "";
    sess_from = "";
    jid       = "";
    stream_error.set_error_name ("");

    // Initialize the first top-level XML object to send
    //
    stream_xml_obj.set_to    (cfg.domain);
    stream_xml_obj.set_from (string("user@")+cfg.domain);
    stream_xml_obj.set_part (XmlObjPart::start);

    // Get the list of IP addresses to try to connect to
    //
    auto addr_list = get_server_address_list (cfg);

    // Set error 'undefined-condition' if the resolver fails.
    //
    if (addr_list.empty()) {
        string host = cfg.server.empty() ? cfg.domain : cfg.server;
        uxmpp_log_warning (log_unit, "Unable to resolv host ", host);
        stream_error.set_app_error ("resolve-error",
                                    string("Unable to resolve host ") + host);
    }

    // Set the XML callback
    //
    xs.set_rx_cb ([this](XmlStream& stream, XmlObject& xml_obj){
            on_rx_xml_obj (stream, xml_obj);
        });

    // Set the connection callback
    //
    bool connected;
    Semaphore sem;
    socket.set_connected_cb ([this, &connected, &sem](SocketConnection& connection, int errnum){
            if (errnum == 0) {
                uxmpp_log_info (log_unit, "XML stream is connected to ",
                                to_string(connection.get_peer_addr()));
                connected = true;
            }else{
                uxmpp_log_info (log_unit, "XML stream failed to connect to ",
                                to_string(connection.get_peer_addr()));
                connected = false;
            }
            sem.post ();
        });

    // Try to connect to the addresses the resolver returned.
    //
    for (auto& addr : addr_list) {
        if (cfg.port) // Override port number ?
            addr.port = htons (cfg.port);

        connected = false;
        stream_error.set_error_name ("");
        socket.connect (addr); // This is a non-blocking call
        if (!sem.wait(chrono::seconds(5)) || !connected) {
            //
            // Timeout or connection failed
            //
            socket.close ();
            sem.try_wait (); // just to make sure
            stream_error.set_app_error ("connect-failed", "Unable to start/connect XML stream");
            uxmpp_log_trace (log_unit, "Connect time out, try next address");
            continue;
        }else{
            stream_error.set_error_name ("");
        }

        // This is a blocking call. The execution of xs.start() could take quite some time.
        //
        connected = xs.run (socket, socket, stream_xml_obj);
        if (connected) {
            // Success !
            // Don't try next server
            break;
        }
        stream_error.set_app_error ("connect-failed", "Unable to start/connect XML stream");
    }

    // Session is done, set the state to 'closed'
    //
    change_state (SessionState::closed);
    socket.set_connected_cb (nullptr);
    socket.close ();
    xs.set_rx_cb (nullptr);
    stream_xml_obj.set_to ("");
    stream_xml_obj.set_from ("");
    sess_id = "";
    sess_from = "";
    jid = "";

    uxmpp_log_info (log_unit, "XMPP session ended");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::stop (bool fast)
{
    uxmpp_log_trace (log_unit, "Stop session in state ", to_string(state));

    if (!fast && (state == SessionState::closed || state==SessionState::closing)) {
        uxmpp_log_trace (log_unit, "Session already ",
                         (state==SessionState::closing?"closing":"closed"), ", do nothing");
        return;
    }

    if (state != SessionState::closing) {
        if (!change_state(SessionState::closing)) {
            uxmpp_log_warning (log_unit, "Unable to stop session in state ", to_string(state));
            return;
        }
    }

    if (xs.is_running()) {
        stream_xml_obj.set_part (XmlObjPart::end);
        if (!fast)
            xs.set_timeout ("stop_session", 500);
        xs.write (stream_xml_obj);
        if (fast)
            xs.stop ();
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Session::change_state (SessionState new_state)
{
    SessionState old_state = state;

    // Check if the new state is accepted.
    //
    if (!valid_session_state_matrix[static_cast<int>(old_state)]
                                   [static_cast<int>(new_state)])
    {
        uxmpp_log_warning (log_unit, "invalid session state: ",
                           to_string(new_state), ", old state: ", to_string(old_state));
        return false;
    }

    state = new_state;

    uxmpp_log_trace (log_unit, "### new session state: ", to_string(state), " ###");

    switch (new_state) {
    case SessionState::closed:
        xs.cancel_timeout ("stop_session"); // Disable stop_session timer
        break;

    case SessionState::connecting:
        break;

    case SessionState::negotiating:
        break;

    case SessionState::bound:
        uxmpp_log_debug (log_unit, "Binding is done: ", jid);
        break;

    case SessionState::closing:
        stop ();
        break;
    }

    // Inform listeners
    //
    uxmpp_log_trace (log_unit, "Call session state listeners - on_state_change");
    for (auto& listener : listeners)
        listener->on_state_change (*this, new_state, old_state);

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::on_rx_xml_obj (XmlStream& stream, XmlObject& xml_obj)
{
    uxmpp_log_debug (log_unit, "Got XML obj: ", to_string(xml_obj, true));

    // Find an XMPP module to handle the XML object.
    //
    bool handled = false;
    for (XmppModule* module : xmpp_modules) {
        uxmpp_log_trace (log_unit, string("Call module ") + module->get_name());
        if (module->process_xml_object(*this, xml_obj)) {
            uxmpp_log_debug (log_unit, string("XML object handled by module ") + module->get_name());
            handled = true;
            break;
        }
    }

    if (handled) {
        //
        // The XML object was handled by a registered XML module.
        //
        return;
    }

    uxmpp_log_debug (log_unit, "XML object not handled by any module ");

    //
    // Handle unhandled XML objects
    //

    // Check for IQ stanza's
    //
    if (xml_obj.get_full_name() == xml::full_tag_iq_stanza) {
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);
        //
        // Respond to unknown 'set' and 'get' with a 'service-unavailable' error.
        //
        if (iq.get_type()==IqType::set || iq.get_type()==IqType::get) {
            // Send result
            send_stanza (IqStanza(IqType::error, iq.get_from(), iq.get_to(), iq.get_id()).
                         add_node(StanzaError(StanzaError::type_cancel, StanzaError::service_unavailable)));
        }
    }

    // If the XML object was not handled and we have got feature 'bind'
    // then send 'bind' to the server. And yes, only if we are in 'negotiating' state.
    //
    if (get_state() == SessionState::negotiating) {
        for (auto& feature : features) {
            if (feature.get_tag_name() == xml::tag_bind) {
                IqStanza iq (IqType::set, "", "", "b#1");
                XmlObject bind_node (xml::tag_bind, xml::namespace_bind, true, true);
                if (!cfg.resource.empty()) {
                    bind_node.add_node (XmlObject("resource", xml::namespace_bind, false).set_content(cfg.resource));
                }
                iq.add_node (bind_node);
                xs.write (iq);
                break;
            }
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StreamError& Session::get_error ()
{
    return stream_error;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Session::have_error ()
{
    return stream_error.have_error ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::add_session_listener (SessionListener& listener)
{
    for (auto& l : listeners) {
        if (l == &listener)
            return; // Already added
    }
    listeners.push_back (&listener);
    uxmpp_log_trace (log_unit, "Added session listener");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::del_session_listener (SessionListener& listener)
{
    for (auto i=listeners.begin(); i!=listeners.end(); ++i) {
        if ((*i) == &listener) {
            listeners.erase (i);
            uxmpp_log_trace (log_unit, "Removed session listener");
            return;
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SessionState Session::get_state () const
{
    return state;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Jid Session::get_jid () const
{
    return Jid (jid);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Jid Session::get_domain () const
{
    return Jid (cfg.domain);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::send_stanza (const XmlObject& xml_obj)
{
    xs.write (xml_obj);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string Session::get_id () const
{
    return sess_id;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string Session::get_stream_from_attr () const
{
    return sess_from;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::register_module (XmppModule& module)
{
    for (XmppModule* m : xmpp_modules) {
        if (&module == m) {
            uxmpp_log_info (log_unit,
                            string("Not registering XMPP module '") +
                            module.get_name() + "' - already registered");
            return;
        }
    }
    xmpp_modules.push_back (&module);
    module.module_registered (*this);
    uxmpp_log_debug (log_unit, string("XMPP module '") + module.get_name() + "' - registered");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::unregister_module (XmppModule& module)
{
    for (auto i=xmpp_modules.begin(); i!=xmpp_modules.end(); ++i) {
        if (*i == &module) {
            xmpp_modules.erase (i);
            module.module_unregistered (*this);
            uxmpp_log_debug (log_unit, string("XMPP module '") + module.get_name() + "' - unregistered");
            return;
        }
    }
    uxmpp_log_info (log_unit,
                    string("Not unregistering XMPP module '") +
                    module.get_name() + "' - not registered");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::list<XmppModule*>& Session::get_modules ()
{
    return xmpp_modules;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlStream& Session::get_xml_stream ()
{
    return xs;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::list<XmlObject>& Session::get_features ()
{
    return features;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::reset ()
{
    xs.reset ();
    features.clear ();
    stream_xml_obj.set_from (cfg.user_id);
    stream_xml_obj.set_part (XmlObjPart::start);
    xs.write (stream_xml_obj);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::set_app_error (const std::string& app_error, const std::string& text)
{
    stream_error.set_app_error (app_error, text);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Session::process_xml_object (Session& session, XmlObject& xml_obj)
{
    // First, check for internal error codes (or end of stream)
    //
    if (xml_obj.get_namespace() == xml::namespace_uxmpp_error) {
        //
        // Check XML parse error
        //
        if (xml_obj.get_tag_name() == "parse-error") {
            uxmpp_log_error (log_unit, "XML parse error: ", xml_obj.get_content());
            stream_error.set_app_error ("parse-error", "Error parsing XML stream");
        }
        //
        // Check stream RX error
        //
        else if (xml_obj.get_tag_name() == "rx-error") {
            uxmpp_log_error (log_unit, "RX faliure: ", xml_obj.get_attribute("errnum"));
            stream_error.set_app_error ("rx-error", "Error readin XML stream");
        }
        //
        // Check stream TX error
        //
        else if (xml_obj.get_tag_name() == "tx-error") {
            uxmpp_log_error (log_unit, "TX faliure: ", xml_obj.get_attribute("errnum"));
            stream_error.set_app_error ("tx-error", "Error writing XML stream");
        }
        //
        // Fallback for future defined errors
        //
        else {
            uxmpp_log_error (log_unit, "Unknown error: ", xml_obj.get_tag_name());
            stream_error.set_app_error (xml_obj.get_tag_name());
        }
        //
        // Stop the stream
        //
        stop ();
        return true;
    }

    // Check for XMPP stream errors before doing anything else.
    //
    if (xml_obj.get_full_name() == xml::full_tag_error) {
        stream_error = xml_obj;
        uxmpp_log_error (log_unit, "Got stream error: ", stream_error.get_error_name());
        stop ();
        return true;
    }

    // Check for end-of-stream before doing anything else
    //
    if (xml_obj.get_full_name() == xml::full_tag_stream && xml_obj.get_part() == XmlObjPart::end) {
        if (state==SessionState::closing) {
            uxmpp_log_trace (log_unit, "Session is already closing, stop XML stream");
            if (xs.is_running())
                xs.stop (); // Close the XML stream
        }else{
            uxmpp_log_trace (log_unit, "Close session");
            if (change_state(SessionState::closing)) {
                if (xs.is_running()) {
                    stream_xml_obj.set_part (XmlObjPart::end);
                    xs.write (stream_xml_obj);
                    xs.stop ();
                }
            }
        }
        return true;
    }

    // Store features.
    //
    if (xml_obj.get_full_name() == xml::full_tag_features) {
        features.clear ();
        for (auto& node : xml_obj.get_nodes()) {
            uxmpp_log_trace (log_unit, "Got feature: ", node.get_tag_name());
            features.push_back (node);
        }
        // Inform listeners about the feature update
        //
        uxmpp_log_trace (log_unit, "Call session state listeners - on_features");
        for (auto& listener : listeners)
            listener->on_features (*this, features);
    }

    // Check timer events
    //
    if (xml_obj.get_full_name() == xml::full_tag_uxmpp_timeout) {
        //
        // Check the close timer
        //
        if (xml_obj.get_attribute("id") == "close") {
            uxmpp_log_info (log_unit, "Timeout, close connection");
            stream_error.set_app_error ("timeout", string("Timeout"));
            stop ();
            return true;
        }
        //
        // Check the stop_session timer
        //
        else if (xml_obj.get_attribute("id") == "stop_session") {
            uxmpp_log_debug (log_unit, "Timeout while waiting for XML session end tag, close XML stream.");
            if (xs.is_running())
                xs.stop ();
            return true;
        }
    }

    //
    // Handle 'stream'
    //
    if (xml_obj.get_full_name() == xml::full_tag_stream) {
        sess_id = xml_obj.get_attribute ("id");
        sess_from = xml_obj.get_attribute ("from");
        uxmpp_log_trace (log_unit, "Got session ID: ", sess_id);
        change_state (SessionState::negotiating);
        return true;
    }

    //
    // Handle 'bind' and 'session'
    //
    if (xml_obj.get_full_name() == xml::full_tag_iq_stanza &&
        get_state() == SessionState::negotiating)
    {

/*
        // Check for 'session' result
        //
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);
        if (iq.getId() == "srq1") {
            if (iq.getType() == IqType::result) {
                // We are bound to a resource and a session is created
                changeState(SessionState::bound);
                return true;
            }
            if (iq.getType() == IqType::error) {
#warning How to handle session request error?
                stop ();
                return true;
            }
        }
*/
        XmlObject* node_jid  = nullptr;
        XmlObject* node_bind = get_child_node (xml_obj, "bind");
        if (node_bind)
            node_jid = get_child_node (*node_bind, "jid");
        if (node_jid)
            jid = node_jid->get_content ();
        if (!jid.empty()) {
            uxmpp_log_trace (log_unit, "Got session JID: ", jid);

/*
            //
            // Handle 'session' feature (obsolete RFC 391, section 3)
            //
            bool have_session_feature = false;
            for (auto& node : features) {
                if (node.getFullName() == "urn:ietf:params:xml:ns:xmpp-session:session") {
                    have_session_feature = true;
                    xs.write (IqStanza(IqType::set, sess_from, "", "srq1").
                              addNode(XmlObject("session", XmlSessionNs)));
                    break;
                }
            }

            if (!have_session_feature) {
                // We are bound to a resource.
                changeState(SessionState::bound);
            }
*/
            change_state (SessionState::bound);

            return true;
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static std::vector<IpHostAddr> get_server_address_list (const SessionConfig& cfg)
{
    // Use the configured domain if server is not specified
    //
    string server = cfg.server.empty() ? cfg.domain : cfg.server;

    BsdResolver resolver;
    std::vector<IpHostAddr> addr_list;

    if (!cfg.disable_srv) {
        uxmpp_log_trace (log_unit, "Looking up host ", server, " using DNS SRV lookup");
        // Select protocol(s) to test in the SRV query
        //
        vector<AddrProto> protocols_to_test;
        if (cfg.protocol==AddrProto::any) { // Try all available protocols in the DNS SRV query.
            protocols_to_test.push_back (AddrProto::tcp);
            protocols_to_test.push_back (AddrProto::udp);
            protocols_to_test.push_back (AddrProto::tls);
            protocols_to_test.push_back (AddrProto::dtls);
        }else{
            protocols_to_test.push_back (cfg.protocol);
        }

        // Perform a DNS SRV query for all protocols
        //
        for (AddrProto protocol : protocols_to_test) {
            // Try a DNS SRV query for the specified protocol
            //
            addr_list = resolver.lookup_srv (server, protocol, "xmpp-client", false);
            if (!addr_list.empty()) {
                break;
            }
        }
    }
    if (addr_list.empty()) {
        //
        // DNS SRV failed(or not used), fallback to normal host resolution
        //
        if (cfg.disable_srv) {
            uxmpp_log_trace (log_unit, "Looking up host ", server, " using normal address resolution");
        }else{
            uxmpp_log_debug (log_unit, std::string("DNS SRV query gave no response, "
                                                   "using normal address resolution for ") + server);
        }
        addr_list = resolver.lookup_host (server,
                                          cfg.port==0 ? 5222 : cfg.port,
                                          cfg.protocol==AddrProto::any ? AddrProto::tcp : cfg.protocol);
    }

    return addr_list;
}


UXMPP_END_NAMESPACE1
