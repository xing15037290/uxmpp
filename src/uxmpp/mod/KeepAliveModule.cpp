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
    keep_alive.setPart (XmlObjPart::body);
    keep_alive.setContent (" ");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void KeepAliveModule::moduleRegistered (uxmpp::Session& session)
{
    sess = &session;
    sess->addSessionListener (*this);

    if (sess->getState() == SessionState::bound) {
        // Start the keep-alive timer
        if (interval && sess)
            sess->getXmlStream().setTimeout ("keep-alive", interval * 1000, true);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void KeepAliveModule::moduleUnregistered (uxmpp::Session& session)
{
    sess->delSessionListener (*this);
    sess = nullptr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool KeepAliveModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    // Check timer events
    //
    if (xml_obj.getFullName() == XmlUxmppTimerTagFull) {
        //
        // Check the keep-alive timer
        //
        if (xml_obj.getAttribute("id") == "keep-alive") {
            if (interval)
                sess->getXmlStream().setTimeout ("keep-alive", interval * 1000, true);
            uxmppLogTrace (THIS_FILE, "Send keep-alive");
            sess->getXmlStream().write (keep_alive);
            return true;
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void KeepAliveModule::onStateChange (uxmpp::Session& session,
                                     uxmpp::SessionState new_state,
                                     uxmpp::SessionState old_state)
{
    if (new_state == SessionState::bound) {
        // Start the keep-alive timer
        if (interval && sess)
            sess->getXmlStream().setTimeout ("keep-alive", interval * 1000, true);
    }
    else if (old_state == SessionState::bound) {
        // Stop the keep-alive timer
        if (sess)
            sess->getXmlStream().setTimeout ("keep-alive", 0);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void KeepAliveModule::setInterval (unsigned new_interval)
{
    interval = new_interval;

    if (!sess)
        return;

    if (interval == 0)
        sess->getXmlStream().setTimeout ("keep-alive", 0, true);
    else if (sess->getState()==SessionState::bound)
        sess->getXmlStream().setTimeout ("keep-alive", interval * 1000, true);
}



UXMPP_END_NAMESPACE2
