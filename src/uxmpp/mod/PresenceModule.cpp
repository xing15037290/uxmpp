/*
 *  Copyright (C) 2014-2015 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/Logger.hpp>
#include <uxmpp/mod/PresenceModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/PresenceStanza.hpp>
#include <uxmpp/xml/names.hpp>


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;

static const std::string log_module {"PresenceModule"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceModule::PresenceModule ()
    : uxmpp::XmppModule ("mod_presence"),
      sess (nullptr),
      presence_handler (nullptr)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::module_registered (uxmpp::Session& session)
{
    sess = &session;
    //sess->addSessionListener (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::module_unregistered (uxmpp::Session& session)
{
    //sess->delSessionListener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PresenceModule::process_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Handle iq stanzas
    //
    if (xml_obj.get_full_name() == xml::full_tag_presence_stanza) {
        PresenceStanza& pr = reinterpret_cast<PresenceStanza&> (xml_obj);
        // Call registered presence handler
        if (presence_handler)
            presence_handler (*this, pr);

        return true;
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::announce (const unsigned last_active)
{
    if (!sess || sess->get_state()!=SessionState::bound) {
        uxmpp_log_trace (log_module, "Can't announce our presence, no session or session not bound");
        return;
    }
    sess->send_stanza (PresenceStanza("", "", last_active));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::request_subscription (const uxmpp::Jid& jid)
{
    if (!sess || sess->get_state()!=SessionState::bound) {
        uxmpp_log_debug (log_module, "Can't request subscription, no session or session not bound");
        return;
    }

    sess->send_stanza (PresenceStanza(to_string(jid.bare()), "", 0, Stanza::make_id()).
                       set_subscribe_op(SubscribeOp::subscribe));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::accept_subscription (const uxmpp::Jid& jid)
{
    if (!sess || sess->get_state()!=SessionState::bound) {
        uxmpp_log_debug (log_module, "Can't accept subscription, no session or session not bound");
        return;
    }

#warning Check server for pre-approval support

    sess->send_stanza (PresenceStanza(to_string(jid.bare()), "", 0, Stanza::make_id()).
                       set_subscribe_op(SubscribeOp::subscribed));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::deny_subscription (const uxmpp::Jid& jid)
{
    if (!sess || sess->get_state()!=SessionState::bound) {
        uxmpp_log_debug (log_module, "Can't deny subscription, no session or session not bound");
        return;
    }

    sess->send_stanza (PresenceStanza(to_string(jid.bare()), "", 0, Stanza::make_id()).
                       set_subscribe_op(SubscribeOp::unsubscribed));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::cancel_subscription (const uxmpp::Jid& jid)
{
    if (!sess || sess->get_state()!=SessionState::bound) {
        uxmpp_log_debug (log_module, "Can't cancel subscription, no session or session not bound");
        return;
    }

    sess->send_stanza (PresenceStanza(to_string(jid.bare()), "", 0, Stanza::make_id()).
                       set_subscribe_op(SubscribeOp::unsubscribe));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::set_presence_handler (std::function<void (PresenceModule&, uxmpp::PresenceStanza&)>
                                           on_presence)
{
    presence_handler = on_presence;
}



UXMPP_END_NAMESPACE2
