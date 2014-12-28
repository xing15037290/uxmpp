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
#include <uxmpp/xml/names.hpp>


UXMPP_START_NAMESPACE2(uxmpp, xml)

//
// Uxmpp internal
//
const std::string namespace_uxmpp_error  {"http://ultramarin.se/uxmpp#internal-error"};
const std::string namespace_uxmpp_timer  {"http://ultramarin.se/uxmpp#internal-timer"};
const std::string full_tag_uxmpp_timeout {"http://ultramarin.se/uxmpp#internal-timer:timeout"};

//
// stream
//
const std::string namespace_stream {"http://etherx.jabber.org/streams"};
const std::string alias_stream     {"stream"};
const std::string tag_stream       {"stream"};
const std::string full_tag_stream  {"http://etherx.jabber.org/streams:stream"};

//
// features
//
const std::string tag_features      {"features"};
const std::string full_tag_features {"http://etherx.jabber.org/streams:features"};

//
// jabber:client
//
const std::string namespace_jabber_client {"jabber:client"};

//
// bind
//
const std::string namespace_bind {"urn:ietf:params:xml:ns:xmpp-bind"};
const std::string tag_bind       {"bind"};

//
// namespace xmpp-streams
//
const std::string namespace_xmpp_streams {"urn:ietf:params:xml:ns:xmpp-streams"};

//
// error
//
const std::string full_tag_error {"http://etherx.jabber.org/streams:error"};

//
// IQ stanza
//
const std::string full_tag_iq_stanza {"jabber:client:iq"};

//
// Error stanza
//
const std::string full_tag_error_stanza {"jabber:client:error"};

//
// Presence stanza
//
const std::string full_tag_presence_stanza {"jabber:client:presence"};

//
// Message stanza
//
const std::string full_tag_message_stanza {"jabber:client:message"};

//
// Namespace for stanza errors
//
const std::string namespace_stanza_error {"urn:ietf:params:xml:ns:xmpp-stanzas"};

//
// IQ roster namespace
//
const std::string namespace_iq_roster {"jabber:iq:roster"};

//
// Roster item full tag
//
const std::string full_tag_roster_item {"jabber:iq:roster:item"};

//
// Roster group full tag
//
const std::string full_tag_roster_group {"jabber:iq:roster:group"};



UXMPP_END_NAMESPACE2
