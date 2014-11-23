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
#ifndef UXMPP_IQSTANZA_HPP
#define UXMPP_IQSTANZA_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/Stanza.hpp>
#include <uxmpp/Jid.hpp>
#include <string>


namespace uxmpp {


    /**
     * Type of IQ stanza.
     */
    enum class IqType {
        get, set, result, error
    };


    /**
     * Return a string representation of the type of IQ stanza.
     */
    std::string to_string (const IqType& iq_type);


    /**
     * IQ stanza.
     */
    class IqStanza : public Stanza {
    public:
        /**
         * Constructor.
         */
        IqStanza (const IqType type, const std::string& to="", const std::string& from="", const std::string& id="");

        /**
         * Constructor.
         */
        IqStanza (const IqType type, const Jid& to, const Jid& from, const std::string& id="");

        /**
         * Copy constructor.
         */
        IqStanza (const IqStanza& iq_stanza);

        /**
         * Move constructor.
         */
        IqStanza (IqStanza&& iq_stanza);

        /**
         * Destructor.
         */
        virtual ~IqStanza () = default;

        /**
         * Assignment operator.
         */
        IqStanza& operator= (const IqStanza& iq_stanza);

        /**
         * Move operator.
         */
        IqStanza& operator= (IqStanza&& iq_stanza);

        /**
         * Get the IQ type.
         */
        IqType get_type () const;

        /**
         * Set the IQ type.
         */
        void set_type (const IqType type);
    };


}


#endif
