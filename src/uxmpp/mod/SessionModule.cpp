/*
 *  Copyright (C) 2014 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/mod/SessionModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/utils.hpp>
#include <uxmpp/IqStanza.hpp>
#include <uxmpp/xml/names.hpp>


#define THIS_FILE "SessionModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


static string XmlSessionNs = "urn:ietf:params:xml:ns:xmpp-session";
static const std::string XmlSessionTag = "session";
static const std::string XmlSessionTagFull  = XmlSessionNs + string(":") + XmlSessionTag;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void send_iq_set_session (uxmpp::Session& s, string& iq_id)
{
    auto& features = s.get_features ();
    for (auto& node : features) {
        if (node.get_full_name() == XmlSessionTagFull) {
            iq_id = Stanza::make_id ();
            s.send_stanza (IqStanza(IqType::set, s.get_stream_from_attr(), "", iq_id).
                           add_node(XmlObject("session", XmlSessionNs)));
            break;
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SessionModule::SessionModule ()
    : uxmpp::XmppModule("mod_session"), iq_id{""}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SessionModule::module_registered (uxmpp::Session& session)
{
    session.add_session_listener (*this);

    if (session.get_state() == SessionState::bound && iq_id=="")
        send_iq_set_session (session, iq_id);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SessionModule::module_unregistered (uxmpp::Session& session)
{
    session.del_session_listener (*this);
    iq_id = "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool SessionModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    //
    // Handle 'session' result
    //
    if (xml_obj.get_full_name() ==  xml::full_tag_iq_stanza) {
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);
        if (iq.get_id() == iq_id) {
            if (iq.get_type() == IqType::result) {
                // Success
                uxmpp_log_debug (THIS_FILE, "Session established");
                session.unregister_module (*this);
            }
            else if (iq.get_type() == IqType::error) {
#warning How to handle session request error?
            }
            return true;
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SessionModule::on_state_change (uxmpp::Session& session,
                                     uxmpp::SessionState new_state,
                                     uxmpp::SessionState old_state)
{
    if (new_state == SessionState::bound && iq_id=="")
        send_iq_set_session (session, iq_id);
}



UXMPP_END_NAMESPACE2
