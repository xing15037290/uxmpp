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
#include <uxmpp.hpp>
#include <iostream>

using namespace std;
using namespace uxmpp;


int main (int argc, char* argv[])
{
    uxmpp_set_log_level (LogLevel::trace);

    if (argc < 2) {
        cerr << "Usage: test_Jid <jid>" << endl;
        return 1;
    }

    Jid jid (argv[1]);

    cout << "Arg: " << argv[1] << endl;
    cout << "Local part   : " << jid.get_local() << endl;
    cout << "Domain part  : " << jid.get_domain() << endl;
    cout << "Resource part: " << jid.get_resource() << endl;
    cout << "JID          : " << to_string(jid) << endl;
    cout << "Is bare?     : " << (jid.is_bare() ? "yes" : "no") << endl;
    cout << "Bare JID     : " << to_string(jid.bare()) << endl;
    cout << "to_string    : \"" << to_string(jid) << "\"" << endl;
    return 0;
}
