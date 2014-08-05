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
     * This is an xml object with tag name 'identity' in
     * namespace http://jabber.org/protocol/disco#info.
     * It should have the attributes 'category' and 'type'.
     */
    class DiscoIdentity : public uxmpp::XmlObject {
    public:

        /**
         * Constructor.
         * Construct an xml object named 'identity' qualified
         * by the namespace http://jabber.org/protocol/disco#info
         * with the attributes 'category', 'type', and optionally
         * 'name'.
         * @param category The 'category' attribute.
         * @param type The 'type' attribute.
         * @param name The 'name' attribute.
         */
        DiscoIdentity (const std::string& category,
                       const std::string& type,
                       const std::string& name="")
            : XmlObject ("identity", "http://jabber.org/protocol/disco#info", false)
        {
            setCategory (category);
            setType (type);
            setName (name);
        }

        /**
         * Constructor.
         * @param identity An xml object viewed as a sevice discovery identity object.
         */
        DiscoIdentity (const uxmpp::XmlObject& identity)
            : XmlObject (identity)
        {
        }

        /**
         * Copy constructor.
         * @param identity The object to copy.
         */
        DiscoIdentity (const DiscoIdentity& identity) : XmlObject (identity) {
        }

        /**
         * Move constructor.
         * @param identity The object to move.
         */
        DiscoIdentity (const DiscoIdentity&& identity) : XmlObject (identity) {
        }

        /**
         * Destructor.
         */
        virtual ~DiscoIdentity () = default;

        /**
         * Assignment operator.
         * @param identity The object to copy.
         */
        DiscoIdentity& operator= (const DiscoIdentity& identity) {
            if (this != &identity)
                XmlObject::operator= (identity);
            return *this;
        }

        /**
         * Move operator.
         * @param identity The object to move.
         */
        DiscoIdentity& operator= (const DiscoIdentity&& identity) {
            XmlObject::operator= (identity);
            return *this;
        }

        /**
         * Return the identity category.
         * This will return the value of the 'category' attribute.
         * @return The identity category.
         */
        const std::string getCategory () const {
            return getAttribute ("category");
        }

        /**
         * Set the identity category.
         * @param category The value of the 'category' attribute.
         */
        void setCategory (const std::string& category) {
            setAttribute ("category", category);
        }

        /**
         * Return the identity type.
         * This will return the value of the 'type' attribute.
         * @return The identity type.
         */
        const std::string getType () const {
            return getAttribute ("type");
        }

        /**
         * Set the identity type.
         * @param type The value of the 'type' attribute.
         */
        void setType (const std::string& type) {
            setAttribute ("type", type);
        }

        /**
         * Return the identity name.
         * This will return the value of the 'name attribute.
         * @return The identity name.
         */
        const std::string getName () const {
            return getAttribute ("name");
        }

        /**
         * Set the identity name.
         * @param name The value of the 'name' attribute.
         */
        void setName (const std::string& name) {
            setAttribute ("name", name);
        }
    };


}}


#endif
