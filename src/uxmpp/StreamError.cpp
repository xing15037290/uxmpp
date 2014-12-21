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
    set_error_name (xml_obj.get_nodes().empty() ? "" : xml_obj.get_nodes().begin()->get_tag_name());
    for (auto& node : xml_obj.get_nodes()) {
        if (node.get_tag_name() == "text") {
            set_text (node.get_content());
            break;
        }
    }
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

    XmlObject& obj = const_cast<XmlObject&> (xml_obj);
    set_error_name (obj.get_nodes().empty() ? "": obj.get_nodes().begin()->get_tag_name());
    for (auto& node : obj.get_nodes()) {
        if (node.get_tag_name() == "text") {
            set_text (node.get_content());
            break;
        }
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const bool StreamError::have_error ()
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
