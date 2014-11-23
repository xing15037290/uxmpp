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
#include <uxmpp/utils.hpp>
#include <uxmpp/IqStanza.hpp>

#define THIS_FILE "IqStanza.cpp"


UXMPP_START_NAMESPACE1(uxmpp)

using namespace std;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const IqType& iq_type)
{
    switch (iq_type) {
    case IqType::get :
        return "get";
    case IqType::set :
        return "set";
    case IqType::result :
        return "result";
    case IqType::error :
    default :
        return "error";
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IqStanza::IqStanza (const IqType type, const std::string& to, const std::string& from, const std::string& id)
    : Stanza (to, from, id)
{
    set_tag_name ("iq");
    set_attribute ("type", to_string(type));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IqStanza::IqStanza (const IqType type, const Jid& to, const Jid& from, const std::string& id)
    : Stanza (to, from, id)
{
    set_tag_name ("iq");
    set_attribute ("type", to_string(type));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IqStanza::IqStanza (const IqStanza& iq_stanza)
    : Stanza (iq_stanza)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IqStanza::IqStanza (IqStanza&& iq_stanza)
    : Stanza (iq_stanza)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IqStanza& IqStanza::operator= (const IqStanza& iq_stanza)
{
    if (this != &iq_stanza)
        Stanza::operator= (iq_stanza);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IqStanza& IqStanza::operator= (IqStanza&& iq_stanza)
{
    Stanza::operator= (iq_stanza);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IqType IqStanza::get_type () const
{
    std::string type = get_attribute ("type");
    if (type == "get")
        return IqType::get;
    if (type == "set")
        return IqType::set;
    if (type == "result")
        return IqType::result;
    if (type == "error")
        return IqType::error;
    return IqType::error; // default
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IqStanza::set_type (const IqType type)
{
    set_attribute ("type", to_string(type));
}



UXMPP_END_NAMESPACE1
