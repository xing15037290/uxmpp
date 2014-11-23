/*
 *  Copyright (C) 2014 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_MOD_ROSTERMODULE_HPP
#define UXMPP_MOD_ROSTERMODULE_HPP

#include <string>
#include <vector>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/mod/Roster.hpp>
#include <uxmpp/mod/RosterItem.hpp>


namespace uxmpp { namespace mod {


    // Forward declarations
        //class RosterModle;


    /**
     * An XMPP roster module.
     */
    class RosterModule : public uxmpp::XmppModule {
    public:

        /**
         * Default Constructor.
         */
        RosterModule ();

        /**
         * Destructor.
         */
        virtual ~RosterModule () = default;

        /**
         * Called when the module is registered to a session.
         */
        virtual void module_registered (uxmpp::Session& session) override;

        /**
         * Called when the module is unregistered from a session.
         */
        virtual void module_unregistered (uxmpp::Session& session) override;

        /**
         * Called whan an XML object is received.
         * @return Return true if this XML object was processed and no further work should be done.
         */
        virtual bool proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj) override;

        /**
         * Send a roster query to the server.
         */
        void refresh ();

        /**
         * Return the roster.
         */
        Roster& get_roster () {
            return roster;
        }

        /**
         * Update/modify a roster item.
         */
        void update_item (const RosterItem& item) {
            roster_set (item);
        }

        /**
         * Add a new roster item.
         */
        void add_item (const RosterItem& item) {
            roster_set (item);
        }

        /**
         * Add a new jid to the roster.
         */
        void add_item (const uxmpp::Jid& jid) {
            roster_set (RosterItem(jid));
        }

        /**
         * Remove a roster item.
         */
        void remove_item (const RosterItem& item) {
            roster_set (item, true);
        }

        /**
         * Remove a jid from the roster.
         */
        void remove_item (const uxmpp::Jid& jid) {
            roster_set (RosterItem(jid), true);
        }

        /**
         *
         */
        void set_roster_handler (std::function<void (RosterModule&, Roster&)> on_roster) {
            roster_handler = on_roster;
        }

        /**
         *
         */
        void set_roster_push_handler (std::function<void (RosterModule&, RosterItem&)> on_roster_push) {
            roster_push_handler = on_roster_push;
        }


    protected:
        uxmpp::Session* sess;
        Roster roster;
        std::string roster_query_id;

        /**
         * Handle a roster push.
         */
        void handle_roster_push (RosterItem& item);


    private:
        /**
         * Add/modify/remove a roster item.
         */
        void roster_set (const RosterItem& item, bool remove=false);

        /**
         * Callback for roster query result.
         */
        std::function<void (RosterModule&, Roster&)> roster_handler;

        /**
         * Callback for roster push'es.
         */
        std::function<void (RosterModule&, RosterItem&)> roster_push_handler;
    };


}}


#endif
