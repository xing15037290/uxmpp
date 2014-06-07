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
#ifndef UXMPP_MESSAGESTANZA_HPP
#define UXMPP_MESSAGESTANZA_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/Stanza.hpp>
#include <string>


namespace uxmpp {


    /**
     * Message stanza type.
     */
    enum class MessageType {
        /**
         * A one-on-one chat session.
         */
        chat,

        /**
         * Message error.
         */
        error,

        /**
         * Multi user chat.
         */
        groupchat,

        /**
         * Alert, notification, or other transient information to which no answer is expected.
         */
        headline,

        /**
         * Standalone message sent outside a one-on-one conversation or groupchat.
         * This is the default type if no 'type' attribute is set.
         */
        normal,
    };


    /**
     * Chat state as defined in XEP-0085.
     */
    enum class ChatState {
        /**
         * No chat state is available. User does not support XEP-0085.
         */
        none,

        /**
         * User is actively participating in the chat session.
         */
        active,

        /**
         * User is composing a message.
         */
        composing,

        /**
         * User had been composing but now has stopped.
         */
        paused,

        /**
         * User has not been actively participating in the chat session.
         */
        inactive,

        /**
         * User has effectively ended their participation in the chat session.
         */
        gone,
    };


    /**
     * Return a string representation of a message type.
     */
    std::string to_string (const MessageType& type);


    /**
     * Return a string representation of a chat state.
     */
    std::string to_string (const ChatState& state);


    /**
     * Message stanza.
     */
    class MessageStanza : public Stanza {
    public:
        /**
         * Constructor.
         */
        MessageStanza (const std::string& to="",
                       const std::string& from="",
                       const std::string& body="",
                       const MessageType  type=MessageType::normal,
                       const ChatState    chat_state=ChatState::none,
                       const std::string& id="",
                       const std::string& lang="")
            : Stanza (to, from, id)
        {
            setName ("message");
            setMessageType (type);
            setChatState (chat_state);
            setBody (body, lang);
        }

        /**
         * Constructor.
         */
        MessageStanza (const Jid&         to,
                       const Jid&         from,
                       const std::string& body="",
                       const MessageType  type=MessageType::normal,
                       const ChatState    chat_state=ChatState::none,
                       const std::string& id="",
                       const std::string& lang="")
            : Stanza (to, from, id)
        {
            setName ("message");
            setMessageType (type);
            setChatState (chat_state);
            setBody (body, lang);
        }

        /**
         * Copy constructor.
         */
        MessageStanza (const MessageStanza& msg_stanza) : Stanza (msg_stanza) {
        }

        /**
         * Move constructor.
         */
        MessageStanza (MessageStanza&& msg_stanza) : Stanza (msg_stanza) {
        }

        /**
         * Destructor.
         */
        virtual ~MessageStanza () = default;

        /**
         * Assignment operator.
         */
        MessageStanza& operator= (const MessageStanza& msg_stanza) {
            if (this != &msg_stanza)
                Stanza::operator= (msg_stanza);
            return *this;
        }

        /**
         * Move operator.
         */
        MessageStanza& operator= (MessageStanza&& msg_stanza) {
            Stanza::operator= (msg_stanza);
            return *this;
        }

        /**
         * Return the message type.
         */
        MessageType getMessageType ();

        /**
         * Set the message type.
         */
        MessageStanza& setMessageType (const MessageType type);

        /**
         * Return the message thread id.
         */
        std::string getThread ();

        /**
         * Set the message thread id.
         */
        MessageStanza& setThread (const std::string& thread_id, const std::string& parent_thread_id="");

        /**
         * Return the message parent thread id.
         */
        std::string getParentThread ();

        /**
         * Set the message parent thread id.
         */
        MessageStanza& setParentThread (const std::string& parent_thread_id);

        /**
         * Return the message body.
         */
        std::string getBody (std::string lang="");

        /**
         * Set the message body.
         */
        MessageStanza& setBody (const std::string& body, std::string lang="");

        /**
         * Return the chat state.
         */
        ChatState getChatState ();

        /**
         * Return the chat state.
         */
        MessageStanza& setChatState (const ChatState state);
    };


}


#endif
