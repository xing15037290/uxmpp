/*
 *  Copyright (C) 2013-2015 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_SESSION_HPP
#define UXMPP_SESSION_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlStream.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/StreamXmlObj.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/SessionConfig.hpp>
#include <uxmpp/SessionListener.hpp>
#include <uxmpp/StreamError.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/io/SocketConnection.hpp>

#include <string>


namespace uxmpp {

    // Forward declarations
    class Session;


    /**
     * A client to server XMPP session.
     */
    class Session : XmppModule {
    public:

        /**
         * Default constructor.
         */
        Session ();

        /**
         * Destructor.
         */
        virtual ~Session ();

        /**
         * Connect to an XMPP server and run the session until it is closed.
         * This call will block until the XMPP session is closed.
         * @param config Session configuration.
         */
        void run (const SessionConfig& config);

        /**
         * Disconnect from the XMPP server.
         * @param fast If true, send </stream:stream> to the peer and
         *             close the socket without waiting for an answer.
         */
        void stop (bool fast=false);

        /**
         * Return a stream error object.
         * When the session is closed, use this method to retrieve a
         * StreamError object that describes an XMPP error, if any.
         * Application specific errors 'undefined-condition':
         * * resolve-error
         * * connect-failed
         * * timeout
         * * parse-error
         * * rx-error
         * * tx-error
         * * 
         */
        StreamError& get_error ();

        /**
         * Did an error occurr?
         * This can be checked when we are in state SessionState::closed to see
         * if the session was closed due to an error.
         * @see getError.
         */
        bool have_error ();

        /**
         * Add a listener object that will receive events from the session.
         */
        virtual void add_session_listener (SessionListener& listener);

        /**
         * Remove a listener object that is receiving events from the session.
         */
        virtual void del_session_listener (SessionListener& listener);

        /**
         * Return the session state.
         */
        SessionState get_state () const;

        /**
         * Get the session jid.
         */
        Jid get_jid () const;

        /**
         * Get the session domain.
         */
        Jid get_domain () const;

        /**
         * Send an XMPP stanza to the server.
         */
        void send_stanza (const XmlObject& xml_obj);

        /**
         * Return the session id.
         */
        std::string get_id () const;

        /**
         * Return the server's 'from' attribute from the initial 'stream' tag.
         */
        std::string get_stream_from_attr () const;

        /**
         * Register an XMPP module that will handle incoming XML objects.
         * The order of the registered XMPP modules are important.
         */
        void register_module (XmppModule& module);

        /**
         * Register an XMPP module that will handle incoming XML objects.
         * The order of the registered XMPP modules are important.
         * @param before_this The module will be added before 'before_this' module.
         *                    If no 'before_this' module exists it will be added last.
         */
        void register_module (XmppModule& module, XmppModule& before_this);

        /**
         * Unregister an XMPP module.
         */
        void unregister_module (XmppModule& module);

        /**
         * Get the list of pointers to registered XMPP modules.
         */
        std::vector<XmppModule*>& get_modules ();

        /**
         * Return a reference to the underlaying XML stream.
         */
        XmlStream& get_xml_stream ();

        /**
         * Get the server features.
         */
        std::vector<XmlObject>& get_features ();

        /**
         * Reset the XML stream and re-send a new XML 'stream' start tag.
         */
        void reset ();

        /**
         * Set an application specific error condition.
         */
        void set_app_error (const std::string& app_error, const std::string& text="");

        /**
         * Get the socket used by the XML stream.
         * Only use this if you know what you are doing!
         */
        io::SocketConnection& get_socket () {
            return socket;
        }


    protected:

        /**
         * Configuration.
         */
        SessionConfig cfg;

        /**
         * IP socket connection.
         */
        io::SocketConnection socket;

        /**
         * An XML stream.
         */
        XmlStream xs;

        /**
         * Session id.
         */
        std::string sess_id;

        /**
         * The 'from' attribute in the initial 'stream' tag from the server.
         */
        std::string sess_from;

        /**
         * The state of the sesssion.
         */
        SessionState state;

        /**
         * Top-level XML object.
         */
        StreamXmlObj stream_xml_obj;

        /**
         * Stream features.
         */
        std::vector<XmlObject> features;

        /**
         * A list of event listeners.
         */
        std::vector<SessionListener*> listeners;

        /**
         * An object to hold an eventual stream error.
         */
        StreamError stream_error;

        /**
         * Set new session state.
         */
        virtual bool change_state (SessionState new_state);

        /**
         * Session jid.
         */
        std::string jid;

        /**
         * Registered XMPP modules.
         */
        std::vector<XmppModule*> xmpp_modules;

        /**
         * Called whan an XML object is received.
         * @return Return true if this XML object was processed and no further work should be done.
         */
        virtual bool process_xml_object (Session& session, XmlObject& xml_obj) override;

        /**
         *
         */
        void on_rx_xml_obj (XmlStream& stream, XmlObject& xml_obj);

    private:
    };


}


#endif
