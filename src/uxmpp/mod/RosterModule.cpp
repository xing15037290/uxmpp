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
#include <uxmpp/mod/RosterModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/IqStanza.hpp>


#define THIS_FILE "RosterModule"


START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterModule::RosterModule ()
    : uxmpp::XmppModule ("mod_roster"),
      sess (nullptr)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::addRosterListener (RosterModuleListener& listener)
{
    for (auto& l : listeners) {
        if (l == &listener)
            return; // Already added
    }
    listeners.push_back (&listener);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::delRosterListener (RosterModuleListener& listener)
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
void RosterModule::moduleRegistered (uxmpp::Session& session)
{
    sess = &session;
    //sess->addSessionListener (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::moduleUnregistered (uxmpp::Session& session)
{
    //sess->delSessionListener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool RosterModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Handle iq stanzas
    //
    if (xml_obj.getFullName() == "jabber:client:iq") {
        IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

        // Check for roster query result
        //
        if (iq.getId()==roster_query_id && iq.getType()==IqType::result) {
            XmlObject node = iq.getNode ("jabber:iq:roster:query", true);
            if (node) {
                uxmppLogTrace (THIS_FILE, "Got roster query result");
                roster = std::move (node);
                // Inform listeners
                for (auto& listener : listeners)
                    listener->onRoster (*this, roster);

                return true;
            }
        }
        //
        // Check for roster query error
        //
        else if (iq.getId()==roster_query_id && iq.getType()==IqType::error) {
            uxmppLogDebug (THIS_FILE, "Got roster result - roster not found");
            roster = Roster ();
            // Inform listeners
            for (auto& listener : listeners)
                listener->onRoster (*this, roster);
            return true;
        }
        //
        // Check for iq 'set'
        //
        else if (iq.getType()==IqType::set) {
            //
            // Check for roster query push
            //
            XmlObject query = iq.getNode ("jabber:iq:roster:query", true);
            //XmlObject node = iq.getNode ("jabber:iq:roster", true);
            XmlObject item = query.getNode ("jabber:iq:roster:item", true);
            if (query && item) {
                //
                // Check 'from' (RFC 6121, section 2.1.6)
                //
                if (to_string(iq.getFrom()).length()!=0 &&
                    iq.getFrom().bare() != sess->getJid().bare())
                {
                    uxmppLogInfo (THIS_FILE, "Got roster push with faulty 'from' attribute: ",
                                  to_string(query, true));
                    // Return an empty result stanza
                    sess->sendStanza (IqStanza(IqType::result,
                                               "",
                                               to_string(sess->getJid()),
                                               iq.getId()));
                    return true;
                }

                //
                // We got a roster push, deal with it!
                //
                handleRosterPush (reinterpret_cast<RosterItem&>(item));

                // Return an empty result stanza
                sess->sendStanza (IqStanza(IqType::result,
                                           "",
                                           to_string(sess->getJid()),
                                           iq.getId()));
                return true;
            }
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterModule::handleRosterPush (RosterItem& item)
{
    bool roster_updated = false;

    uxmppLogDebug (THIS_FILE, "Got roster push: ", to_string(item, true));

    //for (auto& roster_item : roster.getItems()) {
    auto& items = roster.getItems ();
    for (auto i=items.begin(); i!=items.end(); ++i) {
        if ((*i).getJid() == item.getJid()) {
            roster_updated = true;
            // Check if the item is removed
            if (item.getSubscription() == "remove") {
                items.erase (i);
                uxmppLogDebug (THIS_FILE, "Roster item removed");
            }else{
                (*i) = item;
                uxmppLogDebug (THIS_FILE, "Roster item updated");
            }
            break;
        }
    }
    if (!roster_updated) {
        roster.addNode (item);
        uxmppLogDebug (THIS_FILE, "Roster item added");
    }

    // Inform listeners
    for (auto& listener : listeners)
        listener->onRosterPush (*this, item);
}


//------------------------------------------------------------------------------
// RFC 6121 Section 2.1.3
//------------------------------------------------------------------------------
void RosterModule::refresh ()
{
    if (!sess || sess->getState()!=SessionState::bound) {
        uxmppLogDebug (THIS_FILE, "Can't query roster, no session or session not bound");
        return;
    }
    roster_query_id = string("rq") + sess->getId();
    sess->sendStanza (IqStanza(IqType::get, "", to_string(sess->getJid()), roster_query_id).
                      addNode(XmlObject("query", "jabber:iq:roster")));
}


//------------------------------------------------------------------------------
// RFC 6121 Section 2.1.5/2.3/2.4/2.5
// Roster Set / Adding a Roster Item / Updating a Roster Item / Deleting a Roster Item
//------------------------------------------------------------------------------
void RosterModule::rosterSet (const RosterItem& item, bool remove)
{
    if (!sess || sess->getState()!=SessionState::bound) {
        uxmppLogDebug (THIS_FILE, "Can't ",
                       (remove?"remove":"add/update") ,
                       " roster item, no session or session not bound");
        return;
    }
    RosterItem roster_item (item);

    // RFC6121 section 2.1.2.1 - A client MUST NOT include the 'approved' attribute
    roster_item.removeAttribute ("approved");

    // RFC6121 section 2.1.2.2 - A client MUST NOT include the 'ask' attribute
    roster_item.removeAttribute ("ask");

    if (remove) {
        // RFC6121 section 2.1.5 - The server MUST ignore any value of
        //                         the 'subscription' attribute other than "remove"
        roster_item.setAttribute ("subscription", "remove");
    }else{
        // RFC6121 section 2.1.2.5 - Inclusion of the 'subscription' attribute is OPTIONAL
        roster_item.removeAttribute ("subscription");
    }

    // Send the stanza
    //
    sess->sendStanza (IqStanza(IqType::set, "", to_string(sess->getJid()), (remove?"itemremove":"itemupdate")).
                      addNode(XmlObject("query", "jabber:iq:roster", true, true, 1).
                              addNode(roster_item)));
}


END_NAMESPACE2
