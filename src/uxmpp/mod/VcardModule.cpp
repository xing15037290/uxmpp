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
#include <uxmpp/mod/VcardModule.hpp>
#include <uxmpp/Logger.hpp>
#include <uxmpp/Stanza.hpp>
#include <uxmpp/IqStanza.hpp>
#include <uxmpp/xml/names.hpp>


UXMPP_START_NAMESPACE2(uxmpp, mod)

using namespace std;
using namespace uxmpp;

static const string log_module {"VcardModule"};

static const string namespace_vcard {"vcard-temp"};
static const string full_tag_name_vcard {"vcard-temp:vCard"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VcardModule::VcardModule ()
    :
    XmppModule ("mod_search"),
    sess {nullptr},
    set_id {""}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VcardModule::module_registered (uxmpp::Session& session)
{
    set_id = "";
    query_ids.clear ();
    sess = &session;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VcardModule::module_unregistered (uxmpp::Session& session)
{
    sess = nullptr;
    set_id = "";
    query_ids.clear ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool VcardModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // We're only interested in IQ stanzas
    //
    if (xml_obj.get_full_name() != xml::full_tag_iq_stanza)
        return false;

    IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

    // Check for request vcard result
    //
    if (query_ids.find(iq.get_id()) != query_ids.end()) {

        Jid vcard_owner   = iq.have_attribute("from") ? iq.get_from().bare() : sess->get_jid().bare();
        XmlObject vcard   = iq.find_node (full_tag_name_vcard, true);
        StanzaError error = iq.get_error ();
        if (iq.have_error())
            uxmpp_log_info (log_module, "Error getting search fields info: ", error.get_condition());

        query_ids.erase (iq.get_id());
        if (vcard_cb)
            vcard_cb (*sess, vcard_owner, vcard, error);
        
        return true;
    }

    // Check for set vcard result
    //
    if (!set_id.empty() && set_id == iq.get_id()) {
        set_id = "";
        if (iq.have_error()) {
            StanzaError error = iq.get_error ();
            uxmpp_log_info (log_module, "Error setting vCard: ", error.get_condition());
        }
        return true;
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool VcardModule::request_vcard ()
{
    if (!sess) {
        uxmpp_log_warning (log_module, "Unable to send vCard query - no active session");
        return false;
    }

    string stanza_id = Stanza::make_id ();
    query_ids.insert (stanza_id);

    sess->send_stanza (IqStanza(IqType::get, "", to_string(sess->get_jid()), stanza_id).
                       add_node(XmlObject("vCard", namespace_vcard)));
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool VcardModule::request_vcard (const uxmpp::Jid& jid)
{
    if (!sess) {
        uxmpp_log_warning (log_module, "Unable to send vCard query - no active session");
        return false;
    }

    string stanza_id = Stanza::make_id ();
    query_ids.insert (stanza_id);

    sess->send_stanza (IqStanza(IqType::get, jid.bare(), sess->get_jid(), stanza_id).
                       add_node(XmlObject("vCard", namespace_vcard)));
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool VcardModule::set_vcard (const uxmpp::XmlObject& vcard)
{
    if (!sess) {
        uxmpp_log_warning (log_module, "Unable to set vCard query - no active session");
        return false;
    }
    if (vcard.get_full_name() != full_tag_name_vcard) {
        uxmpp_log_debug (log_module, "Unable to set vCard - not a vCard object");
        return false;
    }

    if (!set_id.empty()) {
        uxmpp_log_warning (log_module, "vCard already sent");
        return false;
    }

    string set_id = Stanza::make_id ();
    sess->send_stanza (IqStanza(IqType::set, "", to_string(sess->get_jid()), set_id).add_node(vcard));
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VcardModule::set_vcard_callback (vcard_cb_t callback)
{
    vcard_cb = callback;
}


UXMPP_END_NAMESPACE2
