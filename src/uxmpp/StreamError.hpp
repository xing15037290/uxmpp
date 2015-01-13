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
#ifndef UXMPP_STREAMERROR_HPP
#define UXMPP_STREAMERROR_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <string>


namespace uxmpp {


    /**
     * Stream error object.
     */
    class StreamError : public XmlObject {
    public:

        /**
         * Default Constructor.
         */
        StreamError ();

        /**
         * Construct a stream error from an XmlObject.
         */
        StreamError (XmlObject& xml_obj);

        /**
         * Constructor.
         */
        StreamError (const std::string& error);

        /**
         * Destructor.
         */
        virtual ~StreamError () = default;

        /**
         * Assignment operator.
         */
        StreamError& operator= (const XmlObject& xml_obj);

        /**
         * Is there an error message available.
         */
        bool have_error ();

        /**
         * Set the error condition.
         */
        void set_error_name (const std::string& error);

        /**
         * Return the error condition.
         */
        std::string get_error_name ();

        /**
         * Set the application specific condition.
         */
        void set_app_error (const std::string& app_error, const std::string& text="");

        /**
         * Return the application specific error condition, if any.
         */
        std::string get_app_error ();

        /**
         * Return a descriptive error text (if any).
         */
        std::string get_text ();

        /**
         * Set a descriptive error text.
         */
        void set_text (const std::string& text);

        /**
         * The entity has sent XML that cannot be processed.
         * RFC 6120 section 4.9.3.1.
         */
        static const std::string bad_format;

        /**
         * The entity has sent a namespace prefix that is unsupported, or is missing a namespace.
         * RFC 6120 section 4.9.3.2.
         */
        static const std::string bad_namespace_prefix;

        /**
         * This stream conflicts with another stream.
         * RFC 6120 section 4.9.3.3.
         */
        static const std::string conflict;

        /**
         * The stream is disconnected since one party believes the other is lost.
         * RFC 6120 section 4.9.3.4.
         */
        static const std::string connection_timeout;

        /**
         * The value of the 'to' attribute provided in the initial stream header
         * corresponds to an FQDN that is no longer serviced by the receiving entity. 
         * RFC 6120 section 4.9.3.5.
         */
        static const std::string host_gone;

        /**
         * The value of the 'to' attribute provided in the initial stream header
         * does not correspond to an FQDN that is serviced by the receiving entity. 
         * RFC 6120 section 4.9.3.6.
         */
        static const std::string host_unknown;

        /**
         * A stanza sent between two servers lacks a 'to' or 'from' attribute,
         * the 'from' or 'to' attribute has no value, or the value violates
         * the rules for XMPP addresses.
         * RFC 6120 section 4.9.3.7.
         */
        static const std::string improper_addressing;

        /**
         * The server has experienced a misconfiguration or other internal
         * error that prevents it from servicing the stream. 
         * RFC 6120 section 4.9.3.8.
         */
        static const std::string internal_server_error;

        /**
         * The data provided in a 'from' attribute does not match
         * an authorized JID or validated domain.
         * RFC 6120 section 4.9.3.9.
         */
        static const std::string invalid_from;

        /**
         * The stream namespace name is something other than "http://etherx.jabber.org/streams"
         * or the content namespace declared as the default namespace is not
         * supported (e.g., something other than "jabber:client" or "jabber:server"). 
         * RFC 6120 section 4.9.3.10.
         */
        static const std::string invalid_namespace;

        /**
         * The entity has sent invalid XML over the stream to a server that performs validation.
         * RFC 6120 section 4.9.3.11.
         */
        static const std::string invalid_xml;

        /**
         * The entity has attempted to send XML stanzas or other outbound
         * data before the stream has been authenticated.
         * RFC 6120 section 4.9.3.12.
         */
        static const std::string not_authorized;

        /**
         * The initiating entity has sent XML that violates the well-formedness rules.
         * RFC 6120 section 4.9.3.13.
         */
        static const std::string not_well_formed;

        /**
         * The entity has violated some local service policy
         * (e.g., a stanza exceeds a configured size limit).
         * RFC 6120 section 4.9.3.14.
         */
        static const std::string policy_violation;

        /**
         * The server is unable to properly connect to a remote entity
         * that is needed for authentication or authorization.
         * RFC 6120 section 4.9.3.15.
         */
        static const std::string remote_connection_failed;

        /**
         * The server is closing the stream because it has new
         * (typically security-critical) features to offer.
         * RFC 6120 section 4.9.3.16.
         */
        static const std::string reset;

        /**
         * The server lacks the system resources necessary to service the stream.
         * RFC 6120 section 4.9.3.17.
         */
        static const std::string resource_constraint;

        /**
         * The entity has attempted to send restricted XML features such as a comment,
         * processing instruction, DTD subset, or XML entity reference.
         * RFC 6120 section 4.9.3.18.
         */
        static const std::string restricted_xml;

        /**
         * The server will not provide service to the initiating entity but is
         * redirecting traffic to another host under the administrative control
         * of the same service provider.
         * RFC 6120 section 4.9.3.19.
         */
        static const std::string see_other_host;

        /**
         * The server is being shut down and all active streams are being closed.
         * RFC 6120 section 4.9.3.20.
         */
        static const std::string system_shutdown;

        /**
         * The error condition is not one of those defined by the other conditions
         * in this list; this error condition SHOULD NOT be used except in
         * conjunction with an application-specific condition. 
         * RFC 6120 section 4.9.3.21.
         */
        static const std::string undefined_condition;

        /**
         * The initiating entity has encoded the stream in an
         * encoding that is not supported by the server.
         * RFC 6120 section 4.9.3.22.
         */
        static const std::string unsupported_encoding;

        /**
         * The receiving entity has advertised a mandatory-to-negotiate
         * stream feature that the initiating entity does not support.
         * RFC 6120 section 4.9.3.23.
         */
        static const std::string unsupported_feature;

        /**
         * The initiating entity has sent a first-level child
         * of the stream that is not supported by the server.
         * RFC 6120 section 4.9.3.24.
         */
        static const std::string unsupported_stanza_type;

        /**
         * The 'version' attribute provided by the initiating entity
         * in the stream header specifies a version of XMPP that is
         * not supported by the server.
         * RFC 6120 section 4.9.3.25.
         */
        static const std::string unsupported_version;
    };


}


#endif
