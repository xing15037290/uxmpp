/*
 *  Copyright (C) 2013-2014 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/net/BsdResolver.hpp>

#if (UXMPP_HAVE_BSD_RESOLVER)

#ifdef __APPLE__
#define BIND_8_COMPAT
#endif
#include <algorithm>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <array>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#ifdef __ANDROID__
#include "android_resolver_glue.h"
#endif


#define THIS_FILE "BsdResolver"


UXMPP_START_NAMESPACE2(uxmpp, net)

using namespace std;


struct srv_record {
    uint16_t prio;
    uint16_t weight;
    uint16_t port;
    string target;
};

constexpr uint16_t xmpp_client_port = 5222;
constexpr uint16_t xmpp_server_port = 5269;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static vector<srv_record> do_srv_query (const string& query)
{
    vector<srv_record> srv_records;
    union {
        HEADER hdr;
        u_char buf[4096];
    } response;

    uxmpp_log_trace (THIS_FILE, string("DNS SRV query: ") + query);

    // Make the SRV DNS query
    //
    int len = res_query (query.c_str(),
                         C_IN,  // ns_c_in
                         T_SRV, // ns_t_srv
                         response.buf,
                         sizeof(response.buf));

    // Check if we got an answer
    //
    if (len <= static_cast<int>(sizeof(HEADER)) || ntohs(response.hdr.ancount)==0) {
        uxmpp_log_debug (THIS_FILE, string("No response for DNS SRV query: ") + query);
        return srv_records;
    }

    // Set a pointer that points to the answer beyond the answer header.
    //
    u_char* ptr = response.buf;
    ptr += sizeof (HEADER);

    // Skip the query record(s)
    //
    for (short i=0; i<ntohs(response.hdr.qdcount); ++i) {
        char tmpbuf[256];
        int c = dn_expand (response.buf, response.buf+len, ptr, tmpbuf, sizeof(tmpbuf));
        if (c < 0) {
            uxmpp_log_debug (THIS_FILE, "Error reading query record from DNS SRV answer");
            return srv_records;
        }
        ptr += c + QFIXEDSZ;
    }

    // Check the answer records for SRV records
    //
    for (short i=0; i<ntohs(response.hdr.ancount); ++i) {
        srv_record record;
        char tmpbuf[256];
        int c = dn_expand (response.buf, response.buf+len, ptr, tmpbuf, sizeof(tmpbuf));
        if (c < 0) {
            uxmpp_log_debug (THIS_FILE, "Error reading answer record from DNS SRV answer");
            return srv_records;
        }
        ptr += c;

        // Get the record type
        uint16_t type = ntohs (*reinterpret_cast<uint16_t*>(ptr));
        ptr += sizeof (type);

        // Get the record class
        //uint16_t the_class = ntohs (*reinterpret_cast<uint16_t*>(ptr));
        ptr += sizeof (uint16_t);

        // Get TTL
        //uint32_t ttl = ntohl (*reinterpret_cast<uint32_t*>(ptr));
        ptr += sizeof (uint32_t);

        // Get data length
        uint16_t dlen = ntohs (*reinterpret_cast<uint16_t*>(ptr));
        ptr += sizeof (dlen);

        // Check if this is a SRV record
        if (type != T_SRV/*ns_t_srv*/) {
            ptr += dlen;
            continue;
        }

        // Get priority
        record.prio = ntohs (*reinterpret_cast<uint16_t*>(ptr));
        ptr += sizeof (record.prio);

        // Get weight
        record.weight = ntohs (*reinterpret_cast<uint16_t*>(ptr));
        ptr += sizeof (record.weight);

        // Get port
        record.port = ntohs (*reinterpret_cast<uint16_t*>(ptr));
        ptr += sizeof (record.port);

        // Get target
        c = dn_expand (response.buf, response.buf+len, ptr, tmpbuf, sizeof(tmpbuf));
        if (c < 0) {
            uxmpp_log_debug (THIS_FILE, "Error reading target from DNS SRV answer");
            return srv_records;
        }
        ptr += c;

        record.target = string (tmpbuf);
        srv_records.push_back (record);
    }

    return srv_records;
}


