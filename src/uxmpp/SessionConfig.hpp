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
#ifndef UXMPP_SESSIONCONFIG_HPP
#define UXMPP_SESSIONCONFIG_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/net/IpHostAddr.hpp>
#include <uxmpp/TlsConfig.hpp>
#include <string>


namespace uxmpp {


    /**
     * XMPP Session configuration.
     */
    class SessionConfig {
    public:

        /**
         * Default constructor.
         */
        SessionConfig ();

        /**
         * Destructor.
         */
        virtual ~SessionConfig () = default;

        /**
         * XMPP domain.
         */
        std::string domain;

        /**
         * User id.
         */
        std::string user_id;

        /**
         * Optional resource. Set this if you want to bind to a specific resource.
         */
        std::string resource;
#if 0
        /**
         * TLS configuration.
         */
        TlsConfig tls;
#endif

        /**
         * XMPP sever.
         * Set this it you want to override the domain setting and specify a specific server.
         * A DNS SRV lookup will still be done if 'disable_srv' is not set.
         * Default is an empty string to use the domain setting.
         */
        std::string server;

        /**
         * XMPP server port in host byte order.
         * Set this to 0 to use the default port, or the port returned by a DNS SRV query.
         * Set this t0 non-zero override the default port or the port returned by a DNS SRV query.
         * Default is 0.
         */
        uint16_t port;

        /**
         * The protocol used when connecting to the server. Default is TCP (AddrProto::tcp).
         */
        uxmpp::net::AddrProto protocol;

        /**
         * Don't do DNS SRV queries, only normal host queries (why would anyone want this?).
         * Default is 'false' to use DNS SRV queries.
         */
        bool disable_srv;
    };


}


#endif
