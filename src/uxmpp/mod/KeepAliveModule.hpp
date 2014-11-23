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
#ifndef UXMPP_MOD_KEEPALIVEMODULE_HPP
#define UXMPP_MOD_KEEPALIVEMODULE_HPP

#include <string>
#include <uxmpp/types.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/SessionListener.hpp>


namespace uxmpp { namespace mod {


    /**
     * An XMPP keep-alive module.
     */
    class KeepAliveModule : public uxmpp::XmppModule, uxmpp::SessionListener {
    public:

        /**
         * Default Constructor.
         */
        KeepAliveModule (const unsigned interval=0);

        /**
         * Destructor.
         */
        virtual ~KeepAliveModule () = default;

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
         * Called when the state if the session changes.
         */
        virtual void on_state_change (uxmpp::Session& session,
                                      uxmpp::SessionState new_state,
                                      uxmpp::SessionState old_state);

        /**
         * Set keep-alive timer interval.
         * @param new_interval Timer interval in seconds. If 0, disable the keep-alive timer.
         */
        void set_interval (unsigned new_interval);

        /**
         * Return the keep-alive interval in seconds. If 0, the keep-alive timer is disabled.
         */
        unsigned get_interval () const {
            return interval;
        }


    protected:
        /**
         * Interval in seconds of the keep-alive timer.
         */
        unsigned interval;

        /**
         *
         */
        uxmpp::Session* sess;

        /**
         * XML ojbect used for sending keep-alive.
         */
        uxmpp::XmlObject keep_alive;
    };


}}


#endif
