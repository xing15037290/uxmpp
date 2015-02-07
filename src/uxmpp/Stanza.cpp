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
#include <uxmpp/Logger.hpp>
#include <uxmpp/Stanza.hpp>
#include <uxmpp/utils.hpp>
#include <uxmpp/xml/names.hpp>
#include <random>


#define THIS_FILE "Stanza"


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;


std::function<std::string (void)> Stanza::make_id = make_uuid_v4;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Stanza::Stanza (const std::string& to, const std::string& from, const std::string& id)
    : XmlObject ("", xml::namespace_jabber_client, false, true)
{
    if (!to.empty())
        set_attribute ("to", to);
    if (!from.empty())
        set_attribute ("from", from);
    // Set a random ID if none is provided
    set_id (id.empty() ? make_id() : id);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Stanza::Stanza (const Jid& to, const Jid& from, const std::string& id)
    : XmlObject ("", xml::namespace_jabber_client, false, true)
{
    set_to (to);
    set_from (from);
    // Set a random ID if none is provided
    set_id (id.empty() ? make_id() : id);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Stanza::Stanza (const Stanza& stanza)
    : XmlObject (stanza)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Stanza::Stanza (Stanza&& stanza)
    : XmlObject (stanza)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Stanza& Stanza::operator= (const Stanza& stanza)
{
    if (this != &stanza)
        XmlObject::operator= (stanza);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Stanza& Stanza::operator= (Stanza&& stanza)
{
    XmlObject::operator= (stanza);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Jid Stanza::get_to () const
{
    return Jid (get_attribute("to"));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stanza::set_to (const Jid& to)
{
    std::string to_str = to_string (to);
    if (to_str.length())
        set_attribute ("to", to_str);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Jid Stanza::get_from () const
{
    return Jid (get_attribute("from"));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stanza::set_from (const Jid& from)
{
    std::string from_str = to_string (from);
    if (from_str.length())
        set_attribute ("from", from_str);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string Stanza::get_id () const
{
    return get_attribute ("id");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stanza::set_id (const std::string& id)
{
    set_attribute ("id", id);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string Stanza::get_type () const
{
    return get_attribute ("type");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stanza::set_type (const std::string& type)
{
    set_attribute ("type", type);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Stanza::have_error () const
{
    return get_type() == "error";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StanzaError Stanza::get_error ()
{
    for (auto& node : get_nodes()) {
        if (node.get_full_name() == xml::full_tag_error_stanza) {
            StanzaError& err = reinterpret_cast<StanzaError&> (node);
            return err;
        }
    }
    StanzaError err ("","");
    err.set_tag_name ("");
    return err;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Stanza::set_error (const StanzaError& error)
{
    // Add/replace the error node
    //
    for (auto& node : get_nodes()) {
        if (node.get_full_name() == xml::full_tag_error_stanza) {
            node = error;
            return;
        }
    }
    add_node (error);
}



UXMPP_END_NAMESPACE1
