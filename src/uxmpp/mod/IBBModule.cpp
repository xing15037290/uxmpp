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
#include <uxmpp/mod/IBBModule.hpp>


UXMPP_START_NAMESPACE2(uxmpp, mod)

using namespace std;
using namespace uxmpp;

static const string log_module {"IBBModule"};

static const string namespace_iq_ibb {"http://jabber.org/protocol/ibb"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IBBModule::IBBModule () : XmppModule ("mod_ibb")
{
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IBBModule::module_registered (uxmpp::Session& session)
{
    sess = &session;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IBBModule::module_unregistered (uxmpp::Session& session)
{
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool IBBModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    return false;
}


UXMPP_END_NAMESPACE2