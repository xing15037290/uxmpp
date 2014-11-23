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
#include <uxmpp/mod/DiscoIdentity.hpp>

#define THIS_FILE "DiscoIdentity"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoIdentity::DiscoIdentity (const std::string& category,
                              const std::string& type,
                              const std::string& name)
    : XmlObject ("identity", "http://jabber.org/protocol/disco#info", false)
{
    set_category (category);
    set_type (type);
    set_name (name);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoIdentity::DiscoIdentity (const uxmpp::XmlObject& identity)
    : XmlObject (identity)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoIdentity::DiscoIdentity (const DiscoIdentity& identity)
    : XmlObject (identity)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoIdentity::DiscoIdentity (const DiscoIdentity&& identity)
    : XmlObject (identity)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoIdentity& DiscoIdentity::operator= (const DiscoIdentity& identity)
{
    if (this != &identity)
        XmlObject::operator= (identity);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoIdentity& DiscoIdentity::operator= (const DiscoIdentity&& identity)
{
    XmlObject::operator= (identity);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const std::string DiscoIdentity::get_category () const
{
    return get_attribute ("category");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoIdentity::set_category (const std::string& category)
{
    set_attribute ("category", category);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const std::string DiscoIdentity::get_type () const
{
    return get_attribute ("type");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoIdentity::set_type (const std::string& type)
{
    set_attribute ("type", type);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const std::string DiscoIdentity::get_name () const
{
    return get_attribute ("name");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DiscoIdentity::set_name (const std::string& name)
{
    set_attribute ("name", name);
}



UXMPP_END_NAMESPACE2
