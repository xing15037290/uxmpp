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
#ifndef UXMPP_IO_CONNECTIONMANAGER_HPP
#define UXMPP_IO_CONNECTIONMANAGER_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/io/IoException.hpp>
#include <uxmpp/io/Connection.hpp>
#include <uxmpp/io/io_operation.hpp>

#include <queue>
#include <map>
#include <mutex>
#include <thread>

#include <poll.h>


namespace uxmpp { namespace io {


/**
 * Class to handle all connection I/O operations.
 * Singleton.
 */
class ConnectionManager {
public:
    /**
     * Destructor.
     */
    ~ConnectionManager ();

    /**
     * Disabled copy constructor.
     */
    ConnectionManager (const ConnectionManager& cm) = delete;

    /**
     * Disabled assignment operator.
     */
    ConnectionManager& operator= (const ConnectionManager& cm) = delete;

    /**
     * Return a reference to the ConnectionManager instance.
     */
    static ConnectionManager& getInstance () throw (IoException);

    /**
     *
     */
    void register_connection (Connection& connection);

    /**
     *
     */
    void unregister_connection (Connection& connection);

    /**
     *
     */
    void update_fd (Connection& connection);

    /**
     * Queue a read operation for a connection.
     */
    void read (Connection& conn,
               void* buf,
               size_t size,
               off_t offset,
               Connection::io_callback_t rx_cb=nullptr);

    /**
     * Queue a write operation for a connection.
     */
    void write (Connection& conn,
                void* buf,
                size_t size,
                off_t offset,
                Connection::io_callback_t tx_cb=nullptr);

    /**
     * Cancel all operation for a connection.
     */
    void cancel (Connection& conn);


protected:


private:
    class ConnectionInfo {
    public:
        std::queue<io_operation_t> rx_queue;
        std::queue<io_operation_t> tx_queue;
    };

    ConnectionManager () throw (IoException);
    static ConnectionManager* instance;

    std::map<Connection*, ConnectionInfo> connections;
    std::map<int, Connection*> poll_fd_map;
    std::mutex map_mutex;

    std::thread worker;

    static void run_worker (ConnectionManager& cm);
    bool dispatch_command (int& nfds);
    int handle_poll_io  (struct pollfd& pfd, int& nfds, bool rx);
    void add_poll_fd (Connection* conn, int& nfds, bool rx);
    void del_poll_fd (Connection* conn, int& nfds, bool rx);
    void del_poll_fd (int fd, int& nfds);

    int cmd_pipe[2];
    struct pollfd* fds;
    bool*          fds_tx_more;
    bool*          fds_rx_more;
    int            fds_size;
};


}}
#endif
