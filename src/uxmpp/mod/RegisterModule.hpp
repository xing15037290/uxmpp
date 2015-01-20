/*
 *  Copyright (C) 2014-2015 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_MOD_REGISTERMODULE_HPP
#define UXMPP_MOD_REGISTERMODULE_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/StanzaError.hpp>
#include <uxmpp/IqStanza.hpp>
#include <uxmpp/mod/RegistrationInfo.hpp>

#include <functional>
#include <string>
#include <list>


namespace uxmpp { namespace mod {


    /**
     * In-band registration (XEP-0077).
     */
    class RegisterModule : public uxmpp::XmppModule {
    public:

        /**
         * Registration info callback.
         */
        typedef std::function<void (uxmpp::Session& session, RegistrationInfo& reg_info)> reg_info_cb_t;

        /**
         * Registration result callback.
         * @param session The XMPP session.
         * @param operation Type of operation: "regiser", "unregister", or "password".
         * @param error An error object. Evaluates to false if no error.
         */
        typedef std::function<void (uxmpp::Session& session,
                                    const std::string& operation,
                                    uxmpp::StanzaError& error)> reg_result_cb_t;

        /**
         * Default Constructor.
         */
        RegisterModule ();

        /**
         * Destructor.
         */
        virtual ~RegisterModule () = default;

        /**
         * Called when the module is registered to a session.
         */
        virtual void module_registered (uxmpp::Session& session) override;

        /**
         * Called when the module is unregistered from a session.
         */
        virtual void module_unregistered (uxmpp::Session& session) override;

        /**
         * Called whan an XML object is received.
         * @return Return true if this XML object was processed and no further work should be done.
         */
        virtual bool process_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj) override;

        /**
         *
         */
        void set_info_callback (reg_info_cb_t callback);

        /**
         *
         */
        void set_result_callback (reg_result_cb_t callback);

        /**
         * Request registration info.
         */
        bool request_info (const std::string& domain="");

        /**
         * Perform registration.
         */
        bool register_user (std::list<std::pair<std::string, std::string> >& fields, const std::string& domain="");

        /**
         * Perform un-registration.
         */
        bool unregister_user ();

        /**
         * Change password for a user.
         */
        bool set_password (const std::string& new_passphrase);


    private:
        void handle_info_query_result (IqStanza& iq);

        uxmpp::Session* sess;
        std::string query_id;

        reg_info_cb_t reg_info_cb;
        reg_result_cb_t reg_result_cb;

        std::string registration_id;
        std::string unregistration_id;
        std::string pass_change_id;
    };



}}

#endif
