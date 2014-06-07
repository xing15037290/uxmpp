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


START_NAMESPACE1(uxmpp)


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
    string ns         = xml_escape (xml_obj.getNamespace());
    string default_ns = xml_escape (xml_obj.getDefaultNamespaceAttr());

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
    if (xml_obj.getPart() == XmlObjPart::body) {
        return xml_escape (xml_obj.content);
    }

    // Get the name of the tag to print.
    //
    if (ns.length() && !xml_obj.isNamespaceDefault()) {
        tag_name = xml_escape (ns + string(":") + xml_escape(xml_obj.getName()));
    }
    else {
        tag_name = xml_escape (xml_obj.getName());
    }

    // Check if we should only print the end tag.
    //
    if (xml_obj.getPart() == XmlObjPart::end) {
        ss << "</" << tag_name << ">";
        return ss.str ();
    }

    // Print the start tag.
    //
    ss << "<" << tag_name;

    // Print the attributes.
    //
    for (auto& alias : xml_obj.getNamespaceAlias()) {
        ss << " xmlns:" << xml_escape(alias.first) << "=" << "'" << xml_escape(alias.second) << "'";
    }
    if (default_ns.length())
        ss << " xmlns=" << "'" << default_ns << "'";
    //for (auto attr=xml_obj.attributes.rbegin(); attr!=xml_obj.attributes.rend(); attr++) {
    for (auto attr=xml_obj.attributes.begin(); attr!=xml_obj.attributes.end(); attr++) {
        ss << ' ' << xml_escape(attr->first) << "='" << xml_escape(attr->second) << "'";
    }

    // Check if we should only write the start tag.
    //
    if (xml_obj.getPart() == XmlObjPart::start) {
        ss << ">";
        return ss.str ();
    }

    // End if no children or content.
    //
    if (xml_obj.nodes.size()==0 && xml_obj.content.length()==0) {
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

END_NAMESPACE1
