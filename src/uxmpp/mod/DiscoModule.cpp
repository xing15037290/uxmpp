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
#include <uxmpp/mod/DiscoModule.hpp>
#include <uxmpp/Stanza.hpp>

#define THIS_FILE "DiscoModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;

static const string XmlDiscoQueryTag         {"query"};

static const string XmlDiscoInfoNS           {"http://jabber.org/protocol/disco#info"};
static const string XmlDiscoInfoQueryTagFull {"http://jabber.org/protocol/disco#info:query"};

static const string XmlDiscoItemsNS           {"http://jabber.org/protocol/disco#items"};
static const string XmlDiscoItemsQueryTagFull {"http://jabber.org/protocol/disco#items:query"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void query_server_features (uxmpp::Session& session, string& query_id)
{
    for (auto& node : session.getFeatures()) {
        if (node.getFullName() != "http://jabber.org/protocol/caps:c" || query_id != "")
            continue;

        string node_attr = node.getAttribute ("node");
        string ver_attr  = node.getAttribute ("ver");

        query_id = Stanza::makeId ();

        if (!node_attr.length() || !ver_attr.length()) {
            session.sendStanza (IqStanza(IqType::get,
                                         session.getStreamFromAttr(),
                                         session.getJid(),
                                         query_id).
                                addNode(XmlObject("query", XmlDiscoInfoNS)));
        }else{
            session.sendStanza (IqStanza(IqType::get,
                                         session.getStreamFromAttr(),
                                         session.getJid(),
                                         query_id).
                                addNode(XmlObject("query", XmlDiscoInfoNS).
                                        setAttribute("node", node_attr + string("#") + ver_attr)));
        };
        break;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoModule::DiscoModule ()
    : uxmpp::XmppModule ("mod_disco"),
      sess {nullptr},
      feature_request_id {""},
      feature_version {""}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoModule::moduleRegistered (uxmpp::Session& session)
{
    sess = &session;
    sess->addSessionListener (*this);

    if (sess->getState() == SessionState::bound && !features.size()) {
        features.clear ();
        query_server_features (session, feature_request_id);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoModule::moduleUnregistered (uxmpp::Session& session)
{
    sess->delSessionListener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoModule::handle_feature_request_result (IqStanza& iq)
{
    XmlObject query = iq.getNode (XmlDiscoInfoQueryTagFull, true);
    if (!query)
        return;

    server_info_query_result = query;

    for (auto xml_obj : query.getNodes()) {
        if (xml_obj.getTagName() != "feature")
            continue;

        string feature = xml_obj.getAttribute ("var");
        if (feature.length()) {
            features.push_back (feature);
            uxmppLogTrace (THIS_FILE, "Added feature ", feature);
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool DiscoModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Handle iq stanzas
    //
    if (xml_obj.getFullName() ==  XmlIqStanzaTagFull) {
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

        // Chec for feature query result
        //
        if (iq.getId() == feature_request_id) {
            if (iq.getType() == IqType::result) {
                handle_feature_request_result (iq);
                return true;
            }
            else if (iq.getType() == IqType::error) {
                uxmppLogInfo (THIS_FILE, "Got disco query error: ",
                              iq.getErrorName(), " (", iq.getErrorCode(), ")");
            }
            feature_request_id = "";
            return true;
        }

        // Check for incoming info request
        //
        if (iq.getType() == IqType::get) {
        }else{
            //
            // Check for result
            //
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoModule::onStateChange (uxmpp::Session& session,
                                 uxmpp::SessionState new_state,
                                 uxmpp::SessionState old_state)
{
    if (new_state==SessionState::bound && old_state!=SessionState::bound) {
        features.clear ();
        query_server_features (session, feature_request_id);
    }
}



UXMPP_END_NAMESPACE2
