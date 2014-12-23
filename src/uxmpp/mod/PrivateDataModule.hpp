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
#ifndef UXMPP_MOD_PRIVATEDATAMODULE_HPP
#define UXMPP_MOD_PRIVATEDATAMODULE_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/IqStanza.hpp>

#include <functional>
#include <string>
#include <map>


namespace uxmpp { namespace mod {


    /**
     * Store and fetch private XML data from an XMPP server (XEP-0049).
     */
    class PrivateDataModule : public uxmpp::XmppModule {
    public:

        /**
         * Callback that's called when private data has been set on the XMPP server.
         * @param session The XMPP session in which the data was send to the server.
         * @param full_xml_name The name of the XML object that was stored including
         *                      the namespace.
         * @param stanza_id The ID of the IQ stanza that was sent to the server.
         * @param error_code If not zero, the server has reported an error.
         * @param error_name This is the description of the error if
         *                   <code>error_code</code> is non-zero.
         */
        typedef std::function<void (Session& session,
                                    const std::string& full_xml_name,
                                    const std::string& stanza_id,
                                    const int error_code,
                                    const std::string& error_name)> priv_data_set_callback_t;

        /**
         * Callback that's called when private data has been retrieved from the XMPP server.
         * @param session The XMPP session in which the data was retrieved the server.
         * @param priv_data The private data retrieved from the server.
         * @param stanza_id The ID of the IQ stanza that was sent to the server.
         * @param error_code If not zero, the server has reported an error.
         * @param error_name This is the description of the error if
         *                   <code>error_code</code> is non-zero.
         */
        typedef std::function<void (Session& session,
                                    std::vector<uxmpp::XmlObject>& priv_data,
                                    const std::string& stanza_id,
                                    const int error_code,
                                    const std::string& error_name)> priv_data_get_callback_t;

        /**
         * Default Constructor.
         */
        PrivateDataModule ();

        /**
         * Destructor.
         */
        virtual ~PrivateDataModule () = default;

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
         * Request an XML object to be stored on the server.
         * @param data The data to be stored on the server.
         *             The top level XML element must be scoped by its
         *             own namespace.
         * @param stanza_id The ID of the IQ stanza to be sent. If empty, a unique
         *                  ID will be created.
         * @return The ID of the IQ stanza sent, or an empty string if the
         *          stanza wasn't sent due to invalid parameters.
         */
        std::string set (const uxmpp::XmlObject& data,
                         priv_data_set_callback_t cb=nullptr,
                         const std::string& stanza_id="");

        /**
         * Fetch an XML object that is stored on the server.
         * @param tag_name The tag name of the top level XML object
         *                  to get from the server.
         * @param tag_namespace The namespace of the top level XML object
         *                      to get from the server.
         * @param stanza_id The ID of the IQ stanza to be sent. If empty, a unique
         *                  ID will be created.
         * @return The ID of the IQ stanza sent, or an empty string if the
         *          stanza wasn't sent due to invalid parameters.
         */
        std::string get (const std::string& tag_name,
                         const std::string& tag_namespace,
                         priv_data_get_callback_t cb=nullptr,
                         const std::string& stanza_id="");

        /**
         *
         */
        void set_set_callback (priv_data_set_callback_t cb) {
            set_cb = cb;
        }

        /**
         *
         */
        void set_get_callback (priv_data_get_callback_t cb) {
            get_cb = cb;
        }


    private:
        uxmpp::Session* sess;
        std::map<std::string, std::pair<std::string, priv_data_set_callback_t> > set_ids;
        std::map<std::string, std::pair<std::string, priv_data_get_callback_t> > get_ids;

        bool handle_get_result (const std::string& id, uxmpp::IqStanza& iq_result);
        bool handle_set_result (const std::string& id, uxmpp::IqStanza& iq_result);

        priv_data_set_callback_t set_cb;
        priv_data_get_callback_t get_cb;
    };



}}

#endif
