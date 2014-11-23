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
#ifndef UXMPP_STREAMERROR_HPP
#define UXMPP_STREAMERROR_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <string>


namespace uxmpp {


    /**
     * Stream error object.
     */
    class StreamError : public XmlObject {
    public:

        /**
         * Default Constructor.
         */
        StreamError ();

        /**
         * Construct a stream error from an XmlObject.
         */
        StreamError (XmlObject& xml_obj);

        /**
         * Constructor.
         */
        StreamError (const std::string& error);

        /**
         * Destructor.
         */
        virtual ~StreamError () = default;

        /**
         * Assignment operator.
         */
        StreamError& operator= (const XmlObject& xml_obj);

        /**
         * Is there an error message available.
         */
        const bool have_error ();

        /**
         * Set the error condition.
         */
        void set_error_name (const std::string& error);

        /**
         * Return the error condition.
         */
        std::string get_error_name ();

        /**
         * Set the application specific condition.
         */
        void set_app_error (const std::string& app_error, const std::string& text="");

        /**
         * Return the application specific error condition, if any.
         */
        std::string get_app_error ();

        /**
         * Return a descriptive error text (if any).
         */
        std::string get_text ();

        /**
         * Set a descriptive error text.
         */
        void set_text (const std::string& text);
    };


}


#endif
