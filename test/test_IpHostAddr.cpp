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
using namespace uxmpp::io;


int main (int argc, char* argv[])
{
    IpHostAddr addr1;
    IpHostAddr addr2;

    addr1.ipv4 = 0x0100007f;
    addr2.ipv4 = 0x0100007f;

    addr1.port = 0xc413;
    addr2.port = 0xc413;

    cout << to_string(addr1) << endl;

    cout << to_string(addr1) << " " << (addr1==addr2?"==":"!=") << " " << to_string(addr2) << endl;

    addr2.proto = AddrProto::udp;
    cout << to_string(addr1) << " " << (addr1==addr2?"==":"!=") << " " << to_string(addr2) << endl;

    addr1.proto = AddrProto::udp;
    cout << to_string(addr1) << " " << (addr1==addr2?"==":"!=") << " " << to_string(addr2) << endl;

    addr1.type = AddrType::ipv6;
    cout << to_string(addr1) << " " << (addr1==addr2?"==":"!=") << " " << to_string(addr2) << endl;

    addr2.type = AddrType::ipv6;
    cout << to_string(addr1) << " " << (addr1==addr2?"==":"!=") << " " << to_string(addr2) << endl;

    addr2.ipv6[0] = 0xfe;
    addr2.ipv6[1] = 0x80;
    addr2.ipv6[2] = 0x00;
    addr2.ipv6[3] = 0x00;
    addr2.ipv6[4] = 0x00;
    addr2.ipv6[5] = 0x00;
    addr2.ipv6[6] = 0x00;
    addr2.ipv6[7] = 0x00;
    addr2.ipv6[8] = 0x22;
    addr2.ipv6[9] = 0x26;
    addr2.ipv6[10] = 0xb9;
    addr2.ipv6[11] = 0xff;
    addr2.ipv6[12] = 0xfe;
    addr2.ipv6[13] = 0xec;
    addr2.ipv6[14] = 0xe2;
    addr2.ipv6[15] = 0x7c;
    cout << to_string(addr1) << " " << (addr1==addr2?"==":"!=") << " " << to_string(addr2) << endl;

    return 0;
}
