/*
 *  Copyright (C) 2014 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_MOD_TLSMODULE_HPP
#define UXMPP_MOD_TLSMODULE_HPP

#include <string>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/io/TlsConfig.hpp>


namespace uxmpp { namespace mod {


    /**
     * An XMPP TLS module.
     */
    class TlsModule : public uxmpp::XmppModule {
    public:

        /**
         * Default Constructor.
         */
        TlsModule ();

        /**
         * Constructor.
         */
        TlsModule (const uxmpp::io::TlsConfig& tls_config);

        /**
         * Destructor.
         */
        virtual ~TlsModule () = default;

        /**
         * Called whan an XML object is received.
         * @return Return true if this XML object was processed and no further work should be done.
         */
        virtual bool proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj) override;

        /**
         * TLS configuration.
         */
        uxmpp::io::TlsConfig tls_cfg;
    };


}}


#endif
