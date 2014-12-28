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
#ifndef UXMPP_XML_NAMES_HPP
#define UXMPP_XML_NAMES_HPP

#include <uxmpp/types.hpp>
#include <string>


namespace uxmpp { namespace xml {

        /**
         * XML namespace for uxmpp internal errors:
         * <code>http://ultramarin.se/uxmpp#internal-error</code>.
         */
        extern const std::string namespace_uxmpp_error;

        /**
         * XML namespace for uxmpp internal timers.
         * <code>http://ultramarin.se/uxmpp#internal-timer</code>.
         */
        extern const std::string namespace_uxmpp_timer;

        /**
         * XML tag name for uxmpp internal timers.
         * <code>http://ultramarin.se/uxmpp#internal-timer:timeout</code>.
         */
        extern const std::string full_tag_uxmpp_timeout;

        /**
         * XML namespace for the 'stream' tag: <code>http://etherx.jabber.org/streams</code>.
         */
        extern const std::string namespace_stream;
        /**
         * XML alias for the stream namespace: <code>stream</code>.
         */
        extern const std::string alias_stream;
        /**
         * XML tag name for the stream tag: <code>stream</code>.
         */
        extern const std::string tag_stream;
        /**
         * Fully qualified XML tag name for the stream tag:
         * <code>http://etherx.jabber.org/streams:stream</code>.
         */
        extern const std::string full_tag_stream;


        /**
         * XML tag name for the features tag: <code>features</code>.
         */
        extern const std::string tag_features;
        /**
         * Fully qualified XML tag name for the features tag:
         * <code>http://etherx.jabber.org/streams:features</code>.
         */
        extern const std::string full_tag_features;


        /**
         * XML jabber client namespace: <code>jabber:client</code>.
         */
        extern const std::string namespace_jabber_client;


        /**
         * XML bind namespace: <code>urn:ietf:params:xml:ns:xmpp-bind</code>.
         */
        extern const std::string namespace_bind;
        /**
         * XML bind tag: <code>bind</code>.
         */
        extern const std::string tag_bind;


        /**
         * XMPP-streams namespace: <code>urn:ietf:params:xml:ns:xmpp-streams</code>.
         */
        extern const std::string namespace_xmpp_streams;


        /**
         * Fully qualified XML tag name for the error tag:
         * <code>http://etherx.jabber.org/streams:error</code>.
         */
        extern const std::string full_tag_error;


        /**
         * Fully qualified XML tag name for the IQ stanza tag:
         * <code>jabber:client:iq</code>.
         */
        extern const std::string full_tag_iq_stanza;


        /**
         * Fully qualified XML tag name for the error stanza tag:
         * <code>jabber:client:error</code>.
         */
        extern const std::string full_tag_error_stanza;


        /**
         * Fully qualified XML tag name for the presence stanza tag:
         * <code>jabber:client:presence</code>.
         */
        extern const std::string full_tag_presence_stanza;


        /**
         * Fully qualified XML tag name for the message stanza tag:
         * <code>jabber:client:message</code>.
         */
        extern const std::string full_tag_message_stanza;

        /**
         * XML Namespace Name for Stanza Errors:
         * <code>urn:ietf:params:xml:ns:xmpp-stanzas</code>.
         */
        extern const std::string namespace_stanza_error;


        /**
         * IQ roster namespace: <code>jabber:iq:roster</code>.
         */
        extern const std::string namespace_iq_roster;

        /**
         * Fully qualified XML tag name for the roster item tag:
         * <code>jabber:iq:roster:item</code>.
         */
        extern const std::string full_tag_roster_item;

        /**
         * Fully qualified XML tag name for the roster group tag:
         * <code>jabber:iq:roster:group</code>.
         */
        extern const std::string full_tag_roster_group;

}}


#endif
