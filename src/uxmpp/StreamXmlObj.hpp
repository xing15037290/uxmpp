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


namespace uxmpp {


    /**
     * Stream stanza.
     */
    class StreamXmlObj : public XmlObject {
    public:

        /**
         * Default Constructor.
         */
        StreamXmlObj ();

        /**
         * Constructor.
         */
        StreamXmlObj (const std::string& to, const std::string& from);

        /**
         * Destructor.
         */
        virtual ~StreamXmlObj () = default;

        /**
         *
         */
        void set_to (const std::string& to);

        /**
         *
         */
        std::string get_to () const;

        /**
         *
         */
        void set_id (const std::string& id);

        /**
         *
         */
        std::string get_id () const;

        /**
         *
         */
        void set_from (const std::string& from);

        /**
         *
         */
        std::string get_from () const;
    };


}


#endif
