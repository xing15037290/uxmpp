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
#include <uxmpp/mod/KeepAliveModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/XmlNames.hpp>


#define THIS_FILE "KeepAliveModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
KeepAliveModule::KeepAliveModule (const unsigned interval)
    : XmppModule ("mod_keepalive")
{
    this->interval = interval;
    sess = nullptr;
    keep_alive.set_part (XmlObjPart::body);
    keep_alive.set_content (" ");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void KeepAliveModule::module_registered (uxmpp::Session& session)
{
    sess = &session;
    sess->add_session_listener (*this);

    if (sess->get_state() == SessionState::bound) {
        // Start the keep-alive timer
        if (interval && sess)
            sess->get_xml_stream().set_timeout ("keep-alive", interval * 1000, true);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void KeepAliveModule::module_unregistered (uxmpp::Session& session)
{
    sess->del_session_listener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool KeepAliveModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Check timer events
    //
    if (xml_obj.get_full_name() == XmlUxmppTimerTagFull) {
        //
        // Check the keep-alive timer
        //
        if (xml_obj.get_attribute("id") == "keep-alive") {
            if (interval)
                sess->get_xml_stream().set_timeout ("keep-alive", interval * 1000, true);
            uxmpp_log_trace (THIS_FILE, "Send keep-alive");
            sess->get_xml_stream().write (keep_alive);
            return true;
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void KeepAliveModule::on_state_change (uxmpp::Session& session,
                                       uxmpp::SessionState new_state,
                                       uxmpp::SessionState old_state)
{
    if (new_state == SessionState::bound) {
        // Start the keep-alive timer
        if (interval && sess)
            sess->get_xml_stream().set_timeout ("keep-alive", interval * 1000, true);
    }
    else if (old_state == SessionState::bound) {
        // Stop the keep-alive timer
        if (sess)
            sess->get_xml_stream().set_timeout ("keep-alive", 0);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void KeepAliveModule::set_interval (unsigned new_interval)
{
    interval = new_interval;

    if (!sess)
        return;

    if (interval == 0)
        sess->get_xml_stream().set_timeout ("keep-alive", 0, true);
    else if (sess->get_state()==SessionState::bound)
        sess->get_xml_stream().set_timeout ("keep-alive", interval * 1000, true);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned KeepAliveModule::get_interval () const
{
    return interval;
}



UXMPP_END_NAMESPACE2
