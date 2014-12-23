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
#include <uxmpp/mod/PrivateDataModule.hpp>
#include <uxmpp/Logger.hpp>
#include <uxmpp/Stanza.hpp>
#include <uxmpp/xml/names.hpp>


UXMPP_START_NAMESPACE2(uxmpp, mod)

using namespace std;
using namespace uxmpp;

static const string log_module {"PrivateDataModule"};

static const string namespace_iq_private {"jabber:iq:private"};
static const string tag_name_query       {"jabber:iq:private:query"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PrivateDataModule::PrivateDataModule () : XmppModule ("PrivateDataModule")
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PrivateDataModule::module_registered (uxmpp::Session& session)
{
    set_ids.clear ();
    get_ids.clear ();
    sess = &session;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PrivateDataModule::module_unregistered (uxmpp::Session& session)
{
    set_ids.clear ();
    get_ids.clear ();
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string PrivateDataModule::set (const uxmpp::XmlObject& data,
                                    priv_data_set_callback_t cb,
                                    const std::string& stanza_id)
{
    // Sanity check
    //
    if (!sess || !data) {
        uxmpp_log_warning (log_module, "Don't set private XML object, invalid session or XML object");
        return "";
    }

    // Check for a namespace in the data
    //
    if (data.get_namespace().empty()) {
        uxmpp_log_warning (log_module, "Don't set private XML object, invalid namespace");
        return "";
    }

    // Set IQ id to use
    //
    string id = stanza_id.empty() ? Stanza::make_id() : stanza_id;

    // Check for duplicate IQ iq
    //
    if (set_ids.find(id) != set_ids.end()) {
        uxmpp_log_warning (log_module, "Duplicate stanza id, don't set XML object ", data.get_full_name());
        return "";
    }

    // Send the stanza
    //
    uxmpp_log_debug (log_module, "Set private XML object ", data.get_full_name());
    set_ids[id].first  = data.get_full_name ();
    set_ids[id].second = cb!=nullptr ? cb : set_cb;
    sess->send_stanza (IqStanza(IqType::set, "", "", id).
                       add_node(XmlObject("query", namespace_iq_private).
                                add_node(data)));
    return id;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string PrivateDataModule::get (const std::string& tag_name,
                                    const std::string& tag_namespace,
                                    priv_data_get_callback_t cb,
                                    const std::string& stanza_id)
{
    // Sanity check
    //
    if (!sess) {
        uxmpp_log_warning (log_module, "Don't get private XML object, invalid session");
        return "";
    }

    // Check for a valid tag
    //
    if (tag_name.empty() || tag_namespace.empty()) {
        uxmpp_log_warning (log_module, "Don't get private XML object, invalid tag or namespace");
        return "";
    }
    XmlObject xml_obj (tag_name, tag_namespace);
    string xml_full_name = xml_obj.get_full_name ();

    // Set IQ id to use
    //
    string id = stanza_id.empty() ? Stanza::make_id() : stanza_id;

    // Check for duplicate IQ iq
    //
    if (get_ids.find(id) != get_ids.end()) {
        uxmpp_log_warning (log_module, "Duplicate stanza id, don't get XML object ", xml_full_name);
        return "";
    }

    // Send the stanza
    //
    uxmpp_log_debug (log_module, "Get private xml obj ", xml_full_name);
    get_ids[id].first  = xml_full_name;
    get_ids[id].second = cb!=nullptr ? cb : get_cb;
    sess->send_stanza (IqStanza(IqType::get, "", "", id).
                       add_node(XmlObject("query", namespace_iq_private).
                                add_node(xml_obj)));
    return id;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PrivateDataModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Check for IQ stanzas
    //
    if (xml_obj.get_full_name() != xml::full_tag_iq_stanza)
        return false;

    // Check the id
    //
    string id = xml_obj.get_attribute ("id");
    if (set_ids.find(id) != set_ids.end())
        return handle_set_result (id, reinterpret_cast<IqStanza&>(xml_obj));
    if (get_ids.find(id) != get_ids.end())
        return handle_get_result (id, reinterpret_cast<IqStanza&>(xml_obj));

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PrivateDataModule::handle_set_result (const std::string& id, uxmpp::IqStanza& iq_result)
{
    string data_tag = set_ids[id].first;
    priv_data_set_callback_t cb = set_ids[id].second;
    set_ids.erase (id);
    string error_name {""};
    int error_code {0};

    if (iq_result.get_type() == IqType::result) {
        // Ok
        uxmpp_log_debug (log_module, "Successfully set xml object ", data_tag);
    }
    else if (iq_result.get_type() == IqType::error) {
        // Error
        error_name = iq_result.get_error_name ();
        error_code = iq_result.get_error_code ();
        if (!error_code) {
            error_code = 404;
            uxmpp_log_warning (log_module, "Missing error code in IQ error, set it to ", error_code);
        }
        uxmpp_log_warning (log_module, "Error setting private data ", data_tag, " - ",
                           error_name, " (", error_code, ")");
    }
    else {
        // Undefined error
        error_name = "undefined-condition";
        error_code = 404;
        uxmpp_log_warning (log_module, "Unexpected result setting xml object ", data_tag);
    }

    // Call callback
    //
    if (cb)
        cb (*sess, data_tag, id, error_code, error_name);

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PrivateDataModule::handle_get_result (const std::string& id, uxmpp::IqStanza& iq_result)
{
    string data_tag = get_ids[id].first;
    priv_data_get_callback_t cb = get_ids[id].second;
    get_ids.erase (id);
    string error_name {""};
    int error_code {0};
    std::vector<uxmpp::XmlObject> priv_data;

    if (iq_result.get_type() == IqType::result) {
        // Ok
        uxmpp_log_debug (log_module, "Successfully got xml object ", data_tag);
        auto query_node = iq_result.find_node (tag_name_query, true);
        if (query_node)
            priv_data = std::move (query_node.get_nodes());
    }
    else if (iq_result.get_type() == IqType::error) {
        // Error
        error_name = iq_result.get_error_name ();
        error_code = iq_result.get_error_code ();
        if (!error_code) {
            error_code = 404;
            uxmpp_log_warning (log_module, "Missing error code in IQ error, set it to ", error_code);
        }
        uxmpp_log_warning (log_module, "Error setting private data ", data_tag, " - ",
                           error_name, " (", error_code, ")");
    }
    else {
        // Undefined error
        error_name = "undefined-condition";
        error_code = 404;
        uxmpp_log_warning (log_module, "Unexpected result setting xml object ", data_tag);
    }

    // Call callback
    //
    if (cb)
        cb (*sess, priv_data, id, error_code, error_name);

    return true;
}


UXMPP_END_NAMESPACE2
