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
            if (strjid.length())
                setAttribute ("jid", strjid);
            if (handle.length())
                setAttribute ("name", handle);
        }

        /**
         * Constructor.
         */
        RosterItem (const uxmpp::Jid& jid, const std::vector<std::string>& groups)
            : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
        {
            std::string strjid = to_string (jid);
            if (strjid.length())
                setAttribute ("jid", strjid);
            for (auto group : groups)
                addNode (XmlObject("group", XmlIqRosterNs, false).setContent(group));
        }

        /**
         * Constructor.
         */
        RosterItem (const uxmpp::Jid& jid, const std::string& handle, const std::vector<std::string>& groups)
            : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
        {
            std::string strjid = to_string (jid);
            if (strjid.length())
                setAttribute ("jid", strjid);
            if (handle.length())
                setAttribute ("name", handle);
            for (auto group : groups)
                addNode (XmlObject("group", XmlIqRosterNs, false).setContent(group));
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
        uxmpp::Jid getJid () const {
            return uxmpp::Jid (getAttribute("jid"));
        }

        /**
         * Set the JID of the item.
         */
        void setJid (const uxmpp::Jid& jid) {
            setAttribute ("jid", to_string(jid));
        }

        /**
         * Get the handle of the item.
         */
        std::string getHandle () const {
            return getAttribute ("name");
        }

        /**
         * Set the handle of the item.
         */
        void setHandle (const std::string& handle) {
            setAttribute ("name", handle);
        }

        /**
         *
         */
        bool isApproved () const {
            return getAttribute("approved") == "true";
        }

        /**
         *
         */
        void setApproved (bool approved) {
            setAttribute("approved", approved ? "true" : "");
        }

        /**
         * Get the 'ask' attribute.
         */
        std::string getAsk () const {
            return getAttribute ("ask");
        }

        /**
         * Get the 'subscription' attribute.
         */
        std::string getSubscription () const {
            return getAttribute ("subscription");
        }

        /**
         * Get the group(s) that the item belongs to.
         */
        std::vector<std::string> getGroups ();


    protected:
    private:
    };


}}


#endif
