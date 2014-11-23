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
#include <uxmpp/XmlNames.hpp>
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
        Stanza (const std::string& to="", const std::string& from="", const std::string& id="")
            : XmlObject ("", XmlJabberClientNs, false, true, 1)
        {
            if (!to.empty())
                set_attribute ("to", to);
            if (!from.empty())
                set_attribute ("from", from);
            // Set a random ID if none is provided
            set_id (id.empty() ? make_id() : id);
        }

        /**
         * Constructor.
         */
        Stanza (const Jid& to, const Jid& from, const std::string& id="")
            : XmlObject ("", XmlJabberClientNs, false, true, 1)
        {
            set_to (to);
            set_from (from);
            // Set a random ID if none is provided
            set_id (id.empty() ? make_id() : id);
        }

        /**
         * Copy constructor.
         */
        Stanza (const Stanza& stanza) : XmlObject (stanza) {
        }

        /**
         * Move constructor.
         */
        Stanza (Stanza&& stanza) : XmlObject (stanza) {
        }

        /**
         * Destructor.
         */
        virtual ~Stanza () = default;

        /**
         * Assignment operator.
         */
        Stanza& operator= (const Stanza& stanza) {
            if (this != &stanza)
                XmlObject::operator= (stanza);
            return *this;
        }

        /**
         * Move operator.
         */
        Stanza& operator= (Stanza&& stanza) {
            XmlObject::operator= (stanza);
            return *this;
        }

        /**
         * Get 'to'.
         */
        Jid get_to () const {
            return Jid (get_attribute("to"));
        }

        /**
         * Set 'to'.
         */
        void set_to (const Jid& to) {
            std::string to_str = to_string (to);
            if (to_str.length())
                set_attribute ("to", to_str);
        }

        /**
         * Get 'from'.
         */
        Jid get_from () const {
            return Jid (get_attribute("from"));
        }

        /**
         * Set 'from'.
         */
        void set_from (const Jid& from) {
            std::string from_str = to_string (from);
            if (from_str.length())
                set_attribute ("from", from_str);
        }

        /**
         * Get 'id'.
         */
        std::string get_id () const {
            return get_attribute ("id");
        }

        /**
         * Set 'id'.
         */
        void set_id (const std::string& id) {
            set_attribute ("id", id);
        }

        /**
         * Get 'type'.
         */
        std::string get_type () const {
            return get_attribute ("type");
        }

        /**
         * Set 'type'.
         */
        void set_type (const std::string& type) {
            set_attribute ("type", type);
        }

        /**
         * Check if an error is present.
         */
        bool have_error () const {
            return get_type() == "error";
        }

        /**
         * It the IQ type is 'error' this will return the error type.
         */
        std::string get_error_type () {
            if (!have_error())
                return "";
            for (XmlObject& node : get_nodes()) {
                if (node.get_full_name() == XmlIqErrorStanzaTagFull) {
                    return node.get_attribute ("type");
                }
            }
            return "";
        }

        /**
         * It the IQ type is 'error' this will return the error code.
         */
        int get_error_code () {
            if (!have_error())
                return 0;
            auto node = get_node (XmlIqErrorStanzaTagFull, true);
            return node ? atoi(node.get_attribute("code").c_str()) : 0;
        }

        /**
         * It the IQ type is 'error' this will return the error name.
         */
        std::string get_error_name () {
            if (!have_error())
                return "";
            auto node = get_node (XmlIqErrorStanzaTagFull, true);
            if (node) {
                auto child_nodes = node.get_nodes ();
                if (!child_nodes.empty())
                    return child_nodes.begin()->get_tag_name ();
                else
                    return "";
            }
            return "";
        }

        /**
         * Generate a pseudo random id string.
         * This method is used to generate a random ID string for Stanza's.
         */
        static std::function<std::string (void)> make_id;
    };


}


#endif
