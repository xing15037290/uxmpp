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
#ifndef UXMPP_STANZAERROR_HPP
#define UXMPP_STANZAERROR_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <string>


namespace uxmpp {


    /**
     * Stanza error tag.
     */
    class StanzaError : public XmlObject {
    public:
        /**
         * Constructor.
         */
        StanzaError (const std::string& type, const std::string& condition, const std::string& text="");

        /**
         * Destructor.
         */
        virtual ~StanzaError () = default;

        /**
         * Return the error type.
         */
        std::string get_type ();

        /**
         * Set the error type.
         */
        StanzaError& set_type (const std::string& type);

        /**
         * Return the 'by' attribute.
         */
        std::string get_by ();

        /**
         * Set the 'by' attribute.
         */
        StanzaError& set_by (const std::string& by);

        /**
         * It the IQ type is 'error' this will return the error code.
         */
        //int get_error_code ();

        /**
         * Return the error condition.
         */
        std::string get_condition ();

        /**
         * Set the error condition.
         */
        StanzaError& set_condition (const std::string& condition);

        /**
         * Return the error text.
         */
        std::string get_text ();

        /**
         * Set the error text.
         */
        StanzaError& set_text (const std::string& text);


        /**
         * Stanza error type 'auth' - retry after providing credentials.
         * RFC-6120 section 8.3.2.
         */
        static const std::string type_auth;

        /**
         * Stanza error type 'cancel' - do not retry (the error cannot be remedied).
         * RFC-6120 section 8.3.2.
         */
        static const std::string type_cancel;

        /**
         * Stanza error type 'continue' - proceed (the condition was only a warning).
         * RFC-6120 section 8.3.2.
         */
        static const std::string type_continue;

        /**
         * Stanza error type 'modify' - retry after changing the data sent.
         * RFC-6120 section 8.3.2.
         */
        static const std::string type_modify;

        /**
         * Stanza error type 'wait' - retry after waiting (the error is temporary).
         * RFC-6120 section 8.3.2.
         */
        static const std::string type_wait;

        /**
         * Stanza error condition 'bad-request'.
         * The sender has sent a stanza containing XML that does not conform
         * to the appropriate schema or that cannot be processed.
         * RFC-6120 section 8.3.3.1.
         */
        static const std::string bad_request;

        /**
         * Stanza error condition 'conflict'.
         * Access cannot be granted because an existing resource
         * exists with the same name or address.
         * RFC-6120 section 8.3.3.2.
         */
        static const std::string conflict;

        /**
         * Stanza error condition 'feature-not-implemented'.
         * The feature represented in the XML stanza is not implemented.
         * RFC-6120 section 8.3.3.3.
         */
        static const std::string feature_not_implemented;

        /**
         * Stanza error condition 'forbidden'.
         * The requesting entity does not possess the necessary permissions.
         * RFC-6120 section 8.3.3.4.
         */
        static const std::string forbidden;

        /**
         * Stanza error condition 'gone'.
         * The recipient or server can no longer be contacted at this address.
         * RFC-6120 section 8.3.3.5.
         */
        static const std::string gone;

        /**
         * Stanza error condition 'internal-server-error'.
         * The server has experienced a misconfiguration or other internal error.
         * RFC-6120 section 8.3.3.6.
         */
        static const std::string internal_server_error;

        /**
         * Stanza error condition 'item-not-found'.
         * The addressed JID or item requested cannot be found.
         * RFC-6120 section 8.3.3.7.
         */
        static const std::string item_not_found;

        /**
         * Stanza error condition 'jid-malformed'.
         * The sending entity has provided an invalid XMPP address.
         * RFC-6120 section 8.3.3.8.
         */
        static const std::string jid_malformed;

        /**
         * Stanza error condition 'not-acceptable'.
         * The recipient or server understands the request but cannot process it.
         * RFC-6120 section 8.3.3.9.
         */
        static const std::string not_acceptable;

        /**
         * Stanza error condition 'not-allowed'.
         * The recipient or server does not allow any entity to perform the action.
         * RFC-6120 section 8.3.3.10.
         */
        static const std::string not_allowed;

        /**
         * Stanza error condition 'not-authorized'.
         * The sender needs to provide credentials before being allowed to perform the action.
         * RFC-6120 section 8.3.3.11.
         */
        static const std::string not_authorized;

        /**
         * Stanza error condition 'policy-violation'.
         * The entity has violated some local service policy.
         * RFC-6120 section 8.3.3.12.
         */
        static const std::string policy_violation;

        /**
         * Stanza error condition 'recipient-unavailable'.
         * The intended recipient is temporarily unavailable.
         * RFC-6120 section 8.3.3.13.
         */
        static const std::string recipient_unavailable;

        /**
         * Stanza error condition 'redirect'.
         * The recipient or server is redirecting requests for
         * this information to another entity.
         * RFC-6120 section 8.3.3.14.
         */
        static const std::string redirect;

        /**
         * Stanza error condition 'registration-required'.
         * The requesting entity is not authorized to access the requested
         * service because prior registration is necessary.
         * RFC-6120 section 8.3.3.15.
         */
        static const std::string registration_required;

        /**
         * Stanza error condition 'remote-server-not-found'.
         * A remote server or service specified as part or all of the JID
         * of the intended recipient does not exist or cannot be resolved.
         * RFC-6120 section 8.3.3.16
         */
        static const std::string remote_server_not_found;

        /**
         * Stanza error condition 'remote-server-timeout'.
         * Communication with a remote server could not be
         * established within a reasonable amount of time.
         * RFC-6120 section 8.3.3.17.
         */
        static const std::string remote_server_timeout;

        /**
         * Stanza error condition 'resource-constraint'.
         * The server or recipient is busy or lacks the system resources necessary.
         * RFC-6120 section 8.3.3.18.
         */
        static const std::string resource_constraint;

        /**
         * Stanza error condition 'service-unavailable'.
         * The server or recipient does not currently provide the requested service.
         * RFC-6120 section 8.3.3.19.
         */
        static const std::string service_unavailable;

        /**
         * Stanza error condition 'subscription-required'.
         * The requesting entity is not authorized to access the requested
         * service because a prior subscription is necessary.
         * RFC-6120 section 8.3.3.20.
         */
        static const std::string subscription_required;

        /**
         * Stanza error condition 'undefined-condition'.
         * The error condition is not one of those defined by the other conditions in this list.
         * RFC-6120 section 8.3.3.21.
         */
        static const std::string undefined_condition;

        /**
         * Stanza error condition 'unexpected-request'.
         * The recipient or server understood the request but was not expecting it at this time.
         * RFC-6120 section 8.3.3.22.
         */
        static const std::string unexpected_request;
    };


}


#endif
