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
#ifndef UXMPP_MOD_MESSAGEMODULELISTENER_HPP
#define UXMPP_MOD_MESSAGEMODULELISTENER_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/Jid.hpp>
#include <string>


// Forward declarations
//
namespace uxmpp {
    class MessageStanza;
}


namespace uxmpp { namespace mod {


    // Forward declarations
    //
    class MessageModule;


    /**
     *
     */
    class MessageModuleListener {
    public:

        /**
         * Default constructor.
         */
        MessageModuleListener () = default;

        /**
         * Destructor.
         */
        virtual ~MessageModuleListener () = default;

        /**
         * Called when the presence module receives a message.
         */
        virtual void onMessage (MessageModule& module, uxmpp::MessageStanza& msg) {};

        /**
         * Called when the presence module receives a message receipt.
         */
        virtual void onReceipt (MessageModule& module, const uxmpp::Jid& from, const std::string& id) {};
    };


}}


#endif
