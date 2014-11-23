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
#include <uxmpp/XmlNames.hpp>


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
AuthModule::AuthModule ()
    : uxmpp::XmppModule ("mod_auth")
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool AuthModule::proccess_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    XmlStream& xs = session.get_xml_stream ();
    bool start_auth = false;
    bool have_mechanisms = false;

    //
    // Handle 'features'
    //
    if (xml_obj.get_full_name() ==  XmlFeaturesTagFull) {
        for (auto& node : xml_obj.get_nodes()) {
            if (node.get_full_name() == XmlMechanismsTagFull) {
                mechanisms.clear ();
                have_mechanisms = true;
                //
                // Store authentication methods
                //
                for (auto& subnode : node.get_nodes()) {
                    if (subnode.get_full_name() == XmlMechanismTagFull) {
                        uxmpp_log_trace (THIS_FILE, "Got authentication mechanism: ", subnode.get_content());
                        mechanisms.insert (subnode.get_content());
                    }
                }
            }
            else if (node.get_full_name() == XmlAuthTagFull) {
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
            auth.set_attribute ("mechanism", "PLAIN");

            uxmpp_log_trace (THIS_FILE, "Authenticate user: ", auth_user);
            string challange_str = string("_") + auth_user + string("_") + auth_pass;
            challange_str[0] = '\0';
            challange_str[auth_user.length()+1] = '\0';
            auth.set_content (base64_encode(challange_str));

            xs.write (auth);
            return true;
        }else{
            uxmpp_log_warning (THIS_FILE, "No supported authorization mechanism found");
            session.set_app_error ("unsupported-authorization-mechanism",
                                   "No supported authentication method found");
            session.stop ();
        }
    }

    //
    // Handle 'challenge'
    //
    if (xml_obj.get_full_name() == XmlChallengeTagFull) {
        // If method == PLAIN
        string challange_str = string("_") + auth_user + string("_") + auth_pass;
        challange_str[0] = '\0';
        challange_str[auth_user.length()+1] = '\0';
        XmlObject response ("response", XmlSaslNs);
        response.set_content (base64_encode(challange_str));
        xs.write (response);
        return true;
    }

    //
    // Handle 'success'
    //
    if (xml_obj.get_full_name() == XmlSuccessTagFull) {
        uxmpp_log_info (THIS_FILE, "Logged in to ", to_string(xs.get_peer_addr()));
        session.reset ();
        return true;
    }

    //
    // Handle 'failure'
    //
    if (xml_obj.get_full_name() == XmlFailureTagFull) {
        uxmpp_log_warning (THIS_FILE, "Failure to authenticate: ",
                           xml_obj.get_nodes().empty() ? "unknown" : xml_obj.get_nodes()[0].get_tag_name());
        session.set_app_error ("authentication-failure",
                               "Authentication failed");
        session.stop ();
        return true;
    }



    return false;
}



UXMPP_END_NAMESPACE2
