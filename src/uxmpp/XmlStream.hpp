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
#ifndef UXMPP_XMLSTREAM_HPP
#define UXMPP_XMLSTREAM_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/io/Connection.hpp>
#include <uxmpp/io/Timer.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/XmlInputStream.hpp>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <openssl/ssl.h>
#include <condition_variable>
#include <map>


namespace uxmpp {


/**
 * Maximum size of the input buffer when reading data from the input connection.
 */
#define UXMPP_MAX_RX_BUF_SIZE 2048


    /**
     * An XMPP XML stream.
     */
    class XmlStream {
    public:

        /**
         * Callback for received XML objects.
         * This callback will be called when XML objects are received on
         * the input connection.
         * Two special XML objects can be received from the XmlStream
         * object itself:
         * <http://ultramarin.se/uxmpp#error:parse-error code='error-code'/>
         * and
         * <http://ultramarin.se/uxmpp#timeout:timeout name'timer-name'/>.
         * "parse-error" will be received when an XML parsing error occurs
         * on the input connection.
         * "timeout" will be received when a timer set with method 'set_timeout'
         * expires.
         * @param stream The XmlStream object that received the XML object.
         * @param xml_obj The XML object.
         */
        typedef std::function<void (XmlStream& stream, XmlObject& xml_obj)> rx_func_t;

        /**
         * Constructor.
         * @param top_element The top XML element of the XML stream.
         */
        XmlStream (const XmlObject& top_element);

        /**
         * Destructor.
         */
        virtual ~XmlStream ();

        /**
         * Run the XML stream. This method does not return until the
         * stream has ended. The stream ends when one or both of the
         * supplied connections closes (i.e. a read/write operation
         * results in zero read/written bytes).
         * In a read/write error occurs an XML object in namespace
         * <code>http://ultramarin.se/uxmpp#internal-error</code> will
         * be sent to the XML callback but it is up to the callback
         * to stop the stream.
         * The connections are assumed to be connected before this call.
         * @param rx_connection The connection used for receiving XML objects.
         * @param tx_connection The connection used for sending XML objects.
         * @param tx_obj XML object to send once the stream is started.
         *               If tx_obj evaluates to false, none is sent.
         * @return True if the connection was successful, otherwise false.
         */
        virtual bool run (uxmpp::io::Connection& rx_connection,
                          uxmpp::io::Connection& tx_connection,
                          const XmlObject& tx_obj=XmlObject());

        /**
         * Stop stream.
         * This will stop the XML stream if not already stopped.
         */
        virtual void stop ();

        /**
         * Write an XML object to the stream.
         */
        virtual bool write (const XmlObject& xml_obj);

        /**
         * Reset the stream.
         * This will reset the XML parser to the same state as when the stream
         * was originally started.
         */
        void reset ();

        /**
         * Add/update/remove a timeout.
         * This method will set a timeout that will cause an XmlObject
         * to be received when the timer expires. The xml object will have
         * the tag 'timer' and the XML namespace is 'http://ultramarin.se/uxmpp#timer'.<BR>
         * If, for example, the timer id is "timer-id", then the xml object sent when
         * the timer expires will look like this:<BR>
         * <timer xmlns='http://ultramarin.se/uxmpp#timer' id='timer-id'/>
         * @param id The id of the timer. If the id is an empty string this method
         *           will return and do nothing, no timer will be set.
         * @param msec Amount on milliseconds in the future the timer will expire.
         *             If a timer with this id has already been added and has not yet expired,
         *             then it will be reset with this timeout value.
         *             If 0, the timer will expire as soon as possible.
         */
        void set_timeout (const std::string& id, unsigned msec);

        /**
         * Cancel a timeout.
         * This will cancel a previously set timeout.
         * @param id The ID of the timer to cancel.
         */
        void cancel_timeout (const std::string& id);

        /**
         * Set the XML receive callback function.
         * @param callback The function to be called for received XML objects.
         */
        void set_rx_cb (rx_func_t callback);

        /**
         *
         */
        bool is_running () const {
            return running;
        }


    private:
        XmlObject top_node;

        rx_func_t rx_cb;
        std::thread rx_thread;
        std::mutex mutex;
        bool running;
        
        std::queue<XmlObject> rx_queue;
        std::condition_variable rx_cond;
        std::mutex rx_cond_mutex;

        XmlInputStream xml_istream;
        uxmpp::io::Connection* rx_conn;
        uxmpp::io::Connection* tx_conn;
        std::array<char*, UXMPP_MAX_RX_BUF_SIZE> rx_buf;

        std::map<std::string, io::Timer> timers;

        std::mutex tx_buf_mutex;
        std::map <const char*, std::string> tx_buffers;

        static void rx_queue_thread_func (XmlStream* stream);
        void timer_callback (io::Timer& timer, const std::string& name);
        void rx_callback (io::Connection& conn, void* buf, ssize_t result, int errnum);
        void tx_callback (io::Connection& conn, void* buf, ssize_t result, int errnum);
    };


}


#endif
