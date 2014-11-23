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
#ifndef UXMPP_MOD_PRESENCEMODULE_HPP
#define UXMPP_MOD_PRESENCEMODULE_HPP

#include <string>
#include <vector>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/PresenceStanza.hpp>


namespace uxmpp { namespace mod {


    /**
     * An XMPP presence module.
     */
    class PresenceModule : public uxmpp::XmppModule {
    public:

        /**
         * Default Constructor.
         */
        PresenceModule ();

        /**
         * Destructor.
         */
        virtual ~PresenceModule () = default;

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
         * Announce our presence.
         */
        void announce ();

        /**
         * Request to subscribe to a JID's presence information.
         * This vill send a presence stanza with the type 'subscribe'.
         */
        virtual void request_subscription (const uxmpp::Jid& jid);

        /**
         * Accept a subscription from a JID's.
         * This vill send a presence stanza with the type 'subscribed'.
         */
        virtual void accept_subscription (const uxmpp::Jid& jid);

        /**
         * Deny a subscription from a JID's.
         * This vill send a presence stanza with the type 'unsubscribed'.
         */
        virtual void deny_subscription (const uxmpp::Jid& jid);

        /**
         * Cancel a subscription from a JID's.
         * This vill send a presence stanza with the type 'unsubscribe'.
         */
        virtual void cancel_subscription (const uxmpp::Jid& jid);

        /**
         *
         */
        void set_presence_handler (std::function<void (PresenceModule&, uxmpp::PresenceStanza&)> on_presence);


    protected:
        uxmpp::Session* sess;
        std::function<void (PresenceModule&, uxmpp::PresenceStanza&)> presence_handler;
    };


}}


#endif
