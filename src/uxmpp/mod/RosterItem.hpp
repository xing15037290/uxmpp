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
#ifndef UXMPP_MOD_ROSTERITEM_HPP
#define UXMPP_MOD_ROSTERITEM_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/Jid.hpp>
#include <string>
#include <vector>


namespace uxmpp { namespace mod {


    /**
     *
     */
    class RosterItem : public uxmpp::XmlObject {
    public:

        /**
         * Default constructor.
         */
        RosterItem ();

        /**
         * Constructor.
         */
        RosterItem (const uxmpp::Jid& jid, const std::string& handle="");

        /**
         * Constructor.
         */
        RosterItem (const uxmpp::Jid& jid, const std::vector<std::string>& groups);

        /**
         * Constructor.
         */
        RosterItem (const uxmpp::Jid& jid, const std::string& handle, const std::vector<std::string>& groups);

        /**
         * Copy constructor.
         */
        RosterItem (const RosterItem& item);

        /**
         * Move constructor.
         */
        RosterItem (RosterItem&& item);

        /**
         * Destructor.
         */
        virtual ~RosterItem () = default;

        /**
         * Assignment operator.
         */
        RosterItem& operator= (const RosterItem& item);

        /**
         * Move operator.
         */
        RosterItem& operator= (RosterItem&& item);

        /**
         * Get the JID of the item.
         */
        uxmpp::Jid get_jid () const;

        /**
         * Set the JID of the item.
         */
        void set_jid (const uxmpp::Jid& jid);

        /**
         * Get the handle of the item.
         */
        std::string get_handle () const;

        /**
         * Set the handle of the item.
         */
        void set_handle (const std::string& handle);

        /**
         *
         */
        bool is_approved () const;

        /**
         *
         */
        void set_approved (bool approved);

        /**
         * Get the 'ask' attribute.
         */
        std::string get_ask () const;

        /**
         * Get the 'subscription' attribute.
         */
        std::string get_subscription () const;

        /**
         * Get the group(s) that the item belongs to.
         */
        std::vector<std::string> get_groups ();


    protected:
    private:
    };


}}


#endif
