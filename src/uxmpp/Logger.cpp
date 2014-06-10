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
#include <uxmpp/Logger.hpp>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <event2/event.h>


UXMPP_START_NAMESPACE1(uxmpp)

using namespace std;
using namespace std::chrono;


Logger* Logger::instance = nullptr;



//----------------------------------------------------------
//----------------------------------------------------------
std::string to_string (const LogLevel& level)
{
    switch (level) {
    case LogLevel::silent:
        return "S";//"silent";

    case LogLevel::fatal:
        return "F";//"fatal";

    case LogLevel::error:
        return "E";//"error";

    case LogLevel::warning:
        return "W";//"warning";

    case LogLevel::info:
        return "I";//"info";

    case LogLevel::debug:
        return "D";//"debug";

    case LogLevel::trace:
    default:
        return "T";//"trace";
    }
}


//----------------------------------------------------------
//----------------------------------------------------------
/*
Logger::Logger ()
    : log_level {LogLevel::info}
{
}
*/


//----------------------------------------------------------
//----------------------------------------------------------
/*
Logger::~Logger ()
{
}
*/


//----------------------------------------------------------
//----------------------------------------------------------
static void event_logger (int severity, const char* msg)
{
    LogLevel level = LogLevel::trace;

    switch (severity) {
    case EVENT_LOG_DEBUG:
        level = LogLevel::debug;
        break;
    case EVENT_LOG_MSG:
        level = LogLevel::info;
        break;
    case EVENT_LOG_WARN:
        level = LogLevel::warning;
        break;
    case EVENT_LOG_ERR:
        level = LogLevel::error;
        break;
    }
    Logger::getInstance().log (level, "libevent", msg);
}


//----------------------------------------------------------
//----------------------------------------------------------
Logger& Logger::getInstance ()
{
//    static std::mutex global_log_mutex;
    //global_log_mutex.lock ();
    if (Logger::instance == nullptr) {
        Logger::instance = new Logger;
        Logger::instance->setLogLevel (LogLevel::info);
        //
        // Install libevent log callback.
        //
        event_set_log_callback (event_logger);
    }
    //global_log_mutex.unlock ();

    return *Logger::instance;
}


//----------------------------------------------------------
//----------------------------------------------------------
void Logger::log (LogLevel level, const std::string& prefix, const std::string& message)
{
    time_point<system_clock> now = system_clock::now ();

    mutex.lock ();
    if (level <= log_level && level!=LogLevel::silent) {
        milliseconds ms = duration_cast<milliseconds> (now.time_since_epoch());

        char timestring[32];
        time_t timestamp = system_clock::to_time_t (now);
        strftime (timestring, sizeof(timestring), "%F %T", localtime(&timestamp));
        stringstream ss;

        ss << timestring << "." << setfill('0') << setw(3)
//           << (ms.count()%1000) << " - (" << to_string(level) << ") "
           << (ms.count()%1000) << " (" << to_string(level) << ") "
           << prefix << ": " << message << endl;

        cerr << ss.str();
    }
    mutex.unlock ();
}


//----------------------------------------------------------
//----------------------------------------------------------
LogLevel Logger::setLogLevel (LogLevel level)
{
    mutex.lock ();
    LogLevel old_level = log_level;
    log_level = level;
    mutex.unlock ();

    return old_level;
}


//----------------------------------------------------------
//----------------------------------------------------------
LogLevel Logger::getLogLevel ()
{
    mutex.lock ();
    LogLevel level = log_level;
    mutex.unlock ();

    return level;
}



UXMPP_END_NAMESPACE1