//------------------------------------------------------------------------------
// Return a list of IpHostAddr object.
// Only fields type and ipv4|ipv6 are filled in.
//------------------------------------------------------------------------------
vector<IpHostAddr> do_host_query (const std::string& hostname)
{
    vector<IpHostAddr> addr_list;

    int result;
    struct addrinfo* ai_list;
    struct addrinfo* ai;
    struct addrinfo  hints;

    std::memset (&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags    = /*AI_V4MAPPED |*/ AI_ADDRCONFIG;

    result = getaddrinfo (hostname.c_str(), NULL, &hints, &ai_list);
    if (result) {
        uxmpp_log_warning (THIS_FILE,
                           string("Unable to resolv host ")
                           + hostname
                           + string(": ")
                           + string(gai_strerror(result)));
        return addr_list;
    }

    for (ai=ai_list; ai!=NULL; ai=ai->ai_next) {
        IpHostAddr addr;
        if (ai->ai_family == AF_INET) {
            struct sockaddr_in* saddr = reinterpret_cast<struct sockaddr_in*> (ai->ai_addr);
            addr.type = AddrType::ipv4;
            addr.ipv4 = saddr->sin_addr.s_addr;
        }
        else if (ai->ai_family == AF_INET6) {
            struct sockaddr_in6* saddr = reinterpret_cast<struct sockaddr_in6*> (ai->ai_addr);
            addr.type = AddrType::ipv6;
            std::memcpy (&addr.ipv6, saddr->sin6_addr.s6_addr, sizeof(addr.ipv6));
        }
        else {
            continue;
        }
        addr_list.push_back (addr);
    }
    if (ai_list)
        freeaddrinfo (ai_list);

    return addr_list;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<IpHostAddr> BsdResolver::lookup_srv (const std::string& domain,
                                                 const AddrProto& proto,
                                                 const std::string& service,
                                                 bool dns_fallback)
{
    vector<IpHostAddr> addr_list;

    // Make the DNS SRV query string
    //
    string query = string("_") + service + string("._") + to_string(proto) + string(".") + domain;

    // Do the DNS SRV query
    //
    vector<srv_record> srv_records = do_srv_query (query);

    // Sort to get the highest prio first
    //
    std::sort (srv_records.begin(),
               srv_records.end(),
               [](const srv_record& r1, const srv_record& r2)->bool {
                   return r1.prio!=r2.prio ? (r1.prio < r2.prio) : (r1.weight > r2.weight);
               });

    // Check if the answer is a single record with "." as target.
    // If so, clear the record list.
    //
    if (srv_records.size()==1 && srv_records[0].target==".") {
        srv_records.clear ();
    }

    uxmpp_log_debug (THIS_FILE, "DNS SRV query gave ", srv_records.size(), " response(s) for domain: ", domain);

    // If no SRV records are found, fall back to a normal address lookup.
    //
    if (srv_records.empty() && dns_fallback) {
        uxmpp_log_debug (THIS_FILE, std::string("DNS SRV query gave no response, "
                                                "using normal address resolution for ") + domain);
        return lookup_host (domain,
                            service=="xmpp-client" ? xmpp_client_port : xmpp_server_port,
                            proto);
    }

    for (auto srv_rec : srv_records) {
        uxmpp_log_trace (THIS_FILE, std::string("Get IP addresses for ") + srv_rec.target);
        vector<IpHostAddr> addresses = do_host_query (srv_rec.target);
        for (auto addr : addresses) {
            addr.hostname = srv_rec.target;
            addr.proto = proto;
            addr.port = htons (srv_rec.port);
            addr_list.push_back (addr);
        }
    }

    return addr_list;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<IpHostAddr> BsdResolver::lookup_host (const std::string& host,
                                                  const uint16_t port,
                                                  const AddrProto& proto)
{
    vector<IpHostAddr> addr_list = do_host_query (host);

    for (auto& addr : addr_list) {
        addr.hostname = host;
        addr.proto    = proto;
        addr.port     = htons (port);
    }

    return addr_list;
}




UXMPP_END_NAMESPACE2

#endif
