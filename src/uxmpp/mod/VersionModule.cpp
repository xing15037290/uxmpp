/*
 *  Copyright (C) 2015 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/mod/VersionModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/SessionState.hpp>
#include <uxmpp/xml/names.hpp>
#include <uxmpp/IqStanza.hpp>
#include "config.h"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;

static const std::string log_module {"VersionModule"};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VersionModule::VersionModule ()
    : uxmpp::XmppModule ("mod_version"),
      sess (nullptr),
      name (PACKAGE_NAME),
      version (PACKAGE_VERSION),
      version_cb (nullptr)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VersionModule::module_registered (uxmpp::Session& session)
{
    sess = &session;
    //sess->addSessionListener (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VersionModule::module_unregistered (uxmpp::Session& session)
{
    //sess->delSessionListener (*this);
    sess = nullptr;
    query_ids.clear ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool VersionModule::process_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Sanity check
    //
    if (!sess)
        return false;

    // Only iq stanzas
    //
    if (xml_obj.get_full_name() != xml::full_tag_iq_stanza) {
        return false;
    }

    IqStanza& iq = reinterpret_cast<IqStanza&> (xml_obj);

    // Handle version request
    //
    if (iq.get_type() == IqType::get) {
        XmlObject version_obj = iq.find_node ("jabber:iq:version:query", true);
        if (version_obj) {
            IqStanza response (IqType::result, iq.get_from(), sess->get_jid(), iq.get_id());
            XmlObject version ("query", "jabber:iq:version");
            version.add_node (XmlObject("name").set_content(get_name()));
            version.add_node (XmlObject("version").set_content(get_version()));
            if (!get_os().empty())
                version.add_node (XmlObject("os").set_content(get_os()));
            response.add_node (version);
            sess->send_stanza (response);
            return true;
        }
        return false;
    }

    // Handle version response
    //
    auto query_id = query_ids.find (iq.get_id());
    if (query_id != query_ids.end()) {
        Jid version_owner = query_id->second;
        XmlObject version = iq.find_node ("jabber:iq:version:query", true);
        StanzaError error = iq.get_error ();
        if (iq.have_error())
            uxmpp_log_info (log_module, "IQ error getting version info: ", error.get_condition());

        query_ids.erase (iq.get_id());
        if (version_cb)
            version_cb (*sess, version_owner, version, error);

        return true;
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string VersionModule::get_name ()
{
    return name;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VersionModule::set_name (const std::string& name)
{
    this->name = name;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string VersionModule::get_version ()
{
    return version;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VersionModule::set_version (const std::string& version)
{
    this->version = version;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string VersionModule::get_os ()
{
    return os;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VersionModule::set_os (const std::string& os)
{
    this->os = os;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool VersionModule::request_version (const uxmpp::Jid& jid)
{
    if (!sess) {
        uxmpp_log_warning (log_module, "Unable to send version query - no active session");
        return false;
    }

    string stanza_id = Stanza::make_id ();
    query_ids.insert (pair<string, string>(stanza_id, to_string(jid)));

    sess->send_stanza (IqStanza(IqType::get, jid, sess->get_jid(), stanza_id).
                       add_node(XmlObject("query", "jabber:iq:version")));
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void VersionModule::set_version_callback (version_cb_t callback)
{
    version_cb = callback;
}


UXMPP_END_NAMESPACE2
