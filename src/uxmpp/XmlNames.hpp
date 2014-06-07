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
#ifndef UXMPP_XMLNAMES_HPP
#define UXMPP_XMLNAMES_HPP

#include <uxmpp/types.hpp>
#include <string>


namespace uxmpp {


// stream
//
extern const std::string XmlStreamNs;
//extern const std::string XmlStreamDefNs;
extern const std::string XmlStreamNsAlias;
extern const std::string XmlStreamTag;
extern const std::string XmlStreamTagFull;

// features
//
extern const std::string XmlFeaturesTag;
extern const std::string XmlFeaturesTagFull;

// jabber:client
//
extern const std::string XmlJabberClientNs;
extern const std::string XmlJabberClientDefNs;

// bind
//
extern const std::string XmlBindNs;
extern const std::string XmlBindDefNs;
extern const std::string XmlBindNsAlias;
extern const std::string XmlBindTag;
extern const std::string XmlBindTagFull;

// compression
//
extern const std::string XmlCompressionNs;
extern const std::string XmlCompressionDefNs;
extern const std::string XmlCompressionNsAlias;
extern const std::string XmlCompressionTag;
extern const std::string XmlCompressionTagFull;

// Stream error namespace
//
extern const std::string XmlStreamErrortypeNs;
extern const std::string XmlStreamErrortypeDefNs;
extern const std::string XmlStreamErrorTag;
extern const std::string XmlStreamErrorTagFull;

// SASL namespace
//
extern const std::string XmlSaslNs;
extern const std::string XmlSaslDefNs;

// Stanzas namespace
//
extern const std::string XmlStanzasNs;
extern const std::string XmlStanzasDefNs;

// IQ stanza
//
extern const std::string XmlIqStanzaTag;
extern const std::string XmlIqStanzaTagFull;

// IQ-error stanza
//
extern const std::string XmlIqErrorStanzaTag;
extern const std::string XmlIqErrorStanzaTagFull;

// Presence stanza
//
extern const std::string XmlPresenceStanzaTag;
extern const std::string XmlPresenceStanzaTagFull;

// Message stanza
//
extern const std::string XmlMessageStanzaTag;
extern const std::string XmlMessageStanzaTagFull;

// Roster query
//
extern const std::string XmlIqRosterNs;
extern const std::string XmlIqRosterQueryTag;
extern const std::string XmlIqRosterQueryTagFull;

// Roster item
//
extern const std::string XmlRosterItemTag;
extern const std::string XmlRosterItemTagFull;

// Roster group
//
extern const std::string XmlRosterGroupTag;
extern const std::string XmlRosterGroupTagFull;

// Ping namespace
//
extern const std::string XmlPingNs;
extern const std::string XmlPingDefNs;

// Pubsub namespace
//
extern const std::string XmlPubsubNs;
extern const std::string XmlPubsubDefNs;

// Pubsub errors namespace
//
extern const std::string XmlPubsuberrorNs;
extern const std::string XmlPubsuberrorDefNs;

// Amp namespace
//
extern const std::string XmlAmpNs;
extern const std::string XmlAmpDefNs;

// Amp errors namespace
//
extern const std::string XmlAmperrorNs;
extern const std::string XmlAmperrorDefNs;

// uxmpp error namespace
//
extern const std::string XmlUxmppErrorNs;
extern const std::string XmlUxmppErrorDefNs;

// uxmpp timer namespace
//
extern const std::string XmlUxmppTimerNs;
extern const std::string XmlUxmppTimerDefNs;

// uxmpp timer tag
//
extern const std::string XmlUxmppTimerTag;
extern const std::string XmlUxmppTimerTagFull;



}


#endif
