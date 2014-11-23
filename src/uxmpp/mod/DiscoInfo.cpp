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
#include <uxmpp/mod/DiscoInfo.hpp>

#define THIS_FILE "DiscoInfo"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoInfo::DiscoInfo ()
    : XmlObject ("query", "http://jabber.org/protocol/disco#info")
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoInfo::DiscoInfo (const uxmpp::XmlObject& info)
    : XmlObject (info)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoInfo::DiscoInfo (const DiscoInfo& info)
    : XmlObject (info)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoInfo::DiscoInfo (const DiscoInfo&& info)
    : XmlObject (info)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoInfo& DiscoInfo::operator= (const DiscoInfo& info)
{
    if (this != &info)
        XmlObject::operator= (info);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DiscoInfo& DiscoInfo::operator= (const DiscoInfo&& info)
{
    XmlObject::operator= (info);
    return *this;
}



UXMPP_END_NAMESPACE2
