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
#include <uxmpp/StreamError.hpp>
#include <uxmpp/xml/names.hpp>
#include <uxmpp/Logger.hpp>


#define THIS_FILE "StreamError.cpp"


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;


const std::string StreamError::bad_format               {"bad-format"};
const std::string StreamError::bad_namespace_prefix     {"bad-namespace-prefix"};
const std::string StreamError::conflict                 {"conflict"};
const std::string StreamError::connection_timeout       {"connection-timeout"};
const std::string StreamError::host_gone                {"host-gone"};
const std::string StreamError::host_unknown             {"host-unknown"};
const std::string StreamError::improper_addressing      {"improper-addressing"};
const std::string StreamError::internal_server_error    {"internal-server-error"};
const std::string StreamError::invalid_from             {"invalid-from"};
const std::string StreamError::invalid_namespace        {"invalid-namespace"};
const std::string StreamError::invalid_xml              {"invalid-xml"};
const std::string StreamError::not_authorized           {"not-authorized"};
const std::string StreamError::not_well_formed          {"not-well-formed"};
const std::string StreamError::policy_violation         {"policy-violation"};
const std::string StreamError::remote_connection_failed {"remote-connection-failed"};
const std::string StreamError::reset                    {"reset"};
const std::string StreamError::resource_constraint      {"resource-constraint"};
const std::string StreamError::restricted_xml           {"restricted-xml"};
const std::string StreamError::see_other_host           {"see-other-host"};
const std::string StreamError::system_shutdown          {"system-shutdown"};
const std::string StreamError::undefined_condition      {"undefined-condition"};
const std::string StreamError::unsupported_encoding     {"unsupported-encoding"};
const std::string StreamError::unsupported_feature      {"unsupported-feature"};
const std::string StreamError::unsupported_stanza_type  {"unsupported-stanza-type"};
const std::string StreamError::unsupported_version      {"unsupported-version"};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StreamError::StreamError ()
    : XmlObject ("error", xml::alias_stream, false, false, 2)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StreamError::StreamError (XmlObject& xml_obj)
    : XmlObject ("error", xml::alias_stream, false, false, 2)
{
    nodes   = xml_obj.get_nodes ();
    content = xml_obj.get_content ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StreamError::StreamError (const std::string& error)
    : XmlObject ("error", xml::alias_stream, false, false, 2)
{
    set_error_name (error);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StreamError& StreamError::operator= (const XmlObject& xml_obj)
{
    if (&xml_obj == this)
        return *this;

    nodes = (const_cast<XmlObject&>(xml_obj)).get_nodes ();
    set_content (xml_obj.get_content());

    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool StreamError::have_error ()
{
    return get_nodes().size() != 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StreamError::set_error_name (const std::string& error)
{
    get_nodes().clear ();
    if (error.length())
        add_node (XmlObject(error, xml::namespace_xmpp_streams));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StreamError::get_error_name ()
{
    return get_nodes().empty() ? "": get_nodes().begin()->get_tag_name();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StreamError::set_error_content (const std::string& content)
{
    if (!get_nodes().empty())
        get_nodes().begin()->set_content (content);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StreamError::get_error_content ()
{
    return get_nodes().empty() ? "" : get_nodes().begin()->get_content();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StreamError::set_app_error (const std::string& app_error, const std::string& text)
{
    set_error_name ("undefined-condition");
    add_node (XmlObject(app_error, xml::namespace_uxmpp_error));
    set_text (text);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StreamError::get_app_error ()
{
    auto& nodes = get_nodes ();
    for (unsigned i=1; i<nodes.size(); ++i) {
        if (nodes[i].get_tag_name() != "text")
            return nodes[i].get_tag_name ();
    }
    return "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StreamError::get_text ()
{
    for (auto& node : get_nodes()) {
        if (node.get_tag_name() == "text")
            return node.get_content ();
    }
    return "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StreamError::set_text (const std::string& text)
{
    for (auto& node : get_nodes()) {
        if (node.get_tag_name() == "text") {
            node.set_content (text);
            return;
        }
    }
    XmlObject text_node ("text", xml::namespace_xmpp_streams);
    text_node.set_content (text);
    add_node (text_node);
}



UXMPP_END_NAMESPACE1
