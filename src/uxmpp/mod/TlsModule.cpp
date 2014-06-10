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
#include <uxmpp/mod/TlsModule.hpp>
#include <uxmpp/Session.hpp>


#define THIS_FILE "TlsModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;
using namespace uxmpp::net;


static const std::string XmlStarttlsNs      = "urn:ietf:params:xml:ns:xmpp-tls";
static const std::string XmlStarttlsTag     = "starttls";
static const std::string XmlStarttlsTagFull = XmlStarttlsNs + std::string(":") + XmlStarttlsTag;

static const std::string XmlProceedTag     = "proceed";
static const std::string XmlProceedTagFull = XmlStarttlsNs + std::string(":") + XmlProceedTag;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TlsModule::TlsModule (const uxmpp::TlsConfig& tls_config)
    : XmppModule ("mod_starttls"),
      tls_cfg    {tls_config}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TlsModule::proccessXmlObject (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    XmlStream& xs = session.getXmlStream ();
    bool start_tls = false;

    //
    // Handle 'features'
    //
    if (xml_obj.getFullName() ==  XmlFeaturesTagFull) {
        for (auto& node : xml_obj.getNodes()) {
            if (node.getFullName() == XmlStarttlsTagFull && xs.getPeerAddr().proto != AddrProto::tls) {
                start_tls = true;
                break;
            }
        }
    }
    if (start_tls) {
        XmlObject starttls ("starttls", XmlStarttlsNs);
        xs.write (starttls);
        return true;
    }

    //
    // Handle 'proceed'
    //
    if (xml_obj.getFullName() == XmlProceedTagFull && xs.getPeerAddr().proto != AddrProto::tls) {
        uxmppLogInfo (THIS_FILE, "Restart the stream with TLS enabled");
        string error_text;
        if (xs.enableTls(tls_cfg, error_text)) {
            session.reset ();
        }else{
            uxmppLogError (THIS_FILE, "Unable to restart the stream with TLS enabled");
            session.setAppError ("tls-error", error_text);
            session.stop ();
            //changeState (SessionState::closing);
        }
        return true;
    }


    return false;
}



UXMPP_END_NAMESPACE2
