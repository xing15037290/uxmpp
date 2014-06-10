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
#include <uxmpp/Session.hpp>
#include <uxmpp/net/BsdResolver.hpp>
#include <uxmpp/StreamXmlObj.hpp>
#include <uxmpp/utils.hpp>

#define THIS_FILE "SessionConfig"


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;
using namespace uxmpp::net;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SessionConfig::SessionConfig ()
    :
    domain      {""},
    server      {""},
    port        {0},
    protocol    {AddrProto::tcp},
    disable_srv {false}
{
}



UXMPP_END_NAMESPACE1
