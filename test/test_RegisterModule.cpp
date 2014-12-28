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
#include <string>

#include <getopt.h>
#include <termios.h>
#include <unistd.h>


using namespace std;
using namespace uxmpp;
using namespace uxmpp::io;
using namespace uxmpp::mod;


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
    virtual void on_features (Session& session, std::vector<XmlObject>& features);

    void print_status ();
    void run ();
    void stop ();

    Session        sess;
    SessionConfig  cfg;
    TlsModule      mod_tls;
    AuthModule     mod_auth;
    RegisterModule mod_register;
    SessionModule  mod_session;

    thread session_thread;

    Semaphore reg_info_sem;
    RegistrationInfo reg_info;
};


static void print_help ();


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AppLogic::AppLogic (app_config_t& app_cfg)
{
    // Configure modules
    //
    mod_tls.tls_cfg.method        = TlsMethod::tlsv1_2;
    mod_tls.tls_cfg.verify_server = false;
    mod_auth.auth_user            = app_cfg.jid.get_local ();
    mod_auth.auth_pass            = app_cfg.pass;
    mod_auth.auto_login           = false;

    mod_register.set_info_callback (
        [this](Session& session, RegistrationInfo& ri)
        {
            if (ri.error) {
                cout << "Error getting registration info: "
                     << ri.error.get_condition() << endl;
                stop ();
                return;
            }
            reg_info = std::move (ri);
            reg_info_sem.post ();
        });
    mod_register.set_result_callback ([this](Session& session, const string& operation, StanzaError& error){
            if (operation == "register")
                cout << "Registration: ";
            else if (operation == "unregister")
                cout << "Unregistration: ";
            else if (operation == "password")
                cout << "Password change: ";
            else
                return;
            if (!error) {
                cout << "success" << endl;
            }else{
                cout << "failure - " << error.get_condition() << endl;
            }
        });

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
/*
    cout << "on_state_change: " << to_string(new_state) << endl;
    if (new_state==SessionState::negotiating) {
        if (session.get_features().empty())
            return;
        for (auto& feature : session.get_features())
            if (feature.get_tag_name() == "starttls") {
                cout << "Don't do anything before TLS is enabled" << endl;
                return;
            }
        mod_register.request_registration_info ();
    }
    if (new_state==SessionState::bound && old_state!=SessionState::bound) {

        // Unregister modules that we don't need anymore
        //
        //session.unregister_module (mod_tls);
    }
*/
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::on_features (Session& session, std::vector<XmlObject>& features)
{
/*
    bool got_register {false};

    for (auto& feature : features) {
        if (feature.get_tag_name() == "starttls") {
            cout << "Don't do anything before TLS is enabled" << endl;
            return;
        }
        if (feature.get_full_name() == "http://jabber.org/features/iq-register:register")
            got_register = true;
    }
    if (got_register)
        mod_register.request_registration_info (cfg.domain);
*/
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::print_status ()
{
    cout << "Session state: " << to_string(sess.get_state()) << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AppLogic::run ()
{
    sess.register_module (mod_tls);
    sess.register_module (mod_auth);
    sess.register_module (mod_register);
    sess.register_module (mod_session);

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
static void print_cmdline_help ()
{
    cout << "test_RegisterModule [OPTION] user@domain\n"
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
static void read_passphrase (/*const string& who,*/ string& pass)
{
    //cout << "Enter passphrase for " << who << ": ";

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
        app.print_status ();

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
        // register
        //
        else if (cmd == "register") {
            app.reg_info.fields.clear ();
            app.mod_register.request_info (cfg.jid.get_domain());
            if (app.reg_info.fields.empty()) {
                app.reg_info_sem.wait ();
            }
            cout << app.reg_info.instructions << endl;
            for (auto& field : app.reg_info.fields) {
                cout << field.first << ": ";
                if (field.first == "password")
                    read_passphrase (field.second);
                else
                    cin >> field.second;
            }
            cout << "Register..." << endl;
            app.mod_register.register_user (app.reg_info.fields);
        }
        //
        // Unregister
        //
        else if (cmd == "unregister") {
            cout << "Unregister..." << endl;
            app.mod_register.unregister_user ();
        }
        //
        // Change password
        //
        else if (cmd == "passwd") {
            string new_pass;
            cout << endl << "Enter new password: ";
            read_passphrase (new_pass);
            cout << "Changing password..." << endl;
            app.mod_register.set_password (new_pass);
        }
        //
        // Login
        //
        else if (cmd == "login") {
            if (cfg.pass.empty()) {
                cout << endl << "Enter password: ";
                read_passphrase (app.mod_auth.auth_pass);
            }else{
                app.mod_auth.auth_pass = cfg.pass;
            }
            app.mod_auth.authenticate ();
        }
        else if (cmd == "") {
        }

    }while (!quit);

    // Stop the XMPP applicatin object
    //
    app.stop ();

    return 0;
}
