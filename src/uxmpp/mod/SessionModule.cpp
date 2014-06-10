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


#define THIS_FILE "SessionModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


static string XmlSessionNs = "urn:ietf:params:xml:ns:xmpp-session";
static const std::string XmlSessionTag = "session";
static const std::string XmlSessionTagFull  = XmlSessionNs + string(":") + XmlSessionTag;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void sendIqSetSession (uxmpp::Session& s, string& iq_id)
{
    auto& features = s.getFeatures ();
    for (auto& node : features) {
        if (node.getFullName() == XmlSessionTagFull) {
            iq_id = Stanza::makeId ();
            s.sendStanza (IqStanza(IqType::set, s.getStreamFromAttr(), "", iq_id).
                          addNode(XmlObject("session", XmlSessionNs)));
            break;
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SessionModule::moduleRegistered (uxmpp::Session& session)
{
    session.addSessionListener (*this);

    if (session.getState() == SessionState::bound && iq_id=="")
        sendIqSetSession (session, iq_id);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SessionModule::moduleUnregistered (uxmpp::Session& session)
{
    session.delSessionListener (*this);
    iq_id = "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool SessionModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    //
    // Handle 'session' result
    //
    if (xml_obj.getFullName() ==  XmlIqStanzaTagFull) {
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);
        if (iq.getId() == iq_id) {
            if (iq.getType() == IqType::result) {
                // Success
                uxmppLogDebug (THIS_FILE, "Session established");
                session.unregisterModule (*this);
            }
            else if (iq.getType() == IqType::error) {
#warning How to handle session request error?
            }
            return true;
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SessionModule::onStateChange (uxmpp::Session& session,
                                     uxmpp::SessionState new_state,
                                     uxmpp::SessionState old_state)
{
    if (new_state == SessionState::bound && iq_id=="")
        sendIqSetSession (session, iq_id);
}



UXMPP_END_NAMESPACE2
