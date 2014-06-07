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
#ifndef UXMPP_LOGGER_HPP
#define UXMPP_LOGGER_HPP

#include <uxmpp/types.hpp>
#include <string>
#include <sstream>
#include <mutex>


namespace uxmpp {


    /**
     * A log level used when logging messages.
     */
    enum class LogLevel {
        /**
         * Silent - no messages will be logged (lowest log level)..
         */
        silent=-1,

        /**
         * Fatal error messages.
         */
        fatal=0,

        /**
         * Non-fatal error messages.
         */
        error=1,

        /**
         * Warning messages.
         */
        warning=2,

        /**
         * Info messages.
         */
        info=3,

        /**
         * Debug messages.
         */
        debug=4,

        /**
         * Trace messages (highest log level).
         */
        trace=5
    };


    /**
     * Return a string representation of a log level.
     */
    std::string to_string (const LogLevel& level);



    /**
     * A singelton class that handles message logging.
     */
    class Logger {
    public:

        /**
         * Return the instance of the logger.
         */
        static Logger& getInstance ();

        /**
         * Log a message.
         * @param leve A log level.
         * @param prefix A prefix to the log message.
         * @param message The log message.
         */
        void log (LogLevel level, const std::string& prefix, const std::string& message);

        /**
         * Set the maximum log level when logging messages.
         * Messages with a higher log level will not be logged.
         * @param level The new log level to use.
         * @return The old log level.
         */
        LogLevel setLogLevel (LogLevel level);

        /**
         * Get the current log level.
         * @return The current log level.
         */
        LogLevel getLogLevel ();


    private:

        /**
         * Constructor.
         */
        Logger () = default;

        /**
         * Destructor.
         */
        ~Logger () = default;

        /**
         * Instance.
         */
        static Logger* instance;

        /**
         * The current log level.
         */
        LogLevel log_level;

        /**
         * Mutex to prevent different threads to log at the same time.
         */
        std::mutex mutex;
    };


    /**
     * Set the maximum log level when logging messages.
     * Messages with a higher log level will not be logged.
     * @param level The new log level to use.
     * @return The old log level.
     */
    inline LogLevel uxmppSetLogLevel (LogLevel level)
    {
        return Logger::getInstance().setLogLevel (level);
    }


    /**
     * Log a message using the Logger class.
     * @param leve A log level.
     * @param prefix A prefix to the log message.
     * @param message The log message.
     */
    inline void uxmppLog (LogLevel level, const std::string& prefix, const std::string& msg)
    {
        Logger::getInstance().log (level, prefix, msg);
    }
    template<typename T, typename... Targs>
    void uxmppLog (LogLevel level, const std::string& prefix,
                   const std::string& msg, T arg, Targs... msg_args)
    {
        std::stringstream ss;
        ss << msg << arg;
        uxmppLog (level, prefix, ss.str(), msg_args...);
    }


    /**
     * Log a fatal message using the Logger class.
     * @param prefix A prefix to the log message.
     * @param message The log message.
     */
    inline void uxmppLogFatal (const std::string& prefix, const std::string& msg)
    {
        Logger::getInstance().log (LogLevel::fatal, prefix, msg);
    }
    template<typename T, typename... Targs>
    void uxmppLogFatal (const std::string& prefix, const std::string& msg, T arg, Targs... msg_args)
    {
        std::stringstream ss;
        ss << msg << arg;
        uxmppLogFatal (prefix, ss.str(), msg_args...);
    }


    /**
     * Log an error message using the Logger class.
     * @param prefix A prefix to the log message.
     * @param message The log message.
     */
    inline void uxmppLogError (const std::string& prefix, const std::string& msg)
    {
        Logger::getInstance().log (LogLevel::error, prefix, msg);
    }
    template<typename T, typename... Targs>
    void uxmppLogError (const std::string& prefix, const std::string& msg, T arg, Targs... msg_args)
    {
        std::stringstream ss;
        ss << msg << arg;
        uxmppLogError (prefix, ss.str(), msg_args...);
    }


    /**
     * Log a warning message using the Logger class.
     * @param prefix A prefix to the log message.
     * @param message The log message.
     */
    inline void uxmppLogWarning (const std::string& prefix, const std::string& msg)
    {
        Logger::getInstance().log (LogLevel::warning, prefix, msg);
    }
    template<typename T, typename... Targs>
    void uxmppLogWarning (const std::string& prefix, const std::string& msg, T arg, Targs... msg_args)
    {
        std::stringstream ss;
        ss << msg << arg;
        uxmppLogWarning (prefix, ss.str(), msg_args...);
    }


    /**
     * Log an info message using the Logger class.
     * @param prefix A prefix to the log message.
     * @param message The log message.
     */
    inline void uxmppLogInfo (const std::string& prefix, const std::string& msg)
    {
        Logger::getInstance().log (LogLevel::info, prefix, msg);
    }
    template<typename T, typename... Targs>
    void uxmppLogInfo (const std::string& prefix, const std::string& msg, T arg, Targs... msg_args)
    {
        std::stringstream ss;
        ss << msg << arg;
        uxmppLogInfo (prefix, ss.str(), msg_args...);
    }


    /**
     * Log a debug message using the Logger class.
     * @param prefix A prefix to the log message.
     * @param message The log message.
     */
    inline void uxmppLogDebug (const std::string& prefix, const std::string& msg)
    {
        Logger::getInstance().log (LogLevel::debug, prefix, msg);
    }
    template<typename T, typename... Targs>
    void uxmppLogDebug (const std::string& prefix, const std::string& msg, T arg, Targs... msg_args)
    {
        std::stringstream ss;
        ss << msg << arg;
        uxmppLogDebug (prefix, ss.str(), msg_args...);
    }


    /**
     * Log a trace message using the Logger class.
     * @param prefix A prefix to the log message.
     * @param message The log message.
     */
    inline void uxmppLogTrace (const std::string& prefix, const std::string& msg)
    {
        Logger::getInstance().log (LogLevel::trace, prefix, msg);
    }
    template<typename T, typename... Targs>
    void uxmppLogTrace (const std::string& prefix, const std::string& msg, T arg, Targs... msg_args)
    {
        std::stringstream ss;
        ss << msg << arg;
        uxmppLogTrace (prefix, ss.str(), msg_args...);
    }


#ifndef NDEBUG
#define UXMPP_LOG_DEBUG(prefix, message) uxmppLogDebug(prefix, message)
#define UXMPP_LOG_TRACE(prefix, message) uxmppLogTrace(prefix, message)
#else
#define UXMPP_LOG_DEBUG(prefix, message)
#define UXMPP_LOG_TRACE(prefix, message)
#endif

}


#endif
