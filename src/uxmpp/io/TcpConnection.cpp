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
#include <uxmpp/io/TcpConnection.hpp>
#include <uxmpp/Logger.hpp>
/*
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>
#include <cstring>
*/

/*
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>
*/

UXMPP_START_NAMESPACE2(uxmpp, io)

static constexpr string this_file {"TcpConnection"}

using namespace std;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TcpConnection::TcpConnection ()
    :
    connected (false),
    bind_to_local_addr (false),
    bind_to_local_port (false),
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TcpConnection::TcpConnection (const IpHostAddr& bind_addr)
    :
      local_addr (bind_addr),
      connected (false)
{
    bind_to_local_port = local_addr.port != 0;

    switch (local_addr.type) {
    case AddrType::ipv4:
        bind_to_local_addr = local_addr.ipv4 != 0;
        break;

    case AddrType::ipv6:
        bind_to_local_addr = false;
        for (unsigned i=0; i<local_addr.ipv6.size(); ++i) {
            if (local_addr.ipv6.data()[i] != 0) {
                bind_to_local_addr = true;
                break;
            }
        }
        break;

    case AddrType::any:
        bind_to_local_addr = false;
        break;

    default:
        bind_to_local_addr = false;
        bind_to_local_port = false;
        break;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TcpConnection::~TcpConnection ()
{
    disconnect ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TcpConnection::disconnect ()
{
    close ();
    connected = false;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TcpConnection::open_socket (const IpHostAddr&    addr,
                                 struct sockaddr_in&  saddr4,
                                 struct sockaddr_in6& saddr6,
                                 struct sockaddr*&    saddr,
                                 socklen_t&           saddr_len)
{
    peer_addr = addr;

    // Set socket type
    //
    int sock_type = SOCK_STREAM;
    if (peer_addr.proto==AddrProto::udp || peer_addr.proto==AddrProto::dtls)
        sock_type = SOCK_DGRAM;

    int errnum = 0;

    // Open the socket
    //
    if (peer_addr.type == AddrType::ipv4) {
        saddr4.sin_family      = AF_INET;
        saddr4.sin_addr.s_addr = peer_addr.ipv4;
        saddr4.sin_port        = peer_addr.port;
        saddr = reinterpret_cast<struct sockaddr*> (&saddr4);
        saddr_len = sizeof (saddr4);
        uxmpp_log_trace (THIS_FILE, "Create IPv4 socket");
        fd = socket (AF_INET, sock_type, 0);
    }
    else if (peer_addr.type == AddrType::ipv6) {
        saddr6.sin6_family = AF_INET6;
        memcpy (&saddr6.sin6_addr, &peer_addr.ipv6, sizeof(saddr6.sin6_addr));
        saddr6.sin6_port   = peer_addr.port;
        saddr = reinterpret_cast<struct sockaddr*> (&saddr6);
        saddr_len = sizeof (saddr6);
        uxmpp_log_trace (THIS_FILE, "Create IPv6 socket");
        fd = socket (AF_INET6, sock_type, 0);
    }
    else {
        uxmpp_log_warning (THIS_FILE, "Unable to create socket - Wrong address type");
        msg_timer.set_timer ("error", 0, 0, [this](Connection& timer, const std::string& name){
                // Notify the connection result.
                if (connected_cb)
                    connected_cb (*this, EINVAL);
            });
        return false;
    }
    errnum = errno;
    iom.update_connection (*this);

    // Check for errors
    //
    if (fd == -1) {
        uxmpp_log_error (THIS_FILE, "Error creating socket: ", string(strerror(errnum)));
        msg_timer.set_timer ("error", 0, 0, [errnum, this](Connection& timer, const std::string& name){
                // Notify the connection result.
                if (connected_cb)
                    connected_cb (*this, errnum);
            });
        return false;
    }

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TcpConnection::bind_socket ()
{
    if (!bind_to_local_addr && !bind_to_local_port)
        return true;

    uxmpp_log_trace (THIS_FILE, "Bind socket to ", to_string(local_addr));

    if (local_addr.type == AddrType::ipv6) {
        int on = 1;
        auto result = setsockopt (fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
        if (result)
            uxmpp_log_warning (THIS_FILE, "Failed setting socket to V6ONLY: ", string(strerror(errno)));
    }

    struct sockaddr_in  saddr4;
    struct sockaddr_in6 saddr6;
    struct sockaddr*    saddr;
    socklen_t saddr_len;
    if (local_addr.type == AddrType::ipv4) {
        saddr4.sin_family = AF_INET;
        if (bind_to_local_addr)
            saddr4.sin_addr.s_addr = local_addr.ipv4;
        else
            saddr4.sin_addr.s_addr = INADDR_ANY;
        saddr4.sin_port = bind_to_local_port ? local_addr.port : 0;
        saddr = reinterpret_cast<struct sockaddr*> (&saddr4);
        saddr_len = sizeof (saddr4);
    }
    else if (local_addr.type == AddrType::ipv6) {
        saddr6.sin6_family = AF_INET6;
        if (bind_to_local_addr)
            memcpy (&saddr6.sin6_addr, &local_addr.ipv6, sizeof(saddr6.sin6_addr));
        else
            saddr6.sin6_addr = IN6ADDR_ANY_INIT;
        saddr6.sin6_port = bind_to_local_port ? local_addr.port : 0;
        saddr = reinterpret_cast<struct sockaddr*> (&saddr6);
        saddr_len = sizeof (saddr6);
    }else{
        // Any protocol implicated any address.
        // Use the protocol of the peer address.
        if (!bind_to_local_port)
            return true;
        if (peer_addr.type == AddrType::ipv4) {
            saddr4.sin_family = AF_INET;
            saddr4.sin_addr.s_addr = INADDR_ANY;
            saddr4.sin_port = local_addr.port;
            saddr = reinterpret_cast<struct sockaddr*> (&saddr4);
            saddr_len = sizeof (saddr4);
        }else if (peer_addr.type == AddrType::ipv6) {
            saddr6.sin6_family = AF_INET6;
            saddr6.sin6_addr = IN6ADDR_ANY_INIT;
            saddr6.sin6_port = local_addr.port;
            saddr = reinterpret_cast<struct sockaddr*> (&saddr6);
            saddr_len = sizeof (saddr6);
        }else{
            return true;
        }
    }

    auto result = bind (fd, saddr, saddr_len);
    if (result) {
        int errnum = errno;
        uxmpp_log_warning (THIS_FILE, "Error binding to ",
                           to_string(local_addr),
                           " - ",
                           string(strerror(errno)));
        close ();
        msg_timer.set_timer ("error", 0, 0, [errnum, this](Connection& timer, const std::string& name){
                // Notify the connection result.
                if (connected_cb)
                    connected_cb (*this, errnum);
            });
        return false;
    }

    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TcpConnection::connect (const IpHostAddr& addr, const TlsConfig& tls_cfg)
{
    if (fd != -1) {
        uxmpp_log_debug (THIS_FILE, "Already connected to ", to_string(peer_addr));
        return;
    }

    peer_addr = addr;
    this->tls_cfg = tls_cfg;
    connected = false;

    int errnum = 0;
    struct sockaddr_in  saddr4;
    struct sockaddr_in6 saddr6;
    struct sockaddr*    saddr;
    socklen_t saddr_len;

    // Open the socket
    //
    if (!open_socket(addr, saddr4, saddr6, saddr, saddr_len))
        return;

    // Set non-blocking mode
    //
    uxmpp_log_trace (THIS_FILE, "Set socket to non-blocking");
    int flags = fcntl (fd, F_GETFL, 0);
    if (flags != -1) {
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            uxmpp_log_warning (THIS_FILE,
                               "Unable to set the socket in non-blocking mode - ",
                               string(strerror(errnum)));
    }else{
        uxmpp_log_warning (THIS_FILE, "Unable to set the socket in non-blocking mode - ", string(strerror(errnum)));
    }

    // Bind the socket
    //
    if (!bind_socket()) {
        return;
    }

    // Connect the socket
    //
    uxmpp_log_info (THIS_FILE, "Connect to ", to_string(peer_addr));
    int result = ::connect (fd, saddr, saddr_len);
    if (result == -1 && errno != EINPROGRESS) {
        errnum = errno;
        uxmpp_log_error (THIS_FILE, "Error connecting to ",
                         to_string(peer_addr),
                         " - ",
                         string(strerror(errnum)));
        msg_timer.set_timer ("error", 0, 0, [errnum, this](Connection& timer, const std::string& name){
                close ();
                // Notify the connection result.
                if (connected_cb)
                    connected_cb (*this, errnum);
            });
        return;
    }else{
        // Queue a write operation with zero bytes to write.
        // When the socket is writeable the connection status
        // can be checked.
        Operation op (this, OpType::write, nullptr, 0);
        op.cb = [this](Operation& op) {
            handle_connection_result ();
        };
        iom.queue_op (op);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TcpConnection::handle_connection_result ()
{
    // Check for connection error
    //
    int err = 0;
    socklen_t err_size = sizeof (err);
    int result = getsockopt (fd, SOL_SOCKET, SO_ERROR, &err, &err_size);
    if (result || err) {
        connected = false;
        close ();
        uxmpp_log_info (THIS_FILE, "Error connecting to ",
                        to_string(peer_addr),
                        " - ",
                        string(strerror(err)));
    }else{
        // Get the local address
        local_addr.type = peer_addr.type;
        if (peer_addr.proto==AddrProto::udp || peer_addr.proto==AddrProto::dtls)
            local_addr.proto = AddrProto::udp;
        else
            local_addr.proto = AddrProto::tcp;
        if (peer_addr.type == AddrType::ipv4) {
            struct sockaddr_in saddr;
            socklen_t saddr_len = sizeof (saddr);
            if (!getsockname(fd, reinterpret_cast<struct sockaddr*>(&saddr), &saddr_len)) {
                local_addr.ipv4 = saddr.sin_addr.s_addr;
                local_addr.port = saddr.sin_port;
            }
        }
        else if (peer_addr.type == AddrType::ipv6) {
            struct sockaddr_in6 saddr;
            socklen_t saddr_len = sizeof (saddr);
            if (!getsockname(fd, reinterpret_cast<struct sockaddr*>(&saddr), &saddr_len)) {
                memcpy (&local_addr.ipv6, &saddr.sin6_addr, sizeof(saddr.sin6_addr));
                local_addr.port = saddr.sin6_port;
            }
        }
        connected = true;
        uxmpp_log_info (THIS_FILE, "Connected from ",
                        to_string(local_addr),
                        " to ",
                        to_string(peer_addr));
    }
    // Notify the connection result.
    if (connected_cb)
        connected_cb (*this, result==0 ? err : errno);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TcpConnection::enable_tls (const TlsConfig& tls_cfg)
{
    if (!connected) {
        uxmpp_log_warning (THIS_FILE, "Can't enable TLS, not connected");
        msg_timer.set_timer ("error", 0, 0, [this](Connection& timer, const std::string& name){
                // Notify the TLS connection result.
                if (tls_connected_cb)
                    tls_connected_cb (*this, SSL_ERROR_SSL, "Socket not connected");
            });
        return;
    }
    if (tls_enabled) {
        uxmpp_log_debug (THIS_FILE, "TLS already enabled");
        return;
    }

    uxmpp_log_info (THIS_FILE, "Start TLS handshake");
    this->tls_cfg = tls_cfg;

    // Initialize the SSL library
    //
    static bool ssl_initialized = false;
    if (!ssl_initialized) {
        uxmpp_log_trace (THIS_FILE, "Initialize OpenSSL");
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
    uxmpp_log_trace (THIS_FILE, "Create SSL context, method: ", to_string(tls_cfg.method));
    ssl_ctx = SSL_CTX_new (method);
    if (!ssl_ctx) {
        uxmpp_log_error (THIS_FILE, "Unable to create SSL context");
        msg_timer.set_timer ("error", 0, 0, [this](Connection& timer, const std::string& name){
                // Notify the TLS connection result.
                if (tls_connected_cb)
                    tls_connected_cb (*this, SSL_ERROR_SSL, "Unable to create SSL context");
            });
        return;
    }

    // Set server verification
    //
    SSL_CTX_set_verify (ssl_ctx, tls_cfg.verify_server ? SSL_VERIFY_PEER : SSL_VERIFY_NONE, nullptr);

    // Create the SSL stream object
    //
    uxmpp_log_trace (THIS_FILE, "Create SSL object");
    ssl = SSL_new (ssl_ctx);
    if (!ssl) {
        uxmpp_log_error (THIS_FILE, "Unable to create SSL object");
        SSL_CTX_free (ssl_ctx);
        ssl_ctx = nullptr;
        msg_timer.set_timer ("error", 0, 0, [this](Connection& timer, const std::string& name){
                // Notify the connection result.
                if (tls_connected_cb)
                    tls_connected_cb (*this, SSL_ERROR_SSL, "Unable to create SSL object");
            });
        return;
    }

    // Connect the SSL stream object with the socket
    //
    int result = SSL_set_fd (ssl, fd);
    if (!result) {
        string tls_error = get_tls_error_text (result);
        uxmpp_log_error (THIS_FILE, "Unable to set SSL file descriptor - ", tls_error);
        if (ssl) {
            SSL_free (ssl);
            ssl = nullptr;
        }
        if (ssl_ctx) {
            SSL_CTX_free (ssl_ctx);
            ssl_ctx = nullptr;
        }
        msg_timer.set_timer ("error", 0, 0, [this, tls_error](Connection& timer, const std::string& name){
                // Notify the connection result.
                if (tls_connected_cb)
                    tls_connected_cb (*this, SSL_ERROR_SSL, tls_error);
            });
        return;
    }

    //
    // Perform TLS handshake
    //
    uxmpp_log_trace (THIS_FILE, "Start TLS handshake");
    Operation iop (this, OpType::write, nullptr, 0);
    iop.cb = [this](Operation& op) {
        handle_tls_connection_result (op);
    };
    iom.queue_op (iop);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TcpConnection::handle_tls_connection_result (Operation& op)
{
    int result = SSL_connect (ssl); // Perform (partial) connection
    int err = SSL_get_error (ssl, result);

    if (result == 1) {
        uxmpp_log_info (THIS_FILE, "TLS handshake successful");
        if (tls_connected_cb) {
            tls_enabled = true;
            tls_connected_cb (*this, 0, ""); // Yay, success :)
        }
    }
    else if (result==-1 && (err==SSL_ERROR_WANT_READ || err==SSL_ERROR_WANT_WRITE)) {
        uxmpp_log_trace (THIS_FILE, "TLS handshake in progress");
        op.type = err == SSL_ERROR_WANT_READ ? OpType::read : OpType::write;
        iom.queue_op (op); // Wait unitl readable or writeable
    }else{
        string err_txt = get_tls_error_text (result);
        uxmpp_log_warning (THIS_FILE, "TLS handshake failed - ", err_txt);
        if (ssl) {
            SSL_free (ssl);
            ssl = nullptr;
        }
        if (ssl_ctx) {
            SSL_CTX_free (ssl_ctx);
            ssl_ctx = nullptr;
        }
        if (tls_connected_cb)
            tls_connected_cb (*this, err, err_txt);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ssize_t TcpConnection::do_read (void* buf, size_t size, off_t offset, int& errnum)
{
    if (tls_enabled) {
        int result = SSL_read (ssl, buf, size);
        errnum = SSL_get_error (ssl, result);
        if (errnum == SSL_ERROR_WANT_READ)
            errnum = EAGAIN;
        return result<0 ? -1 : result;
    }else{
        return Connection::do_read (buf, size, -1, errnum);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ssize_t TcpConnection::do_write (void* buf, size_t size, off_t offset, int& errnum)
{
    if (tls_enabled) {
        int result = SSL_write (ssl, buf, size);
        errnum = SSL_get_error (ssl, result);
        if (errnum == SSL_ERROR_WANT_WRITE)
            errnum = EAGAIN;
        return result<0 ? -1 : result;
    }else{
        return Connection::do_write (buf, size, -1, errnum);
    }
}


UXMPP_END_NAMESPACE2
