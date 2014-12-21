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
#ifndef UXMPP_HPP
#define UXMPP_HPP

/*! \mainpage Ultramarin XMPP library (uxmpp)
 *
 * uxmpp is an XMPP client stack written in C++ that implements RFC6120, RFC6121 and a number of XEPs.
 *
 */


/**
 * This is the top-level namespace for all classes in libuxmpp.
 */
namespace uxmpp {
}

#include <uxmpp/uxmpp_config.hpp>
#include <uxmpp/types.hpp>
#include <uxmpp/UxmppException.hpp>
#include <uxmpp/Logger.hpp>

#include <uxmpp/Semaphore.hpp>
#include <uxmpp/Jid.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/StreamXmlObj.hpp>
#include <uxmpp/XmlStream.hpp>
#include <uxmpp/XmlStreamListener.hpp>
#include <uxmpp/Stanza.hpp>
#include <uxmpp/IqStanza.hpp>
#include <uxmpp/MessageStanza.hpp>
#include <uxmpp/PresenceStanza.hpp>
#include <uxmpp/StreamError.hpp>
#include <uxmpp/XmppModule.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/SessionConfig.hpp>
#include <uxmpp/SessionListener.hpp>
#include <uxmpp/Session.hpp>

#include <uxmpp/xml.hpp>
#include <uxmpp/io.hpp>
#include <uxmpp/mod.hpp>

#endif
