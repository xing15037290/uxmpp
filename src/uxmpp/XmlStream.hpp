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
#include <uxmpp/io/IpHostAddr.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/XmlStreamListener.hpp>
#include <uxmpp/io/TlsConfig.hpp>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <openssl/ssl.h>
#include <condition_variable>
#include <event2/event.h>
#include <map>


namespace uxmpp {


    // Forward declaration.
        //class XmlStream;

    /**
     * Opaque class to store the XML parsing state.
     */
    class XmlStreamParseData;

    /**
     * Opaque class to handle timer events.
     */
    class XmlTimerEvent;


    /**
     * An XMPP XML stream.
     */
    class XmlStream {
    public:

        /**
         * Constructor.
         */
        XmlStream (const XmlObject& top_element);

        /**
         * Destructor.
         */
        virtual ~XmlStream ();

        /**
         * Start a new XML stream. This method does not return until the
         * stream has ended.
         * @param addr The address used to open an XML stream to.
         * @return True if the connection was successful, otherwise false.
         */
        virtual bool start (const uxmpp::io::IpHostAddr& addr);

        /**
         * Stop and close the stream.
         */
        virtual void stop ();

        /**
         * Write an XML object to the stream.
         */
        virtual bool write (const XmlObject& xml_obj);

        /**
         * Add a listener object that will receive events from the stream.
         */
        virtual void add_listener (XmlStreamListener& listener);

        /**
         * Remove a listener object that is receiving events from the stream.
         */
        virtual void del_listener (XmlStreamListener& listener);

        /**
         * Check if the stream is open.
         */
        bool is_open () const;

        /**
         * Return the IP(v4|v6) address of the peer.
         */
        uxmpp::io::IpHostAddr get_peer_addr () const;

        /**
         * Start TLS.
         */
        bool enable_tls (const uxmpp::io::TlsConfig& tls_cfg, std::string& error_description);

        /**
         * Reset the stream.
         * This will reset the XML parser to the same state as when the stream
         * was originally openend.
         */
        void reset ();

        /**
         * Add/update/remove a timeout.
         * This method will set a timeout that will cause an XmlObject to be sent to any
         * registered XmlStreamListener when the timer expires. The xml object will have
         * the tag 'timer' and the XML namespace is 'http://ultramarin.se/uxmpp#timer'.<BR>
         * If, for example, the timer id is "timer-id", then the xml object sent when
         * the timer expires will look like this:<BR>
         * <timer xmlns='http://ultramarin.se/uxmpp#timer' id='timer-id'/>
         * @param id The id of the timer. If the id is an empty string this method
         *           will return and do nothing, no timer will be set.
         * @param msec Amount on milliseconds in the future the timer will expire.
         *             If a timer with this id has already been added and has not yet expired,
         *             then it will be reset with this timeout value.
         *             If 0, any timer with this id will be disabled if not already expired.
         * @param cache_timer If set to true the internal timer object with this id
         *                    will be cached to make the resource handling more efficient.
         *                    Set this to true if you are reusing a timer id often. Set this
         *                    to false if you are using many timers with different id's.
         */
        void set_timeout (const std::string& id, unsigned msec, bool cache_timer=false);


    protected:

        /**
         * The network socket.
         */
        int sock;

        /**
         * SSL context.
         */
        SSL_CTX* ssl_ctx;

        /**
         * SSL connection object.
         */
        SSL* ssl;

        /**
         * The IP address of the peer.
         */
        uxmpp::io::IpHostAddr peer_addr;

        /**
         * A list of event listeners.
         */
        std::vector<XmlStreamListener*> listeners;

        /**
         * Mutex protecting resources.
         */
        std::mutex mutex;

        /**
         * Thread object.
         */
        std::thread rx_thread;

        /**
         * Flag indicating if the thread is running of not.
         */
        bool running;

        /**
         * XML parse data.
         */
        XmlStreamParseData* parse_data;

        /**
         * Callback on RX data.
         */
        void on_rx (const char* buf, int size);

        void initializeParseData ();

        /**
         * Method running in a separate thread (handling network traffic).
         */
        static void rx_queue_thread_func (XmlStream* stream);

        /**
         * Run the stream.
         */
        void run ();


    private:
        friend class XmlTimerEvent;

        void freeResources ();

        XmlObject top_node;
        std::queue<XmlObject> rx_queue;
        std::condition_variable rx_cond;
        std::mutex rx_cond_mutex;

        bool broken_socket;
        char buf[2048];
        static void event_rx_callback (evutil_socket_t fd, short what, void* stream);
        struct event_base* ebase;
        struct event* rx_event;

        std::map<std::string, XmlTimerEvent*> timers;
    };


}


#endif
