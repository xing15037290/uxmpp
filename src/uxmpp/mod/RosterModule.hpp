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
        virtual void moduleRegistered (uxmpp::Session& session);

        /**
         * Called when the module is unregistered from a session.
         */
        virtual void moduleUnregistered (uxmpp::Session& session);

        /**
         * Called whan an XML object is received.
         * @return Return true if this XML object was processed and no further work should be done.
         */
        virtual bool proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj);

        /**
         * Send a roster query to the server.
         */
        void refresh ();

        /**
         * Return the roster.
         */
        Roster& getRoster () {
            return roster;
        }

        /**
         * Update/modify a roster item.
         */
        void updateItem (const RosterItem& item) {
            rosterSet (item);
        }

        /**
         * Add a new roster item.
         */
        void addItem (const RosterItem& item) {
            rosterSet (item);
        }

        /**
         * Add a new jid to the roster.
         */
        void addItem (const uxmpp::Jid& jid) {
            rosterSet (RosterItem(jid));
        }

        /**
         * Remove a roster item.
         */
        void removeItem (const RosterItem& item) {
            rosterSet (item, true);
        }

        /**
         * Remove a jid from the roster.
         */
        void removeItem (const uxmpp::Jid& jid) {
            rosterSet (RosterItem(jid), true);
        }

        /**
         *
         */
        void setRosterHandler (std::function<void (RosterModule&, Roster&)> on_roster) {
            roster_handler = on_roster;
        }

        /**
         *
         */
        void setRosterPushHandler (std::function<void (RosterModule&, RosterItem&)> on_roster_push) {
            roster_push_handler = on_roster_push;
        }


    protected:
        uxmpp::Session* sess;
        Roster roster;
        std::string roster_query_id;

        /**
         * Handle a roster push.
         */
        void handleRosterPush (RosterItem& item);


    private:
        /**
         * Add/modify/remove a roster item.
         */
        void rosterSet (const RosterItem& item, bool remove=false);

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
