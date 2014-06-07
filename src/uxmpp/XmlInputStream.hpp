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
#ifndef UXMPP_XMLINPUTSTREAM_HPP
#define UXMPP_XMLINPUTSTREAM_HPP

#include <uxmpp/types.hpp>
#include <XmlObject.hpp>
#include <mutex>


namespace uxmpp {


    /**
     * An XML input stream.
     */
    class XmlInputStream {
    public:

        /**
         * Constructor.
         * @param top_element An XML object representing the top-level XML tag
         *                    defining the start of the XML document.
         */
        XmlInputStream (const XmlObject& top_element);

        /**
         * Destructor.
         */
        virtual ~XmlInputStream ();

        /**
         * Set the XML object input handler.
         */
        void setXmlHandler (std::function<void (XmlInputStream&, XmlObject&)> xml_handler);

        /**
         * Set the XML error handler.
         */
        void setErrorHandler (std::function<void (XmlInputStream&)> err_handler);

        /**
         * Reset the stream.
         * This will reset the XML parser to the same state
         * as when the stream was originally created.
         */
        void reset ();

        /**
         * Add a character to be parsed.
         */
        XmlInputStream& operator<< (const char ch);

        /**
         * Add a string to be parsed.
         */
        XmlInputStream& operator<< (const std::string& input);

        /**
         * Insert an XML object in the stream.
         */
        XmlInputStream& operator<< (const XmlObject& xml_obj);


    private:

        /**
         * Opaque class to store temporary XML parsing state.
         */
        class XmlParseData;

        /**
         * Free allocated parsing resources.
         */
        void freeResources ();

        /**
         * Callback for incoming XML object.
         */
        std::function<void (XmlInputStream&, XmlObject&)> rx_func;

        /**
         * Callback for XML parse error.
         */
        std::function<void (XmlInputStream&)> err_func;

        /**
         * XML parse data.
         */
        XmlParseData* parse_data;
        XmlObject top_node;

        std::mutex mutex;
    };

}


#endif
