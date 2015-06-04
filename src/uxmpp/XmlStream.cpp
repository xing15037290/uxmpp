/*
 *  Copyright (C) 2013-2015 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/XmlStream.hpp>


#define THIS_FILE "XmlStream"

// Uncomment this for debug trace
//
//#define DEBUG_TRACE

#ifdef DEBUG_TRACE
#define TRACE(prefix, msg, ...) uxmpp_log_trace(prefix, msg, ## __VA_ARGS__)
#else
#define TRACE(prefix, msg, ...)
#endif



UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;
using namespace uxmpp::io;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlStream::XmlStream (const XmlObject& top_element)
    :
    top_node {top_element},
    running {false},
    xml_istream (top_element),
    rx_conn {nullptr},
    tx_conn {nullptr}
{
    // Handler for incoming XML objects
    //
    xml_istream.set_xml_handler ([this](XmlInputStream& stream, XmlObject& xml_obj){
            bool notify = false;
            rx_cond_mutex.lock ();
            if (running) {
                if (uxmpp_get_log_level() >= LogLevel::trace)
                    uxmpp_log_trace (THIS_FILE, "RX: ", to_string(xml_obj));
                rx_queue.push (xml_obj);
                notify = true;
            }
            rx_cond_mutex.unlock ();
            if (notify)
                rx_cond.notify_all ();
        });

    // Handler for XML parsing errors
    //
    xml_istream.set_error_handler ([this](XmlInputStream& stream, int code, const std::string& msg){
            if (uxmpp_get_log_level() >= LogLevel::info)
                uxmpp_log_info (THIS_FILE, "XML parse error: ", code, " - ", msg);
            XmlObject xml_obj ("parse-error", "http://ultramarin.se/uxmpp#internal-error");
            xml_obj.set_attribute ("code", std::to_string(code));
            xml_obj.set_content (msg);
            xml_istream << xml_obj;
        });
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlStream::~XmlStream ()
{
    stop ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool XmlStream::run (uxmpp::io::Connection& rx_connection,
                     uxmpp::io::Connection& tx_connection,
                     const XmlObject& tx_obj)
{
    std::lock_guard<std::mutex> lock (mutex);

    rx_conn = &rx_connection;
    tx_conn = &tx_connection;

    // Check if the stream is already running.
    //
    if (running) {
        uxmpp_log_warning (THIS_FILE, "Stream already running");
        return false;
    }
    if (std::this_thread::get_id() == rx_thread.get_id()) {
        uxmpp_log_warning (THIS_FILE, "Can't start the stream from the an XmlStream callback");
        return false;
    }

    uxmpp_log_debug (THIS_FILE, "Starting the XML stream");

    // Reset the XML input stream
    //
    xml_istream.reset ();

    // Start the RX queue thread.
    //
    running = true;
    rx_thread = std::thread (XmlStream::rx_queue_thread_func, this);

    // Set the connection RX callback
    //
    rx_conn->set_rx_cb ([this](Connection& c, void* b, ssize_t r, int e){
            rx_callback (c, b, r, e);
        });

    // Set the connection TX callback
    //
    tx_conn->set_tx_cb ([this](Connection& c, void* b, ssize_t r, int e){
            tx_callback (c, b, r, e);
        });

    // Send the initial XML object
    //
    write (tx_obj);

    // Start receiving data
    //
    rx_conn->read (rx_buf.data(), rx_buf.size());

    // Wain until all is done
    //
    mutex.unlock ();
    rx_thread.join ();
    mutex.lock ();

    TRACE (THIS_FILE, "Wait for TX to finish, ", tx_buffers.size(), " left");
    mutex.unlock ();
    while (! tx_buffers.empty()) {
        this_thread::sleep_for (chrono::milliseconds(10));
    }
    mutex.lock ();
    TRACE (THIS_FILE, "TX finished");

    // Clean up
    //
    //rx_conn->set_timer_cb (nullptr);
    rx_conn->set_rx_cb (nullptr);
    tx_conn->set_tx_cb (nullptr);
    mutex.unlock ();
    reset ();
    mutex.lock ();

    // Free TX resources
    //
    std::lock_guard<std::mutex> tx_buf_lock (tx_buf_mutex);
    tx_buffers.clear ();

    uxmpp_log_debug (THIS_FILE, "XML stream ended");

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::timer_callback (Timer& timer, const std::string& name)
{
    if (uxmpp_get_log_level() >= LogLevel::trace)
        uxmpp_log_trace (THIS_FILE, "Got timeout: ", name);
    mutex.lock ();
    auto ti = timers.find (name);
    if (ti == timers.end()) {
        mutex.unlock ();
        return;
    }
    //timers.erase (ti);
    mutex.unlock ();

    XmlObject xml_obj ("timeout", "http://ultramarin.se/uxmpp#internal-timer");
    xml_obj.set_attribute ("id", name);
    xml_istream << xml_obj;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::rx_callback (Connection& conn, void* buf, ssize_t result, int errnum)
{
    if (result > 0) {
        // We have received data, parse XML and continue reading
        string xml_data (static_cast<char*>(buf), result);
        rx_conn->read (rx_buf.data(), rx_buf.size());
        xml_istream << xml_data;
        return;
    }

    // Read failed

    if (result < 0) {
        //
        // Error
        // Send an XML object with namespace "http://ultramarin.se/uxmpp#internal-error"
        //
        uxmpp_log_warning (THIS_FILE, "Error reading RX connection, errno: ", errnum);
        XmlObject xml_obj ("rx-error", "http://ultramarin.se/uxmpp#internal-error");
        xml_obj.set_attribute ("errnum", std::to_string(errnum));
        xml_istream << xml_obj;
    } else {
        //
        // End-of-stream
        // Stop the stream
        //
        uxmpp_log_info (THIS_FILE, "RX connection closed");
        stop ();
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::tx_callback (Connection& conn, void* buf, ssize_t result, int errnum)
{
    TRACE (THIS_FILE, "Data written, result: ", result, ", data: ",
           (result>0?string(static_cast<char*>(buf), result):""));

    // Release TX resources
    //
    tx_buf_mutex.lock ();
    auto tx_buf = tx_buffers.find (static_cast<char*>(buf));
    if (tx_buf != tx_buffers.end()) {
        TRACE (THIS_FILE, "Remove TX buffer");
        tx_buffers.erase (tx_buf);
    }
    TRACE (THIS_FILE, "TX buffers left: ", tx_buffers.size(), (tx_buffers.empty()?", empty":""));
    tx_buf_mutex.unlock ();

    // Don't check for TX errors if we aren't running
    //
    if (!running || result>0)
        return;

    // Write failed

    if (result < 0) {
        //
        // Error
        // Send an XML object with namespace "http://ultramarin.se/uxmpp#internal-error"
        //
        uxmpp_log_warning (THIS_FILE, "Error writing TX connection, errno: ", errnum);
        XmlObject xml_obj ("tx-error", "http://ultramarin.se/uxmpp#internal-error");
        xml_obj.set_attribute ("errnum", std::to_string(errnum));
        xml_istream << xml_obj;
    }else{
        //
        // End-of-stream
        // Stop the stream
        //
        uxmpp_log_info (THIS_FILE, "TX connection closed");
        stop ();
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::stop ()
{
    uxmpp_log_debug (THIS_FILE, "Stop the XML stream");
    rx_cond_mutex.lock ();
    if (running) {
///*
        if (rx_conn)
            rx_conn->cancel ();
        if (tx_conn)
            tx_conn->cancel ();
//*/
        running = false;
        rx_cond_mutex.unlock ();
        rx_cond.notify_all ();
    }else{
        rx_cond_mutex.unlock ();
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool XmlStream::write (const XmlObject& xml_obj)
{
    if (!xml_obj)
        return true;

    std::lock_guard<std::mutex> lock (rx_cond_mutex);
    if (!running || !tx_conn) {
        uxmpp_log_debug (THIS_FILE, "TX ignored, stream is not open");
        return false;
    }

    string tx_buf (to_string(xml_obj));

    if (uxmpp_get_log_level() >= LogLevel::trace)
        uxmpp_log_trace (THIS_FILE, "TX: ", tx_buf);

    std::lock_guard<std::mutex> tx_buf_lock (tx_buf_mutex);
    auto key = tx_buf.c_str ();           // key is the pointer to the string buffer
    tx_buffers[key] = std::move (tx_buf); // put the string buffer in the buffer map
    //tx_conn->write ((void*)(tx_buffers[key].c_str()), tx_buffers[key].length());
    tx_conn->write ((void*)key, tx_buffers[key].length());

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::reset ()
{
    std::lock_guard<std::mutex> lock (mutex);
    uxmpp_log_debug (THIS_FILE, "Reset the XML stream");
    xml_istream.reset ();

    // Clear I/O operations
    //
    rx_conn->cancel ();

    // Clear any lingering timeout
    if (rx_conn) {
        for (auto i=timers.begin(); i!=timers.end(); ++i)
            i->second.cancel ();
    }
    timers.clear ();

    // Start receiving data
    //
    if (running)
        rx_conn->read (rx_buf.data(), rx_buf.size());
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::set_timeout (const std::string& id, unsigned msec)
{
    std::lock_guard<std::mutex> lock (mutex);

    // Sanity check
    //
    if (id.empty()) {
        uxmpp_log_warning (THIS_FILE, "Unable to set nameless timeout");
        return;
    }

    // Logging
    //
    if (uxmpp_get_log_level() >= LogLevel::trace)
        uxmpp_log_trace (THIS_FILE, "Set timeout '", id, "' to ", msec, " msec");

    // Set the timer
    //
    auto& timer = timers[id];
    timer.set (Timer::milliseconds(msec), [this, &timer, id](){
            timer_callback (timer, id);
        });
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::cancel_timeout (const std::string& id)
{
    std::lock_guard<std::mutex> lock (mutex);

    // Sanity check
    //
    if (id.empty()) {
        uxmpp_log_warning (THIS_FILE, "Unable to cancel nameless timeout");
        return;
    }

    // Cancel the timer
    //
    auto ti = timers.find (id);
    if (ti != timers.end()) {
        ti->second.cancel ();
        timers.erase (ti);
    }

    // Logging
    //
    if (uxmpp_get_log_level() >= LogLevel::trace)
        uxmpp_log_trace (THIS_FILE, "Cancel timeout '", id, "'");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::set_rx_cb (rx_func_t callback)
{
    std::lock_guard<std::mutex> lock (mutex);
    rx_cb = callback;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::rx_queue_thread_func (XmlStream* stream)
{
    XmlStream& self = *stream;
    unique_lock<std::mutex> ul (self.rx_cond_mutex);

    TRACE (THIS_FILE, "Starting RX thread");
    while (self.running) {

        // Wait unitl we have an XML object or we should stop running
        //
        self.rx_cond.wait (ul, [&self]{return !self.running || !self.rx_queue.empty();});

        // Check if we are done
        //
        if (!self.running) {
            break;
        }

        // Handle received XML objects
        //
        while (!self.rx_queue.empty() /*&& self.running*/) {
            XmlObject& xml_obj = self.rx_queue.front ();
/*
            // If we got a stream error we should stop the stream after notifying
            // the callback.
            bool stop_stream;
            stop_stream = xml_obj.get_namespace() == "http://ultramarin.se/uxmpp#internal-error";
*/
            // Notify the callback
            //
            if (self.rx_cb) {
                self.rx_cond_mutex.unlock ();
                self.rx_cb (self, xml_obj);
                self.rx_cond_mutex.lock ();
            }
            self.rx_queue.pop ();
/*
            // Signal the stream to stop if we have a stream error
            //
            if (stop_stream)
                stop ();
*/
        }
    }
    TRACE (THIS_FILE, "Ending RX thread");
}



UXMPP_END_NAMESPACE1
