/*
 *  Copyright (C) 2014,2015 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_MOD_HPP
#define UXMPP_MOD_HPP

namespace uxmpp {

    /**
     * This namespace contains modules that can be loaded
     * into an XMPP session to extend it's functionality.
     */
    namespace mod {
    }

}

#include <uxmpp/mod/TlsModule.hpp>
#include <uxmpp/mod/AuthModule.hpp>
#include <uxmpp/mod/KeepAliveModule.hpp>
#include <uxmpp/mod/DiscoIdentity.hpp>
#include <uxmpp/mod/DiscoInfo.hpp>
#include <uxmpp/mod/DiscoModule.hpp>
#include <uxmpp/mod/RosterItem.hpp>
#include <uxmpp/mod/Roster.hpp>
#include <uxmpp/mod/RosterModule.hpp>
#include <uxmpp/mod/PresenceModule.hpp>
#include <uxmpp/mod/MessageModule.hpp>
#include <uxmpp/mod/SessionModule.hpp>
#include <uxmpp/mod/PingModule.hpp>
#include <uxmpp/mod/PrivateDataModule.hpp>
#include <uxmpp/mod/RegistrationInfo.hpp>
#include <uxmpp/mod/RegisterModule.hpp>
#include <uxmpp/mod/SearchModule.hpp>
#include <uxmpp/mod/VcardModule.hpp>
#include <uxmpp/mod/IBBModule.hpp>
#include <uxmpp/mod/PepModule.hpp>
#include <uxmpp/mod/PubSubModule.hpp>

#endif
