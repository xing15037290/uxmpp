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
#ifndef UXMPP_MOD_MESSAGEMODULE_HPP
#define UXMPP_MOD_MESSAGEMODULE_HPP

#include <string>
#include <vector>
#include <map>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/MessageStanza.hpp>


namespace uxmpp { namespace mod {


    /**
     * An XMPP presence module.
     * Supports XEP-0308 - Last Message Correction, XEP-0184 - Message Delivery Recepits.
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
         * Return a list of service discovery information features supported
         * by the module;
         */
        virtual std::vector<std::string> get_disco_features ();

        /**
         * Send a message.
         * @param msg The message stanza to send.
         * @param want_receipt If a recepipt is required as defined in XEP-0184.
         */
        virtual void send_message (const uxmpp::MessageStanza& msg, bool want_receipt=false);

        /**
         * Send a message.
         * @param to The receiver of the message.
         * @param body The message body.
         * @param want_receipt If a recepipt is required as defined in XEP-0184.
         */
        virtual void send_message (const uxmpp::Jid& to,
                                   const std::string& body,
                                   bool want_receipt=false,
                                   const std::string& lang="");

        /**
         * Set or disable support for Last Message Correction (XEP-0308).
         * It is supported by default.
         */
        void support_correction (bool supported);

        /**
         * After this call, corrections to this JID will fail until a normal message has been sent.
         * @param to A JID.
         */
        virtual void reset_correction (const uxmpp::Jid& to);

        /**
         * Try to send a correction for the last sent message.
         * A correction can't be sent if no previous message to the recipient has been sent.
         * @param body The corrected message body.
         * @return <code>true</code> if a correction was sent, <code>false</code> if not.
         */
        virtual bool correct_message (const uxmpp::Jid& to, const std::string& body);

        /**
         *
         */
        void set_message_handler (std::function<void (MessageModule&,
                                                      uxmpp::MessageStanza&,
                                                      bool corr,
                                                      const std::string& id)> on_message);

        /**
         *
         */
        void set_receipt_handler (std::function<void (MessageModule&, const uxmpp::Jid&, const std::string&)>
                                  on_receipt);

        /**
         * If true, a message receipt (XEP-0184) will always be sent even if the sender
         * if not authorized to view our presence (violating section 8 in XEP-0184).
         * Default is <code>false</code>.
         */
        bool always_send_receipt;


    protected:
        bool correction_supported;
        uxmpp::Session* sess;
        std::map<std::string, uxmpp::MessageStanza> correctable_messages;
        std::function<void (MessageModule&, uxmpp::MessageStanza&, bool corr, const std::string& id)> message_handler;
        std::function<void (MessageModule&, const uxmpp::Jid& from, const std::string& id)> receipt_handler;
    };


}}


#endif
