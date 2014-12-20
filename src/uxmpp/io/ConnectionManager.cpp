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
#include <uxmpp/io/ConnectionManager.hpp>
#include <uxmpp/io/Connection.hpp>
#include <uxmpp/io/io_operation.hpp>
#include <uxmpp/Logger.hpp>

#include <cerrno>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>


UXMPP_START_NAMESPACE2(uxmpp, io)

#define THIS_FILE "ConnectionManager"

#ifdef DEBUG_TRACE
#undef DEBUG_TRACE
#endif

#if 0
#define DEBUG_TRACE(prefix, ...) uxmpp_log_trace(prefix, ## __VA_ARGS__)
#define UXMPP_IO_CONNECTIONMANAGER_DEBUG
#else
#define DEBUG_TRACE(prefix, ...)
#undef UXMPP_IO_CONNECTIONMANAGER_DEBUG
#endif


using namespace std;
using namespace uxmpp;


/*
 * Types
 */
enum class io_command_op {
    quit,
    add_rx,
    add_tx,
    del_rx,
    del_tx,
    del_fd,
};
struct io_command_t {
    io_command_op op;
    Connection* conn;
    int fd;
};


/*
 * Static class attributes.
 */
ConnectionManager* ConnectionManager::instance = nullptr;

/*
 * File scope variables.
 */
static std::mutex instance_mutex;
static constexpr int pipe_rx = 0;
static constexpr int pipe_tx = 1;


