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
#include <uxmpp/XmlNames.hpp>


UXMPP_START_NAMESPACE1(uxmpp)

// stream
//
const std::string XmlStreamNs      = "http://etherx.jabber.org/streams";
//const std::string XmlStreamDefNs   = "";
const std::string XmlStreamNsAlias = "stream";
const std::string XmlStreamTag     = "stream";
const std::string XmlStreamTagFull = XmlStreamNs + std::string(":") + XmlStreamTag;

// features
//
const std::string XmlFeaturesTag     = "features";
const std::string XmlFeaturesTagFull = XmlStreamNs + std::string(":") + XmlFeaturesTag;

// jabber:client
//
const std::string XmlJabberClientNs = "jabber:client";

// bind
//
const std::string XmlBindNs      = "urn:ietf:params:xml:ns:xmpp-bind";
const std::string XmlBindTag     = "bind";
const std::string XmlBindTagFull = XmlBindNs + std::string(":") + XmlBindTag;

// compression
//
const std::string XmlCompressionNs      = "http://jabber.org/features/compress";
const std::string XmlCompressionDefNs   = XmlCompressionNs;
const std::string XmlCompressionNsAlias = "";
const std::string XmlCompressionTag     = "compression";
const std::string XmlCompressionTagFull = XmlCompressionNs + std::string(":") + XmlCompressionTag;

// Namespace for stream error names
//
const std::string XmlStreamErrortypeNs = "urn:ietf:params:xml:ns:xmpp-streams";
const std::string XmlStreamErrorTag = "error";
const std::string XmlStreamErrorTagFull = XmlStreamNs + std::string(":") + XmlStreamErrorTag;

// SASL namespace
//
const std::string XmlSaslNs = "urn:ietf:params:xml:ns:xmpp-sasl";
const std::string XmlSaslDefNs = XmlSaslNs;

// Stanzas namespace
//
const std::string XmlStanzasNs = "urn:ietf:params:xml:ns:xmpp-stanzas";
const std::string XmlStanzasDefNs = XmlStanzasNs;

// IQ stanza
//
const std::string XmlIqStanzaTag = "iq";
const std::string XmlIqStanzaTagFull = XmlJabberClientNs + std::string(":") + XmlIqStanzaTag;

// IQ-error stanza
//
const std::string XmlIqErrorStanzaTag = "error";
const std::string XmlIqErrorStanzaTagFull = XmlJabberClientNs + std::string(":") + XmlIqErrorStanzaTag;

// Presence stanza
//
const std::string XmlPresenceStanzaTag = "presence";
const std::string XmlPresenceStanzaTagFull = XmlJabberClientNs + std::string(":") + XmlPresenceStanzaTag;

// Message stanza
//
const std::string XmlMessageStanzaTag = "message";
const std::string XmlMessageStanzaTagFull = XmlJabberClientNs + std::string(":") + XmlMessageStanzaTag;

// Roster query
//
const std::string XmlIqRosterNs = "jabber:iq:roster";
const std::string XmlIqRosterQueryTag = "query";
const std::string XmlIqRosterQueryTagFull = XmlIqRosterNs + std::string(":") + XmlIqRosterQueryTag;

// Roster item
//
const std::string XmlRosterItemTag = "item";
const std::string XmlRosterItemTagFull = XmlIqRosterNs + std::string(":") + XmlRosterItemTag;

// Roster group
//
const std::string XmlRosterGroupTag = "group";
const std::string XmlRosterGroupTagFull = XmlIqRosterNs + std::string(":") + XmlRosterGroupTag;

// Ping namespace
//
const std::string XmlPingNs = "urn:xmpp:ping";
const std::string XmlPingDefNs = XmlPingNs;

// Pubsub namespace
//
const std::string XmlPubsubNs = "http://jabber.org/protocol/pubsub";
const std::string XmlPubsubDefNs = XmlPubsubNs;

// Pubsub errors namespace
//
const std::string XmlPubsuberrorNs = "http://jabber.org/protocol/pubsub#errors";
const std::string XmlPubsuberrorDefNs = XmlPubsuberrorNs;

// Amp namespace
//
const std::string XmlAmpNs = "http://jabber.org/protocol/amp";
const std::string XmlAmpDefNs = XmlAmpNs;

// Amp errors namespace
//
const std::string XmlAmperrorNs = "http://jabber.org/protocol/amp#errors";
const std::string XmlAmperrorDefNs = XmlAmperrorNs;


// uxmpp internal error namespace
//
const std::string XmlUxmppInternalErrorNs = "http://ultramarin.se/uxmpp#internal-error";
const std::string XmlUxmppInternalErrorDefNs = XmlUxmppInternalErrorNs;

// uxmpp internal timer namespace
//
const std::string XmlUxmppInternalTimerNs = "http://ultramarin.se/uxmpp#internal-timer";
const std::string XmlUxmppInternalTimerDefNs = XmlUxmppInternalTimerNs;
const std::string XmlUxmppInternalTimerTagFull = XmlUxmppInternalTimerNs + std::string(":timeout");


UXMPP_END_NAMESPACE1
