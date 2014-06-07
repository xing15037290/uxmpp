/*
 *  Copyright (C) 2013 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_UXMPPEXCEPTION_HPP
#define UXMPP_UXMPPEXCEPTION_HPP

#include <string>


namespace uxmpp {

    /**
     * Base class for all exceptions in namespace uxmpp.
     */
    class UxmppException {
    public:

        /**
         * Constructor.
         */
        UxmppException (const std::string& description="") : msg{description} {
        }

        /**
         * Destructor.
         */
        virtual ~UxmppException () = default;

        /**
         * Return the error description.
         */
        const std::string& what () const {
            return msg;
        }


    protected:
        /**
         * An error description.
         */
        std::string msg;
    };

};


#endif
