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
#ifndef UXMPP_MOD_PINGMODULE_HPP
#define UXMPP_MOD_PINGMODULE_HPP

#include <string>
#include <vector>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Jid.hpp>
#include <map>
//#include <uxmpp/mod/PingModuleListener.hpp>


namespace uxmpp { namespace mod {


    /**
     * An XMPP ping module. XEP-0199.
     */
    class PingModule : public uxmpp::XmppModule {
    public:

        /**
         * Default Constructor.
         */
        PingModule ();

        /**
         * Destructor.
         */
        virtual ~PingModule () = default;

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
         * Ping the server.
         */
        virtual void ping ();

        /**
         * Ping a specific JID.
         */
        virtual void ping (const uxmpp::Jid& jid);

        /**
         * Return a list of service discovery information features supported
         * by the module;
         */
        virtual std::vector<std::string> get_disco_features ();


    protected:

        uxmpp::Session* sess;

        // messade_id, (jid, timestamp)
        std::map<std::string, std::pair<uxmpp::Jid, unsigned long> > ping_map;
    };



}}

#endif
