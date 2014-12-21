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
#include <uxmpp/xml/names.hpp>

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
    for (auto& node : session.get_features()) {
        if (node.get_full_name() != "http://jabber.org/protocol/caps:c" || query_id != "")
            continue;

        string node_attr = node.get_attribute ("node");
        string ver_attr  = node.get_attribute ("ver");

        query_id = Stanza::make_id ();

        if (node_attr.empty() || ver_attr.empty()) {
            session.send_stanza (IqStanza(IqType::get,
                                          session.get_stream_from_attr(),
                                          session.get_jid(),
                                          query_id).
                                 add_node(XmlObject("query", XmlDiscoInfoNS)));
        }else{
            session.send_stanza (IqStanza(IqType::get,
                                          session.get_stream_from_attr(),
                                          session.get_jid(),
                                          query_id).
                                 add_node(XmlObject("query", XmlDiscoInfoNS).
                                          set_attribute("node", node_attr + string("#") + ver_attr)));
        };
        break;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoModule::DiscoModule ()
    : uxmpp::XmppModule ("mod_disco"),
      sess {nullptr},
      server_feature_request_id {""},
      server_feature_version {""},
      info_handler {nullptr},
      items_handler {nullptr}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoModule::module_registered (uxmpp::Session& session)
{
    sess = &session;
    sess->add_session_listener (*this);

    if (sess->get_state() == SessionState::bound && server_features.empty()) {
        server_features.clear ();
        query_server_features (session, server_feature_request_id);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoModule::module_unregistered (uxmpp::Session& session)
{
    sess->del_session_listener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoModule::handle_feature_request_result (IqStanza& iq)
{
    XmlObject query = iq.find_node (XmlDiscoInfoQueryTagFull, true);
    if (!query)
        return;

    server_info_query_result = query;

    for (auto xml_obj : query.get_nodes()) {
        if (xml_obj.get_tag_name() == "identity") {
            server_identities.push_back (DiscoIdentity(xml_obj));
        }
        else if (xml_obj.get_tag_name() == "feature") {
            string feature = xml_obj.get_attribute ("var");
            if (!feature.empty()) {
                server_features.push_back (feature);
                uxmpp_log_trace (THIS_FILE, "Added feature ", feature);
            }
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool DiscoModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Only handle iq stanzas
    //
    if (xml_obj.get_full_name() != xml::full_tag_iq_stanza)
        return false;

    IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

    // Chec for server feature query result
    //
    if (iq.get_id() == server_feature_request_id) {
        if (iq.get_type() == IqType::result) {
            handle_feature_request_result (iq);
            return true;
        }
        else if (iq.get_type() == IqType::error) {
            uxmpp_log_info (THIS_FILE, "Got disco query error: ",
                            iq.get_error_name(), " (", iq.get_error_code(), ")");
        }
        server_feature_request_id = "";
        return true;
    }

    // Handle query results
    //
    auto id_iter = query_ids.find (iq.get_id());
    if (id_iter != query_ids.end()) {
        // handle query result

        query_ids.erase (id_iter);
        return true;
    }

    // Check for incoming info request
    //
    if (iq.get_type() == IqType::get) {
    }else{
        //
        // Check for result
        //
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoModule::on_state_change (uxmpp::Session& session,
                                   uxmpp::SessionState new_state,
                                   uxmpp::SessionState old_state)
{
    if (new_state==SessionState::bound && old_state!=SessionState::bound) {
        server_features.clear ();
        query_server_features (session, server_feature_request_id);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<DiscoIdentity>& DiscoModule::get_server_identities ()
{
    return server_identities;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<std::string>& DiscoModule::get_server_features ()
{
    return server_features;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string DiscoModule::query_info (const uxmpp::Jid& jid, const std::string& query_id)
{
    // Sanity check
    //
    if (!sess)
        return "";

    // Save the query id
    //
    string qid = query_id=="" ? Stanza::make_id() : query_id;
    query_ids.insert (qid);

    // Send the query
    //
    sess->send_stanza (IqStanza(IqType::get, jid, sess->get_jid(), qid).
                       add_node(XmlObject(XmlDiscoQueryTag, "http://jabber.org/protocol/disco#info")));
//                      addNode(XmlObject(XmlDiscoQueryTag, XmlDiscoInfoNS)));

    return qid;
}



UXMPP_END_NAMESPACE2
