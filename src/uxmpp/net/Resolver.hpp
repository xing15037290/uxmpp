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
#ifndef UXMPP_RESOLVER_HPP
#define UXMPP_RESOLVER_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/net/IpHostAddr.hpp>
#include <string>
#include <vector>


namespace uxmpp { namespace net {

    /**
     * A DNS resolver.
     */
    class Resolver {
    public:

        /**
         * Default constructor.
         */
        Resolver () = default;

        /**
         * Destructor.
         */
        virtual ~Resolver () = default;

        /**
         * Lookup a domain using a DNS SRV query.
         * @param domain The domain to look up.
         * @param proto The protocol to use. Default is TCP.
         * @param service The service to use. Default is "xmpp-server".
         * @param dns_fallback If the SRV query fails, fallback to a normal DNS host query.
         * @return A list of IpHostAddr objects.
         */
        virtual std::vector<IpHostAddr> lookup_srv (const std::string& domain,
                                                    const AddrProto& proto=AddrProto::tcp,
                                                    const std::string& service="xmpp-server",
                                                    bool dns_fallback=true) = 0;

        /**
         * Lookup a domain using a normal address resolution.
         * @param host The host to look up.
         * @param port The port to use (in host byte order). Default is 5222.
         * @param proto The protocol to use. Default is TCP.
         * @return A list of IpHostAddr objects.
         */
        virtual std::vector<IpHostAddr> lookup_host (const std::string& host,
                                                     const uint16_t port=5222,
                                                     const AddrProto& proto=AddrProto::tcp) = 0;
    };


}}


#endif
