/*
 *  Copyright (C) 2013-2015 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_STANZA_HPP
#define UXMPP_STANZA_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/StanzaError.hpp>
#include <string>
#include <cstdlib>
#include <functional>


namespace uxmpp {


    /**
     * Base class for xmpp stanzas.
     */
    class Stanza : public XmlObject {
    public:
        /**
         * Constructor.
         */
        Stanza (const std::string& to="", const std::string& from="", const std::string& id="");

        /**
         * Constructor.
         */
        Stanza (const Jid& to, const Jid& from, const std::string& id="");

        /**
         * Copy constructor.
         */
        Stanza (const Stanza& stanza);

        /**
         * Move constructor.
         */
        Stanza (Stanza&& stanza);

        /**
         * Destructor.
         */
        virtual ~Stanza () = default;

        /**
         * Assignment operator.
         */
        Stanza& operator= (const Stanza& stanza);

        /**
         * Move operator.
         */
        Stanza& operator= (Stanza&& stanza);

        /**
         * Get 'to'.
         */
        Jid get_to () const;

        /**
         * Set 'to'.
         */
        void set_to (const Jid& to);

        /**
         * Get 'from'.
         */
        Jid get_from () const;

        /**
         * Set 'from'.
         */
        void set_from (const Jid& from);

        /**
         * Get 'id'.
         */
        std::string get_id () const;

        /**
         * Set 'id'.
         */
        void set_id (const std::string& id);

        /**
         * Get 'type'.
         */
        std::string get_type () const;

        /**
         * Set 'type'.
         */
        void set_type (const std::string& type);

        /**
         * Check if an error is present.
         */
        bool have_error () const;

        /**
         * Get the stanza error (if any).
         */
        StanzaError get_error ();

        /**
         * Set a stanza error.
         */
        void set_error (const StanzaError& error);

        /**
         * Set a delay child node as described in XEP-0203.
         * To remove the delay node, set parameter 'stamp' to an empty string.
         * @param from The Jabber ID of the entity that originally
         *             sent the XML stanza or that delayed the
         *             delivery of the stanza.
         * @param stamp The time stamp that the stanza was originally
         *              sent. The timstamp must be of the DateTime
         *              format as described in XEP-0082.
         *              If an empty string, the delay node is removed.
         * @param reason Optional reason of the delay.
         */
        void set_delay (const std::string& from, const std::string& stamp, const std::string& reason="");

        /**
         * If the stanza is delayed, get the Jabber ID that originally
         * sent the stanza as described in XEP-0203.
         * @return A Jabber ID or an empty string.
         */
        std::string get_delay_from ();

        /**
         * If the stanza is delayed, get the time stamp of the
         * originally sent stanza as described in XEP-0203.
         * @return A time stamp or an empty string.
         */
        std::string get_delay_stamp ();

        /**
         * If the stanza is delayed, get the optional
         * delay reason of the originally sent stanza
         * as described in XEP-0203.
         * @return A delay message or an empty string.
         */
        std::string get_delay_reason ();

        /**
         * Generate a pseudo random id string.
         * This method is used to generate a random ID string for Stanza's.
         */
        static std::function<std::string (void)> make_id;
    };


}


#endif
