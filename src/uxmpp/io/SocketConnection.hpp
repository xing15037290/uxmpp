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
#ifndef UXMPP_IO_SOCKETCONNECTION_HPP
#define UXMPP_IO_SOCKETCONNECTION_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/io/Connection.hpp>
#include <uxmpp/io/TlsConfig.hpp>
#include <uxmpp/io/IpHostAddr.hpp>
#include <uxmpp/io/Timer.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>


namespace uxmpp { namespace io {

    /**
     * Socket I/O connection.
     */
    class SocketConnection : public Connection {
    public:

        /**
         * Callback that is called when a socket connection is made (or failed).
         * @param connection The connection object that attempted to connect to some host.
         * @param errnum The value of errno after the connection attempt. 0 if successful.
         */
        typedef std::function<void (SocketConnection& connection, int errnum)> connect_cb_t;

        /**
         * Callback that is called when a TLS/DTLS handshake is finished (or failed).
         * @param connection The connection object that attempted to establish a secure connection.
         * @param errnum The value of errno after the connection attempt. 0 if successful.
         * @param errstr An error string describing the error. This could be an empty string.
         */
        typedef std::function<void (SocketConnection& connection,
                                    int errnum, const std::string& errstr)> tls_connected_cb_t;

        /**
         * Creates a SocketConnection object.
         * The TCP connection is not bound to a local address nor connected to
         * a remote host.
         */
        SocketConnection ();

        /**
         * Creates a SocketConnection object.
         * @param bind_addr The local address/port to bind to before connecting.
         */
        SocketConnection (const IpHostAddr& bind_addr);

        /**
         * Destructor.
         */
        virtual ~SocketConnection ();

        /**
         * Connect to a remote host.
         * @param peer_addr The address of the remote host. The protocol used
         *                  is determined by the type and proto attributes of
         *                  the address object.
         *                  If the socket is bound to a local address the proto
         *                  attribute of the local address will be set to the
         *                  same as the peer address.
         */
        void connect (const IpHostAddr& peer_addr);

        /**
         * Connect to a remote host.
         * @param peer_addr The address of the remote host. The protocol used
         *                  is determined by the type and proto attributes of
         *                  the address object.
         *                  If the socket is bound to a local address the proto
         *                  attribute of the local address will be set to the
         *                  same as the peer address.
         * @param tls_config TLS configuration used if the prot attribute of
         *                   the peer_addr object is AddrProto::tls or AddrProto::dtls.
         */
        void connect (const IpHostAddr& peer_addr, const TlsConfig& tls_config);

        /**
         * Disconnect the socket.
         */
        void disconnect ();

        /**
         * Set connect callback.
         */
        void set_connected_cb (connect_cb_t connected_cb) {
            this->connected_cb = connected_cb;
        }

        /**
         * Set tls connect callback.
         */
        void set_tls_connected_cb (tls_connected_cb_t tls_connected_cb) {
            this->tls_connected_cb = tls_connected_cb;
        }

        /**
         * Get the address of the peer.
         */
        IpHostAddr get_peer_addr () const {
            return peer_addr;
        }

        /**
         * Get the local address of the socket.
         */
        IpHostAddr get_local_addr () const {
            return local_addr;
        }

        /**
         * Perform a TLS/DTLS handshake if not already done.
         */
        void enable_tls (const TlsConfig& tls_cfg);

        /**
         * Do the actual reading from the file descriptor.
         * This method should not be called directly and should
         * be overridden by classes that needs to process
         * the data. For exmple to implement support for encryption.
         * @param buf A pointer to the memory area where data should be read.
         * @param size The number of bytes to read.
         * @param offset Not relevant for a socket connection.
         * @param errnum The value of errno after the read operation,
                         or a SSL specific error if TLS is enabled.
         */
        virtual ssize_t do_read (void* buf, size_t size, off_t offset, int& errnum);

        /**
         * Do the actual writing to the file descriptor.
         * This method should not be called directly and should
         * be overridden by classes that needs to process
         * the data. For exmple to implement support for encryption.
         * @param buf A pointer to the memory area that should be written.
         * @param size The number of bytes to write.
         * @param offset Not relevant for a socket connection.
         * @param errnum The value of errno after the write operation,
                         or a SSL specific error if TLS is enabled.
         */
        virtual ssize_t do_write (void* buf, size_t size, off_t offset, int& errnum);


    protected:
        std::function<void (SocketConnection&, int)> connected_cb;
        std::function<void (SocketConnection&, int, const std::string&)> tls_connected_cb;


    private:
        Timer msg_timer;
        IpHostAddr local_addr;
        IpHostAddr peer_addr;
        TlsConfig tls_cfg;
        bool connected;
        bool tls_enabled;
        bool bind_to_local_addr;
        bool bind_to_local_port;

        /**
         * SSL context.
         */
        SSL_CTX* ssl_ctx;

        /**
         * SSL connection object.
         */
        SSL* ssl;

        bool open_socket (const IpHostAddr& addr,
                          struct sockaddr_in& saddr4,
                          struct sockaddr_in6& saddr6,
                          struct sockaddr*&   saddr,
                          socklen_t& saddr_len);

        bool bind_socket ();
        void handle_connection_result ();
        void handle_tls_connection_result (Connection& c, void* p, ssize_t r, int e);
    };


}}
#endif
