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
#include <uxmpp/Logger.hpp>
#include <uxmpp/Stanza.hpp>
#include <uxmpp/utils.hpp>
#include <random>


#define THIS_FILE "Stanza"


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;

static std::string default_make_id ();


std::function<std::string (void)> Stanza::make_id = default_make_id;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static std::string default_make_id ()
{
    static random_device rd;
    static default_random_engine re (rd());
    //static uniform_int_distribution<int> dlen (12, 16);  // Random between 12 .. 16
    static uniform_int_distribution<int> d0 (0, 2);  // Random between 0 .. 2
    static uniform_int_distribution<int> d1 (0, 9);  // Random between 0 .. 9
    static uniform_int_distribution<int> d2 (0, 25); // Random between 0 .. 25

    string id = "";
    char ch;
    int len = 16;//dlen (re);

    for (auto i=0; i<len; ++i) {
        switch (d0(re)) {
        case 0:
            ch = '0' + d1(re);
            break;
        case 1:
            ch = 'a' + d2(re);
            break;
        case 2:
            ch = 'A' + d2(re);
            break;
        }
        id += ch;
    }
    return id;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Stanza::Stanza (const std::string& to, const std::string& from, const std::string& id)
    : XmlObject ("", XmlJabberClientNs, false, true, 1)
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
    : XmlObject ("", XmlJabberClientNs, false, true, 1)
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
std::string Stanza::get_error_type ()
{
    if (!have_error())
        return "";
    for (XmlObject& node : get_nodes()) {
        if (node.get_full_name() == XmlIqErrorStanzaTagFull) {
            return node.get_attribute ("type");
        }
    }
    return "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int Stanza::get_error_code ()
{
    if (!have_error())
        return 0;
    auto node = get_node (XmlIqErrorStanzaTagFull, true);
    return node ? atoi(node.get_attribute("code").c_str()) : 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string Stanza::get_error_name ()
{
    if (!have_error())
        return "";
    auto node = get_node (XmlIqErrorStanzaTagFull, true);
    if (node) {
        auto child_nodes = node.get_nodes ();
        if (!child_nodes.empty())
            return child_nodes.begin()->get_tag_name ();
        else
            return "";
    }
    return "";
}



UXMPP_END_NAMESPACE1
