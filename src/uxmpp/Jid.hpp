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
#ifndef UXMPP_JID_HPP
#define UXMPP_JID_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/UxmppException.hpp>
#include <string>


namespace uxmpp {


/**
 * This class represents a Jabber Identifier (JID).
 */
class Jid {
public:

    /**
     * Constructor.
     * @param jid A JID represented as a string.
     */
    Jid (const std::string& jid="");

    /**
     * Constructor.
     * No part(localpart, domainpart, or resourcepart) of the JID
     * may be longer than 1023 bytes long. If it is longer it will
     * be truncated.
     * @param localpart The localpart of the JID.
     * @param domainpart The domainpart of the JID.
     * @param resourcepart An optional resourcepart.
     */
    Jid (const std::string& localpart, const std::string& domainpart, const std::string& resourcepart="");

    /**
     * Copy constructor.
     * @param jid The JID to copy.
     */
    Jid (const Jid& jid) {
        local    = jid.local;
        domain   = jid.domain;
        resource = jid.resource;
    }

    /**
     * Move constructor.
     * @param jid The JID to move.
     */
    Jid (Jid& jid) {
        local    = std::move (jid.local);
        domain   = std::move (jid.domain);
        resource = std::move (jid.resource);
    }

    /**
     * Destructor.
     */
    virtual ~Jid () = default;

    /**
     * Assignment operator.
     * @param jid The JID to copy.
     * @return A reference to this object.
     */
    Jid& operator= (const Jid& jid) {
        if (&jid != this) {
            local    = jid.local;
            domain   = jid.domain;
            resource = jid.resource;
        }
        return *this;
    }

    /**
     * Move operator.
     * @param jid The JID to move.
     * @return A reference to this object.
     */
    Jid& operator= (Jid& jid) {
        local    = std::move (jid.local);
        domain   = std::move (jid.domain);
        resource = std::move (jid.resource);
        return *this;
    }

    /**
     * Equality operator.
     * @param jid The JID to compare.
     * @return true if the two jid's are identical, false otherwise.
     */
    bool operator== (const Jid& jid) {
        return jid.local==local && jid.domain==domain && jid.resource==resource;
    }

    /**
     * Inequality operator.
     * @param jid The JID to compare.
     * @return true if the two jid's differ, false otherwise.
     */
    bool operator!= (const Jid& jid) {
        return !(*this == jid);
    }

    /**
     * Set the local part of the JID.
     * @param localpart The localpart of the JID.
     *                  If it is longer than 1023 bytes it will be truncated.
     */
    void setLocal (const std::string& localpart);

    /**
     * Return the local part of the JID.
     * @return The local part of the JID.
     */
    const std::string& getLocal () const {
        return local;
    }

    /**
     * Set the domain part of the JID.
     * @param domaipart The domainpart of the JID.
     *                  If it is longer than 1023 bytes it will be truncated.
     */
    void setDomain (const std::string& domainpart);

    /**
     * Return the domain part of the JID.
     * @return The domain part of the JID.
     */
    const std::string& getDomain () const {
        return domain;
    }

    /**
     * Set the resource part of the JID.
     * @param resourcepart The resourcepart of the JID.
     *                     If it is longer than 1023 bytes it will be truncated.
     */
    void setResource (const std::string& resourcepart);

    /**
     * Return the resource part of the JID.
     * @return The resource part of the JID.
     */
    const std::string& getResource () const {
        return resource;
    }

    /**
     * Return a 'bare' jid based on this jid.
     * A bare JID is a JID with only a localpart and a domainpart.
     * @return A copy of this JID but without the resourcepart.
     */
    Jid bare () const {
        return Jid (local, domain, "");
    }

    /**
     * Return true if this is a bare jid.
     * A bare JID is a JID with only a localpart and a domainpart.
     * @return true if the resourcepart is empty, false otherwise.
     */
    bool isBare () const {
        return resource.length() == 0;
    }


protected:

    std::string local;    /**< The localpart of the JID. */
    std::string domain;   /**< The domainpart of the JID. */
    std::string resource; /**< The resourcepart of the JID. */
};


/**
 * Return a string represenation of a Jid.
 * @return A string representing the Jid object.
 */
std::string to_string (const Jid& jid);



}


#endif
