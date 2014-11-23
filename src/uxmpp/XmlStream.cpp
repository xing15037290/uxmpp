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
#include <uxmpp/XmlStream.hpp>
#include <uxmpp/XmlNames.hpp>
#include <cstring>
#include <stack>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <expat.h>
#include <openssl/err.h>
#include <forward_list>


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
using namespace uxmpp::net;


static void start_stream_element (void* user_data,
                                  const XML_Char* name,
                                  const XML_Char** attributes);
static void start_xml_node (void* user_data,
                            const XML_Char* name,
                            const XML_Char** attributes);
static void end_xml_node (void* user_data,
                          const XML_Char* name);
static void xml_character_data (void* userData,
                                const XML_Char* s,
                                int len);

/**
 *
 */
class XmlStreamParseElement {
public:
    XmlStreamParseElement (int reserved_nodes) : xml_obj(reserved_nodes) {
    }
    XmlObject xml_obj;
    string default_namespace;
    map<string, string> namespace_aliases;
};

/**
 *
 */
class XmlStreamParseData {
public:
    XML_Parser xml_parser;
    forward_list<XmlStreamParseElement*> element_stack;
    //XmlStreamParseElement* element;
    bool error;
    bool top_element_found;

    XmlStream* stream;
    std::mutex* mutex;
    std::vector<XmlStreamListener*>* listeners;

    std::queue<XmlObject>* rx_queue;
    std::condition_variable* rx_cond;
    std::mutex* rx_cond_mutex;

    string default_namespace;
    map<string, string> namespace_aliases;

    XmlObject top_node;
};


/**
 *
 */
class XmlTimerEvent {
public:
    /**
     * Constructor.
     */
    XmlTimerEvent (const string& timeout_id,
                   XmlStream& timeout_stream,
                   struct event_base* timeout_ebase,
                   XmlStreamParseData* parse_data,
                   bool cache_object)
        :
        id     {timeout_id},
        stream (timeout_stream),
        event  {nullptr},
        cached {cache_object}
    {
        if (stream.ebase)
            event = evtimer_new (stream.ebase, event_timeout_callback, this);
    }

    /**
     * Destructor.
     */
    ~XmlTimerEvent () {
        if (event)
            event_free (event);
    }

    /**
     * Start the timer.
     */
    void start (unsigned msec) {
        if (!event) {
            if (stream.ebase) {
                event = evtimer_new (stream.ebase, event_timeout_callback, this);
            }
        }
        if (!event) {
            uxmpp_log_error (THIS_FILE, "Unable to start timer ", id);
            return;
        }

        stop (); // Stop it first if it is running

        struct timeval tv;
        if (msec >= 1000) {
            tv.tv_sec = msec / 1000;
            msec %= 1000;
        }else{
            tv.tv_sec = 0;
        }
        tv.tv_usec = msec * 1000;

        evtimer_add (event, &tv);
    }

    /**
     * Stop the timer.
     */
    void stop () {
        if (event)
            evtimer_del (event);
    }

    /**
     * Timeout callback.
     */
    static void event_timeout_callback (evutil_socket_t fd, short what, void* stream);

    string        id;
    XmlStream&    stream;
    struct event* event;
    bool          cached;
};


