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
 * A Jabber Identifier (JID).
 */
class Jid {
public:

    /**
     * Constructor.
     */
    Jid (const std::string& jid="");

    /**
     * Constructor.
     */
    Jid (const std::string& localpart, const std::string& domainpart, const std::string& resourcepart="");

    /**
     * Copy constructor.
     */
    Jid (const Jid& jid) {
        local    = jid.local;
        domain   = jid.domain;
        resource = jid.resource;
    }

    /**
     * Move constructor.
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
     */
    Jid& operator= (Jid& jid) {
        local    = std::move (jid.local);
        domain   = std::move (jid.domain);
        resource = std::move (jid.resource);
        return *this;
    }

    /**
     * Equality operator.
     */
    bool operator== (const Jid& jid) {
        return jid.local==local && jid.domain==domain && jid.resource==resource;
    }

    /**
     * Inequality operator.
     */
    bool operator!= (const Jid& jid) {
        return !(*this == jid);
    }

    /**
     * Set the local part of the JID.
     */
    void setLocal (const std::string& localpart);

    /**
     * Return the local part of the JID.
     */
    const std::string& getLocal () const {
        return local;
    }

    /**
     * Set the domain part of the JID.
     */
    void setDomain (const std::string& domainpart);

    /**
     * Return the domain part of the JID.
     */
    const std::string& getDomain () const {
        return domain;
    }

    /**
     * Set the resource part of the JID.
     */
    void setResource (const std::string& resourcepart);

    /**
     * Return the resource part of the JID.
     */
    const std::string& getResource () const {
        return resource;
    }

    /**
     * Return a 'bare' jid based on this jid.
     */
    Jid bare () const {
        return Jid (local, domain, "");
    }

    /**
     * Return true if this is a bare jid.
     */
    bool isBare () const {
        return resource.length() == 0;
    }


protected:

    std::string local;
    std::string domain;
    std::string resource;
};


/**
 * Return a string represenation of a Jid.
 */
std::string to_string (const Jid& jid);



}


#endif
