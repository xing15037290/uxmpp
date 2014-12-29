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
#ifndef UXMPP_MOD_VCARDMODULE_HPP
#define UXMPP_MOD_VCARDMODULE_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/StanzaError.hpp>

#include <functional>
#include <string>
#include <set>


namespace uxmpp { namespace mod {


    /**
     * vCard-temp (XEP-0054).
     */
    class VcardModule : public uxmpp::XmppModule {
    public:

        /**
         * vCard result callback.
         */
        typedef std::function<void (uxmpp::Session& session,
                                    uxmpp::Jid& jid,
                                    uxmpp::XmlObject& vcard,
                                    uxmpp::StanzaError& error)> vcard_cb_t;

        /**
         * Default Constructor.
         */
        VcardModule ();

        /**
         * Destructor.
         */
        virtual ~VcardModule () = default;

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
         * Request vCard.
         */
        bool request_vcard ();

        /**
         * Request vCard.
         */
        bool request_vcard (const uxmpp::Jid& jid);

        /**
         *
         */
        void set_vcard_callback (vcard_cb_t callback);

        /**
         *
         */
        bool set_vcard (const uxmpp::XmlObject& vcard);


    private:
        uxmpp::Session* sess;
        std::set<std::string> query_ids;
        std::string set_id;
        vcard_cb_t vcard_cb;
    };



}}

#endif
