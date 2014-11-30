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
#include <uxmpp/types.hpp>
#include <uxmpp/net/IpHostAddr.hpp>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>
#include <sys/socket.h>


UXMPP_START_NAMESPACE2(uxmpp, net)


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const AddrProto& proto)
{
    switch (proto) {
    case AddrProto::udp:
        return "udp";
    case AddrProto::tcp:
        return "tcp";
    case AddrProto::tls:
        return "tls";
    case AddrProto::dtls:
        return "dtls";
    case AddrProto::any:
        return "any";
    default:
        return "n/a";
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const IpHostAddr& addr)
{
    std::stringstream ss;
    char ip_str[INET6_ADDRSTRLEN];
    const char* result = nullptr;

    if (addr.type == AddrType::ipv4) {
        result = inet_ntop (AF_INET, &addr.ipv4, ip_str, sizeof(ip_str));
    }
    else if (addr.type == AddrType::ipv6) {
        result = inet_ntop (AF_INET6, &addr.ipv6, ip_str, sizeof(ip_str));
    }
    if (!result)
        ip_str[0] = '\0';

    ss << to_string(addr.proto) << ':'
       << addr.hostname << "(" << ip_str << "):"
       << std::setw(1) << std::dec << ntohs(addr.port);

    return ss.str ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IpHostAddr::IpHostAddr ()
    :
    hostname {""},
    proto    {AddrProto::tcp},
    type     {AddrType::ipv4},
    ipv6     {{0}},
    port     {0}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool IpHostAddr::operator== (const IpHostAddr& addr)
{
    return hostname == addr.hostname
        && type == addr.type
        && proto == addr.proto
        && (type==AddrType::ipv4 ? ipv4==addr.ipv4 : ipv6==addr.ipv6)
        && port == addr.port;
}


UXMPP_END_NAMESPACE2
