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
#ifndef UXMPP_MOD_DISCOINFO_HPP
#define UXMPP_MOD_DISCOINFO_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <string>


namespace uxmpp { namespace mod {


    /**
     * Disco Info.
     * This is the xml object that is the result of a query stanza
     * qualified by the <code>http://jabber.org/protocol/disco#info</code>
     * namespace.
     */
    class DiscoInfo : public uxmpp::XmlObject {
    public:

        /**
         * Constructor.
         * Construct an xml object named 'query' qualified
         * by the namespace http://jabber.org/protocol/disco#info.
         */
        DiscoInfo ();

        /**
         * Constructor.
         * @param info An xml object viewed as a sevice discovery info query result.
         */
        DiscoInfo (const uxmpp::XmlObject& info);

        /**
         * Copy constructor.
         * @param info The object to copy.
         */
        DiscoInfo (const DiscoInfo& info);

        /**
         * Move constructor.
         * @param info The object to move.
         */
        DiscoInfo (const DiscoInfo&& info);

        /**
         * Destructor.
         */
        virtual ~DiscoInfo () = default;

        /**
         * Assignment operator.
         * @param info The object to copy.
         */
        DiscoInfo& operator= (const DiscoInfo& info);

        /**
         * Move operator.
         * @param info The object to move.
         */
        DiscoInfo& operator= (const DiscoInfo&& info);
    };


}}


#endif
