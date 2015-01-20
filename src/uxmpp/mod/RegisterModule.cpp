/*
 *  Copyright (C) 2014-2015 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/mod/RegisterModule.hpp>
#include <uxmpp/Logger.hpp>
#include <uxmpp/Stanza.hpp>
#include <uxmpp/xml/names.hpp>


UXMPP_START_NAMESPACE2(uxmpp, mod)

using namespace std;
using namespace uxmpp;

static const string log_module {"RegisterModule"};

static const string namespace_iq_register {"jabber:iq:register"};
static const string full_tag_name_query   {"jabber:iq:register:query"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RegisterModule::RegisterModule ()
    :
    XmppModule ("mod_register"),
    sess {nullptr},
    query_id {""},
    reg_info_cb {nullptr},
    registration_id {""},
    unregistration_id {""},
    pass_change_id {""}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RegisterModule::module_registered (uxmpp::Session& session)
{
    sess = &session;
    query_id = "";
    registration_id = "";
    unregistration_id = "";
    pass_change_id = "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RegisterModule::module_unregistered (uxmpp::Session& session)
{
    sess = nullptr;
    query_id = "";
    registration_id = "";
    unregistration_id = "";
    pass_change_id = "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool RegisterModule::process_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // We're only interested in IQ stanzas
    //
    if (xml_obj.get_full_name() != xml::full_tag_iq_stanza)
        return false;

    IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

    // Check for info query result
    //
    if (!query_id.empty() && iq.get_id()==query_id) {
        handle_info_query_result (iq);
        return true;
    }

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

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RegisterModule::handle_info_query_result (IqStanza& iq)
{
    RegistrationInfo reg_info;

    reg_info.error = iq.get_error ();
    if (iq.have_error()) {
        uxmpp_log_info (log_module, "Error getting registration info: ", reg_info.error.get_condition());
    }else{
        auto query = iq.find_node (full_tag_name_query, true);
        for (auto& field : query.get_nodes()) {
            if (field.get_tag_name() == "instructions") {
                reg_info.instructions = field.get_content ();
            }
            else if (field.get_tag_name() == "registered") {
                reg_info.registered = true;
            }
            else {
                reg_info.fields.push_back (pair<string, string>(field.get_tag_name(), field.get_content()));
            }
        }
    }
    query_id = "";

    if (reg_info_cb)
        reg_info_cb (*sess, reg_info);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RegisterModule::set_info_callback (reg_info_cb_t callback)
{
    reg_info_cb = callback;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RegisterModule::set_result_callback (reg_result_cb_t callback)
{
    reg_result_cb = callback;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool RegisterModule::request_info (const std::string& domain)
{
    if (!sess) {
        uxmpp_log_debug (log_module, "Unable to send registration info query - no active session");
        return false;
    }

    if (!query_id.empty()) {
        uxmpp_log_warning (log_module, "Registration info query already sent");
        return false;
    }

    string to;
    if (domain.empty())
        to = sess->get_domain().get_domain ();
    else
        to = domain;
    query_id = Stanza::make_id ();
    sess->send_stanza (IqStanza(IqType::get, to, "", query_id).
                       add_node(XmlObject("query", namespace_iq_register)));

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool RegisterModule::register_user (std::list<std::pair<std::string, std::string> >& fields,
                                    const std::string& domain)
{
    if (!sess) {
        uxmpp_log_debug (log_module, "Unable to send registration - no active session");
        return false;
    }

    if (!registration_id.empty()) {
        uxmpp_log_warning (log_module, "Registration request already sent");
        return false;
    }

    string to;
    if (domain.empty())
        to = sess->get_domain().get_domain ();
    else
        to = domain;
    registration_id = Stanza::make_id ();
    XmlObject query ("query", namespace_iq_register);
    for (auto& field : fields)
        query.add_node (XmlObject(field.first).set_content(field.second));

    sess->send_stanza (IqStanza(IqType::set, to, "", registration_id).add_node(std::move(query)));
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool RegisterModule::unregister_user ()
{
    if (!sess) {
        uxmpp_log_debug (log_module, "Unable to send un-registration - no active session");
        return false;
    }

    if (!unregistration_id.empty()) {
        uxmpp_log_warning (log_module, "Un-registration request already sent");
        return false;
    }

    unregistration_id = Stanza::make_id ();
    sess->send_stanza (IqStanza(IqType::set, Jid(), sess->get_jid().bare(), unregistration_id).
                       add_node(XmlObject("query", namespace_iq_register).
                                add_node(XmlObject("remove"))));
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool RegisterModule::set_password (const std::string& new_passphrase)
{
    if (!sess) {
        uxmpp_log_debug (log_module, "Unable to send un-registration - no active session");
        return false;
    }

    if (!pass_change_id.empty()) {
        uxmpp_log_warning (log_module, "Un-registration request already sent");
        return false;
    }

    pass_change_id = Stanza::make_id ();

    XmlObject query ("query", namespace_iq_register);
    query.add_node (XmlObject("username").set_content(sess->get_jid().get_local()));
    query.add_node (XmlObject("password").set_content(new_passphrase));

    sess->send_stanza (IqStanza(IqType::set, sess->get_jid().get_domain(), "", pass_change_id).
                       add_node(std::move(query)));
    return true;
}



UXMPP_END_NAMESPACE2
