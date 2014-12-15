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
#ifndef UXMPP_IO_TCPCONNECTION_HPP
#define UXMPP_IO_TCPCONNECTION_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/io/Connection.hpp>
#include <uxmpp/io/IpHostAddr.hpp>


namespace uxmpp { namespace io {

    /**
     * TCP I/O connection.
     */
    class TcpConnection : public Connection {
    public:

        /**
         * Callback that is called when a TCP connection is made (or failed) or lost.
         * @param connection The connection object that attempted to connect to some host.
         * @param errnum The value of errno after the connection attempt. 0 if successful.
         */
        typedef std::function<void (TcpConnection& connection, int errnum)> connect_cb_t;

        /**
         * Creates a TcpConnection object.
         * The TCP connection is not bound to a local address nor connected to
         * a remote host.
         */
        TcpConnection ();

        /**
         * Creates a TcpConnection object.
         * @param bind_addr The local address/port to bind to before connecting.
         *                  The proto attribute of the address is ignored
         *                  and AddrProto::tcp will be used.
         * @param io_manager The I/O manager handling the I/O.
         */
        TcpConnection (const IpHostAddr& bind_addr);

        /**
         * Destructor.
         */
        virtual ~TcpConnection ();

        /**
         * Connect to a remote host.
         * @param addr The address and port to connect to. The proto attribute
         *             of the address is ignored and AddrProto::tcp will be used.
         */
        void connect (const IpHostAddr& addr);

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


    private:
        bool connected;
        bool bind_to_local_addr;
        bool bind_to_local_port;
/*
    protected:
        std::function<void (TcpConnection&, int)> connected_cb;
        std::function<void (TcpConnection&, int, const std::string&)> tls_connected_cb;


        IpHostAddr local_addr;
        IpHostAddr peer_addr;



        bool open_socket (const IpHostAddr& addr,
                          struct sockaddr_in& saddr4,
                          struct sockaddr_in6& saddr6,
                          struct sockaddr*&   saddr,
                          socklen_t& saddr_len);

        bool bind_socket ();
        void handle_connection_result ();
        void handle_tls_connection_result (Operation& op);
*/
    };


}}
#endif
