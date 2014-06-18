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
#ifndef UXMPP_MOD_DISCOMODULE_HPP
#define UXMPP_MOD_DISCOMODULE_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/IqStanza.hpp>
#include <vector>


namespace uxmpp { namespace mod {


    /**
     * An XMPP Service Discovery module (XEP-0030, XEP-0115).
     */
    class DiscoModule : public uxmpp::XmppModule, uxmpp::SessionListener {
    public:

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
         * Called when the state if the session changes.
         */
        virtual void onStateChange (uxmpp::Session& session,
                                    uxmpp::SessionState new_state,
                                    uxmpp::SessionState old_state);

        /**
         * Return the features received from the server.
         */
        std::vector<std::string>& getFeatures () {
            return features;
        }


    protected:
        uxmpp::Session* sess;
        std::string feature_request_id;
        std::string feature_version;

        std::vector<std::string> features;
        uxmpp::XmlObject server_info_query_result;

    private:
        void handle_feature_request_result (uxmpp::IqStanza& iq);
    };


}}

#endif
