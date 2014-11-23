/*
 *  Copyright (C) 2013,2014 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/XmlObject.hpp>
#include <sstream>


#define THIS_FILE "XmlObject"


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static string xml_escape (const string& str)
{
    stringstream ss;

    for (auto c : str) {
        switch (c) {
        case '&':
            ss << "&amp;";
            break;

        case '<':
            ss << "&lt;";
            break;

        case '>':
            ss << "&gt;";
            break;

        case '\'':
            ss << "&apos;";
            break;

        case '\"':
            ss << "&quot;";
            break;

        default:
            ss << c;
        }
    }
    return ss.str ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const XmlObjPart& part)
{
    switch (part) {
    case XmlObjPart::start :
        return "start";

    case XmlObjPart::body :
        return "body";

    case XmlObjPart::end :
        return "end";

    case XmlObjPart::all :
        return "all";
    }
    return "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const XmlObject& xml_obj, bool pretty, const std::string& indentation)
{
    stringstream ss;

    string tag_name;
    string ns         = xml_escape (xml_obj.get_namespace());
    string default_ns = xml_escape (xml_obj.get_default_namespace_attr());

    // Don't print anything if the xml object is invalid (doesn't hava a tag name).
    //
    if (!xml_obj)
        return "";

    // Start on a new line and indent
    //
    if (pretty)
        ss << endl << indentation;

    // Check if we should only print the body (content).
    //
    if (xml_obj.get_part() == XmlObjPart::body) {
        return xml_escape (xml_obj.content);
    }

    // Get the name of the tag to print.
    //
    if (ns.length() && !xml_obj.is_namespace_default()) {
        tag_name = xml_escape (ns + string(":") + xml_escape(xml_obj.get_tag_name()));
    }
    else {
        tag_name = xml_escape (xml_obj.get_tag_name());
    }

    // Check if we should only print the end tag.
    //
    if (xml_obj.get_part() == XmlObjPart::end) {
        ss << "</" << tag_name << ">";
        return ss.str ();
    }

    // Print the start tag.
    //
    ss << "<" << tag_name;

    // Print the attributes.
    //
    for (auto& alias : xml_obj.get_namespace_alias()) {
        ss << " xmlns:" << xml_escape(alias.first) << "=" << "'" << xml_escape(alias.second) << "'";
    }
    if (default_ns.length())
        ss << " xmlns=" << "'" << default_ns << "'";
    //for (auto attr=xml_obj.attributes.rbegin(); attr!=xml_obj.attributes.rend(); ++attr)
    for (auto attr=xml_obj.attributes.begin(); attr!=xml_obj.attributes.end(); ++attr)
        ss << ' ' << xml_escape(attr->first) << "='" << xml_escape(attr->second) << "'";

    // Check if we should only write the start tag.
    //
    if (xml_obj.get_part() == XmlObjPart::start) {
        ss << ">";
        return ss.str ();
    }

    // End if no children or content.
    //
    if (xml_obj.nodes.empty() && xml_obj.content.length()==0) {
        ss << "/>";
        return ss.str ();
    }
    ss << ">";

    // Print the child nodes.
    //
    for (auto node : xml_obj.nodes) {
        //
        // Recursion... gotta love it!
        //
        if (pretty)
            ss << to_string (node, true, indentation + "    ");
        else
            ss << to_string (node, false, indentation);
    }

    if (pretty && xml_obj.nodes.size()) {
        ss << endl << indentation;
    }

    // Print the content.
    //
    ss << xml_escape(xml_obj.content);

    // Print the end tag.
    //
    ss << "</" << tag_name << ">";

    return ss.str ();
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
std::string to_string (const XmlObject& xml_obj)
{
    return print_xml_obj (xml_obj, "");
}
*/


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject::XmlObject (int reserved_nodes)
    :
    namespace_is_default {false},
    part {XmlObjPart::all}
{
    nodes.reserve (reserved_nodes);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject::XmlObject (const std::string& the_name,
                      const std::string& the_namespace,
                      const bool         set_namespace_attr,
                      const bool         namespace_is_default,
                      const int          reserved_nodes)
    :
    tag_name             {the_name},
    xml_namespace        {the_namespace},
    namespace_is_default {namespace_is_default},
    part                 {XmlObjPart::all}
{
    nodes.reserve (reserved_nodes);
    if (set_namespace_attr)
        set_default_namespace_attr (the_namespace);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject::XmlObject (const XmlObject& xml_obj)
{
    tag_name             = xml_obj.tag_name;
    xml_namespace        = xml_obj.xml_namespace;
    namespace_is_default = xml_obj.namespace_is_default;
    namespace_alias      = xml_obj.namespace_alias;
    default_namespace    = xml_obj.default_namespace;
    attributes           = xml_obj.attributes;
    nodes                = xml_obj.nodes;
    content              = xml_obj.content;
    part                 = xml_obj.part;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject::XmlObject (XmlObject&& xml_obj)
{
    tag_name             = std::move (xml_obj.tag_name);
    xml_namespace        = std::move (xml_obj.xml_namespace);
    namespace_is_default = xml_obj.namespace_is_default;
    namespace_alias      = std::move (xml_obj.namespace_alias);
    default_namespace    = std::move (xml_obj.default_namespace);
    attributes           = std::move (xml_obj.attributes);
    nodes                = std::move (xml_obj.nodes);
    content              = std::move (xml_obj.content);
    part                 = xml_obj.part;
    xml_obj.namespace_is_default = false;
    xml_obj.part                 = XmlObjPart::all;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::operator= (const XmlObject& xml_obj)
{
    if (&xml_obj != this) {
        tag_name             = xml_obj.tag_name;
        xml_namespace        = xml_obj.xml_namespace;
        namespace_is_default = xml_obj.namespace_is_default;
        namespace_alias      = xml_obj.namespace_alias;
        default_namespace    = xml_obj.default_namespace;
        attributes           = xml_obj.attributes;
        nodes                = xml_obj.nodes;
        content              = xml_obj.content;
        part                 = xml_obj.part;
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::operator= (XmlObject&& xml_obj)
{
    tag_name             = std::move (xml_obj.tag_name);
    xml_namespace        = std::move (xml_obj.xml_namespace);
    namespace_is_default = xml_obj.namespace_is_default;
    namespace_alias      = std::move (xml_obj.namespace_alias);
    default_namespace    = std::move (xml_obj.default_namespace);
    attributes           = std::move (xml_obj.attributes);
    nodes                = std::move (xml_obj.nodes);
    content              = std::move (xml_obj.content);
    part                 = xml_obj.part;
    xml_obj.namespace_is_default = false;
    xml_obj.part                 = XmlObjPart::all;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string XmlObject::get_tag_name () const
{
    return tag_name;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::set_tag_name (const std::string& name)
{
    tag_name = name;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string XmlObject::get_namespace () const
{
    return xml_namespace;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::set_namespace (const std::string& xml_namespace)
{
    this->xml_namespace = xml_namespace;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::set_namespace (const std::string& xml_namespace, bool is_default)
{
    this->xml_namespace = xml_namespace;
    is_namespace_default (is_default);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool XmlObject::is_namespace_default () const
{
    return namespace_is_default;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::is_namespace_default (const bool is_default)
{
    namespace_is_default = is_default;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const std::unordered_map<std::string, std::string>& XmlObject::get_namespace_alias () const
{
    return namespace_alias;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string XmlObject::get_namespace_alias (const std::string& alias) const
{
    auto element = namespace_alias.find (alias);
    return element==namespace_alias.end() ? "" : (*element).second;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::add_namespace_alias (const std::string& alias, const std::string& xml_namespace)
{
    namespace_alias[alias] = xml_namespace;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::remove_namespace_alias (const std::string& alias)
{
    namespace_alias.erase (alias);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string XmlObject::get_default_namespace_attr () const
{
    return default_namespace;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::set_default_namespace_attr (const std::string& default_namespace)
{
    this->default_namespace = default_namespace;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string XmlObject::get_full_name () const
{
    if (xml_namespace.length())
        return xml_namespace + std::string(":") + tag_name;
    /*
      else if (namespace_alias.length())
      return namespace_alias + std::string(":") + tag_name;
    */
    else if (default_namespace.length())
        return default_namespace + std::string(":") + tag_name;
    else
        return tag_name;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const std::string XmlObject::get_attribute (const std::string& name) const
{
    auto value = attributes.find (name);
    return value == attributes.end() ? "" : (*value).second;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::unordered_map<std::string, std::string>& XmlObject::get_attributes ()
{
    return attributes;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::set_attribute (const std::string& name, const std::string& value)
{
    attributes[name] = value;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::remove_attribute (const std::string& name)
{
    attributes.erase (name);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::add_node (const XmlObject& xml_obj)
{
    nodes.push_back (xml_obj);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::add_node (XmlObject&& xml_obj)
{
    nodes.push_back (std::move(xml_obj));
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<XmlObject>& XmlObject::get_nodes ()
{
    return nodes;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject XmlObject::get_node (const std::string& name, bool full_name)
{
    for (auto& node : get_nodes()) {
        std::string node_name = full_name ? node.get_full_name() : node.get_tag_name();
        if (node_name == name)
            return node;
    }
    return XmlObject (0); // Return an empty object
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject XmlObject::get_ns_node (const std::string& name_space)
{
    for (auto& node : get_nodes()) {
        if (name_space == xml_namespace)
            return node;
    }
    return XmlObject (0); // Return an empty object
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const std::string& XmlObject::get_content () const
{
    return content;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::set_content (const std::string& content)
{
    this->content = content;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::set_content (std::string&& content)
{
    this->content = std::move (content);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObject& XmlObject::set_part (const XmlObjPart obj_part)
{
    part = obj_part;
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlObjPart XmlObject::get_part () const
{
    return part;
}



UXMPP_END_NAMESPACE1
