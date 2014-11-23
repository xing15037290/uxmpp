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
#ifndef UXMPP_PRESENCESTANZA_HPP
#define UXMPP_PRESENCESTANZA_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/Stanza.hpp>
#include <string>
#include <sstream>


namespace uxmpp {


    /**
     * Presence subscription operation.
     */
    enum class SubscribeOp {
        /**
         * A subscribe is a request from a user for authorization
         * to permanently subscribe to a contact's presence information.
         */
        subscribe,

        /**
         * An unsubscribe request is sent if a user would like to
         * unsubscribe from a contact's presence.
         */
        unsubscribe,

        /**
         * A client approves a subscription request by sending a presence stanza of type "subscribed".
         */
        subscribed,

        /**
         * A client sends 'unsubscribed' to cancel a subscription that it has
         * previously granted to a user, to cancel a subscription pre-approval,
         * or to deny a subscription request.
         */
        unsubscribed
    };


    /**
     * Return a string representation of the presence type.
     */
    std::string to_string (const SubscribeOp& type);


    /**
     * Presence stanza.
     */
    class PresenceStanza : public Stanza {
    public:
        /**
         * Constructor.
         */
        PresenceStanza (const std::string& to="",
                        const std::string& from="",
                        const std::string& id="")
            : Stanza (to, from, id)
        {
            set_tag_name ("presence");
        }

        /**
         * Constructor.
         */
        PresenceStanza (const Jid& to, const Jid& from, const std::string& id="")
            : Stanza (to, from, id)
        {
            set_tag_name ("presence");
        }

        /**
         * Copy constructor.
         */
        PresenceStanza (const PresenceStanza& msg_stanza) : Stanza (msg_stanza) {
        }

        /**
         * Move constructor.
         */
        PresenceStanza (PresenceStanza&& msg_stanza) : Stanza (msg_stanza) {
        }

        /**
         * Destructor.
         */
        virtual ~PresenceStanza () = default;

        /**
         * Assignment operator.
         */
        PresenceStanza& operator= (const PresenceStanza& msg_stanza) {
            if (this != &msg_stanza)
                Stanza::operator= (msg_stanza);
            return *this;
        }

        /**
         * Move operator.
         */
        PresenceStanza& operator= (PresenceStanza&& msg_stanza) {
            Stanza::operator= (msg_stanza);
            return *this;
        }

        /**
         * Return the subscription type.
         */
        SubscribeOp get_subscribe_op ();

        /**
         * Set the subscription type.
         */
        PresenceStanza& set_subscribe_op (const SubscribeOp& type);

        /**
         * Get the content of the 'show' element. It it is missing an empty string is returned.
         * A missing 'show' element means that the entity is assumed to be online and available.
         * Values for the content defined in RFC 6121, section 4.7.2.1:<br/>
         * * away -- The entity or resource is temporarily away.<br/>
         * * chat -- The entity or resource is actively interested in chatting.<br/>
         * * dnd -- The entity or resource is busy (dnd = "Do Not Disturb").<br/>
         * * xa -- The entity or resource is away for an extended period (xa = "eXtended Away").<br/>
         */
        std::string get_show ();

        /**
         * Set the content of the 'show' element.
         * It the new content is an empty string the 'show' element is removed.
         * A missing 'show' element means that the entity is assumed to be online and available.
         * Values for the content defined in RFC 6121, section 4.7.2.1:<br/>
         * * away -- The entity or resource is temporarily away.<br/>
         * * chat -- The entity or resource is actively interested in chatting.<br/>
         * * dnd -- The entity or resource is busy (dnd = "Do Not Disturb").<br/>
         * * xa -- The entity or resource is away for an extended period (xa = "eXtended Away").<br/>
         */
        PresenceStanza& set_show (const std::string& content);

        /**
         *
         */
        std::string get_status (const std::string& lang="");

        /**
         *
         */
        std::vector<std::pair<std::string, std::string> > get_status_list ();

        /**
         *
         */
        PresenceStanza& set_status (const std::string& status, const std::string& lang="");

        /**
         *
         */
        int get_priority ();

        /**
         *
         */
        PresenceStanza& set_priority (int prio);
    };


}


#endif
