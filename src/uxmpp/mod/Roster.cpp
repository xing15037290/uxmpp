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
#include <uxmpp/mod/Roster.hpp>
#include <uxmpp/xml/names.hpp>


#define THIS_FILE "Roster"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Roster::Roster (const std::string& version)
    : uxmpp::XmlObject ("query", xml::namespace_iq_roster, true, true, 1)
{
    if (!version.empty())
        set_attribute ("ver", version);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Roster::Roster (const Roster& roster)
    : uxmpp::XmlObject (roster)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Roster::Roster (Roster&& roster)
    : uxmpp::XmlObject (roster)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Roster& Roster::operator= (const Roster& roster)
{
    if (this != &roster) {
        uxmpp::XmlObject::operator= (roster);
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Roster& Roster::operator= (const uxmpp::XmlObject& roster)
{
    if (this != &roster) {
        uxmpp::XmlObject::operator= (roster);
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Roster& Roster::operator= (Roster&& roster)
{
    uxmpp::XmlObject::operator= (roster);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<RosterItem>& Roster::get_items ()
{
    return reinterpret_cast<std::vector<RosterItem>&> (get_nodes());
}


UXMPP_END_NAMESPACE2
