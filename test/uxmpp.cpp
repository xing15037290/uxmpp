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
#include <uxmpp.hpp>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <getopt.h>
#include <cstdio>
#include <cstring>
#include <sstream>

#include <termios.h>
#include <unistd.h>


using namespace std;
using namespace uxmpp;
using namespace uxmpp::io;
using namespace uxmpp::mod;

//------------------------------------------------------------------------------
// D E F I N E S
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// T Y P E   D E F I N I T I O N S
//------------------------------------------------------------------------------

/**
 * Application configuration.
 */
struct app_config_t {
    Jid      jid;
    string   pass;
    LogLevel log_level;
    int      keep_alive;
    string   dir;
    string   server;
    unsigned short    port;
};

/**
 * Application logic.
 */
class AppLogic : public SessionListener {
public:
    AppLogic (app_config_t& app_config);
    virtual ~AppLogic () = default;

    virtual void on_state_change (Session& session, SessionState new_state, SessionState old_state);
    void print_status ();
    void run ();
    void stop ();

    virtual void on_roster (RosterModule& module, Roster& roster) {
        print_status ();
    }
    virtual void on_roster_push (RosterModule& module, RosterItem& item) {
        print_status ();
    }
    virtual void on_presence (PresenceModule& module, uxmpp::PresenceStanza& presence);

    virtual void on_message (MessageModule& module, uxmpp::MessageStanza& msg);
    virtual void on_receipt (MessageModule& module, const uxmpp::Jid& from, const std::string& id);

    Session           sess;
    SessionConfig     cfg;
    TlsModule         mod_tls;
    AuthModule        mod_auth;
    SessionModule     mod_session;
    KeepAliveModule   mod_alive;
    RosterModule      mod_roster;
    PresenceModule    mod_pr;
    MessageModule     mod_msg;
    PingModule        mod_ping;
    PrivateDataModule mod_priv_data;
    DiscoModule       mod_disco;
    RegisterModule    mod_register;

    thread session_thread;
};




