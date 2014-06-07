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
#ifndef UXMPP_MOD_AUTHMODULE_HPP
#define UXMPP_MOD_AUTHMODULE_HPP

#include <string>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <unordered_set>


namespace uxmpp { namespace mod {


    /**
     * An XMPP authentication module.
     */
    class AuthModule : public uxmpp::XmppModule {
    public:

        /**
         * Default Constructor.
         */
        AuthModule () : uxmpp::XmppModule ("mod_auth") { }

        /**
         * Destructor.
         */
        virtual ~AuthModule () = default;

        /**
         * Called whan an XML object is received.
         * @return Return true if this XML object was processed and no further work should be done.
         */
        virtual bool proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj);

        /**
         * User name used for authentication.
         */
        std::string auth_user;

        /**
         * Pass phrase used for authentication.
         */
        std::string auth_pass;


    protected:

        /**
         * Authentication mechanisms
         */
        std::unordered_set<std::string> mechanisms;
    };


}}


#endif