#ifdef UXMPP_IO_CONNECTIONMANAGER_DEBUG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static string to_string (io_command_op op) {
    switch (op) {
    case io_command_op::quit:
        return "quit";
    case io_command_op::add_rx:
        return "add_rx";
    case io_command_op::add_tx:
        return "add_tx";
    case io_command_op::del_rx:
        return "del_rx";
    case io_command_op::del_tx:
        return "del_tx";
    case io_command_op::del_fd:
        return "del_fd";
    }
    return "n/a";
}
#endif


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ConnectionManager::ConnectionManager () throw (IoException)
{
    // Create the command pipe
    //
    if (pipe(cmd_pipe)) {
        int errnum = errno;
        string error_message = string("Unable to create command pipe: ") + string(strerror(errnum));
        uxmpp_log_error (THIS_FILE, error_message);
        throw IoException (error_message);
    }

    // Set non-blocking mode
    //
    for (int i=0; i<2; ++i) {
        int result = 0;
        int flags  = fcntl (cmd_pipe[i], F_GETFL, 0);
        if (flags != -1)
            result = fcntl (cmd_pipe[i], F_SETFL, flags | O_NONBLOCK);
        if (flags==-1 || result==-1) {
            uxmpp_log_warning (THIS_FILE,
                               "Unable to set command pipe in non-blocking mode: ",
                               string(strerror(errno)));
            break;
        }
    }

    // Start the worker thread
    //
    worker = thread ([this](){
            run_worker (*this);
        });

    DEBUG_TRACE (THIS_FILE, "Install 'atexit' function to clean up connection manager");
    atexit ([](){
            if (ConnectionManager::instance)
                delete ConnectionManager::instance;
        });
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ConnectionManager::~ConnectionManager ()
{
    map_mutex.lock ();
    connections.clear ();
    map_mutex.unlock ();

    io_command_t cmd;
    cmd.op = io_command_op::quit;

    DEBUG_TRACE (THIS_FILE, "End worker thread");
    auto result = ::write (cmd_pipe[pipe_tx], &cmd, sizeof(cmd));
    if (result <= 0) {
        uxmpp_log_debug (THIS_FILE, "Error sending connection manager quit command");
        worker.detach ();
    }else{
        worker.join ();
        DEBUG_TRACE (THIS_FILE, "Worker thread ended");
    }

    close (cmd_pipe[pipe_rx]);
    close (cmd_pipe[pipe_tx]);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ConnectionManager::register_connection (Connection& connection)
{
    lock_guard<mutex> lock (map_mutex);
    connections.emplace (&connection, ConnectionInfo());
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ConnectionManager::unregister_connection (Connection& connection)
{
    lock_guard<mutex> lock (map_mutex);
    connections.erase (&connection);

    // Remove the file descripto from the poll list
    //
    io_command_t cmd;
    cmd.op = io_command_op::del_fd;
    cmd.fd = connection.get_fd ();
    auto result = ::write (cmd_pipe[pipe_tx], &cmd, sizeof(cmd));
    if (result <= 0) {
        uxmpp_log_debug (THIS_FILE, "Error sending command del_fd for fd ", cmd.fd);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ConnectionManager::update_fd (Connection& connection)
{
    //lock_guard<mutex> lock (map_mutex);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ConnectionManager& ConnectionManager::getInstance () throw (IoException)
{
    if (!instance) {
        lock_guard<mutex> lock (instance_mutex);
        if (!instance) {
            uxmpp_log_debug (THIS_FILE, "Creating the ConnectionManager instance");
            instance = new ConnectionManager;
        }
    }
    return *instance;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ConnectionManager::read (Connection& conn,
                              void* buf,
                              size_t size,
                              off_t offset,
                              Connection::io_callback_t rx_cb)
{
    io_operation_t rx_op;
    rx_op.connection = &conn;
    rx_op.buf        = buf;
    rx_op.size       = size;
    rx_op.offset     = offset;
    rx_op.result     = 0;
    rx_op.errnum     = 0;
    rx_op.callback   = rx_cb;

    DEBUG_TRACE (THIS_FILE, "read, fd: ", conn.get_fd(), ", size: ", size);

    lock_guard<mutex> lock (map_mutex);
    if (connections.find(&conn) == connections.end()) {
        uxmpp_log_debug (THIS_FILE, "read - connection not registered");
        return;
    }

    bool add_rx_fd = connections[&conn].rx_queue.empty ();

    connections[&conn].rx_queue.push (rx_op);

    if (add_rx_fd) {
        DEBUG_TRACE (THIS_FILE, "read - add file descriptor ", conn.get_fd(), " to poll list");
        io_command_t cmd;
        cmd.op   = io_command_op::add_rx;
        cmd.conn = &conn;
        auto result = ::write (cmd_pipe[pipe_tx], &cmd, sizeof(cmd));
        if (result <= 0) {
            uxmpp_log_debug (THIS_FILE, "Error sending command add_rx for fd ", conn.get_fd());
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ConnectionManager::write (Connection& conn,
                               void* buf,
                               size_t size,
                               off_t offset,
                               Connection::io_callback_t tx_cb)
{
    io_operation_t tx_op;
    tx_op.connection = &conn;
    tx_op.buf        = buf;
    tx_op.size       = size;
    tx_op.offset     = offset;
    tx_op.result     = 0;
    tx_op.errnum     = 0;
    tx_op.callback   = tx_cb;

    DEBUG_TRACE (THIS_FILE, "write, fd: ", conn.get_fd(), ", size: ", size);

    lock_guard<mutex> lock (map_mutex);
    if (connections.find(&conn) == connections.end()) {
        uxmpp_log_debug (THIS_FILE, "write - connection not registered");
        return;
    }

    bool add_tx_fd = connections[&conn].tx_queue.empty ();

    connections[&conn].tx_queue.push (tx_op);

    if (add_tx_fd) {
        DEBUG_TRACE (THIS_FILE, "write - add file descriptor ", conn.get_fd(), " to poll list");
        io_command_t cmd;
        cmd.op   = io_command_op::add_tx;
        cmd.conn = &conn;
        auto result = ::write (cmd_pipe[pipe_tx], &cmd, sizeof(cmd));
        if (result <= 0) {
            uxmpp_log_debug (THIS_FILE, "Error sending command add_tx for fd ", conn.get_fd());
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ConnectionManager::cancel (Connection& conn)
{
    lock_guard<mutex> lock (map_mutex);
    if (connections.find(&conn) == connections.end())
        return;

    DEBUG_TRACE (THIS_FILE, "Cancel I/O operations for fd " , conn.get_fd());
    ConnectionInfo& ci = connections[&conn];
    while (!ci.rx_queue.empty())
        ci.rx_queue.pop ();
    while (!ci.tx_queue.empty())
        ci.tx_queue.pop ();

    // Remove the file descripto from the poll list
    //
    DEBUG_TRACE (THIS_FILE, "Remove file descriptor " , conn.get_fd(), " from poll list");
    io_command_t cmd;
    cmd.op = io_command_op::del_fd;
    cmd.fd = conn.get_fd ();
    auto result = ::write (cmd_pipe[pipe_tx], &cmd, sizeof(cmd));
    if (result <= 0) {
        uxmpp_log_debug (THIS_FILE, "Error sending command del_fd for fd ", cmd.fd);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ConnectionManager::run_worker (ConnectionManager& cm)
{
    cm.fds_size = 8;
    cm.fds = new struct pollfd[cm.fds_size];
    int nfds;
    bool done = false;

    DEBUG_TRACE (THIS_FILE, "Worker thread started");

    // Setup the command pipe in the poller
    //
    cm.fds[0].fd      = cm.cmd_pipe[pipe_rx];
    cm.fds[0].events  = POLLIN;
    cm.fds[0].revents = 0;
    nfds = 1;

    while (!done) {
        DEBUG_TRACE (THIS_FILE, "Poll ", nfds, " file descriptors");
#ifdef UXMPP_IO_CONNECTIONMANAGER_DEBUG
        for (auto i=1; i<nfds; ++i) {
            DEBUG_TRACE (THIS_FILE, "fd ", cm.fds[i].fd, ":",
                         (cm.fds[i].events & POLLIN ? " RX" : ""),
                         (cm.fds[i].events & POLLOUT ? " TX" : ""));
        }
#endif
        auto result = poll (cm.fds, nfds, -1);
        if (result <= 0) {
            if (result==0 || errno==EINTR)
                continue;
            uxmpp_log_error (THIS_FILE, "poll failed");
            continue;
        }
        // Check the command pipe first
        //
        if (cm.fds[0].revents & POLLIN) {
            cm.fds[0].revents &= ~POLLIN; // reset POLLIN result flag
            done = cm.dispatch_command (nfds);
            continue;
        }
        for (auto i=1; i<nfds; ++i) {
            if (cm.fds[i].revents == 0)
                continue;
            if (cm.fds[i].revents & POLLOUT) {
                DEBUG_TRACE (THIS_FILE, "poll: fd ", cm.fds[i].fd, " ready for write");
                DEBUG_TRACE (THIS_FILE, "fd ", cm.fds[i].fd, ":",
                             (cm.fds[i].events & POLLIN ? " RX" : ""),
                             (cm.fds[i].events & POLLOUT ? " TX" : ""));
                //cm.fds[i].revents &= ~POLLOUT; // reset flag
                cm.handle_poll_io (cm.fds[i], nfds, false);
            }
            if (cm.fds[i].revents & POLLIN) {
                DEBUG_TRACE (THIS_FILE, "poll: fd ", cm.fds[i].fd, " ready for read");
                DEBUG_TRACE (THIS_FILE, "fd ", cm.fds[i].fd, ":",
                             (cm.fds[i].events & POLLIN ? " RX" : ""),
                             (cm.fds[i].events & POLLOUT ? " TX" : ""));
                //cm.fds[i].revents &= ~POLLIN; // reset flag
                cm.handle_poll_io (cm.fds[i], nfds, true);
            }
        }
    }

    delete[] cm.fds;
    cm.fds = nullptr;
    cm.fds_size = 0;

    DEBUG_TRACE (THIS_FILE, "Worker thread ending");
}


//------------------------------------------------------------------------------
// Called from worker thread
//------------------------------------------------------------------------------
void ConnectionManager::handle_poll_io  (struct pollfd& pfd, int& nfds, bool rx)
{
    lock_guard<mutex> lock (map_mutex);
    Connection* conn = poll_fd_map[pfd.fd];

    // Check if the connection is still valid
    //
    if (connections.find(conn) == connections.end()) {
        io_command_t cmd;
        cmd.op = io_command_op::del_fd;
        cmd.fd = pfd.fd;
        pfd.events = 0; // Make sure poll() ignores this
        auto result = ::write (cmd_pipe[pipe_tx], &cmd, sizeof(cmd));
        if (result <= 0) {
            uxmpp_log_debug (THIS_FILE, "Error sending command del_fd for fd ", cmd.fd);
        }
        return;
    }

    ConnectionInfo& ci = connections[conn];
    std::queue<io_operation_t>& queue = rx ? ci.rx_queue : ci.tx_queue;
    if (!queue.empty()) {
        io_operation_t& op = queue.front ();
        if (op.connection->get_fd()==-1 || (op.size && op.buf==nullptr)) {
            // Invalid file descriptor or buffer null pointer
            op.result = -1;
            op.errnum = op.connection->get_fd()==-1 ? EBADF : EINVAL;
        }else if (op.size) {
            ssize_t result;
            if (rx)
                result = op.connection->do_read (op.buf, op.size, op.offset, op.errnum);
            else
                result = op.connection->do_write (op.buf, op.size, op.offset, op.errnum);
            if (result==-1 && op.errnum==EAGAIN)
                return;
            op.result = result;
            op.errnum = errno;
        }else{
            op.result = 0;
            op.errnum = 0;
        }
        if (op.callback) {
            map_mutex.unlock ();
            op.callback (*conn, op.buf, op.result, op.errnum);
            map_mutex.lock ();
        }
        if (!queue.empty())
            queue.pop ();
    }

    if (queue.empty()) {
        if (rx) {
            DEBUG_TRACE (THIS_FILE, "RX queue empty for fd ", conn->get_fd(), " remove it from poll list");
        }else{
            DEBUG_TRACE (THIS_FILE, "TX queue empty for fd ", conn->get_fd(), " remove it from poll list");
        }
        io_command_t cmd;
        cmd.op = rx ? io_command_op::del_rx : io_command_op::del_tx;
        cmd.conn = conn;
        auto result = ::write (cmd_pipe[pipe_tx], &cmd, sizeof(cmd));
        if (result <= 0) {
            uxmpp_log_debug (THIS_FILE, "Error sending command add_rx");
        }
    }
}

//------------------------------------------------------------------------------
// Called from worker thread
//------------------------------------------------------------------------------
bool ConnectionManager::dispatch_command (int& nfds)
{
    io_command_t cmd;
    auto result = ::read (fds[0].fd, &cmd, sizeof(cmd));
    if (result <= 0) {
        if (errno != EAGAIN)
            uxmpp_log_error (THIS_FILE, "Command pipe I/O error");
        return false;
    }
    DEBUG_TRACE (THIS_FILE, "Got command: ", to_string(cmd.op));

    switch (cmd.op) {
    case io_command_op::quit:
        return true;

    case io_command_op::add_rx:
        add_poll_fd (cmd.conn, nfds, true);
        break;

    case io_command_op::add_tx:
        add_poll_fd (cmd.conn, nfds, false);
        break;

    case io_command_op::del_rx:
        del_poll_fd (cmd.conn, nfds, true);
        break;

    case io_command_op::del_tx:
        del_poll_fd (cmd.conn, nfds, false);
        break;

    case io_command_op::del_fd:
        del_poll_fd (cmd.fd, nfds);
        break;
    }

    return false;
}


//------------------------------------------------------------------------------
// Called from worker thread
//------------------------------------------------------------------------------
void ConnectionManager::add_poll_fd (Connection* conn, int& nfds, bool rx)
{
    short poll_op = rx ? POLLIN : POLLOUT;
    lock_guard<mutex> lock (map_mutex);

    DEBUG_TRACE (THIS_FILE, "Add ", rx?"RX":"TX", " file descriptor for fd ", conn->get_fd());

    // Make sure the connection is still valid
    //
    if (connections.find(conn) == connections.end()) {
        DEBUG_TRACE (THIS_FILE, "Connection not valid, don't add ", rx?"RX":"TX", " file descriptor");
        return;
    }

    // Check for invalid file handle
    //
    if (conn->get_fd() == -1) {
        DEBUG_TRACE (THIS_FILE, "File descriptor not valid, call callbacks with error EBADF");
        ConnectionInfo& ci = connections[conn];
        std::queue<io_operation_t>& queue = rx ? ci.rx_queue : ci.tx_queue;
        while (!queue.empty()
               && conn->get_fd() == -1
               && connections.find(conn) != connections.end())
        {
            io_operation_t& op = queue.front ();
            if (op.callback) {
                map_mutex.unlock ();
                op.callback (*conn, op.buf, -1, EBADF);
                map_mutex.lock ();
            }
            queue.pop ();
        }
        if (queue.empty() || connections.find(conn)==connections.end())
            return; // No RX/TX operation left
    }

    // Find the file descriptor in the poll list
    //
    for (int i=1; i<nfds; ++i) {
        if (fds[i].fd == conn->get_fd()) {
            DEBUG_TRACE (THIS_FILE, "Found file descriptor in poll list, set ", (rx?"POLLIN":"POLLOUT"));
            fds[i].events  |=  poll_op;
            fds[i].revents &= ~poll_op;
            return;
        }
    }

    // Add the file descriptor to the poll list
    //
    DEBUG_TRACE (THIS_FILE, "Add file descriptor to poll list, set ", (rx?"POLLIN":"POLLOUT"));
    if (nfds == fds_size) {
        DEBUG_TRACE (THIS_FILE, "Increase file descriptor poll list max size");
        // Resize cmd.fds
        struct pollfd* tmp = fds;
        fds = new struct pollfd[fds_size + 16];
        memcpy (fds, tmp, sizeof(struct pollfd) * fds_size);
        fds_size += 16;
        delete[] tmp;
    }
    fds[nfds].fd      = conn->get_fd ();
    fds[nfds].events  = poll_op;
    fds[nfds].revents = 0;
    poll_fd_map[fds[nfds].fd] = conn;
    ++nfds;
}


//------------------------------------------------------------------------------
// Called from worker thread
//------------------------------------------------------------------------------
void ConnectionManager::del_poll_fd (Connection* conn, int& nfds, bool rx)
{
    short poll_op = rx ? POLLIN : POLLOUT;
    lock_guard<mutex> lock (map_mutex);

    DEBUG_TRACE (THIS_FILE, "Remove ", rx?"RX":"TX", " file descriptor for fd ", conn->get_fd());

    // Make sure the connection is still valid
    //
    if (connections.find(conn) == connections.end()) {
        DEBUG_TRACE (THIS_FILE,
                     "Connection not valid, don't remove ",rx?"RX":"TX"," file descriptor from poll list");
        return;
    }

    // Check for invalid file handle
    //
    if (conn->get_fd() == -1) {
        DEBUG_TRACE (THIS_FILE, "File descriptor not valid, can't remove it from poll list");
        return;
    }

    // Find the file descriptor in the poll list
    //
    for (int i=1; i<nfds; ++i) {
        if (fds[i].fd == conn->get_fd()) {
            DEBUG_TRACE (THIS_FILE, "Found file descriptor in poll list, unset ", (rx?"POLLIN":"POLLOUT"));
            fds[i].events  &= ~poll_op;
            fds[i].revents &= ~poll_op;
/*
            if ((fds[i].events & (POLLIN | POLLOUT)) == 0) {
                DEBUG_TRACE (THIS_FILE, "Remove file descriptor from poll list");
                fds[i].fd      = fds[nfds-1].fd;
                fds[i].events  = fds[nfds-1].events;
                fds[i].revents = fds[nfds-1].revents;
                --nfds;
            }
*/
            return;
        }
    }
}


//------------------------------------------------------------------------------
// Called from worker thread
//------------------------------------------------------------------------------
void ConnectionManager::del_poll_fd (int fd, int& nfds)
{
    DEBUG_TRACE (THIS_FILE, "Remove fd ", fd, " from poll list");

    for (int i=1; i<nfds; ++i) {
        if (fds[i].fd == fd) {
            DEBUG_TRACE (THIS_FILE, "Remove file descriptor from poll list");
            fds[i].fd      = fds[nfds-1].fd;
            fds[i].events  = fds[nfds-1].events;
            fds[i].revents = fds[nfds-1].revents;
            --nfds;
            break;
        }
    }
    poll_fd_map.erase (fd);
}


UXMPP_END_NAMESPACE2
