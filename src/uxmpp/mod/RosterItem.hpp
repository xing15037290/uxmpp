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
        RosterItem ()
            : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
        {
        }

        /**
         * Constructor.
         */
        RosterItem (const uxmpp::Jid& jid, const std::string& handle="")
            : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
        {
            std::string strjid = to_string (jid);
            if (!strjid.empty())
                set_attribute ("jid", strjid);
            if (!handle.empty())
                set_attribute ("name", handle);
        }

        /**
         * Constructor.
         */
        RosterItem (const uxmpp::Jid& jid, const std::vector<std::string>& groups)
            : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
        {
            std::string strjid = to_string (jid);
            if (!strjid.empty())
                set_attribute ("jid", strjid);
            for (auto group : groups)
                add_node (XmlObject("group", XmlIqRosterNs, false).set_content(group));
        }

        /**
         * Constructor.
         */
        RosterItem (const uxmpp::Jid& jid, const std::string& handle, const std::vector<std::string>& groups)
            : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
        {
            std::string strjid = to_string (jid);
            if (!strjid.empty())
                set_attribute ("jid", strjid);
            if (!handle.empty())
                set_attribute ("name", handle);
            for (auto group : groups)
                add_node (XmlObject("group", XmlIqRosterNs, false).set_content(group));
        }

        /**
         * Copy constructor.
         */
        RosterItem (const RosterItem& item) : uxmpp::XmlObject (item) {
        }

        /**
         * Move constructor.
         */
        RosterItem (RosterItem&& item) : uxmpp::XmlObject (item) {
        }

        /**
         * Destructor.
         */
        virtual ~RosterItem () = default;

        /**
         * Assignment operator.
         */
        RosterItem& operator= (const RosterItem& item) {
            if (this != &item)
                uxmpp::XmlObject::operator= (item);
            return *this;
        }

        /**
         * Move operator.
         */
        RosterItem& operator= (RosterItem&& item) {
            uxmpp::XmlObject::operator= (item);
            return *this;
        }

        /**
         * Get the JID of the item.
         */
        uxmpp::Jid get_jid () const {
            return uxmpp::Jid (get_attribute("jid"));
        }

        /**
         * Set the JID of the item.
         */
        void set_jid (const uxmpp::Jid& jid) {
            set_attribute ("jid", to_string(jid));
        }

        /**
         * Get the handle of the item.
         */
        std::string get_handle () const {
            return get_attribute ("name");
        }

        /**
         * Set the handle of the item.
         */
        void set_handle (const std::string& handle) {
            set_attribute ("name", handle);
        }

        /**
         *
         */
        bool is_approved () const {
            return get_attribute("approved") == "true";
        }

        /**
         *
         */
        void set_approved (bool approved) {
            set_attribute("approved", approved ? "true" : "");
        }

        /**
         * Get the 'ask' attribute.
         */
        std::string get_ask () const {
            return get_attribute ("ask");
        }

        /**
         * Get the 'subscription' attribute.
         */
        std::string get_subscription () const {
            return get_attribute ("subscription");
        }

        /**
         * Get the group(s) that the item belongs to.
         */
        std::vector<std::string> get_groups ();


    protected:
    private:
    };


}}


#endif
