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
#ifndef UXMPP_NET_IPHOSTADDR_HPP
#define UXMPP_NET_IPHOSTADDR_HPP

#include <uxmpp/types.hpp>
#include <array>
#include <string>


namespace uxmpp { namespace net {

    /**
     * A type of IP address.
     */
    enum class AddrType {
        /** An IPv4 address. */
        ipv4=1,
        /** An IPv6 address. */
        ipv6=2,
        /** An IPv4 or IPv6 address. */
        any=3
    };

    /**
     * A type of protocol.
     */
    enum class AddrProto {
        /** TCP - Transmission Control Protocol. */
        tcp=1,
        /** UDP - User Datagram Protocol. */
        udp=2,
        /** TLS - Transport Layer Security. */
        tls=4,
        /** DTLS - Datagram Transport Layer Security. */
        dtls=8,
        /** Any of the above. */
        any=15
    };


    /**
     * The name, address, protocol and port used to connect to an entity on an IP network.
     */
    class IpHostAddr {
    public:

        /**
         * Default constructor.
         */
        IpHostAddr ();

        /**
         * Destructor.
         */
        virtual ~IpHostAddr () = default;

        /**
         * Compare operator.
         */
        bool operator== (const IpHostAddr& addr);

        /**
         * The name of the host.
         */
        std::string hostname;

        /**
         * The protocol used.
         */
        AddrProto proto;

        /**
         * The type of address (IPv4 or IPv6).
         */
        AddrType type;

        union {
            /**
             * An IPv4 address in network byte order.
             */
            uint32_t ipv4;

            /**
             * An IPv6 address.
             */
            std::array<unsigned char, 16> ipv6;
        };

        /**
         * The port in network byte order.
         */
        uint16_t port;

    protected:

    };


    /**
     * Return a string representation of a transport protocol.
     */
    std::string to_string (const AddrProto& proto);


    /**
     * Return a string representation of the host.
     * The form of the string is: proto:address:port the port is
     * printed in host byte order.
     */
    std::string to_string (const IpHostAddr& addr);


}}


#endif
