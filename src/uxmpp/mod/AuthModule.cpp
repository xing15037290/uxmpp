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
#include <uxmpp/mod/AuthModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/utils.hpp>


#define THIS_FILE "AuthModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;


static const std::string XmlSaslNs = "urn:ietf:params:xml:ns:xmpp-sasl";
static const std::string XmlMechanismsTag = "mechanisms";
static const std::string XmlMechanismTag  = "mechanism";
static const std::string XmlMechanismsTagFull = XmlSaslNs + string(":") + XmlMechanismsTag;
static const std::string XmlMechanismTagFull  = XmlSaslNs + string(":") + XmlMechanismTag;
static const std::string XmlChallengeTag      = "challenge";
static const std::string XmlChallengeTagFull  = XmlSaslNs + string(":") + XmlChallengeTag;
static const std::string XmlSuccessTag      = "success";
static const std::string XmlSuccessTagFull  = XmlSaslNs + string(":") + XmlSuccessTag;
static const std::string XmlFailureTag      = "failure";
static const std::string XmlFailureTagFull  = XmlSaslNs + string(":") + XmlFailureTag;

static const std::string XmlIqAuthNs = "http://jabber.org/features/iq-auth";
static const std::string XmlAuthTag     = "auth";
static const std::string XmlAuthTagFull = XmlIqAuthNs + string(":") + XmlAuthTag;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool AuthModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    XmlStream& xs = session.getXmlStream ();
    bool start_auth = false;
    bool have_mechanisms = false;

    //
    // Handle 'features'
    //
    if (xml_obj.getFullName() ==  XmlFeaturesTagFull) {
        for (auto& node : xml_obj.getNodes()) {
            if (node.getFullName() == XmlMechanismsTagFull) {
                mechanisms.clear ();
                have_mechanisms = true;
                //
                // Store authentication methods
                //
                for (auto& subnode : node.getNodes()) {
                    if (subnode.getFullName() == XmlMechanismTagFull) {
                        uxmppLogTrace (THIS_FILE, "Got authentication mechanism: ", subnode.getContent());
                        mechanisms.insert (subnode.getContent());
                    }
                }
            }
            else if (node.getFullName() == XmlAuthTagFull) {
                start_auth = true;
                break;
            }
        }
    }
    if (start_auth || have_mechanisms/*some brain dead servers sends 'mechanisms' without 'auth'*/) {
        //
        // Right now we only support PLAIN
        //
        if (mechanisms.find("PLAIN") != mechanisms.end()) {
            XmlObject auth ("auth", XmlSaslNs);
            auth.setAttribute ("mechanism", "PLAIN");

            uxmppLogTrace (THIS_FILE, "Authenticate user: ", auth_user);
            string challange_str = string("_") + auth_user + string("_") + auth_pass;
            challange_str[0] = '\0';
            challange_str[auth_user.length()+1] = '\0';
            auth.setContent (base64_encode(challange_str));

            xs.write (auth);
            return true;
        }else{
            uxmppLogWarning (THIS_FILE, "No supported authorization mechanism found");
            session.setAppError ("unsupported-authorization-mechanism",
                                 "No supported authentication method found");
            session.stop ();
        }
    }

    //
    // Handle 'challenge'
    //
    if (xml_obj.getFullName() == XmlChallengeTagFull) {
        // If method == PLAIN
        string challange_str = string("_") + auth_user + string("_") + auth_pass;
        challange_str[0] = '\0';
        challange_str[auth_user.length()+1] = '\0';
        XmlObject response ("response", XmlSaslNs);
        response.setContent (base64_encode(challange_str));
        xs.write (response);
        return true;
    }

    //
    // Handle 'success'
    //
    if (xml_obj.getFullName() == XmlSuccessTagFull) {
        uxmppLogInfo (THIS_FILE, "Logged in to ", to_string(xs.getPeerAddr()));
        session.reset ();
        return true;
    }

    //
    // Handle 'failure'
    //
    if (xml_obj.getFullName() == XmlFailureTagFull) {
        uxmppLogWarning (THIS_FILE, "Failure to authenticate: ",
                         xml_obj.getNodes().size() ? xml_obj.getNodes()[0].getName() : "unknown");
        session.setAppError ("authentication-failure",
                             "Authentication failed");
        session.stop ();
        return true;
    }



    return false;
}



UXMPP_END_NAMESPACE2
