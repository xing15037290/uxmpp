/*
 *  Copyright (C) 2013 Ultramarin Design AB <dan@ultramarin.se>
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
using namespace uxmpp::io;


int main (int argc, char* argv[])
{
    uxmpp_set_log_level (LogLevel::trace);

    if (argc < 2) {
        cerr << "Usage: test_Resolver <domain>" << endl;
        return 1;
    }

#if (UXMPP_HAVE_BSD_RESOLVER)
    BsdResolver resolver;

    auto addr_list = resolver.lookup_srv (argv[1], AddrProto::tcp, "xmpp-client");
    //auto addr_list = resolver.lookup_host (argv[1]);

    for (auto addr : addr_list) {
        cout << to_string(addr) << endl;
    }
#endif

    return 0;
}
