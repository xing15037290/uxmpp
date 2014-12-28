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
#include <uxmpp/mod/SearchModule.hpp>
#include <uxmpp/Logger.hpp>
#include <uxmpp/xml/names.hpp>


UXMPP_START_NAMESPACE2(uxmpp, mod)

using namespace std;
using namespace uxmpp;

static const string log_module {"SearchModule"};

static const string namespace_iq_search {"jabber:iq:search"};
static const string full_tag_name_query {"jabber:iq:search:query"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SearchModule::SearchModule ()
    :
    XmppModule ("mod_search"),
    sess {nullptr},
    query_id {""}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SearchModule::module_registered (uxmpp::Session& session)
{
    query_id = "";
    sess = &session;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SearchModule::module_unregistered (uxmpp::Session& session)
{
    sess = nullptr;
    query_id = "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool SearchModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // We're only interested in IQ stanzas
    //
    if (xml_obj.get_full_name() != xml::full_tag_iq_stanza)
        return false;

    IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

    // Check for info query result
    //
    if (!query_id.empty() && iq.get_id()==query_id) {
        handle_fields_query_result (iq);
        return true;
    }
/*
    // Check for registration result
    //
    if (!registration_id.empty() && iq.get_id()==registration_id) {
        if (reg_result_cb) {
            StanzaError error = iq.get_error ();
            reg_result_cb (*sess, "register", error);
        }
        return true;
    }

    // Check for un-registration result
    //
    if (!unregistration_id.empty() && iq.get_id()==unregistration_id) {
        if (reg_result_cb) {
            StanzaError error = iq.get_error ();
            reg_result_cb (*sess, "unregister", error);
        }
        return true;
    }

    // Check for password change result
    //
    if (!pass_change_id.empty() && iq.get_id()==pass_change_id) {
        if (reg_result_cb) {
            StanzaError error = iq.get_error ();
            reg_result_cb (*sess, "password", error);
        }
        return true;
    }
*/
    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SearchModule::handle_fields_query_result (IqStanza& iq)
{
    list<string> fields;
    string instructions {""};

    if (iq.have_error()) {
        StanzaError error = iq.get_error ();
        uxmpp_log_info (log_module, "Error getting search fields info: ", error.get_condition());
    }else{
        auto query = iq.find_node (full_tag_name_query, true);
        for (auto& field : query.get_nodes()) {
            if (field.get_tag_name() == "instructions")
                instructions = field.get_content ();
            else
                fields.push_back (field.get_tag_name());
        }
    }
    query_id = "";

    if (fields_info_cb)
        fields_info_cb (*sess, instructions, fields);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool SearchModule::request_fields ()
{
    if (!sess) {
        uxmpp_log_debug (log_module, "Unable to send search fields query - no active session");
        return false;
    }

    if (!query_id.empty()) {
        uxmpp_log_warning (log_module, "Search fields query already sent");
        return false;
    }

    query_id = Stanza::make_id ();
    sess->send_stanza (IqStanza(IqType::get, sess->get_jid().get_domain(), sess->get_jid(), query_id).
                       add_node(XmlObject("query", namespace_iq_search)));

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SearchModule::set_fields_info_callback (fields_info_cb_t callback)
{
    fields_info_cb = callback;
}


UXMPP_END_NAMESPACE2
