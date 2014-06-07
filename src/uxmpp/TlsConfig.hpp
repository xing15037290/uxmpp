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
#ifndef UXMPP_TLSCONFIG_HPP
#define UXMPP_TLSCONFIG_HPP

#include <uxmpp/types.hpp>
#include <string>


namespace uxmpp {


    /**
     * TLS method.
     */
    enum class TlsMethod {
        sslv3, tlsv1, tlsv1_1, tlsv1_2, dtlsv1, sslv23
    };

    /**
     * Return a string representation of the TLS method.
     */
    static inline std::string to_string (const TlsMethod& method) {
        switch (method) {
        case TlsMethod::sslv3:
            return "SSLv2";
        case TlsMethod::tlsv1:
            return "TLSv1";
        case TlsMethod::tlsv1_1:
            return "TLSv1.1";
        case TlsMethod::tlsv1_2:
            return "TLSv1.2";
        case TlsMethod::dtlsv1:
            return "DTLSv1";
        case TlsMethod::sslv23:
            break;
        }
        return "SSLv23";
    }

    /**
     * TLS configuration.
     */
    class TlsConfig {
    public:
        /**
         * Default constructor.
         */
        TlsConfig () : method{TlsMethod::tlsv1_2}, verify_server{false} {
        }

        /**
         * TLS method.
         */
        TlsMethod method;

        /**
         * Verify the server's certificate.
         */
        bool verify_server;
    };


}


#endif
