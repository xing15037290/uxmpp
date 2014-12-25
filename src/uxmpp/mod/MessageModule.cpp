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
#include <uxmpp/xml/names.hpp>


#define THIS_FILE "MessageModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageModule::MessageModule ()
    : uxmpp::XmppModule ("mod_message"),
      sess (nullptr),
      message_handler (nullptr),
      receipt_handler (nullptr)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::module_registered (uxmpp::Session& session)
{
    sess = &session;
    //sess->addSessionListener (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::module_unregistered (uxmpp::Session& session)
{
    //sess->delSessionListener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
// Return true if the jid is in our roster and is authorized to se our presence
//------------------------------------------------------------------------------
static bool is_jid_authorized (uxmpp::Session& session, uxmpp::Jid jid)
{
    for (auto mod : session.get_modules()) {
        if (!mod) // Sanity check
            continue;
        if (RosterModule* rm = dynamic_cast<RosterModule*>(mod)) {
            for (auto& item : rm->get_roster().get_items()) {
                if (item.get_jid().bare() != jid)
                    continue;
                //
                // Jid found.
                //
                string s = item.get_subscription ();
                if (s=="from" || s=="both") {
                    return true;  // Authorized
                }else{
                    uxmpp_log_debug (THIS_FILE, to_string(jid),
                                     " is not authorized to view our presence, don't send a message receipt");
                    return false; // Not authorized
                }
            }
        }
    }
    uxmpp_log_debug (THIS_FILE, to_string(jid), " is not found in our roster, don't send a message receipt");
    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool MessageModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Handle message stanzas
    //
    if (xml_obj.get_full_name() == xml::full_tag_message_stanza) {
        MessageStanza& msg = reinterpret_cast<MessageStanza&> (xml_obj);

        // Check if this is a receipt
        //
        XmlObject receipt = msg.find_node ("urn:xmpp:receipts:received", true);
        if (receipt) {
            // Call registered receipt handler
            if (receipt_handler)
                receipt_handler (*this, msg.get_from(), receipt.get_attribute("id"));

            // The receipt should not include a message body, but if it does we
            // call the registered message handler
            if (msg.find_node("body")) {
                if (message_handler)
                    message_handler (*this, msg);
            }
        }else{
            // Call registered message handler
            if (message_handler)
                message_handler (*this, msg);
        }

        // Check for requested receipt (XEP-0184)
        // Don't send a receipt if the sender is not authorized to view our presence (XEP-0184, section 8).
        //
        XmlObject request = msg.find_node ("urn:xmpp:receipts:request", true);
        if (request && is_jid_authorized(session, msg.get_from().bare())) {
            sess->send_stanza (MessageStanza(msg.get_from(), sess->get_jid()).
                               remove_attribute("type").
                               add_node(XmlObject("received", "urn:xmpp:receipts").
                                        set_attribute("id", msg.get_id())));
        }

        return true;
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<std::string> MessageModule::get_disco_features ()
{
    return {"urn:xmpp:receipts"}; // XEP-0184 - Message Delivery Receipts
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::send_message (const MessageStanza& msg, bool want_receipt)
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
        if (!ms.find_node("urn:xmpp:receipts:request", true))
            ms.add_node (XmlObject("request", "urn:xmpp:receipts"));
    }else{
        //
        // Remove any receipt request
        //
        auto& nodes = ms.get_nodes ();
        for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
            if (i->get_full_name() == "urn:xmpp:receipts:request")
                nodes.erase (i);
        }
    }

    // Send the message
    sess->send_stanza (std::move(ms));
    return;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::send_message (const uxmpp::Jid& to, const std::string& body, bool want_receipt)
{
    // Sanity check
    //
    if (!sess)
        return;

    MessageStanza ms (to, sess->get_jid(), body);
    if (want_receipt)
        ms.add_node (XmlObject("request", "urn:xmpp:receipts"));

    // Send the message
    sess->send_stanza (std::move(ms));
    return;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::set_message_handler (std::function<void (MessageModule&, uxmpp::MessageStanza&)>
                                         on_message)
{
    message_handler = on_message;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MessageModule::set_receipt_handler (std::function<void (MessageModule&, const uxmpp::Jid&, const std::string&)>
                                         on_receipt)
{
    receipt_handler = on_receipt;
}


UXMPP_END_NAMESPACE2
