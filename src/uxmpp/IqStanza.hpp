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
    static inline std::string to_string (const IqType& iq_type) {
        switch (iq_type) {
        case IqType::get :
            return "get";
        case IqType::set :
            return "set";
        case IqType::result :
            return "result";
        case IqType::error :
        default :
            return "error";
        }
    }


    /**
     * IQ stanza.
     */
    class IqStanza : public Stanza {
    public:
        /**
         * Constructor.
         */
        IqStanza (const IqType type, const std::string& to="", const std::string& from="", const std::string& id="")
            : Stanza (to, from, id)
        {
            set_tag_name ("iq");
            set_attribute ("type", to_string(type));
        }

        /**
         * Constructor.
         */
        IqStanza (const IqType type, const Jid& to, const Jid& from, const std::string& id="")
            : Stanza (to, from, id)
        {
            set_tag_name ("iq");
            set_attribute ("type", to_string(type));
        }

        /**
         * Copy constructor.
         */
        IqStanza (const IqStanza& iq_stanza) : Stanza (iq_stanza) {
        }

        /**
         * Move constructor.
         */
        IqStanza (IqStanza&& iq_stanza) : Stanza (iq_stanza) {
        }

        /**
         * Destructor.
         */
        virtual ~IqStanza () = default;

        /**
         * Assignment operator.
         */
        IqStanza& operator= (const IqStanza& iq_stanza) {
            if (this != &iq_stanza)
                Stanza::operator= (iq_stanza);
            return *this;
        }

        /**
         * Move operator.
         */
        IqStanza& operator= (IqStanza&& iq_stanza) {
            Stanza::operator= (iq_stanza);
            return *this;
        }

        /**
         * Get the IQ type.
         */
        IqType get_type () const {
            std::string type = get_attribute ("type");
            if (type == "get")
                return IqType::get;
            if (type == "set")
                return IqType::set;
            if (type == "result")
                return IqType::result;
            if (type == "error")
                return IqType::error;
            return IqType::error; // default
        }

        /**
         * Set the IQ type.
         */
        void set_type (const IqType type) {
            set_attribute ("type", to_string(type));
        }
    };


}


#endif
