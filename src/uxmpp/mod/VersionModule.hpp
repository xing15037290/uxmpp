/*
 *  Copyright (C) 2015 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_MOD_VERSIONMODULE_HPP
#define UXMPP_MOD_VERSIONMODULE_HPP

#include <string>
#include <map>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/StanzaError.hpp>


namespace uxmpp { namespace mod {


    /**
     * XEP-0092: Software Version
     */
    class VersionModule : public uxmpp::XmppModule {
    public:

        /**
         * Version result callback.
         */
        typedef std::function<void (uxmpp::Session& session,
                                    uxmpp::Jid& jid,
                                    uxmpp::XmlObject& version,
                                    uxmpp::StanzaError& error)> version_cb_t;

        /**
         * Default Constructor.
         */
        VersionModule ();

        /**
         * Destructor.
         */
        virtual ~VersionModule () = default;

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
         *
         */
        virtual std::string get_name ();

        /**
         *
         */
        virtual void set_name (const std::string& name);

        /**
         *
         */
        virtual std::string get_version ();

        /**
         *
         */
        virtual void set_version (const std::string& version);

        /**
         *
         */
        virtual std::string get_os ();

        /**
         *
         */
        virtual void set_os (const std::string& os);

        /**
         *
         */
        bool request_version (const uxmpp::Jid& jid);

        /**
         *
         */
        void set_version_callback (version_cb_t callback);


    private:
        uxmpp::Session* sess;
        std::string name;
        std::string version;
        std::string os;
        //std::set<std::string> query_ids;
        std::map<std::string, std::string> query_ids;
        version_cb_t version_cb;
    };


}}


#endif