static const int net_buf_size {2048};
static const char namespace_delim {':'};
//static const string start_stream_node_name = "stream:stream";



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void parse_xml_tag_name (const XML_Char* name, string& tag_name, string& xml_namespace)
{
    char* pos = strrchr ((char*)name, namespace_delim);
    if (!pos) {
        tag_name = name;
        xml_namespace = "";
    }else{
        tag_name      = string (pos+1, name+strlen(name)-(pos+1));
        xml_namespace = string (name, pos-name);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void parse_xml_attributes (const XML_Char** attributes,
                                  XmlObject& xml_obj,
                                  string& default_namespace,
                                  map<string, string>& namespace_aliases)
{
    string xml_namespace = xml_obj.get_namespace ();
    string name;
    string value;

    default_namespace = "";
    namespace_aliases.clear ();

    for (const XML_Char** i=attributes; *i!=NULL; i++) {
        name  = string (*i);
        if (++i != NULL)
            value = string (*i);
        else
            value = "";

        // Handle default namespace attribute
        //
        if (name == "xmlns") {
            if (value.length() == 0)
                continue;

            TRACE (THIS_FILE, "parse_xml_attributes - set default namespace: ", value);

            default_namespace = value;
            xml_obj.set_default_namespace_attr (default_namespace);
            if (xml_namespace.length() == 0) {
                xml_namespace = default_namespace;
                xml_obj.set_namespace (xml_namespace);
            }
            if (default_namespace == xml_namespace)
                xml_obj.is_namespace_default (true);
        }
        //
        // Handle namespace alias attribute
        //
        else if (name.find("xmlns:") == 0) {
            if (value.length() == 0)
                continue;
            string alias = name.substr (6);
            if (alias.length()) {

                TRACE (THIS_FILE, "parse_xml_attributes - add namespace alias: ", alias, "=", value);

                namespace_aliases[alias] = value;
/*
                if (alias == xml_obj.getNamespace()) {
                    uxmpp_log_debug (THIS_FILE, "Set namespace to: ->", value, "<-");
                    xml_obj.setNamespace (value);
                    if (xml_obj.getDefaultNamespaceAttr() == xml_obj.getNamespace())
                        xml_obj.isNamespaceDefault (true);
                }
*/
            }
        }
        //
        // Handle 'normal' attribute
        //
        else{
            xml_obj.set_attribute (name, value);
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void normalize_namespace (XmlStreamParseData& pd, XmlObject& xml_obj)
{
    string xml_namespace = xml_obj.get_namespace ();

    if (xml_namespace.length() == 0) {
        //
        // Find the default namespace
        //
        TRACE (THIS_FILE, "normalize_namespace - find the default namespace");
        xml_namespace = pd.default_namespace; // Fallback to top-level default namespace
        TRACE (THIS_FILE, "normalize_namespace - top level default namespace is: ", xml_namespace);
        for (auto element : pd.element_stack) {
            TRACE (THIS_FILE, "normalize_namespace - peek namespace in stack: ", element->default_namespace);
            if (element->default_namespace.length() > 0) {
                xml_namespace = element->default_namespace;
                break;
            }
        }
        if (xml_namespace.length() != 0) {
            TRACE (THIS_FILE, "normalize_namespace - found default namespace: ", xml_namespace);
            xml_obj.set_namespace (xml_namespace);
            xml_obj.is_namespace_default (true);
        }
    }else{
        //
        // Find a namespace alias
        //
        TRACE (THIS_FILE, "normalize_namespace - find a namespace alias");
        string alias_value = "";
        for (auto element : pd.element_stack) {
            alias_value = element->namespace_aliases[xml_namespace];
            if (alias_value.length() > 0)
                break;
        }
        if (alias_value.length() == 0)
            alias_value = pd.namespace_aliases[xml_namespace];

        if (alias_value.length()) {
            xml_obj.set_namespace (alias_value);
            // See if this is also the default namespace
            string default_namespace = pd.default_namespace;
            for (auto element : pd.element_stack) {
                if (element->default_namespace.length() > 0) {
                    default_namespace = element->default_namespace;
                    break;
                }
            }
            if (default_namespace.length() && default_namespace == xml_obj.get_default_namespace_attr())
                xml_obj.is_namespace_default (true);
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlStream::XmlStream (const XmlObject& top_element)
    :
    sock       {-1},
    ssl_ctx    {nullptr},
    ssl        {nullptr},
    running    {false},
    parse_data {nullptr},
    top_node   {top_element},
    ebase      {nullptr},
    rx_event   {nullptr}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlStream::~XmlStream ()
{
    stop ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool XmlStream::start (const uxmpp::net::IpHostAddr& addr)
{
    std::lock_guard<std::mutex> lock (mutex);

    // Check if the stream is already running.
    //
    if (running) {
        uxmpp_log_warning (THIS_FILE, "Stream already connected to ", to_string(peer_addr));
        return false;
    }
    if (std::this_thread::get_id() == rx_thread.get_id()) {
        uxmpp_log_warning (THIS_FILE, "Can't start the stream from the XmlStreamListener");
        return false;
    }

    // Set peer address
    //
    peer_addr = addr;
    uxmpp_log_debug (THIS_FILE, "Open XML stream to ", to_string(peer_addr));

    /*
     * Assume TCP for now.
     */

    struct sockaddr_in  saddr4;
    struct sockaddr_in6 saddr6;
    struct sockaddr*    saddr;
    socklen_t saddr_len;

    // Open the socket
    //
    if (peer_addr.type == AddrType::ipv4) {
        saddr4.sin_family      = AF_INET;
        saddr4.sin_addr.s_addr = peer_addr.ipv4;
        saddr4.sin_port        = peer_addr.port;
        saddr = reinterpret_cast<struct sockaddr*> (&saddr4);
        saddr_len = sizeof (saddr4);
        sock = socket (AF_INET, SOCK_STREAM, 0);
    }
    else if (peer_addr.type == AddrType::ipv6) {
        saddr6.sin6_family = AF_INET6;
        memcpy (&saddr6.sin6_addr, &peer_addr.ipv6, sizeof(saddr6.sin6_addr));
        saddr6.sin6_port   = peer_addr.port;
        saddr = reinterpret_cast<struct sockaddr*> (&saddr6);
        saddr_len = sizeof (saddr6);
        sock = socket (AF_INET6, SOCK_STREAM, 0);
    }
    else {
        uxmpp_log_error (THIS_FILE, "Wrong address type");
        return false;
    }

    // Check for errors
    //
    if (sock == -1) {
        uxmpp_log_error (THIS_FILE, "Error creating socket");
        return false;
    }

    // Connect the socket
    //
    int result = connect (sock, saddr, saddr_len);
    if (result == -1) {
        uxmpp_log_error (THIS_FILE, "Error connecting socket");
        ::close (sock);
        sock = -1;
        return false;
    }

    // Start the RX queue thread.
    //
    running = true;
    rx_thread = std::thread (XmlStream::rx_queue_thread_func, this);

    // Create the event base
    //
    ebase = event_base_new ();

    // Run the stream
    //
    run ();

    rx_cond_mutex.lock ();
    if (running) {
        running = false;
        rx_cond_mutex.unlock ();
        rx_cond.notify_all ();
    }else{
        rx_cond_mutex.unlock ();
    }

    if (rx_thread.joinable()) {
        mutex.unlock ();
        rx_thread.join ();
        mutex.lock ();
    }

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static string get_tls_error_text (int result)
{
    int last_error = result;
    int ssl_error = ERR_get_error ();
    while (ssl_error) {
        last_error = ssl_error;
        ssl_error = ERR_get_error ();
    }
    const char* err_str = ERR_error_string (last_error, NULL);
    if (err_str)
        return string (err_str);
    else
        return string ("");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool XmlStream::enable_tls (const TlsConfig& tls_cfg, string& error_description)
{
    mutex.lock ();

    // Check if the stream is running
    //
    if (!running) {
        mutex.unlock ();
        uxmpp_log_debug (THIS_FILE, "Can't enable TLS, stream not open");
        error_description = "Stream no open";
        return false;
    }

    // Check if TLS is already enabled
    //
    if (peer_addr.proto == AddrProto::tls) {
        mutex.unlock ();
        uxmpp_log_trace (THIS_FILE, "TLS already enabled");
        error_description = "";
        return true;
    }

    // Temporarily stop the event loop
    //
    event_del (rx_event);

    // Initialize the SSL library.
    //
    static bool ssl_initialized = false;
    if (!ssl_initialized) {
        SSL_load_error_strings ();
        ERR_load_crypto_strings ();
        SSL_library_init ();
        ssl_initialized = true;
    }

    // Create the SSL context
    //
    const SSL_METHOD* method = nullptr;
    switch (tls_cfg.method) {
    case TlsMethod::sslv3:
        method = SSLv3_method ();
        break;
    case TlsMethod::tlsv1:
        method = TLSv1_method ();
        break;
    case TlsMethod::tlsv1_1:
        method = TLSv1_1_method ();
        break;
    case TlsMethod::tlsv1_2:
        method = TLSv1_2_method ();
        break;
    case TlsMethod::dtlsv1:
        method = DTLSv1_method ();
        break;
    case TlsMethod::sslv23:
    default:
        method = SSLv23_method ();
    }
    ssl_ctx = SSL_CTX_new (method);
    if (!ssl_ctx) {
        uxmpp_log_error (THIS_FILE, "Unable to create SSL context");
        event_add (rx_event, NULL);//&tv);
        mutex.unlock ();
        stop ();
        error_description = "Unable to create SSL context";
        return false;
    }

    // Set server verification
    //
    SSL_CTX_set_verify (ssl_ctx, tls_cfg.verify_server ? SSL_VERIFY_PEER : SSL_VERIFY_NONE, nullptr);

    // Create the SSL stream object
    //
    ssl = SSL_new (ssl_ctx);
    if (!ssl) {
        uxmpp_log_error (THIS_FILE, "Unable to create SSL object");
        event_add (rx_event, NULL);//&tv);
        mutex.unlock ();
        stop ();
        error_description = "Unable to create SSL object";
        return false;
    }

    // Connect the SSL stream object with the socket
    //
    int result = SSL_set_fd (ssl, sock);
    if (!result) {
        string tls_error = get_tls_error_text (result);
        error_description = string("Unable to initialize the SSL stream, ") + tls_error;
        uxmpp_log_error (THIS_FILE, error_description);
        event_add (rx_event, NULL);//&tv);
        mutex.unlock ();
        stop ();
        return false;
    }

    // Perform TLS handshake
    //
    uxmpp_log_trace (THIS_FILE, "Perform TLS handshake");
    ERR_clear_error ();
    result = SSL_connect (ssl);
    if (result != 1) {
        string tls_error = get_tls_error_text (result);
        error_description = string("TLS handshake failed, ") + tls_error;
        uxmpp_log_error (THIS_FILE, error_description);
        event_add (rx_event, NULL);//&tv);
        mutex.unlock ();
        stop ();
        return false;
    }
    peer_addr.proto = AddrProto::tls;

    event_add (rx_event, NULL);//&tv);
    mutex.unlock ();
    uxmpp_log_debug (THIS_FILE, "TLS (", to_string(tls_cfg.method), ") enabled");
    error_description = "";
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::stop ()
{
    std::lock_guard<std::mutex> lock (mutex);

    if (!running) {
        uxmpp_log_trace (THIS_FILE, "Stop non-running xml stream");
        return;
    }

    if (std::this_thread::get_id() == rx_thread.get_id()) {
        uxmpp_log_trace (THIS_FILE, "Stop xml stream from RX thread");
        running = false;
        event_base_loopbreak (ebase);
        return;
    }

    uxmpp_log_trace (THIS_FILE, "Stop xml stream and notify RX thread");
    rx_cond_mutex.lock ();
    if (running) {
        running = false;
        event_base_loopbreak (ebase);
        rx_cond_mutex.unlock ();
        rx_cond.notify_all ();
        rx_thread.join ();
    }else{
        rx_cond_mutex.unlock ();
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool XmlStream::write (const XmlObject& xml_obj)
{
    std::lock_guard<std::mutex> lock (mutex);

    if (sock == -1) {
        uxmpp_log_warning (THIS_FILE, "Unable to write to stream: not opened");
        return false;
    }

    string xml_str = to_string (xml_obj);
    size_t len = xml_str.length ();
    size_t sent = 0;

    uxmpp_log_debug (THIS_FILE, "Send xml object: ", /*xml_str*/ to_string(xml_obj, true));
    while (sent < len) {
        int result;

        if (peer_addr.proto == AddrProto::tls)
            result = SSL_write (ssl, xml_str.c_str()+sent, len-sent);
        else
            result = ::write (sock, xml_str.c_str()+sent, len-sent);

        if (result <= 0) {
            if (result==0) {
                uxmpp_log_error (THIS_FILE, "Unable to write XML object, connection reset by peer");
            }else{
                uxmpp_log_error (THIS_FILE, "Error writing XML object to stream");
            }
            return false;
        }
        sent += result;
    }
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::add_listener (XmlStreamListener& listener)
{
    std::lock_guard<std::mutex> lock (mutex);

    for (auto l : listeners) {
        if (l == &listener)
            return; // Already added
    }
    listeners.push_back (&listener);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::del_listener (XmlStreamListener& listener)
{
    std::lock_guard<std::mutex> lock (mutex);

    for (auto i=listeners.begin(); i!=listeners.end(); i++) {
        if ((*i) == &listener) {
            listeners.erase (i);
            return;
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool XmlStream::is_open () const
{
    return sock != -1;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uxmpp::net::IpHostAddr XmlStream::get_peer_addr () const
{
    return peer_addr;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::set_timeout (const std::string& id, unsigned msec, bool cache_timer)
{
    if (id.length() == 0)
        return;

    std::lock_guard<std::mutex> lock (mutex);
    XmlTimerEvent* te = timers[id];
    if (!te) {
        if (!msec)
            return;
        uxmpp_log_trace (THIS_FILE, "Creating timer ", id);
        te = new XmlTimerEvent (id, *this, ebase, parse_data, cache_timer);
        timers[id] = te;
    }else{
        uxmpp_log_trace (THIS_FILE, "Found cached timer ", id);
        if (!cache_timer)
            te->cached = false;
    }

    if (msec) {
        uxmpp_log_trace (THIS_FILE, "Start timer ", id);
        te->start (msec);
    }else{
        uxmpp_log_trace (THIS_FILE, "Disable timer ", id);
        te->stop ();
        if (!te->cached) {
            uxmpp_log_trace (THIS_FILE, "Delete timer ", id);
            timers[id] = nullptr;
            delete te;
            timers.erase (id);
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void end_xml_node (void* user_data,
                          const XML_Char* name)
{
    XmlStreamParseData* pd = reinterpret_cast<XmlStreamParseData*> (user_data);

    // Check for end of the top-level xml tag
    //
    if (pd->element_stack.empty()) {
        if (pd->top_element_found) {
            pd->top_element_found = false;
            XmlObject xml_obj (XmlStreamTag, XmlStreamNs, false);
            xml_obj.set_part (XmlObjPart::end);
            pd->rx_cond_mutex->lock ();
            pd->rx_queue->push (xml_obj);
            pd->rx_cond_mutex->unlock ();
            pd->rx_cond->notify_all ();
            XML_SetElementHandler (pd->xml_parser,
                                   start_stream_element,
                                   NULL);
        }
        return;
    }

    // Pop a parsed element from the stack
    //
    XmlStreamParseElement* element = pd->element_stack.front ();
    TRACE (THIS_FILE, "end_xml_node - popping element from stack (", element->xml_obj.getName(), ")");
    pd->element_stack.pop_front ();

    if (pd->element_stack.empty()) {
        TRACE (THIS_FILE, "end_xml_node - complete XML object fround (", element->xml_obj.getName(), ")");
        pd->rx_cond_mutex->lock ();
        pd->rx_queue->push (std::move(element->xml_obj));
        pd->rx_cond_mutex->unlock ();
        pd->rx_cond->notify_all ();
    }else{
        XmlStreamParseElement* parent_element = pd->element_stack.front ();
        parent_element->xml_obj.add_node (std::move(element->xml_obj));
    }
    delete element;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void start_xml_node (void* user_data,
                            const XML_Char* name,
                            const XML_Char** attributes)
{
    XmlStreamParseData* pd = reinterpret_cast<XmlStreamParseData*> (user_data);
    string tag_name;
    string xml_namespace;
    XmlStreamParseElement* parent_element = pd->element_stack.empty() ? nullptr : pd->element_stack.front();
    XmlStreamParseElement* element = new XmlStreamParseElement (parent_element ? 0 : 4);
    XmlObject& xml_obj = element->xml_obj;

    // Split the full tag name into name and namespace.
    //
    parse_xml_tag_name (name, tag_name, xml_namespace);
    xml_obj.set_tag_name (tag_name);
    if (xml_namespace.length())
        xml_obj.set_namespace (xml_namespace);

    // Parse xml attributes
    //
    TRACE (THIS_FILE, "start_xml_node - before parsing attributes: (", xml_obj.getNamespace(), ")", to_string(xml_obj));
    parse_xml_attributes (attributes, xml_obj, element->default_namespace, element->namespace_aliases);
    TRACE (THIS_FILE, "start_xml_node - after parsing attributes: (", xml_obj.getNamespace(), ")", to_string(xml_obj));

    // Normalize namespace
    //
    normalize_namespace (*pd, xml_obj);
    TRACE (THIS_FILE, "start_xml_node - after normalizing namespace: (", xml_obj.getFullName(), ")", to_string(xml_obj));

    // Push the current XML object on the stack
    //
    pd->element_stack.push_front (element);
    TRACE (THIS_FILE, "start_xml_node - pushing element to stack (", element->xml_obj.getName(), ")");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void xml_character_data (void* user_data,
                                const XML_Char* data,
                                int len)
{
    XmlStreamParseData* pd = reinterpret_cast<XmlStreamParseData*> (user_data);
    XmlStreamParseElement* element = pd->element_stack.empty() ? nullptr : pd->element_stack.front();

    if (element == nullptr)
        return;

    element->xml_obj.set_content (element->xml_obj.get_content() + string(data, len));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void start_stream_element (void* user_data,
                                  const XML_Char* name,
                                  const XML_Char** attributes)
{
    XmlStreamParseData* pd = reinterpret_cast<XmlStreamParseData*> (user_data);
    string tag_name;
    string xml_namespace;
    string default_namespace = "";
    string full_name = "";
    XmlObject xml_obj (0);

    // Split the name into namespace and name
    //
    parse_xml_tag_name (name, tag_name, xml_namespace);
    xml_obj.set_tag_name (tag_name);
    if (xml_namespace.length())
        xml_obj.set_namespace (xml_namespace);

    // Parse xml attributes
    //
    TRACE (THIS_FILE, "start_stream_element - before parsing attributes: ", to_string(xml_obj));
    parse_xml_attributes (attributes, xml_obj, pd->default_namespace, pd->namespace_aliases);
    TRACE (THIS_FILE, "start_stream_element - after parsing attributes: ", to_string(xml_obj));

    // Normalize namespace
    //
    normalize_namespace (*pd, xml_obj);
    xml_namespace = xml_obj.get_namespace ();
    TRACE (THIS_FILE, "start_stream_element - after normalizing namespace: ", to_string(xml_obj));

    // Find default namespace if needed
    //
    if (!xml_namespace.length() && pd->default_namespace.length()) {
        // Find default namespace
        xml_namespace = pd->default_namespace;
        xml_obj.set_namespace (xml_namespace);
        xml_obj.is_namespace_default (true);
    }

    // Get the canonical tag name
    //
    if (xml_namespace.length()) {
        /*
        full_name = xml_obj.getNamespaceAlias (xml_namespace);
        if (full_name.length()) {
            full_name += string(":") + tag_name;
        }else{
        */
            full_name = xml_namespace + string(":") + tag_name;
            //}
    }else if (default_namespace.length() > 0) {
        full_name = default_namespace + string(":") + tag_name;
    }else{
        full_name = tag_name;
    }

    // Check if this is the XMPP start stream tag.
    //
    if (pd->top_node.get_full_name() == full_name) {
        uxmpp_log_debug (THIS_FILE, "Got start stream element");
        XML_SetElementHandler (pd->xml_parser,
                               start_xml_node,
                               end_xml_node);
        XML_SetCharacterDataHandler (pd->xml_parser, xml_character_data);

        xml_obj.set_part (XmlObjPart::start);

        pd->top_element_found = true;
        pd->rx_cond_mutex->lock ();
        pd->rx_queue->push (xml_obj);
        pd->rx_cond_mutex->unlock ();
        pd->rx_cond->notify_all ();
    }else{
        uxmpp_log_debug (THIS_FILE, "Ignoring XML element: ", full_name);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::reset ()
{
    uxmpp_log_debug (THIS_FILE, "Reset XML parse data");
    mutex.lock ();
    initializeParseData ();
    mutex.unlock ();
    uxmpp_log_debug (THIS_FILE, "Reset XML parse data - done");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::initializeParseData ()
{
    if (parse_data != nullptr) {
        while (!parse_data->element_stack.empty()) {
            delete parse_data->element_stack.front ();
            parse_data->element_stack.pop_front ();
        }
        if (parse_data->xml_parser != nullptr) {
            XML_ParserFree (parse_data->xml_parser);
            parse_data->xml_parser = nullptr;
        }
    }else{
        parse_data = new XmlStreamParseData;
    }

    //parse_data->xml_parser = XML_ParserCreateNS (NULL, namespace_delim);
    parse_data->xml_parser = XML_ParserCreate (NULL);
    parse_data->error      = false;
    parse_data->stream     = this;
    parse_data->mutex      = &mutex;
    parse_data->listeners  = &listeners;
    parse_data->top_node   = top_node;
    parse_data->top_element_found = false;

    parse_data->default_namespace = "";
    parse_data->namespace_aliases.clear ();

    rx_cond_mutex.lock ();
    parse_data->rx_queue      = &rx_queue;
    parse_data->rx_cond       = &rx_cond;
    parse_data->rx_cond_mutex = &rx_cond_mutex;
    rx_cond_mutex.unlock ();

    XML_SetUserData (parse_data->xml_parser, parse_data);
    XML_SetElementHandler (parse_data->xml_parser,
                           start_stream_element,
                           NULL);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::rx_queue_thread_func (XmlStream* stream)
{
    XmlStream& xs = *stream;

    uxmpp_log_trace (THIS_FILE, "Starting XML RX queue thread.");
    unique_lock<std::mutex> ul (xs.rx_cond_mutex);


    xs.rx_cond_mutex.unlock ();
    for (auto listener : xs.listeners)
        listener->on_open (xs);
    xs.rx_cond_mutex.lock ();

    while (xs.running) {
        xs.rx_cond.wait (ul, [&]{return !xs.running || !xs.rx_queue.empty();});

        while (!xs.rx_queue.empty()) {
            XmlObject& xml_obj = xs.rx_queue.front ();
            xs.rx_cond_mutex.unlock ();
            for (auto listener : xs.listeners)
                listener->on_rx_xml_obj (xs, xml_obj);
            xs.rx_cond_mutex.lock ();
            xs.rx_queue.pop ();
        }
        if (!xs.running) {
            break;
        }
    }

    xs.rx_cond_mutex.unlock ();
    for (auto listener : xs.listeners)
        listener->on_close (xs);
    xs.rx_cond_mutex.lock ();

    event_base_loopbreak (xs.ebase);

    uxmpp_log_trace (THIS_FILE, "RX queue thread ended");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlTimerEvent::event_timeout_callback (evutil_socket_t fd, short what, void* arg)
{
    XmlObject xml_obj (XmlUxmppTimerTag, XmlUxmppTimerNs);
    XmlTimerEvent* te = reinterpret_cast<XmlTimerEvent*> (arg);
    if (!te)
        return;
    uxmpp_log_trace (THIS_FILE, "Timeout: ", te->id);
    xml_obj.set_attribute ("id", te->id);

    std::lock_guard<std::mutex> lock (te->stream.mutex);

    te->stream.rx_cond_mutex.lock ();
    te->stream.rx_queue.push (xml_obj);
    te->stream.rx_cond_mutex.unlock ();
    te->stream.rx_cond.notify_all ();

    if (!te->cached) {
        uxmpp_log_trace (THIS_FILE, "Delete timer ", te->id);
        te->stream.timers[te->id] = nullptr;
        te->stream.timers.erase (te->id);
        delete te;
    }
}


//------------------------------------------------------------------------------
// Called by 'event_base_dispatch' in method 'run'.
//------------------------------------------------------------------------------
void XmlStream::event_rx_callback (evutil_socket_t fd, short what, void* stream)
{
    XmlStream* self = reinterpret_cast<XmlStream*> (stream);
    std::lock_guard<std::mutex> lock (self->mutex);

    if (!self->running) {
        // Disable all timers
        for (auto& t : self->timers) {
            if (t.second) {
                uxmpp_log_trace (THIS_FILE, "Disable timer ", t.second->id);
                t.second->stop ();
            }
        }
        event_base_loopbreak (self->ebase);
        return;
    }

    // Check timeout
    //
    if (what & EV_TIMEOUT) {
        //uxmpp_log_trace (THIS_FILE, "RX timed out");
        return;
    }

    // Make sure we got a read event
    //
    if (!(what & EV_READ)) {
        return;
    }

    // Read data from socket
    //
    int result;
    if (self->peer_addr.proto == AddrProto::tls)
        result = SSL_read (self->ssl, self->buf, sizeof(self->buf));
    else
        result = ::read (self->sock, self->buf, sizeof(self->buf));

    if (result == -1) {
        uxmpp_log_error (THIS_FILE, "RX read error");
        self->broken_socket = true;
        event_base_loopbreak (self->ebase);
    }
    else if (result == 0) {
        uxmpp_log_debug (THIS_FILE, "Connection reset by peer");
        self->broken_socket = true;
        event_base_loopbreak (self->ebase);
    }

    // Check for broken socket
    //
    if (self->broken_socket) {
        // Disable all timers
        for (auto& t : self->timers) {
            if (t.second) {
                uxmpp_log_trace (THIS_FILE, "Disable timer ", t.second->id);
                t.second->stop ();
            }
        }
        event_base_loopbreak (self->ebase);
        return;
    }

    self->on_rx (self->buf, result);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::run ()
{
    // Initialize parse data
    //
    initializeParseData ();

    rx_event = event_new (ebase, sock, EV_TIMEOUT | EV_READ | EV_PERSIST, event_rx_callback, this);

    int result;
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 500000;

    broken_socket = false;
    while (running && !broken_socket) {
        event_add (rx_event, &tv);
        mutex.unlock ();
        uxmpp_log_trace (THIS_FILE, "Run event loop");
        result = event_base_loop (ebase, 0);
        uxmpp_log_trace (THIS_FILE, "Event loop returned ", result);
        mutex.lock ();
        event_del (rx_event);
    }

    freeResources ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::freeResources ()
{
    if (sock == -1)
        return;

    // Close the socket
    //
    uxmpp_log_debug (THIS_FILE, "Closing XML stream to ", to_string(peer_addr));
    if (peer_addr.proto == AddrProto::tls && ssl) {
        // Gracefully close the TLS connection.
        int result = SSL_shutdown (ssl);
        if (result < 0)
            uxmpp_log_warning (THIS_FILE, "Error sending TLS close_notify - ", get_tls_error_text(result));
    }
    ::close (sock);
    sock = -1;

    // Free XML resources
    //
    uxmpp_log_debug (THIS_FILE, "Deinitialize XML parser");
    XML_ParserFree (parse_data->xml_parser);
    delete parse_data;
    parse_data = nullptr;

    // Free SSL resources
    //
    if (ssl) {
        SSL_free (ssl);
        ssl = nullptr;
    }
    if (ssl_ctx) {
        SSL_CTX_free (ssl_ctx);
        ssl_ctx = nullptr;
    }

    // Free timer resources
    //
    for (auto& t : timers) {
        if (t.second) {
            uxmpp_log_trace (THIS_FILE, "Free timer ", t.second->id);
            delete t.second;
        }
    }
    timers.clear ();

    // Free libevent resources
    //
    if (rx_event) {
        event_free (rx_event);
        rx_event = nullptr;
    }
    if (ebase) {
        event_base_free (ebase);
        ebase = nullptr;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlStream::on_rx (const char* buf, int bytes)
{
    //std::lock_guard<std::mutex> lock (mutex);
    uxmpp_log_trace (THIS_FILE, "RX:\n", string(buf, bytes), "\n");

    // Ignore incoming data on error.
    //
    if (parse_data->error)
        return;

    // Parse the incoming data
    //
    if (!XML_Parse(parse_data->xml_parser, buf, bytes, 0)) {
        parse_data->error = true;
        uxmpp_log_error (THIS_FILE, "RX XML parse error");
        while (parse_data && !parse_data->element_stack.empty()) {
            delete parse_data->element_stack.front ();
            parse_data->element_stack.pop_front ();
        }
    }
}



UXMPP_END_NAMESPACE1
