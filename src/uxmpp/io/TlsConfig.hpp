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
#ifndef UXMPP_IO_TLSCONFIG_HPP
#define UXMPP_IO_TLSCONFIG_HPP

#include <uxmpp/types.hpp>
#include <string>


namespace uxmpp { namespace io {


    /**
     * TLS method.
     * This specified what type of protocol to be used for the TLS communication.
     */
    enum class TlsMethod {
        sslv23,  /**< Secure Socket Layer (SSL) compatible with version 2 and 3. */
        sslv3,   /**< Secure Socket Layer (SSL) version 3.      */
        tlsv1,   /**< Transport Layer Security (TLS) version 1. */
        tlsv1_1, /**< Transport Layer Security (TLS) version 1.1. */
        tlsv1_2, /**< Transport Layer Security (TLS) version 1.2. */
        dtlsv1,  /**< Datagram Transport Layer Security (DTLS) version 1. */
    };

    /**
     * Return a string representation of the TLS method.
     * @param method The SSL/TLS method.
     * @return A string representing the TLS method.
     */
    static inline std::string to_string (const TlsMethod& method) {
        switch (method) {
        case TlsMethod::sslv23:
            return "SSLv23";
        case TlsMethod::sslv3:
            return "SSLv3";
        case TlsMethod::tlsv1:
            return "TLSv1";
        case TlsMethod::tlsv1_1:
            return "TLSv1.1";
        case TlsMethod::tlsv1_2:
            return "TLSv1.2";
        case TlsMethod::dtlsv1:
            return "DTLSv1";
        }
        return "SSLv23";
    }

    /**
     * TLS configuration.
     * The configuration parameters to be used when encrypting the network traffic using
     * Transport Layer Security (TLS).
     */
    class TlsConfig {
    public:
        /**
         * Default constructor.
         * The default configuration uses TLS v1.2 as protocol and does not verify
         * the server certificate.
         */
        TlsConfig () : method{TlsMethod::tlsv1_2}, verify_server{false} {
        }

        /**
         * TLS method.
         * The protocol version to be used for the encrypted communication.
         */
        TlsMethod method;

        /**
         * Verify the server's certificate.
         * If true, the connection will only be allowed if the
         * server certificate can be verified.
         */
        bool verify_server;
    };


}}


#endif
