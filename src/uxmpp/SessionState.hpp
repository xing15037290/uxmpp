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
#ifndef UXMPP_SESSIONSTATE_HPP
#define UXMPP_SESSIONSTATE_HPP

#include <uxmpp/types.hpp>
#include <string>


namespace uxmpp {

    /**
     * The state of an XMPP session.
     */
    enum class SessionState {
        /**
         * Session is closed.
         */
        closed = 0,

        /**
         * Session is connecting to the server.
         */
        connecting = 1,

        /**
         * Session is in negotiating state until it is bound to a resource.
         */
        negotiating = 2,

        /**
         * The session is bound to a resource.
         */
        bound = 3,

        /**
         * The session is in process of closing.
         */
        closing = 4,
    };

    /**
     * Return a string representation of a session state.
     */
    inline std::string to_string (const SessionState& state) {
        switch (state) {
        case SessionState::closed:
            return "closed";
        case SessionState::connecting:
            return "connecting";
        case SessionState::negotiating:
            return "negotiating";
        case SessionState::bound:
            return "bound";
        case SessionState::closing:
            return "closing";
        default:
            return "n/a";
        }
    }

}


#endif