//------------------------------------------------------------------------------
// F U N C T I O N   D E C L A R A T I O N S
//------------------------------------------------------------------------------
static void print_cmdline_help ();
static int handle_arguments (int argc, char* argv[], app_config_t& cfg);
static void read_passphrase (const string& who, string& pass);
static void print_help ();




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AppLogic::AppLogic (app_config_t& app_cfg)
{
    // Configure modules
    //
    mod_alive.set_interval (app_cfg.keep_alive);
    mod_tls.tls_cfg.method        = TlsMethod::tlsv1_2;
    mod_tls.tls_cfg.verify_server = false;
    mod_auth.auth_user            = app_cfg.jid.get_local ();
    mod_auth.auth_pass            = app_cfg.pass;

    // Configure xmpp session
    //
    cfg.domain   = app_cfg.jid.get_domain ();
    cfg.server   = app_cfg.server;
    cfg.user_id  = to_string (app_cfg.jid.bare());
    cfg.resource = app_cfg.jid.get_resource ();
    if (cfg.server.length()) {
        cfg.disable_srv = true; // Don't use DNS SRV lookup if we manually specify the server host/IP.
    }
    cfg.port     = app_cfg.port;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::on_state_change (Session& session, SessionState new_state, SessionState old_state)
{
    if (new_state==SessionState::bound && old_state!=SessionState::bound) {

        // Unregister modules that we don't need anymore
        //
        session.unregister_module (mod_tls);
        session.unregister_module (mod_auth);

        // Send initial stanzas
        //
        mod_roster.refresh ();
        mod_pr.announce ();
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::on_presence (PresenceModule& module, uxmpp::PresenceStanza& presence)
{
    cout << "Got presence stanza:" << to_string(presence) << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::on_message (MessageModule& module, uxmpp::MessageStanza& msg)
{
    //cout << "Got message from " << to_string(msg.getFrom().bare()) << ": " << msg.getBody() << endl;
    cout << "Got message from " << to_string(msg.get_from()) << ": " << msg.get_body() << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::on_receipt (MessageModule& module, const uxmpp::Jid& from, const std::string& id)
{
    cout << "Got message receipt " << id << " from " << to_string(from) << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::run ()
{
    sess.register_module (mod_tls);
    sess.register_module (mod_auth);
    sess.register_module (mod_session);
    sess.register_module (mod_roster);
    sess.register_module (mod_pr);
    sess.register_module (mod_msg);
    sess.register_module (mod_alive);
    sess.register_module (mod_ping);
    sess.register_module (mod_priv_data);
    sess.register_module (mod_disco);
    sess.register_module (mod_register);

    mod_roster.set_roster_handler ([this](RosterModule& rm, Roster& r) { on_roster (rm, r); });
    mod_roster.set_roster_push_handler ([this](RosterModule& rm, RosterItem& ri) { on_roster_push (rm, ri); });
    mod_pr.set_presence_handler ([this](PresenceModule& pm, PresenceStanza& ps) { on_presence (pm, ps); });
    mod_msg.set_message_handler ([this](MessageModule& mm, MessageStanza& ms) { on_message(mm, ms); });
    mod_msg.set_receipt_handler ([this](MessageModule& mm, const Jid& f, const string& id) { on_receipt(mm, f, id); });

    sess.add_session_listener (*this);

    session_thread = thread ([this](){
            sess.run (cfg);
            //
            // If TLS v1.2 is not supported, try TLS v1.1
            //
            if (sess.get_error().get_app_error() == "tls-error") {
                mod_tls.tls_cfg.method = TlsMethod::tlsv1_1;
                sess.run (cfg);
                //
                // If TLS v1.1 is not supported, try TLS v1.0
                //
                if (sess.get_error().get_app_error() == "tls-error") {
                    mod_tls.tls_cfg.method = TlsMethod::tlsv1;
                    sess.run (cfg);
                }
            }
        });
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::stop ()
{
    sess.stop ();
    session_thread.join ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::print_status ()
{
    StreamError& err = sess.get_error ();
    if (err.get_error_name() != "") {
        string err_name = err.get_app_error ();
        string err_text = err.get_text ();
        if (err_name.length() == 0)
            err_name = err.get_error_name ();

        cout << endl;
        cout << "# XMPP Error: " << err_name;
        if (err_text.length())
            cout << " - " << err_text;
        cout << endl;
        return;
    }

    cout << endl;
    cout << endl;
    cout << "JID: " << to_string(sess.get_jid()) << endl;
    cout << endl;
    cout << "Roster:" << endl;

    Roster& r = mod_roster.get_roster ();
    int i=0;
    for (auto& item : r.get_items()) {
        string handle = item.get_handle ();
        if (handle.length())
            cout << setw(4) << i++ << setw(0) << ": " << handle << " <" << to_string(item.get_jid().bare()) << ">";
        else
            cout << setw(4) << i++ << setw(0) << ": " << to_string(item.get_jid().bare());
        cout << " (" << to_string(item, false) << ")" << endl;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void print_help ()
{
    cout << endl
         << "+----- Buddies -----------------------------------------------------" << endl
         << "| buddy-add <jid>      - Add a new buddy" << endl
         << "| buddy-accept <index> - Accept a buddy request" << endl
         << "| buddy-del <index>    - Remove a buddy" << endl
         << "| msg <index|jid>      - Send a message to a buddy" << endl
         << "| bn <index> <name>    - Set buddy name." << endl
         << "|" << endl
         << "+----- Presence (low level) ----------------------------------------" << endl
         << "| pr-subscribe <jid>   - Request presence subscription" << endl
         << "| pr-accept <index>    - Accept presence subscription" << endl
         << "| pr-deny <index>      - Deny presence subscription" << endl
         << "|" << endl
         << "+----- Roster (low level) ------------------------------------------" << endl
         << "| r-add <jid>          - Add a roster item" << endl
         << "| r-del <index>        - Remove a roster item" << endl
         << "|" << endl
         << "+----- Account------------------------------------------------------" << endl
         << "| reg-info             - Send registration info query." << endl
         << "+----- Misc --------------------------------------------------------" << endl
         << "| data-set [tag]       - Set private data on server." << endl
         << "| data-get [tag]       - Get private data from server." << endl
         << "| ping [index|jid]     - Send XMPP ping." << endl
         << "| dq [index|jid]       - Send disco info query." << endl
         << "| ll <value>           - Set log level (0-5)."             << endl
         << "| ka <value>           - Set keep alive timer in seconds (0 to disable)." << endl
         << "| help                 - Print this help." << endl
         << "| quit                 - Quit the application."            << endl
         << "+-------------------------------------------------------------------" << endl
         << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool get_int_argument (const string& message, stringstream& ss, int& value)
{
    if (ss.eof()) {
        cout << message;
        char cmdbuf[512];
        cin.getline (cmdbuf, sizeof(cmdbuf));
        if (cmdbuf[0] == '\0')
            return false;
        value = atoi (cmdbuf);
    }else{
        string arg;
        ss >> arg;
        value = atoi (arg.c_str());
    }
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool get_string_argument (const string& message, stringstream& ss, string& value)
{
    char cmdbuf[512];
    value = "";

    if (ss.eof()) {
        cout << message;
        cin.getline (cmdbuf, sizeof(cmdbuf));
        value = string (cmdbuf);
    }else{
        ss >> value;
        if (!ss.eof()) {
            ss.getline (cmdbuf, sizeof(cmdbuf));
            value += string (cmdbuf);
        }
    }

    return value.length() > 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static pair<bool, bool> get_int_or_string_argument (const string& message,
                                                    stringstream& ss,
                                                    int& int_val,
                                                    string& str_val)
{
    pair<bool, bool> retval (false, false);
    string arg;

    if (ss.eof()) {
        cout << message;
        char cmdbuf[512];
        cin.getline (cmdbuf, sizeof(cmdbuf));
        if (cmdbuf[0] == '\0')
            return retval;
        arg = string (cmdbuf);
    }else{
        ss >> arg;
    }

    for (auto c : arg) {
        if (!isdigit(c)) {
            //
            // The argument is a string
            //
            str_val = arg;
            retval.second = true;
            return retval;
        }
    }

    //
    // The argument is an int
    //
    int_val = (int) atoi (arg.c_str());
    retval.first = true;
    return retval;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main (int argc, char* argv[])
{
    app_config_t cfg;

    // Parse command line parameters
    //
    if (handle_arguments(argc, argv, cfg))
        return 1;

    // Set log level
    //
    uxmpp_set_log_level (cfg.log_level);

    // Read passphrase (if not present)
    //
    if (cfg.pass.length() == 0)
        read_passphrase (to_string(cfg.jid.bare()), cfg.pass);

    // Create and start the XMPP application object
    //
    AppLogic app (cfg);
    app.run ();

    // Main loop
    //
    char cmdbuf[512];
    bool quit = false;
    string cmd = "";
    stringstream ss;

    do {
        //if (cmd.length() > 0) {
        app.print_status ();
        //print_help ();
        //}

        // Read command line
        //
        cout << endl << ": ";
        ss << "";
        ss.clear ();
        cin.getline (cmdbuf, sizeof(cmdbuf));
        if (cmdbuf[0] == '\0')
            continue;
        ss << cmdbuf;
        ss >> cmd;

        // Command 'quit'
        //
        if (cmd == "quit") {
            quit = true;
        }
        // Command 'help'
        //
        if (cmd == "help") {
            print_help ();
        }
        //
        // Command 'll' - set log level
        //
        else if (cmd == "ll") {
            int level;
            if (get_int_argument("Enter new log level (0-5): ", ss, level)) {
                if (level<-1 || level>5) {
                    cout << "Invalid log level argument" << endl;
                }else{
                    uxmpp_set_log_level (static_cast<LogLevel>(level));
                }
            }else{
                cout << "Invalid log level argument" << endl;
            }
        }
        //
        // Command 'ka' - set keep alive timer
        //
        else if (cmd == "ka") {
            int sec;
            bool got_arg;
            got_arg = get_int_argument ("Enter new keep alive timeout in seconds (0 to disable): ", ss, sec);
            if (got_arg && sec>=0)
                app.mod_alive.set_interval (sec);
            else
                cout << "Invalid argument" << endl;
        }
        //
        // Command 'bn' - set buddy name
        //
        else if (cmd == "bn") {
            int index;
            string name;
            bool got_arg;

            got_arg = get_int_argument ("Enter buddy index: ", ss, index);
            if (!got_arg || index<0) {
                cout << "Invalid buddy index" << endl;
                continue;
            }
            get_string_argument ("Enter buddy name: ", ss, name);

            auto items = app.mod_roster.get_roster().get_items ();
            if ((unsigned)index >= items.size()) {
                cout << "Invalid buddy index" << endl;
                continue;
            }
            items[index].set_handle (name);
            app.mod_roster.update_item (items[index]);
        }
        //
        // Command 'r-add' - Add roster item
        //
        else if (cmd == "r-add") {
            string jid;
            if (!get_string_argument("Enter jid: ", ss, jid))
                continue;
            app.mod_roster.add_item (Jid(jid));
        }
        //
        // Command 'r-del' - Remove roster item
        //
        else if (cmd == "r-del") {
            int index;
            bool got_arg;

            got_arg = get_int_argument ("Enter buddy index: ", ss, index);
            if (!got_arg || index<0) {
                cout << "Invalid buddy index" << endl;
                continue;
            }
            auto items = app.mod_roster.get_roster().get_items ();
            if ((unsigned)index >= items.size()) {
                cout << "Invalid buddy index" << endl;
                continue;
            }
            app.mod_roster.remove_item (items[index]);
        }
        //
        // Command 'pr-subscribe' - Request presence subscription
        //
        else if (cmd == "pr-subscribe") {
            string jid;
            if (!get_string_argument("Enter jid: ", ss, jid))
                continue;
            app.mod_pr.request_subscription (Jid(jid));
        }
        //
        // Command 'pr-accept' - Accept presence subscription
        //
        else if (cmd == "pr-accept") {
            int index;
            bool got_arg;

            got_arg = get_int_argument ("Enter buddy index: ", ss, index);
            if (!got_arg || index<0) {
                cout << "Invalid buddy index" << endl;
                continue;
            }
            auto items = app.mod_roster.get_roster().get_items ();
            if ((unsigned)index >= items.size()) {
                cout << "Invalid buddy index" << endl;
                continue;
            }
            app.mod_pr.accept_subscription (items[index].get_jid().bare());
        }
        //
        // Command 'pr-deny' - Deny presence subscription
        //
        else if (cmd == "pr-deny") {
            int index;
            bool got_arg;

            got_arg = get_int_argument ("Enter buddy index: ", ss, index);
            if (!got_arg || index<0) {
                cout << "Invalid buddy index" << endl;
                continue;
            }
            auto items = app.mod_roster.get_roster().get_items ();
            if ((unsigned)index >= items.size()) {
                cout << "Invalid buddy index" << endl;
                continue;
            }
            app.mod_pr.deny_subscription (items[index].get_jid().bare());
        }
        else if (cmd == "buddy-add") {
        }
        else if (cmd == "buddy-accept") {
        }
        else if (cmd == "buddy-del") {
        }
        else if (cmd == "msg") {
            pair<bool/*int-arg*/, bool/*sting-arg*/> got_arg;
            int index {0};
            string jid {""};

            got_arg = get_int_or_string_argument ("Enter buddy index or JID: ", ss, index, jid);
            if (got_arg.first) {
                auto items = app.mod_roster.get_roster().get_items ();
                if (index < 0 || (unsigned)index >= items.size()) {
                    cout << "Invalid buddy index" << endl;
                    continue;
                }
                jid = to_string (items[index].get_jid().bare());
            }

            if (jid != "") {
                string msg {""};
                get_string_argument ("Enter message: ", ss, msg);
                if (msg != "") {
                    cout << "Send message to " << jid << ": " << msg << endl;
                    //app.mod_msg.sendMessage (MessageStanza(jid, app.sess.getJid(), msg));
                    app.mod_msg.send_message (jid, msg, true);
                }
            }
        }
        else if (cmd == "reg-info") {
            app.mod_register.set_info_callback (
                [](Session& session, RegistrationInfo& ri)
                {
                    if (ri.error) {
                        cout << "Error getting registration info: " << ri.error.get_condition() << endl;
                    }
                    else {
                        cout << "Registration info:" << endl;
                        cout << (ri.registered ? "Already registered" : "Not registered") << endl;
                        cout << "Registration fields: " << endl;
                        for (auto& field : ri.fields) {
                            cout << "\t" << field.first << ": " << field.second << endl;
                        }
                        cout << "Registration instructions: " << endl << "\t" << ri.instructions << endl;
                    }
                });
            app.mod_register.request_info ();
        }
        else if (cmd == "data-set") {
            string tag;
            bool got_arg = get_string_argument ("Enter data tag: ", ss, tag);
            if (!got_arg)
                continue;
            string content {""};
            get_string_argument ("Enter data content: ", ss, content);
            app.mod_priv_data.set (XmlObject(tag, "uxmpp:priv-data").set_content(content));
        }
        else if (cmd == "data-get") {
            string tag;
            bool got_arg = get_string_argument ("Enter data tag: ", ss, tag);
            if (!got_arg)
                continue;
            app.mod_priv_data.get (tag, "uxmpp:priv-data", [](Session& session,
                                                              std::vector<uxmpp::XmlObject>& priv_data,
                                                              const std::string& stanza_id,
                                                              const std::string& error_name){
                                       cout << "Got private data: " << endl;
                                       for (auto& obj : priv_data) {
                                           cout << to_string(obj) << endl;
                                       }
                });
        }
        else if (cmd == "ping") {
            string jid;
            bool got_arg = get_string_argument ("Enter jid (empty to ping server): ", ss, jid);
            if (!got_arg)
                app.mod_ping.ping ();
            else
                app.mod_ping.ping (Jid(jid));
        }
        else if (cmd == "dq") {
            pair<bool/*int-arg*/, bool/*sting-arg*/> got_arg;
            int index {0};
            string jid_arg;
            Jid jid;

            got_arg = get_int_or_string_argument ("Enter buddy index or JID: ", ss, index, jid_arg);
            if (got_arg.first) { // int argument
                auto items = app.mod_roster.get_roster().get_items ();
                if (index < 0 || (unsigned)index >= items.size()) {
                    cout << "Invalid buddy index" << endl;
                    continue;
                }
                jid = items[index].get_jid ();
            }
            else if (got_arg.second) { // string argument
                jid = Jid (jid_arg);
            }
            else {
                continue;
            }
            app.mod_disco.query_info (jid);
        }
        else if (cmd == "") {
        }

    }while (!quit);

    // Stop the XMPP applicatin object
    //
    app.stop ();

    return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void print_cmdline_help ()
{
    cout << "uxmpp [OPTION] user@domain[/resource]\n"
        "  -p, --password <passphrase>  Passphrase used when logging in the user.\n"
        "  -s, --server <server>        Set the server host/IP to connect to, this overrides the domain.\n"
        "                               When manually setting the server a DNS SRV lookup will not be used.\n"
        "  -o, --port <port>            Port number used when connecting to the server. This overrides the\n"
        "                               default port number and any port number received from a DNS SRV result.\n"
        "  -l, --log-level <level>      Log level (0-5). Default is 3.\n"
        "  -a, --keep-alive <interval>  Keep-alive interval in seconds, 0 to disable. Default is 300.\n"
        "  -d, --dir <directory>        Directory where to store cache files. Default is $HOME/.uxmpp\n"
        "      --help                   Print this help text.\n\n";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static int handle_arguments (int argc, char* argv[], app_config_t& cfg)
{
    // Set default values
    //
    cfg.log_level  = LogLevel::info;
    cfg.keep_alive = 300;
    cfg.dir = "~/.uxmpp";
    cfg.server = "";
    cfg.port = 0;

    static struct option long_options[] {
        { "password",   required_argument, NULL, 'p' },
        { "server",     required_argument, NULL, 's' },
        { "port",       required_argument, NULL, 'o' },
        { "log-level",  required_argument, NULL, 'l' },
        { "keep-alive", required_argument, NULL, 'a' },
        { "dir",        required_argument, NULL, 'd' },
        { "help",       no_argument,       NULL,  0  },
    };

    while (true) {
        int option_index {0};
        int c;

        c = getopt_long (argc, argv, "p:s:o:l:a:d:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            print_cmdline_help ();
            exit (0);
            break;

        case 'p':
            cfg.pass = optarg;
            break;

        case 'o':
            cfg.port = atoi (optarg);
            break;

        case 's':
            cfg.server = optarg;
            break;

        case 'l':
            c = atoi (optarg);
            if (c<-1 || c>5) {
                cerr << "Invalid log level argument." << endl;
                return 1;
            }
            cfg.log_level = static_cast<LogLevel> (c);
            break;

        case 'a':
            cfg.keep_alive = atoi (optarg);
            if (cfg.keep_alive < 0) {
                cerr << "Invalid keep-alive argument." << endl;
                return 1;
            }
            break;

        case 'd':
            cfg.dir = optarg;
            break;

        default:
            return 1;
        }
    }

    if (optind >= argc) {
        cerr << "Missing argument" << endl;
        print_cmdline_help ();
        return 1;
    }

    cfg.jid = Jid (argv[optind]);

    return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void read_passphrase (const string& who, string& pass)
{
    cout << "Enter passphrase for " << who << ": ";

    // Disable echo in the terminal
    //
    struct termios orig_ios;
    struct termios noecho_ios;
    bool termios_changed = false;
    if (tcgetattr(STDIN_FILENO, &orig_ios) >= 0) {
        memcpy (&noecho_ios, &orig_ios, sizeof(struct termios));
        noecho_ios.c_lflag &= ~ECHO;
        termios_changed = tcsetattr(STDIN_FILENO, TCSANOW, &noecho_ios) >= 0;
    }

    // Read passphrase
    //
    cin >> pass;

    // Restore terminal settings
    //
    if (termios_changed) {
        tcsetattr (STDIN_FILENO, TCSANOW, &orig_ios);
        cout << endl;
    }

    // Flush cin
    //
    cin.clear ();
    cin.ignore (numeric_limits<streamsize>::max(), '\n');
}
