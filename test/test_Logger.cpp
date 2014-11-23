/*
 *  Copyright (C) 2013 Ultramarin Design AB <dan@ultramarin.se>
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

using namespace std;
using namespace uxmpp;


static void log_messages ()
{
    uxmpp_log (LogLevel::silent, "test", "Silent message, this should not be visisble.");
    uxmpp_log_fatal   ("test", "Fatal message.");
    uxmpp_log_error   ("test", "Error message.");
    uxmpp_log_warning ("test", "Warning message.");
    uxmpp_log_info    ("test", "Info message.");
    uxmpp_log_debug   ("test", "Debug message.");
    uxmpp_log_trace   ("test", "Trace message.");
}



int main (int argc, char* argv[])
{
    cout << "Set log level to 'trace' and log with all log levels:" << endl;
    uxmpp_set_log_level (LogLevel::trace);
    log_messages ();

    cout << endl << "Set log level to 'debug' and log with all log levels:" << endl;
    uxmpp_set_log_level (LogLevel::debug);
    log_messages ();

    cout << endl << "Set log level to 'info' and log with all log levels:" << endl;
    uxmpp_set_log_level (LogLevel::info);
    log_messages ();

    cout << endl << "Set log level to 'warning' and log with all log levels:" << endl;
    uxmpp_set_log_level (LogLevel::warning);
    log_messages ();

    cout << endl << "Set log level to 'error' and log with all log levels:" << endl;
    uxmpp_set_log_level (LogLevel::error);
    log_messages ();

    cout << endl << "Set log level to 'fatal' and log with all log levels:" << endl;
    uxmpp_set_log_level (LogLevel::fatal);
    log_messages ();

    cout << endl << "Set log level to 'silent' and log with all log levels:" << endl;
    uxmpp_set_log_level (LogLevel::silent);
    log_messages ();

    return 0;
}
