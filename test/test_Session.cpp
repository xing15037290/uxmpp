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
#include <uxmpp.hpp>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace uxmpp;
using namespace uxmpp::net;
using namespace uxmpp::mod;


static TlsModule tls_module;
static AuthModule auth_module;


class StateListener : public SessionListener,
                      RosterModuleListener,
                      PresenceModuleListener,
                      MessageModuleListener
{
public:
    StateListener (KeepAliveModule& mod_alive,
                   RosterModule& mod_roster,
                   PresenceModule& mod_pr,
                   MessageModule& mod_msg)
        :
        alive_module (mod_alive),
        roster_module (mod_roster),
        pr_module (mod_pr),
        msg_module (mod_msg)
    {
    }
    virtual ~StateListener () = default;

    virtual void onStateChange (Session& session, SessionState new_state, SessionState old_state) {
        cout << "##" << endl;
        cout << "##" << endl;
        cout << "##" << endl;
        cout << "## New session state: " << to_string(new_state) << endl;
        cout << "##" << endl;
        cout << "##" << endl;
        cout << "##" << endl;
        if (new_state==SessionState::bound && old_state!=SessionState::bound) {

            // Unregister modules that we don't need anymore
            //
            session.unregisterModule (tls_module);
            session.unregisterModule (auth_module);

            // Register new modules
            //
            roster_module.addRosterListener (*this);
            pr_module.addPresenceListener (*this);
            msg_module.addMessageListener (*this);

            session.registerModule (roster_module);
            session.registerModule (pr_module);
            session.registerModule (msg_module);

            // Send stanzas
            //
            roster_module.refresh ();
            pr_module.announce ();
        }
    }

    /**
     * Called when the roster module receives a roster.
     */
    virtual void onRoster (RosterModule& module, Roster& roster) {
        cout << "**" << endl;
        cout << "** Got roster:" << to_string(roster) << endl;
        cout << "**" << endl;
    }

    /**
     * Called when the roster module receives a roster.
     */
    virtual void onRosterPush (RosterModule& module, RosterItem& item) {
        cout << "**" << endl;
        cout << "** Got roster push:" << to_string(item) << endl;
        cout << "**" << endl;
    }

    /**
     * Called when the presence module receives a presence stanza.
     */
    virtual void onPresence (PresenceModule& module, uxmpp::PresenceStanza& presence) {
        string type = presence.getType ();

        cout << "**" << endl;
        cout << "** Got presence '" << type << "' from " << to_string(presence.getFrom()) << endl;
        cout << "**" << endl;
        if (type == "subscribe") {
            roster_module.addItem (presence.getFrom());
        }
        else if (type == "") {
        }
    }

    /**
     * Called when the presence module receives a presence stanza.
     */
    virtual void onMessage (MessageModule& module, uxmpp::MessageStanza& presence) {
        string content = presence.getBody ();
        if (content.length() == 0)
            return;

        cout << "Got message from " << to_string(presence.getFrom().bare()) << ": " << content << endl;
    }

private:
    KeepAliveModule& alive_module;
    RosterModule& roster_module;
    PresenceModule& pr_module;
    MessageModule& msg_module;

};



int main (int argc, char* argv[])
{
    //uxmppSetLogLevel (LogLevel::debug);
    //uxmppSetLogLevel (LogLevel::info);
    uxmppSetLogLevel (LogLevel::trace);

    if (argc < 2) {
        cerr << "Usage: test_Session <user@domain> [passphrase]" << endl;
        return 1;
    }

    //TlsModule tls_module;
    //AuthModule auth_module;
    KeepAliveModule alive_module;
    RosterModule roster_module;
    PresenceModule pr_module;
    MessageModule msg_module;

    StateListener sl (alive_module, roster_module, pr_module, msg_module);

    Jid user (argv[1]);
    Session sess;
    SessionConfig cfg;
    sess.addSessionListener (sl);


    // Configure modules
    //
    tls_module.tls_cfg.method        = TlsMethod::tlsv1_2;
    tls_module.tls_cfg.verify_server = false;
    auth_module.auth_user  = user.getLocal ();
    auth_module.auth_pass  = argc>2 ? argv[2] : "";
    alive_module.setInterval (300);

    // Add XMPP modules
    //
    sess.registerModule (tls_module);
    sess.registerModule (auth_module);
    sess.registerModule (alive_module);
    //sess.registerModule (roster_module);

    cfg.domain     = user.getDomain ();
    cfg.user_id    = to_string (user.bare());
    cfg.resource   = user.getResource ();

//    cfg.auth_user  = user.getLocal ();
//    cfg.auth_pass  = argc>2 ? argv[2] : "";

//    cfg.tls.method = TlsMethod::tlsv1_2;
//    cfg.tls.verify_server = false;
    //cfg.tls.verify_server = true;


    //thread t ([&sess, &cfg, &tls_module](){
    thread t ([&sess, &cfg](){
            sess.start (cfg);
            //
            // If TLS v1.2 is not supported, try TLS v1.1
            //
            if (sess.getError().getAppError() == "tls-error") {
                tls_module.tls_cfg.method = TlsMethod::tlsv1_1;
                sess.start (cfg);
                //
                // If TLS v1.1 is not supported, try TLS v1.0
                //
                if (sess.getError().getAppError() == "tls-error") {
                    tls_module.tls_cfg.method = TlsMethod::tlsv1;
                    sess.start (cfg);
                }
            }
        });

    string cmd;
    bool done = false;
    while (!done) {
        cin >> cmd;
        if (cmd == "q") {
            done = true;
        }
    }
    sess.stop ();
    t.join ();







    // Check for errors
    //
    StreamError& err = sess.getError ();
    if (err.getErrorName() != "") {
        string err_name = err.getAppError ();
        string err_text = err.getText ();
        if (err_name.length() == 0)
            err_name = err.getErrorName ();

        cerr << "Error: " << err_name;
        if (err_text.length())
            cerr << " - " << err_text;
        cerr << endl;
        return 1;
    }

    return 0;
}
