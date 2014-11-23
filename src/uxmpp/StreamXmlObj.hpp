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
#ifndef UXMPP_STREAMXMLOBJ_HPP
#define UXMPP_STREAMXMLOBJ_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/XmlNames.hpp>


namespace uxmpp {


    /**
     * Stream stanza.
     */
    class StreamXmlObj : public XmlObject {
    public:

        /**
         * Default Constructor.
         */
        StreamXmlObj () : XmlObject (XmlStreamTag, XmlStreamNsAlias, false, false) {
            add_namespace_alias (XmlStreamNsAlias, XmlStreamNs);
            set_default_namespace_attr (XmlJabberClientNs);
            set_attribute ("version", "1.0");
            set_attribute ("xml:lang", "en");
            set_part (XmlObjPart::start);
        }

        /**
         * Constructor.
         */
        StreamXmlObj (const std::string& to, const std::string& from)
            : XmlObject (XmlStreamTag, XmlStreamNsAlias, false, false)
        {
            add_namespace_alias (XmlStreamNsAlias, XmlStreamNs);
            set_default_namespace_attr (XmlJabberClientNs);
            set_attribute ("version", "1.0");
            set_attribute ("xml:lang", "en");
            set_part (XmlObjPart::start);

            set_to (to);
            set_from (from);
        }

        /**
         * Destructor.
         */
        virtual ~StreamXmlObj () = default;

        /**
         *
         */
        void set_to (const std::string& to) {
            set_attribute ("to", to);
        }

        /**
         *
         */
        std::string get_to () const {
            return get_attribute ("to");
        }

        /**
         *
         */
        void set_id (const std::string& id) {
            set_attribute ("id", id);
        }

        /**
         *
         */
        std::string get_id () const {
            return get_attribute ("id");
        }

        /**
         *
         */
        void set_from (const std::string& from) {
            set_attribute ("from", from);
        }

        /**
         *
         */
        std::string get_from () const {
            return get_attribute ("from");
        }

    };


}


#endif
