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
#include <uxmpp/mod/PresenceModuleListener.hpp>


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
         * Add a listener object that will receive events from the module.
         */
        virtual void addPresenceListener (PresenceModuleListener& listener);

        /**
         * Remove a listener object that is receiving events from the module.
         */
        virtual void delPresenceListener (PresenceModuleListener& listener);

        /**
         * Called when the module is registered to a session.
         */
        virtual void moduleRegistered (uxmpp::Session& session);

        /**
         * Called when the module is unregistered from a session.
         */
        virtual void moduleUnregistered (uxmpp::Session& session);

        /**
         * Called whan an XML object is received.
         * @return Return true if this XML object was processed and no further work should be done.
         */
        virtual bool proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj);

        /**
         * Announce our presence.
         */
        void announce ();

        /**
         * Request to subscribe to a JID's presence information.
         * This vill send a presence stanza with the type 'subscribe'.
         */
        virtual void requestSubscription (const uxmpp::Jid& jid);

        /**
         * Accept a subscription from a JID's.
         * This vill send a presence stanza with the type 'subscribed'.
         */
        virtual void acceptSubscription (const uxmpp::Jid& jid);

        /**
         * Deny a subscription from a JID's.
         * This vill send a presence stanza with the type 'unsubscribed'.
         */
        virtual void denySubscription (const uxmpp::Jid& jid);

        /**
         * Cancel a subscription from a JID's.
         * This vill send a presence stanza with the type 'unsubscribe'.
         */
        virtual void cancelSubscription (const uxmpp::Jid& jid);


    protected:
        uxmpp::Session* sess;

        /**
         * A list of event listeners.
         */
        std::vector<PresenceModuleListener*> listeners;
    };


}}


#endif
