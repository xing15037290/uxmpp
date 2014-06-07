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
#include <uxmpp/mod/PingModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/IqStanza.hpp>
#include <chrono>

#define THIS_FILE "PingModule"


START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;

static const string XmlPingTag     {"ping"};
static const string XmlPingNs      {"urn:xmpp:ping"};
static const string XmlPingTagFull {"urn:xmpp:ping:ping"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PingModule::PingModule ()
    : uxmpp::XmppModule ("mod_ping"),
      sess (nullptr)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PingModule::moduleRegistered (uxmpp::Session& session)
{
    sess = &session;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PingModule::moduleUnregistered (uxmpp::Session& session)
{
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PingModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Handle iq stanzas
    //
    if (xml_obj.getFullName() ==  XmlIqStanzaTagFull) {
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

        // Check for incoming ping
        //
        if (iq.getType() == IqType::get) {
            XmlObject ping = xml_obj.getNode (XmlPingTagFull, true);
            if (ping) {
                uxmppLogDebug (THIS_FILE, "Got ping from ", to_string(iq.getFrom()));
                sess->sendStanza (IqStanza(IqType::result, iq.getFrom(), sess->getJid(), iq.getId()));
                return true;
            }
        }else{
            //
            // Check for ping result
            //
            auto i = ping_map.find (iq.getId());
            if (i != ping_map.end()) {
                auto now = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().
                                                                        time_since_epoch()).count();
                if (iq.getType() == IqType::result) {
                    uxmppLogDebug (THIS_FILE, "Got ping result from ",
                                   to_string(i->second.first),
                                   ", rtt: ", (now - i->second.second), "ms");
                }
                else if (iq.getType() == IqType::error) {
                    uxmppLogInfo (THIS_FILE, "Got ping error from ",
                                  to_string(i->second.first), ": ",
                                  iq.getErrorName(), " (", iq.getErrorCode(), ")");
                }

                ping_map.erase (i);
                return true;
            }
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void do_ping (const uxmpp::Jid& target,
                     uxmpp::Session* sess,
                     std::map<std::string, std::pair<uxmpp::Jid, unsigned long> >& ping_map)
{
    string ping_id = Stanza::makeId ();

    uxmppLogDebug (THIS_FILE, "Send ping to ", to_string(target));

    auto now = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().
                                                            time_since_epoch()).count();

    ping_map[ping_id] = std::pair<uxmpp::Jid, unsigned long> (target, now);

    sess->sendStanza (IqStanza(IqType::get, target, sess->getJid(), ping_id).
                      addNode(XmlObject(XmlPingTag, XmlPingNs)));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PingModule::ping ()
{
    // Sanity check
    //
    if (!sess || sess->getState() != SessionState::bound)
        return;
    do_ping (sess->getDomain(), sess, ping_map);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PingModule::ping (const uxmpp::Jid& jid)
{
    // Sanity check
    //
    if (!sess || sess->getState() != SessionState::bound)
        return;
    do_ping (jid, sess, ping_map);
}



END_NAMESPACE2
