/*
 *  Copyright (C) 2013,2014 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/Session.hpp>
#include <uxmpp/net/BsdResolver.hpp>
#include <uxmpp/StreamXmlObj.hpp>
#include <uxmpp/utils.hpp>
#include <uxmpp/IqStanza.hpp>
#include <arpa/inet.h>

#define THIS_FILE "Session"


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;
using namespace uxmpp::net;


static string XmlDiscoQueryNs = "http://jabber.org/protocol/disco#info";


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
static XmlObject* getChildNode (XmlObject& xml_obj, const string& name)
{
    for (auto& node : xml_obj.getNodes())
        if (node.getName() == name)
            return &node;
    return nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Session::Session ()
    :
    XmppModule ("core"),
    xs         (XmlObject(XmlStreamTag, XmlStreamNs, false, false)),
    sess_id    {""},
    sess_from  {""},
    state      {SessionState::closed}
{
    xs.addListener (*this);
    registerModule (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Session::~Session ()
{
    stop ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::addSessionListener (SessionListener& listener)
{
    for (auto& l : listeners) {
        if (l == &listener)
            return; // Already added
    }
    listeners.push_back (&listener);
    uxmppLogTrace (THIS_FILE, "Added session listener");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::delSessionListener (SessionListener& listener)
{
    for (auto i=listeners.begin(); i!=listeners.end(); i++) {
        if ((*i) == &listener) {
            listeners.erase (i);
            uxmppLogTrace (THIS_FILE, "Removed session listener");
            return;
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::registerModule (XmppModule& module)
{
    for (XmppModule* m : xmpp_modules) {
        if (&module == m) {
            uxmppLogInfo (THIS_FILE,
                          string("Not registering XMPP module '") +
                          module.getName() + "' - already registered");
            return;
        }
    }
    xmpp_modules.push_back (&module);
    module.moduleRegistered (*this);
    uxmppLogInfo (THIS_FILE, string("XMPP module '") + module.getName() + "' - registered");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::unregisterModule (XmppModule& module)
{
    for (auto i=xmpp_modules.begin(); i!=xmpp_modules.end(); i++) {
        if (*i == &module) {
            xmpp_modules.erase (i);
            module.moduleUnregistered (*this);
            uxmppLogInfo (THIS_FILE, string("XMPP module '") + module.getName() + "' - unregistered");
            return;
        }
    }
    uxmppLogInfo (THIS_FILE,
                  string("Not unregistering XMPP module '") +
                  module.getName() + "' - not registered");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::reset ()
{
    xs.reset ();
    features.clear ();
    stream_xml_obj.setFrom (cfg.user_id);
    stream_xml_obj.setPart (XmlObjPart::start);
    xs.write (stream_xml_obj);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::start (const SessionConfig& config)
{
    if (!changeState(SessionState::connecting)) {
        uxmppLogWarning (THIS_FILE, "Unable to connect stream in state ", to_string(state));
        return;
    }

    uxmppLogDebug (THIS_FILE, "Starting XMPP session");

    // Initialize session data
    //
    cfg       = config;
    sess_id   = "";
    sess_from = "";
    jid       = "";
    stream_error.setErrorName ("");

    stream_xml_obj.setTo   (cfg.domain);
    stream_xml_obj.setFrom (string("user@")+cfg.domain);
    stream_xml_obj.setPart (XmlObjPart::start);

    // Get the list of IP addresses to try to connect to
    //
    auto addr_list = get_server_address_list (cfg);

    // Set error 'undefined-condition' if the resolver fails.
    //
    if (addr_list.size() == 0) {
        string host = cfg.server.length() ? cfg.server : cfg.domain;
        uxmppLogWarning (THIS_FILE, "Unable to resolv host ", host);
        stream_error.setAppError ("resolve-error",
                                  string("Unable to resolve host ") + host);
    }

    // Try to connect to the addresses the resolver returned.
    //
    bool connected = false;
    for (auto& addr : addr_list) {
        if (cfg.port) // Override port number ?
            addr.port = htons (cfg.port);
        uxmppLogInfo (THIS_FILE, "Connect to ", to_string(addr));
        stream_error.setErrorName ("");
        //
        // This is a blocking call. The execution of xs.start() could take quite some time.
        //
        connected = xs.start (addr);
        if (connected) {
            // Success !
            // Don't try next server
            break;
        }
        uxmppLogInfo (THIS_FILE, "Failed connecting to ", to_string(addr));
        stream_error.setAppError ("connection-refused",
                                  string("Unable to connect to ") + to_string(addr));
    }

    // Session is done, set the state to 'closed'
    //
    changeState (SessionState::closed);
    stream_xml_obj.setTo ("");
    stream_xml_obj.setFrom ("");
    sess_id = "";
    sess_from = "";
    jid = "";

    uxmppLogDebug (THIS_FILE, "XMPP session ended");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::stop (bool fast)
{
    uxmppLogTrace (THIS_FILE, "Stop session in state ", to_string(state));

    if (!fast && (state == SessionState::closed || state==SessionState::closing)) {
        uxmppLogTrace (THIS_FILE, "Session already ",
                       (state==SessionState::closing?"closing":"closed"), ", do nothing");
        return;
    }

    if (state != SessionState::closing) {
        if (!changeState(SessionState::closing)) {
            uxmppLogWarning (THIS_FILE, "Unable to stop session in state ", to_string(state));
            return;
        }
    }

    if (xs.isOpen()) {
        stream_xml_obj.setPart (XmlObjPart::end);
        if (!fast)
            xs.setTimeout ("stop_session", 500);
        xs.write (stream_xml_obj);
        if (fast)
            xs.stop ();
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Session::changeState (SessionState new_state)
{
    SessionState old_state = state;

    // Check if the new state is accepted.
    //
    if (!valid_session_state_matrix[static_cast<int>(old_state)]
                                   [static_cast<int>(new_state)])
    {
        uxmppLogWarning (THIS_FILE, "invalid session state: ",
                         to_string(new_state), ", old state: ", to_string(old_state));
        return false;
    }

    state = new_state;

    uxmppLogTrace (THIS_FILE, "### new session state: ", to_string(state), " ###");

    switch (new_state) {
    case SessionState::closed:
        xs.setTimeout ("stop_session", 0); // Disable stop_session timer
        break;

    case SessionState::connecting:
        break;

    case SessionState::negotiating:
    {
        if (old_state == SessionState::connecting) {
            xs.write (stream_xml_obj);
        }
    }
        break;

    case SessionState::bound:
        uxmppLogDebug (THIS_FILE, "Binding is done: ", jid);
        break;

    case SessionState::closing:
        stop ();
        break;
    }

    // Inform listeners
    //
    for (auto& listener : listeners)
        listener->onStateChange (*this, new_state, old_state);

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::onOpen (XmlStream& stream)
{
    uxmppLogTrace (THIS_FILE, "XML stream is open");
    changeState (SessionState::negotiating);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::onClose (XmlStream& stream)
{
    uxmppLogTrace (THIS_FILE, "XML stream is closed");
    stream_xml_obj.setTo ("");
    stream_xml_obj.setFrom ("");
    stop ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::onRxXmlObj (XmlStream& stream, XmlObject& xml_obj)
{
    uxmppLogDebug (THIS_FILE, "Got XML obj: ", to_string(xml_obj, true));

    // Find an XMPP module to handle the XML object.
    //
    bool handled = false;
    for (XmppModule* module : xmpp_modules) {
        uxmppLogTrace (THIS_FILE, string("Call module ") + module->getName());
        if (module->proccessXmlObject(*this, xml_obj)) {
            uxmppLogDebug (THIS_FILE, string("XML object handled by module ") + module->getName());
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

    uxmppLogDebug (THIS_FILE, "XML object not handled by any module ");

    //
    // Handle unhandled XML objects
    //

    // Check for IQ stanza's
    //
    if (xml_obj.getFullName() == XmlIqStanzaTagFull) {
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);
        //
        // Respond to unknown 'set' and 'get' with an empty result.
        //
        if (iq.getType()==IqType::set || iq.getType()==IqType::get) {
            // Send result
            sendStanza (IqStanza(IqType::result, "", "", iq.getId()));
        }
    }

    // If the XML object was not handled and we have got feature 'bind'
    // then send 'bind' to the server. And yes, only if we are in 'negotiating' state.
    //
    if (getState() == SessionState::negotiating) {
        for (auto& feature : features) {
            if (feature.getName() == "bind") {
                IqStanza iq (IqType::set, "", "", "b#1");
                XmlObject bind_node (XmlBindTag, XmlBindNs, true, true, 1);
                if (cfg.resource.length() > 0) {
                    bind_node.addNode (XmlObject("resource", XmlBindNs, false).setContent(cfg.resource));
                }
                iq.addNode (bind_node);
                xs.write (iq);
                break;
            }
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Session::onRxXmlError (XmlStream& stream)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Session::proccessXmlObject (Session& session, XmlObject& xml_obj)
{
    // Check for stream errors before doing anything else.
    //
    if (xml_obj.getFullName() == XmlStreamErrorTagFull) {
        stream_error = xml_obj;
        uxmppLogError (THIS_FILE, "Got stream error: ", stream_error.getErrorName());
        stop ();
        return true;
    }

    // Check for end-of-stream before doing anything else
    //
    if (xml_obj.getFullName() == XmlStreamTagFull && xml_obj.getPart() == XmlObjPart::end) {
        if (state==SessionState::closing) {
            uxmppLogTrace (THIS_FILE, "Session is already closing, stop XML stream");
            if (xs.isOpen())
                xs.stop (); // Close the XML stream
        }else{
            uxmppLogTrace (THIS_FILE, "Close session");
            if (changeState(SessionState::closing)) {
                if (xs.isOpen()) {
                    stream_xml_obj.setPart (XmlObjPart::end);
                    xs.write (stream_xml_obj);
                    xs.stop ();
                }
            }
        }
        return true;
    }

    // Store features.
    //
    if (xml_obj.getFullName() ==  XmlFeaturesTagFull) {
        features.clear ();
        for (auto& node : xml_obj.getNodes()) {
            uxmppLogTrace (THIS_FILE, "Got feature: ", node.getName());
            features.push_back (node);
        }
    }

    // Check timer events
    //
    if (xml_obj.getFullName() == XmlUxmppTimerTagFull) {
        //
        // Check the close timer
        //
        if (xml_obj.getAttribute("id") == "close") {
            uxmppLogInfo (THIS_FILE, "Timeout, close connection");
            stream_error.setAppError ("timeout", string("Timeout"));
            stop ();
            return true;
        }
        //
        // Check the stop_session timer
        //
        else if (xml_obj.getAttribute("id") == "stop_session") {
            uxmppLogDebug (THIS_FILE, "Timeout while waiting for XML session end tag, close XML stream.");
            if (xs.isOpen())
                xs.stop ();
            return true;
        }
    }

    //
    // Handle 'stream'
    //
    if (xml_obj.getFullName() == XmlStreamTagFull) {
        sess_id = xml_obj.getAttribute ("id");
        sess_from = xml_obj.getAttribute ("from");
        uxmppLogTrace (THIS_FILE, "Got session ID: ", sess_id);
        return true;
    }

    //
    // Handle 'bind' and 'session'
    //
    if (xml_obj.getFullName() ==  XmlIqStanzaTagFull && getState()==SessionState::negotiating) {

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
        XmlObject* node_bind = getChildNode (xml_obj, "bind");
        if (node_bind)
            node_jid = getChildNode (*node_bind, "jid");
        if (node_jid)
            jid = node_jid->getContent ();
        if (jid.length()) {
            uxmppLogTrace (THIS_FILE, "Got session JID: ", jid);

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
            changeState(SessionState::bound);

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
    string server = cfg.server.length() ? cfg.server : cfg.domain;

    BsdResolver resolver;
    std::vector<IpHostAddr> addr_list;

    if (!cfg.disable_srv) {
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
            if (addr_list.size() != 0) {
                break;
            }
        }
    }
    if (addr_list.size() == 0) {
        //
        // DNS SRV failed(or not used), fallback to normal host resolution
        //
        uxmppLogDebug (THIS_FILE, std::string("DNS SRV query gave no response, "
                                              "using normal address resolution for ") + server);
        addr_list = resolver.lookup_host (server,
                                          cfg.port==0 ? 5222 : cfg.port,
                                          cfg.protocol==AddrProto::any ? AddrProto::tcp : cfg.protocol);
    }

    return addr_list;
}


UXMPP_END_NAMESPACE1
