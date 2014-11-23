/*
 *  Copyright (C) 2013,2014 Ultramarin Design AB <dan@ultramarin.se>
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
#if 0
#include <uxmpp.hpp>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace uxmpp;
using namespace uxmpp::net;
using namespace uxmpp::core;
using namespace uxmpp::imp;


class AppLogic : public SessionListener, public ImpSessionListener {
public:

    /**
     * Default constructor.
     */
    AppLogic () = default;

    /**
     * Destructor.
     */
    virtual ~AppLogic () = default;

    /**
     * Called when the state if the session changes.
     */
    virtual void onStateChange (Session& session, SessionState old_state, SessionState new_state) {
        switch (new_state) {
        case SessionState::closed:
            closed = true;
            if (session.have_error())
                cerr << "Session is closed with error: " << session.get_error().get_error_name() << endl;
            else
                cerr << "Session is closed" << endl;
            break;

        case SessionState::connecting:
            closed = false;
            cerr << "Session is connecting" << endl;
            break;

        case SessionState::binding:
            cerr << "Session is binding" << endl;
            break;

        case SessionState::open:
            cerr << "Session is open" << endl;
            break;

        case SessionState::closing:
            cerr << "Session is closing" << endl;
            break;
        }
    }

    /**
     * Called when the session receives a roster.
     */
    virtual void onRoster (ImpSession& session, Roster& roster) override {
        auto& contacts = roster.get_contacts ();

        cout << "Got roster (" << contacts.size() << " contacts)" << endl;

        for (auto& contact : contacts) {
            string name = contact.get_name ();
            if (name.length()) {
                cout << name << " <" << to_string(contact.get_jid()) << ">" << endl;
            }else{
                cout << to_string(contact.get_jid()) << endl;
            }
        }
    };


    bool closed;
};



int main (int argc, char* argv[])
{
    AppLogic al;
    ImpSession sess;

    uxmppSetLogLevel (LogLevel::trace);
    //uxmppSetLogLevel (LogLevel::debug);
    //uxmppSetLogLevel (LogLevel::info);

    if (argc < 2) {
        cerr << "Usage: test_Session <user@domain> [passphrase]" << endl;
        return 1;
    }

    SessionConfig cfg;
    sess.addSessionListener (al);
    sess.addImpListener (al);

    Jid user (argv[1]);

    cfg.domain     = user.getDomain ();
    cfg.user_id    = to_string (user.bare());
    cfg.auth_user  = user.getLocal ();
    cfg.auth_pass  = argc>2 ? argv[2] : "";

    cfg.tls.method = TlsMethod::tlsv1_2;
    //cfg.tls.method = TlsMethod::tlsv1;
    cfg.tls.verify_server = false;
    //cfg.tls.verify_server = true;

    //cfg.keep_alive = 3600;
    cfg.keep_alive = 30;


    //
    // Run the XMPP session.
    //


    thread t ([&al, &sess, &cfg](){
            sess.start (cfg);
            //
            // If TLS v1.2 is not supported, try TLS v1.1
            //
            if (sess.getError().getAppError() == "tls-error") {
                cfg.tls.method = TlsMethod::tlsv1_1;
                sess.start (cfg);
                //
                // If TLS v1.1 is not supported, try TLS v1.0
                //
                if (sess.getError().getAppError() == "tls-error") {
                    cfg.tls.method = TlsMethod::tlsv1;
                    sess.start (cfg);
                }
            }
        });

    bool done = false;
    string cmd;
    while (!done) {
        cin >> cmd;
        if (cmd == "q") {
            done = true;
        }
        else if (cmd == "add") {
            string jid;
            cin >> jid;
            if (jid.length()) {
                sess.subscribe (Jid(jid));
            }
        }
        else if (cmd == "del") {
            string jid;
            cin >> jid;
            if (jid.length()) {
                sess.unsubscribe (Jid(jid));
            }
        }
        else if (cmd.length()) {
            sess.sendStanza (XmlObject(cmd));
        }
    }
    sess.stop ();
    t.join ();

/*
    Stanza p;
    p.setName ("presence");
    p.setPart (XmlObjPart::start);
    do {
        //cout << "Running" << endl;
        //sleep (1);
        sess.sendStanza (p);
    }while (!al.closed);
*/

    //
    // The XMPP session has ended.
    //

    // Check for errors
    //
    StreamError& err = sess.getError ();
    if (err.getErrorName() != "") {
        string err_name = err.getAppError ();
        string err_text = err.getText ();
        if (err_name.length() == 0)
            string err_name = err.getErrorName ();

        cerr << "Error: " << err_name;
        if (err_text.length())
            cerr << " - " << err_text;
        cerr << endl;
        return 1;
    }

    return 0;
}
#else
#include <iostream>

int main (int argc, char* argv[])
{
    return 0;
}

#endif
