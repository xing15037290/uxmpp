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
#ifndef UXMPP_MOD_IBBMODULE_HPP
#define UXMPP_MOD_IBBMODULE_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/Jid.hpp>

#include <string>
#include <map>


namespace uxmpp { namespace mod {


    /**
     * In-band Bytestream (XEP-0047).
     */
    class IBBModule : public uxmpp::XmppModule {
    public:

        /**
         * Default Constructor.
         */
        IBBModule ();

        /**
         * Destructor.
         */
        virtual ~IBBModule () = default;

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
         * Open a byte stream to a JID.
         * @param to The receiver of the byte stream.
         * @param block_size Size of the transmitted blocks.
         * @return A byte stream ID used when sending data and ending the stream.
         */
        std::string open (const Jid& to, size_t block_size=4096);

        /**
         * Send a piece of data in a byte stream.
         * @param sid The ID of the byte stream.
         * @param data A pointer to a data chunk to send.
         * @param size The size of the data chunk to send.
         */
        void send (const std::string& sid, const void* data, size_t size);

        /**
         * Close a byte stream.
         * @param sid The ID of the byte stream to close.
         */
        void close (const std::string& sid);

        /**
         * Accept a byte stream request.
         */
        void accept (const std::string& sid);

        /**
         * Deny a byte stream request.
         */
        void deny (const std::string& sid);


    private:
        struct ibb_session_t {
            Jid peer;
            unsigned short seq;
            size_t chunk_size;
        };

        uxmpp::Session* sess;
        std::map<std::string, ibb_session_t> rx_streams;
        std::map<std::string, ibb_session_t> tx_streams;
    };



}}

#endif
