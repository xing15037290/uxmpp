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
#ifndef UXMPP_MOD_DISCOIDENTITY_HPP
#define UXMPP_MOD_DISCOIDENTITY_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <string>


namespace uxmpp { namespace mod {


    /**
     * Disco Identity.
     */
    class DiscoIdentity : public XmlObject {
    public:
        /**
         * Constructor.
         */
        DiscoIdentity (const std::string& to="",
                       const std::string& from="",
                       const std::string& id="",
                       const IdentityType& type=IdentityType::normal)
            : Stanza (to, from, id)
        {
            setName ("identity");
            setIdentityType (type);
        }

        /**
         * Constructor.
         */
        DiscoIdentity (const Jid& to,
                       const Jid& from,
                       const std::string& id="",
                       const IdentityType& type=IdentityType::normal)
            : Stanza (to, from, id)
        {
            setName ("identity");
            setIdentityType (type);
        }

        /**
         * Copy constructor.
         */
        DiscoIdentity (const DiscoIdentity& msg_stanza) : Stanza (msg_stanza) {
        }

        /**
         * Move constructor.
         */
        DiscoIdentity (DiscoIdentity&& msg_stanza) : Stanza (msg_stanza) {
        }

        /**
         * Destructor.
         */
        virtual ~DiscoIdentity () = default;

        /**
         * Assignment operator.
         */
        DiscoIdentity& operator= (const DiscoIdentity& msg_stanza) {
            if (this != &msg_stanza)
                Stanza::operator= (msg_stanza);
            return *this;
        }

        /**
         * Move operator.
         */
        DiscoIdentity& operator= (DiscoIdentity&& msg_stanza) {
            Stanza::operator= (msg_stanza);
            return *this;
        }

        /**
         * Return the identity type.
         */
        IdentityType getIdentityType ();

        /**
         * Set the identity type.
         */
        DiscoIdentity& setIdentityType (const IdentityType& type);

        /**
         * Return the identity thread id.
         */
        std::string getThread ();

        /**
         * Set the identity thread id.
         */
        DiscoIdentity& setThread (const std::string& thread_id, const std::string& parent_thread_id="");

        /**
         * Return the identity parent thread id.
         */
        std::string getParentThread ();

        /**
         * Set the identity parent thread id.
         */
        DiscoIdentity& setParentThread (const std::string& parent_thread_id);

        /**
         * Return the identity body.
         */
        std::string getBody (std::string lang="");

        /**
         * Set the identity body.
         */
        DiscoIdentity& setBody (const std::string& body, std::string lang="");

    };


}}


#endif
