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
#include <uxmpp/XmlNames.hpp>
#include <uxmpp/mod/RosterItem.hpp>

#define THIS_FILE "RosterItem"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<std::string> RosterItem::getGroups ()
{
    std::vector<std::string> g;
    for (auto& node : getNodes()) {
        if (node.getFullName() == XmlRosterGroupTagFull) {
            string group_name = node.getContent ();
            if (group_name.length())
                g.push_back (group_name);
        }
    }
    return g;
}


UXMPP_END_NAMESPACE2
