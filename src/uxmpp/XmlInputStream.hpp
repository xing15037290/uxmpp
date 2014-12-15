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
#include <uxmpp/XmlObject.hpp>
#include <mutex>


namespace uxmpp {



    /**
     * An XML input stream.
     */
    class XmlInputStream {
    public:

        /**
         * XML object callback.
         * Called when an XML object is received.
         * @param stream The XML input stream object that received the XML object.
         * @param xml_obj The received XML object.
         */
        typedef std::function<void (XmlInputStream& stream, XmlObject& xml_obj)> xml_func_t;


        /**
         * XML parse error callback.
         * Called when a XML parse error ocurrs.
         * @param stream The XML input stream object that generated the error.
         * @param code An error code.
         * @param msg An error message.
         */
        typedef std::function<void (XmlInputStream& stream, int code, const std::string& msg)> err_func_t;

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
        void set_xml_handler (xml_func_t xml_handler);

        /**
         * Set the XML error handler.
         */
        void set_error_handler (err_func_t err_handler);

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
        void free_resources ();

        /**
         * Callback for incoming XML object.
         */
        xml_func_t rx_func;

        /**
         * Callback for XML parse error.
         */
        err_func_t err_func;

        /**
         * XML parse data.
         */
        XmlParseData* parse_data;

        /**
         * The base of the XML document.
         */
        XmlObject top_node;

        std::mutex mutex;
    };

}


#endif
