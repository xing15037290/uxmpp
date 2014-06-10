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
#include <uxmpp/Logger.hpp>
#include <uxmpp/mod/PresenceModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/PresenceStanza.hpp>


#define THIS_FILE "PresenceModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;

static const string XmlPresenceTag     {"presence"};
static const string XmlPresenceTagFull {"jabber:client:presence"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceModule::PresenceModule ()
    : uxmpp::XmppModule ("mod_presence"),
      sess (nullptr)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::addPresenceListener (PresenceModuleListener& listener)
{
    for (auto& l : listeners) {
        if (l == &listener)
            return; // Already added
    }
    listeners.push_back (&listener);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::delPresenceListener (PresenceModuleListener& listener)
{
    for (auto i=listeners.begin(); i!=listeners.end(); i++) {
        if ((*i) == &listener) {
            listeners.erase (i);
            return;
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::moduleRegistered (uxmpp::Session& session)
{
    sess = &session;
    //sess->addSessionListener (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::moduleUnregistered (uxmpp::Session& session)
{
    //sess->delSessionListener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PresenceModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Handle iq stanzas
    //
    if (xml_obj.getFullName() == XmlPresenceTagFull) {
        PresenceStanza& pr = reinterpret_cast<PresenceStanza&> (xml_obj);
        // Inform listeners
        for (auto& listener : listeners)
            listener->onPresence (*this, pr);
        return true;
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::announce ()
{
    if (!sess || sess->getState()!=SessionState::bound) {
        uxmppLogTrace (THIS_FILE, "Can't announce our presence, no session or session not bound");
        return;
    }
    sess->sendStanza (PresenceStanza());
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::requestSubscription (const uxmpp::Jid& jid)
{
    if (!sess || sess->getState()!=SessionState::bound) {
        uxmppLogDebug (THIS_FILE, "Can't request subscription, no session or session not bound");
        return;
    }

    sess->sendStanza (PresenceStanza(to_string(jid.bare()), "", Stanza::makeId()).
                      setSubscribeOp(SubscribeOp::subscribe));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::acceptSubscription (const uxmpp::Jid& jid)
{
    if (!sess || sess->getState()!=SessionState::bound) {
        uxmppLogDebug (THIS_FILE, "Can't accept subscription, no session or session not bound");
        return;
    }

#warning Check server for pre-approval support

    sess->sendStanza (PresenceStanza(to_string(jid.bare()), "", Stanza::makeId()).
                      setSubscribeOp(SubscribeOp::subscribed));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::denySubscription (const uxmpp::Jid& jid)
{
    if (!sess || sess->getState()!=SessionState::bound) {
        uxmppLogDebug (THIS_FILE, "Can't deny subscription, no session or session not bound");
        return;
    }

    sess->sendStanza (PresenceStanza(to_string(jid.bare()), "", Stanza::makeId()).
                      setSubscribeOp(SubscribeOp::unsubscribed));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PresenceModule::cancelSubscription (const uxmpp::Jid& jid)
{
    if (!sess || sess->getState()!=SessionState::bound) {
        uxmppLogDebug (THIS_FILE, "Can't cancel subscription, no session or session not bound");
        return;
    }

    sess->sendStanza (PresenceStanza(to_string(jid.bare()), "", Stanza::makeId()).
                      setSubscribeOp(SubscribeOp::unsubscribe));
}



UXMPP_END_NAMESPACE2
