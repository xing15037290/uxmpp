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
#include <uxmpp/mod/RosterModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/IqStanza.hpp>


#define THIS_FILE "RosterModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterModule::RosterModule ()
    : uxmpp::XmppModule ("mod_roster"),
      sess (nullptr),
      roster_handler (nullptr),
      roster_push_handler (nullptr)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::module_registered (uxmpp::Session& session)
{
    sess = &session;
    //sess->addSessionListener (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::module_unregistered (uxmpp::Session& session)
{
    //sess->delSessionListener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool RosterModule::process_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Handle iq stanzas
    //
    if (xml_obj.get_full_name() == "jabber:client:iq") {
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

        // Check for roster query result
        //
        if (iq.get_id()==roster_query_id && iq.get_type()==IqType::result) {
            XmlObject node = iq.find_node ("jabber:iq:roster:query", true);
            if (node) {
                uxmpp_log_trace (THIS_FILE, "Got roster query result");
                roster = std::move (node);
                // Call registered roster handler
                if (roster_handler != nullptr)
                    roster_handler (*this, roster);

                return true;
            }
        }
        //
        // Check for roster query error
        //
        else if (iq.get_id()==roster_query_id && iq.get_type()==IqType::error) {
            uxmpp_log_debug (THIS_FILE, "Got roster result - roster not found");
            roster = Roster ();
            // Call registered roster handler
            if (roster_handler != nullptr)
                roster_handler (*this, roster);

            return true;
        }
        //
        // Check for iq 'set'
        //
        else if (iq.get_type()==IqType::set) {
            //
            // Check for roster query push
            //
            XmlObject query = iq.find_node ("jabber:iq:roster:query", true);
            //XmlObject node = iq.find_node ("jabber:iq:roster", true);
            XmlObject item = query.find_node ("jabber:iq:roster:item", true);
            if (query && item) {
                //
                // Check 'from' (RFC 6121, section 2.1.6)
                //
                if (to_string(iq.get_from()).empty()!=false &&
                    iq.get_from().bare() != sess->get_jid().bare())
                {
                    uxmpp_log_info (THIS_FILE, "Got roster push with faulty 'from' attribute: ",
                                    to_string(query, true));
                    // Return an empty result stanza
                    sess->send_stanza (IqStanza(IqType::result,
                                                "",
                                                to_string(sess->get_jid()),
                                                iq.get_id()));
                    return true;
                }

                //
                // We got a roster push, deal with it!
                //
                handle_roster_push (reinterpret_cast<RosterItem&>(item));

                // Return an empty result stanza
                sess->send_stanza (IqStanza(IqType::result,
                                            "",
                                            to_string(sess->get_jid()),
                                            iq.get_id()));
                return true;
            }
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::handle_roster_push (RosterItem& item)
{
    bool roster_updated = false;

    uxmpp_log_debug (THIS_FILE, "Got roster push: ", to_string(item, true));

    //for (auto& roster_item : roster.getItems()) {
    auto& items = roster.get_items ();
    for (auto i=items.begin(); i!=items.end(); ++i) {
        if ((*i).get_jid() == item.get_jid()) {
            roster_updated = true;
            // Check if the item is removed
            if (item.get_subscription() == "remove") {
                items.erase (i);
                uxmpp_log_debug (THIS_FILE, "Roster item removed");
            }else{
                (*i) = item;
                uxmpp_log_debug (THIS_FILE, "Roster item updated");
            }
            break;
        }
    }
    if (!roster_updated) {
        roster.add_node (item);
        uxmpp_log_debug (THIS_FILE, "Roster item added");
    }

    // Call roster push handler
    if (roster_push_handler)
        roster_push_handler (*this, item);
}


//------------------------------------------------------------------------------
// RFC 6121 Section 2.1.3
//------------------------------------------------------------------------------
void RosterModule::refresh ()
{
    if (!sess || sess->get_state()!=SessionState::bound) {
        uxmpp_log_debug (THIS_FILE, "Can't query roster, no session or session not bound");
        return;
    }
    roster_query_id = string("rq") + sess->get_id();
    sess->send_stanza (IqStanza(IqType::get, "", to_string(sess->get_jid()), roster_query_id).
                       add_node(XmlObject("query", "jabber:iq:roster")));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Roster& RosterModule::get_roster ()
{
    return roster;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::update_item (const RosterItem& item)
{
    roster_set (item);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::add_item (const RosterItem& item)
{
    roster_set (item);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::add_item (const uxmpp::Jid& jid)
{
    roster_set (RosterItem(jid));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::remove_item (const RosterItem& item)
{
    roster_set (item, true);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::remove_item (const uxmpp::Jid& jid)
{
    roster_set (RosterItem(jid), true);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::set_roster_handler (std::function<void (RosterModule&, Roster&)> on_roster)
{
    roster_handler = on_roster;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::set_roster_push_handler (std::function<void (RosterModule&, RosterItem&)> on_roster_push)
{
    roster_push_handler = on_roster_push;
}


//------------------------------------------------------------------------------
// RFC 6121 Section 2.1.5/2.3/2.4/2.5
// Roster Set / Adding a Roster Item / Updating a Roster Item / Deleting a Roster Item
//------------------------------------------------------------------------------
void RosterModule::roster_set (const RosterItem& item, bool remove)
{
    if (!sess || sess->get_state()!=SessionState::bound) {
        uxmpp_log_debug (THIS_FILE, "Can't ",
                         (remove?"remove":"add/update") ,
                         " roster item, no session or session not bound");
        return;
    }
    RosterItem roster_item (item);

    // RFC6121 section 2.1.2.1 - A client MUST NOT include the 'approved' attribute
    roster_item.remove_attribute ("approved");

    // RFC6121 section 2.1.2.2 - A client MUST NOT include the 'ask' attribute
    roster_item.remove_attribute ("ask");

    if (remove) {
        // RFC6121 section 2.1.5 - The server MUST ignore any value of
        //                         the 'subscription' attribute other than "remove"
        roster_item.set_attribute ("subscription", "remove");
    }else{
        // RFC6121 section 2.1.2.5 - Inclusion of the 'subscription' attribute is OPTIONAL
        roster_item.remove_attribute ("subscription");
    }

    // Send the stanza
    //
    sess->send_stanza (IqStanza(IqType::set, "", to_string(sess->get_jid()), (remove?"itemremove":"itemupdate")).
                       add_node(XmlObject("query", "jabber:iq:roster", true, true, 1).
                                add_node(roster_item)));
}


UXMPP_END_NAMESPACE2
