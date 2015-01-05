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
#include <uxmpp/XmppModule.hpp>


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmppModule::XmppModule (const std::string& name)
{
    this->name = name.length() ? name : "<anonymous>";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const std::string& XmppModule::get_name () const
{
    return name;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmppModule::module_registered (Session& session)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmppModule::module_unregistered (Session& session)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool XmppModule::proccess_xml_object (Session& session, XmlObject& xml_obj)
{
    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<std::string> XmppModule::get_disco_features ()
{
    return std::vector<std::string> ();
}


UXMPP_END_NAMESPACE1
