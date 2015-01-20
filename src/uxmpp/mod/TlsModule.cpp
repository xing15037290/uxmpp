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
#include <uxmpp/mod/TlsModule.hpp>
#include <uxmpp/Session.hpp>
#include <uxmpp/xml/names.hpp>
#include <uxmpp/Semaphore.hpp>
#include <uxmpp/io/SocketConnection.hpp>


#define THIS_FILE "TlsModule"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;
using namespace uxmpp::io;


static const std::string XmlStarttlsNs      = "urn:ietf:params:xml:ns:xmpp-tls";
static const std::string XmlStarttlsTag     = "starttls";
static const std::string XmlStarttlsTagFull = XmlStarttlsNs + std::string(":") + XmlStarttlsTag;

static const std::string XmlProceedTag     = "proceed";
static const std::string XmlProceedTagFull = XmlStarttlsNs + std::string(":") + XmlProceedTag;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TlsModule::TlsModule ()
    : uxmpp::XmppModule ("mod_starttls")
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TlsModule::TlsModule (const uxmpp::io::TlsConfig& tls_config)
    : XmppModule ("mod_starttls"),
      tls_cfg    {tls_config}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TlsModule::process_xml_object (uxmpp::Session& session, uxmpp::XmlObject& xml_obj)
{
    XmlStream& xs = session.get_xml_stream ();
    bool start_tls = false;

    //
    // Handle 'features'
    //
    if (xml_obj.get_full_name() == xml::full_tag_features) {
        for (auto& node : xml_obj.get_nodes()) {
            if (node.get_full_name() == XmlStarttlsTagFull && !session.get_socket().is_tls_enabled()) {
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
    if (xml_obj.get_full_name() == XmlProceedTagFull  && !session.get_socket().is_tls_enabled()) {
        uxmpp_log_info (THIS_FILE, "Restart the stream with TLS enabled");
        bool connected = false;
        Semaphore sem;
        string error_text;
        io::SocketConnection& s = session.get_socket ();
        s.cancel (); // Cancel all I/O operations
        s.set_tls_connected_cb ([this, &connected, &sem, &error_text](SocketConnection& connection,
                                                                      int errnum,
                                                                      const std::string& errstr) {
                                    if (!errnum) {
                                        connected = true;
                                    }else{
                                        connected = false;
                                    }
                                    error_text = errstr;
                                    sem.post ();
                                });
        s.enable_tls (tls_cfg); // This is a non-blocking call
        if (!sem.wait(chrono::seconds(5)) || !connected) {
            //
            // Timeout or connection failed
            //
            sem.try_wait (); // just to make sure
            uxmpp_log_error (THIS_FILE, "Unable to restart the stream with TLS enabled");
            session.set_app_error ("tls-error", error_text);
            session.stop ();
        }else{
            session.reset ();
        }
        s.set_tls_connected_cb (nullptr);
        return true;
    }


    return false;
}



UXMPP_END_NAMESPACE2
