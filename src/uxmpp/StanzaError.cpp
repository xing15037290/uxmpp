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
#include <uxmpp/StanzaError.hpp>
#include <uxmpp/xml/names.hpp>


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;


//
// Error types
//
const std::string StanzaError::type_auth     {"auth"};
const std::string StanzaError::type_cancel   {"cancel"};
const std::string StanzaError::type_continue {"continue"};
const std::string StanzaError::type_modify   {"modify"};
const std::string StanzaError::type_wait     {"wait"};

//
// Error conditions
//
const std::string StanzaError::bad_request             {"bad-request"};
const std::string StanzaError::conflict                {"conflict"};
const std::string StanzaError::feature_not_implemented {"feature-not-implemented"};
const std::string StanzaError::forbidden               {"forbidden"};
const std::string StanzaError::gone                    {"gone"};
const std::string StanzaError::internal_server_error   {"internal-server-error"};
const std::string StanzaError::item_not_found          {"item-not-found"};
const std::string StanzaError::jid_malformed           {"jid-malformed"};
const std::string StanzaError::not_acceptable          {"not-acceptable"};
const std::string StanzaError::not_allowed             {"not-allowed"};
const std::string StanzaError::not_authorized          {"not-authorized"};
const std::string StanzaError::policy_violation        {"policy-violation"};
const std::string StanzaError::recipient_unavailable   {"recipient-unavailable"};
const std::string StanzaError::redirect                {"redirect"};
const std::string StanzaError::registration_required   {"registration-required"};
const std::string StanzaError::remote_server_not_found {"remote-server-not-found"};
const std::string StanzaError::remote_server_timeout   {"remote-server-timeout"};
const std::string StanzaError::resource_constraint     {"resource-constraint"};
const std::string StanzaError::service_unavailable     {"service-unavailable"};
const std::string StanzaError::subscription_required   {"subscription-required"};
const std::string StanzaError::undefined_condition     {"undefined-condition"};
const std::string StanzaError::unexpected_request      {"unexpected-request"};


static const std::string text_full_xml_name {"urn:ietf:params:xml:ns:xmpp-stanzas:text"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StanzaError::StanzaError (const std::string& type, const std::string& condition, const std::string& text)
    : XmlObject ("error", xml::namespace_jabber_client, false, true, 1)
{
    set_type (type);
    set_condition (condition);
    if (!text.empty())
        set_text (text);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StanzaError::get_type ()
{
    return get_attribute ("type");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StanzaError& StanzaError::set_type (const std::string& type)
{
    set_attribute ("type", type);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StanzaError::get_by ()
{
    return get_attribute ("by");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StanzaError& StanzaError::set_by (const std::string& by)
{
    set_attribute ("by", by);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StanzaError::get_condition ()
{
    for (auto& node : get_nodes()) {
        if (node.get_namespace() == xml::namespace_stanza_error &&
            node.get_tag_name() != "text")
        {
            return node.get_tag_name ();
        }
    }
    return "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StanzaError& StanzaError::set_condition (const std::string& condition)
{
    // Add/replace the error node
    //
    for (auto& node : get_nodes()) {
        if (node.get_namespace() == xml::namespace_stanza_error &&
            node.get_tag_name() != "text")
        {
            node.set_tag_name (condition);
            return *this;
        }
    }
    add_node (XmlObject(condition, xml::namespace_stanza_error));
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StanzaError::get_text ()
{
    return find_node(text_full_xml_name, true).get_content ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StanzaError& StanzaError::set_text (const std::string& text)
{
    // Add/replace the text node
    //
    for (auto& node : get_nodes()) {
        if (node.get_full_name() == text_full_xml_name) {
            node.set_content (text);
            return *this;
        }
    }
    add_node (XmlObject("text", xml::namespace_stanza_error).set_content(text));
    return *this;
}



UXMPP_END_NAMESPACE1
