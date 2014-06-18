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
            if (to.length())
                setAttribute ("to", to);
            if (from.length())
                setAttribute ("from", from);
            // Set a random ID if none is provided
            setId (id.length() ? id : makeId());
        }

        /**
         * Constructor.
         */
        Stanza (const Jid& to, const Jid& from, const std::string& id="")
            : XmlObject ("", XmlJabberClientNs, false, true, 1)
        {
            setTo (to);
            setFrom (from);
            // Set a random ID if none is provided
            setId (id.length() ? id : makeId());
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
        Jid getTo () const {
            return Jid (getAttribute("to"));
        }

        /**
         * Set 'to'.
         */
        void setTo (const Jid& to) {
            std::string to_str = to_string (to);
            if (to_str.length())
                setAttribute ("to", to_str);
        }

        /**
         * Get 'from'.
         */
        Jid getFrom () const {
            return Jid (getAttribute("from"));
        }

        /**
         * Set 'from'.
         */
        void setFrom (const Jid& from) {
            std::string from_str = to_string (from);
            if (from_str.length())
                setAttribute ("from", from_str);
        }

        /**
         * Get 'id'.
         */
        std::string getId () const {
            return getAttribute ("id");
        }

        /**
         * Set 'id'.
         */
        void setId (const std::string& id) {
            setAttribute ("id", id);
        }

        /**
         * Get 'type'.
         */
        std::string getType () const {
            return getAttribute ("type");
        }

        /**
         * Set 'type'.
         */
        void setType (const std::string& type) {
            setAttribute ("type", type);
        }

        /**
         * Check if an error is present.
         */
        bool haveError () const {
            return getType() == "error";
        }

        /**
         * It the IQ type is 'error' this will return the error type.
         */
        std::string getErrorType () {
            if (!haveError())
                return "";
            for (XmlObject& node : getNodes()) {
                if (node.getFullName() == XmlIqErrorStanzaTagFull) {
                    return node.getAttribute ("type");
                }
            }
            return "";
        }

        /**
         * It the IQ type is 'error' this will return the error code.
         */
        int getErrorCode () {
            if (!haveError())
                return 0;
            auto node = getNode (XmlIqErrorStanzaTagFull, true);
            return node ? atoi(node.getAttribute("code").c_str()) : 0;
        }

        /**
         * It the IQ type is 'error' this will return the error name.
         */
        std::string getErrorName () {
            if (!haveError())
                return "";
            auto node = getNode (XmlIqErrorStanzaTagFull, true);
            if (node) {
                auto child_nodes = node.getNodes ();
                if (child_nodes.size() > 0)
                    return child_nodes[0].getTagName ();
                else
                    return "";
            }
            return "";
        }

        /**
         * Generate a pseudo random id string.
         * This method is used to generate a random ID string for Stanza's.
         */
        static std::function<std::string (void)> makeId;
    };


}


#endif
