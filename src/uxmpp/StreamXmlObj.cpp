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
#include <uxmpp/StreamXmlObj.hpp>
#include <uxmpp/Logger.hpp>
#include <uxmpp/xml/names.hpp>


#define THIS_FILE "StreamXmlObj.cpp"


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StreamXmlObj::StreamXmlObj (const std::string& lang)
    : XmlObject (xml::tag_stream, xml::alias_stream, false, false)
{
    add_namespace_alias (xml::alias_stream, xml::namespace_stream);
    set_default_namespace_attr (xml::namespace_jabber_client);
    set_attribute ("version", "1.0");
    set_attribute ("xml:lang", lang);
    set_part (XmlObjPart::start);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StreamXmlObj::StreamXmlObj (const std::string& to, const std::string& from, const std::string& lang)
    : XmlObject (xml::tag_stream, xml::alias_stream, false, false)
{
    add_namespace_alias (xml::alias_stream, xml::namespace_stream);
    set_default_namespace_attr (xml::namespace_jabber_client);
    set_attribute ("version", "1.0");
    set_attribute ("xml:lang", lang);
    set_part (XmlObjPart::start);

    set_to (to);
    set_from (from);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StreamXmlObj::set_to (const std::string& to)
{
    set_attribute ("to", to);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StreamXmlObj::get_to () const
{
    return get_attribute ("to");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StreamXmlObj::set_id (const std::string& id)
{
    set_attribute ("id", id);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StreamXmlObj::get_id () const
{
    return get_attribute ("id");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StreamXmlObj::set_from (const std::string& from)
{
    set_attribute ("from", from);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string StreamXmlObj::get_from () const
{
    return get_attribute ("from");
}



UXMPP_END_NAMESPACE1
