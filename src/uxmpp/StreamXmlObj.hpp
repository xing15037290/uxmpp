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
            addNamespaceAlias (XmlStreamNsAlias, XmlStreamNs);
            setDefaultNamespaceAttr (XmlJabberClientNs);
            setAttribute ("version", "1.0");
            setAttribute ("xml:lang", "en");
            setPart (XmlObjPart::start);
        }

        /**
         * Constructor.
         */
        StreamXmlObj (const std::string& to, const std::string& from)
            : XmlObject (XmlStreamTag, XmlStreamNsAlias, false, false)
        {
            addNamespaceAlias (XmlStreamNsAlias, XmlStreamNs);
            setDefaultNamespaceAttr (XmlJabberClientNs);
            setAttribute ("version", "1.0");
            setAttribute ("xml:lang", "en");
            setPart (XmlObjPart::start);

            setTo (to);
            setFrom (from);
        }

        /**
         * Destructor.
         */
        virtual ~StreamXmlObj () = default;

        /**
         *
         */
        void setTo (const std::string& to) {
            setAttribute ("to", to);
        }

        /**
         *
         */
        std::string getTo () const {
            return getAttribute ("to");
        }

        /**
         *
         */
        void setId (const std::string& id) {
            setAttribute ("id", id);
        }

        /**
         *
         */
        std::string getId () const {
            return getAttribute ("id");
        }

        /**
         *
         */
        void setFrom (const std::string& from) {
            setAttribute ("from", from);
        }

        /**
         *
         */
        std::string getFrom () const {
            return getAttribute ("from");
        }

    };


}


#endif
