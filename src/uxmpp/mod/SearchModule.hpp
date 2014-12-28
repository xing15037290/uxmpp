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
#ifndef UXMPP_MOD_SEARCHMODULE_HPP
#define UXMPP_MOD_SEARCHMODULE_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/StanzaError.hpp>
#include <uxmpp/IqStanza.hpp>

#include <functional>
#include <string>
#include <list>


namespace uxmpp { namespace mod {


    /**
     * Jabber Search (XEP-0055).
     */
    class SearchModule : public uxmpp::XmppModule {
    public:

        /**
         * Search fields info callback.
         */
        typedef std::function<void (uxmpp::Session& session,
                                    std::string& instructions,
                                    std::list<std::string>& fields)> fields_info_cb_t;

        /**
         * Default Constructor.
         */
        SearchModule ();

        /**
         * Destructor.
         */
        virtual ~SearchModule () = default;

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
        virtual bool proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj) override;

        /**
         * Request search fields.
         */
        bool request_fields ();

        /**
         *
         */
        void set_fields_info_callback (fields_info_cb_t callback);


    private:
        void handle_fields_query_result (IqStanza& iq);

        uxmpp::Session* sess;
        std::string query_id;
        fields_info_cb_t fields_info_cb;
    };



}}

#endif
