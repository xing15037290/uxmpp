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
#ifndef UXMPP_STANZA_HPP
#define UXMPP_STANZA_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/Jid.hpp>
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
         * It the IQ type is 'error' this will return the error type.
         */
        std::string get_error_type ();

        /**
         * It the IQ type is 'error' this will return the error code.
         */
        int get_error_code ();

        /**
         * It the IQ type is 'error' this will return the error name.
         */
        std::string get_error_name ();

        /**
         * Generate a pseudo random id string.
         * This method is used to generate a random ID string for Stanza's.
         */
        static std::function<std::string (void)> make_id;
    };


}


#endif
