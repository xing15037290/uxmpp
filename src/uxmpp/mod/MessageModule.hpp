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
#ifndef UXMPP_MOD_MESSAGEMODULE_HPP
#define UXMPP_MOD_MESSAGEMODULE_HPP

#include <string>
#include <vector>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/mod/MessageModuleListener.hpp>
#include <uxmpp/Jid.hpp>


namespace uxmpp { namespace mod {


    /**
     * An XMPP presence module.
     */
    class MessageModule : public uxmpp::XmppModule {
    public:

        /**
         * Default Constructor.
         */
        MessageModule ();

        /**
         * Destructor.
         */
        virtual ~MessageModule () = default;

        /**
         * Add a listener object that will receive events from the module.
         */
        virtual void addMessageListener (MessageModuleListener& listener);

        /**
         * Remove a listener object that is receiving events from the module.
         */
        virtual void delMessageListener (MessageModuleListener& listener);

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
         * Return a list of service discovery information features supported
         * by the module;
         */
        virtual std::vector<std::string> getDiscoFeatures () {
            return {"urn:xmpp:receipts"};
        }

        /**
         * Send a message.
         * @param msg The message stanza to send.
         * @param want_receipt If a recepipt is required as defined in XEP-0184.
         */
        virtual void sendMessage (const MessageStanza& msg, bool want_receipt=false);

        /**
         * Send a message.
         * @param to The receiver of the message.
         * @param body The message body.
         * @param want_receipt If a recepipt is required as defined in XEP-0184.
         */
        virtual void sendMessage (const uxmpp::Jid& to, const std::string& body, bool want_receipt=false);


    protected:
        uxmpp::Session* sess;

        /**
         * A list of event listeners.
         */
        std::vector<MessageModuleListener*> listeners;
    };


}}


#endif
