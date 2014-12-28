/*
 *  Copyright (C) 2013,2014 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_SESSIONLISTENER_HPP
#define UXMPP_SESSIONLISTENER_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/XmlObject.hpp>
#include <vector>


namespace uxmpp {


    // Forward declarations
    //
    class Session;


    /**
     *
     */
    class SessionListener {
    public:

        /**
         * Default constructor.
         */
        SessionListener () = default;

        /**
         * Destructor.
         */
        virtual ~SessionListener () = default;

        /**
         * Called when the state if the session changes.
         */
        virtual void on_state_change (Session& session, SessionState new_state, SessionState old_state) = 0;

        /**
         * Called when feature list updated.
         */
        virtual void on_features (Session& session, std::vector<XmlObject>& features) {
        }
    };


}


#endif
