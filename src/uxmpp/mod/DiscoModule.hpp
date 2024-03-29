/*
 *  Copyright (C) 2014-2015 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_MOD_DISCOMODULE_HPP
#define UXMPP_MOD_DISCOMODULE_HPP

#include <functional>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/IqStanza.hpp>
#include <uxmpp/mod/DiscoIdentity.hpp>
//#include <uxmpp/mod/DiscoInfo.hpp>
#include <vector>
#include <set>


namespace uxmpp { namespace mod {


    /**
     * An XMPP Service Discovery module (XEP-0030, XEP-0115).
     */
    class DiscoModule : public uxmpp::XmppModule, uxmpp::SessionListener {
    public:

        /**
         *
         */
        typedef std::function <void (uxmpp::Session&,
                                     DiscoModule& module,
                                     uxmpp::StanzaError& error)> on_server_disco_cb_t;

        /**
         * Default Constructor.
         */
        DiscoModule ();

        /**
         * Destructor.
         */
        virtual ~DiscoModule () = default;

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
        virtual bool process_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj) override;

        /**
         * Called when the state if the session changes.
         */
        virtual void on_state_change (uxmpp::Session& session,
                                      uxmpp::SessionState new_state,
                                      uxmpp::SessionState old_state) override;

        /**
         * Return the identities received from the server.
         */
        std::vector<DiscoIdentity>& get_server_identities ();

        /**
         * Return the features received from the server.
         */
        std::vector<std::string>& get_server_features ();

        /**
         * Send an info query to a specific jid.
         * This will send a 
         * @param jid The JID where to send the query.
         * @param query_id An optional identifier for the query. If not given,
         *                 <code>uxmpp::Stanza::makeId()</code> will be used.
         *                 The identifier must be unique.
         */
        std::string query_info (const uxmpp::Jid& jid);

        /**
         * Set a callback to be called when the server features has been received.
         * @param callback The callback to be called.
         */
        void set_on_server_disco (on_server_disco_cb_t callback);


    private:
        uxmpp::Session* sess; /**< The XMPP session */
        std::string server_feature_request_id; /**< Id string used when querying the server features. */
        std::string server_feature_version;    /**< The version of the server features. */

        std::vector<DiscoIdentity> server_identities; /**< The server identities. */
        std::vector<std::string> server_features;     /**< The server features. */
        std::vector<std::string> server_items;       /**< The server itemd. */
        uxmpp::XmlObject server_info_query_result;    /**< The xmlobject returned when querying server features. */

        std::set<std::string> query_ids; /**< A set of id strings of sent stanzas. */

        void handle_feature_request_result (uxmpp::IqStanza& iq);

        //std::function<void (DiscoModule&, DiscoInfo&)>  info_handler;
        //std::function<void (DiscoModule&, DiscoItems&)> items_handler;
        std::function<void (DiscoModule&, XmlObject&)> info_handler;
        std::function<void (DiscoModule&, XmlObject&)> items_handler;

        on_server_disco_cb_t on_server_disco_cb;
    };


}}

#endif
