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
#ifndef UXMPP_XMLSTREAMLISTENER_HPP
#define UXMPP_XMLSTREAMLISTENER_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>


namespace uxmpp {


    // Forward declaration.
    class XmlStream;


    /**
     * An XML stream listener.
     */
    class XmlStreamListener {
    public:

        /**
         * Constructor.
         */
        XmlStreamListener () = default;

        /**
         * Destructor.
         */
        virtual ~XmlStreamListener () = default;

        /**
         * Called when the stream is opened.
         */
        virtual void onOpen (XmlStream& stream) {
        }

        /**
         * Called when the stream is closed.
         */
        virtual void onClose (XmlStream& stream) {
        }

        /**
         * Called whan an XML object is received.
         */
        virtual void onRxXmlObj (XmlStream& stream, XmlObject& xml_obj) = 0;

        /**
         * Called whan an XML object is received.
         */
        virtual void onRxXmlError (XmlStream& stream) = 0;
    };


}


#endif
