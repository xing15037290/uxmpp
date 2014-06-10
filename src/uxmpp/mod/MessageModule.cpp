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
#include <uxmpp/mod/MessageModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/MessageStanza.hpp>
#include <uxmpp/mod/RosterModule.hpp>


#define THIS_FILE "MessageModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


static const std::string XmlMessageTag     {"message"};
static const std::string XmlMessageTagFull {"jabber:client:message"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageModule::MessageModule ()
    : uxmpp::XmppModule ("mod_message"),
      sess (nullptr)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::addMessageListener (MessageModuleListener& listener)
{
    for (auto& l : listeners) {
        if (l == &listener)
            return; // Already added
    }
    listeners.push_back (&listener);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::delMessageListener (MessageModuleListener& listener)
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
void MessageModule::moduleRegistered (uxmpp::Session& session)
{
    sess = &session;
    //sess->addSessionListener (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::moduleUnregistered (uxmpp::Session& session)
{
    //sess->delSessionListener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
// Return true if the jid is in our roster and is authorized to se our presence
//------------------------------------------------------------------------------
static bool is_jid_authorized (uxmpp::Session& session, uxmpp::Jid jid)
{
    for (auto mod : session.getModules()) {
        if (!mod) // Sanity check
            continue;
        if (RosterModule* rm = dynamic_cast<RosterModule*>(mod)) {
            for (auto& item : rm->getRoster().getItems()) {
                if (item.getJid().bare() != jid)
                    continue;
                //
                // Jid found.
                //
                string s = item.getSubscription ();
                if (s=="from" || s=="both") {
                    return true;  // Authorized
                }else{
                    uxmppLogDebug (THIS_FILE, to_string(jid),
                                   " is not authorized to view our presence, don't send a message receipt");
                    return false; // Not authorized
                }
            }
        }
    }
    uxmppLogDebug (THIS_FILE, to_string(jid), " is not found in our roster, don't send a message receipt");
    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool MessageModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Handle message stanzas
    //
    if (xml_obj.getFullName() == XmlMessageTagFull) {
        MessageStanza& msg = reinterpret_cast<MessageStanza&> (xml_obj);

        // Check if this is a receipt
        //
        XmlObject receipt = msg.getNode ("urn:xmpp:receipts:received", true);
        if (receipt) {
            // Inform listeners of incoming receipt
            for (auto& listener : listeners)
                listener->onReceipt (*this, msg.getFrom(), receipt.getAttribute("id"));

            // The receipt should not include a message body, but if it does we
            // inform the listeners of an incoming message.
            if (msg.getNode("body")) {
                for (auto& listener : listeners) {
                    listener->onMessage (*this, msg);
                }
            }
        }else{
            // Inform listeners of incoming message
            for (auto& listener : listeners)
                listener->onMessage (*this, msg);
        }

        // Check for requested receipt (XEP-0184)
        // Don't send a receipt if the sender is not authorized to view our presence (XEP-0184, section 8).
        //
        XmlObject request = msg.getNode ("urn:xmpp:receipts:request", true);
        if (request && is_jid_authorized(session, msg.getFrom().bare())) {
            sess->sendStanza (MessageStanza(msg.getFrom(), sess->getJid()).
                              removeAttribute("type").
                              addNode(XmlObject("received", "urn:xmpp:receipts").
                                      setAttribute("id", msg.getId())));
        }

        return true;
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::sendMessage (const MessageStanza& msg, bool want_receipt)
{
    // Sanity check
    //
    if (!sess)
        return;

    MessageStanza ms (msg);
    if (want_receipt) {
        //
        // Add a receipt request if not already added
        //
        if (!ms.getNode("urn:xmpp:receipts:request", true))
            ms.addNode (XmlObject("request", "urn:xmpp:receipts"));
    }else{
        //
        // Remove any receipt request
        //
        auto& nodes = ms.getNodes ();
        for (auto i=nodes.begin(); i!=nodes.end(); i++) {
            if (i->getFullName() == "urn:xmpp:receipts:request")
                nodes.erase (i);
        }
    }

    // Send the message
    sess->sendStanza (std::move(ms));
    return;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::sendMessage (const uxmpp::Jid& to, const std::string& body, bool want_receipt)
{
    // Sanity check
    //
    if (!sess)
        return;

    MessageStanza ms (to, sess->getJid(), body);
    if (want_receipt)
        ms.addNode (XmlObject("request", "urn:xmpp:receipts"));

    // Send the message
    sess->sendStanza (std::move(ms));
    return;
}


UXMPP_END_NAMESPACE2
