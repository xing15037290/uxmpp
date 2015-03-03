/*
 *  Copyright (C) 2014-2015 Ultramarin Design AB <dan@ultramarin.se>
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

#define WHILE_HELL_BURNS -1
#define FASTER_THAN_LIGHT 0

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

    // Let method 'handle_poll_io' know that a
    // callback have cancelled I/O operations
    ci.cancel_in_callback = true;

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
    cm.fds_size    = 8;
    cm.fds         = new struct pollfd[cm.fds_size];
    cm.fds_rx_more = new bool[cm.fds_size];
    cm.fds_tx_more = new bool[cm.fds_size];
    int nfds;
    bool done = false;

    DEBUG_TRACE (THIS_FILE, "Worker thread started");

    // Setup the command pipe in the poller
    //
    cm.fds[0].fd      = cm.cmd_pipe[pipe_rx];
    cm.fds[0].events  = POLLIN;
    cm.fds[0].revents = 0;
    cm.fds_rx_more[0] = false;
    cm.fds_tx_more[0] = false;
    nfds = 1;

    int poll_timeout = WHILE_HELL_BURNS;
    while (!done) {
        // Poll file descriptors
        //
        DEBUG_TRACE (THIS_FILE, "Poll ", nfds, " file descriptors, timeout: ", poll_timeout);
        auto result = poll (cm.fds, nfds, poll_timeout);
        if (result <= 0) {
            // Check for errors or timeout
            //
            if (result<0 || poll_timeout!=FASTER_THAN_LIGHT) {
                if (result==0 || errno==EINTR)
                    continue;
                uxmpp_log_error (THIS_FILE, "poll failed");
                continue;
            }
        }

        // Check the command pipe first
        //
        if (cm.fds[0].revents & POLLIN)
            done = cm.dispatch_command (nfds);

        // Check file descriptors
        //
        int new_timeout = WHILE_HELL_BURNS;
        for (auto i=1; i<nfds; ++i) {
            bool have_tx = cm.fds_tx_more[i] || ((cm.fds[i].revents & POLLOUT) != 0);
            bool have_rx = cm.fds_rx_more[i] || ((cm.fds[i].revents & POLLIN)  != 0);

            if (have_tx && cm.handle_poll_io(cm.fds[i], nfds, false) == FASTER_THAN_LIGHT) {
                cm.fds_tx_more[i] = true;
                new_timeout = FASTER_THAN_LIGHT;
            }else{
                cm.fds_tx_more[i] = false;
            }

            if (have_rx && cm.handle_poll_io(cm.fds[i], nfds, true) == FASTER_THAN_LIGHT) {
                cm.fds_rx_more[i] = true;
                new_timeout = FASTER_THAN_LIGHT;
            }else{
                cm.fds_rx_more[i] = false;
            }
        }
        poll_timeout = new_timeout;
    }

    delete[] cm.fds;
    delete[] cm.fds_rx_more;
    delete[] cm.fds_tx_more;
    cm.fds = nullptr;
    cm.fds_rx_more = cm.fds_tx_more = nullptr;
    cm.fds_size = 0;

    DEBUG_TRACE (THIS_FILE, "Worker thread ending");
}


//------------------------------------------------------------------------------
// Called from worker thread
//------------------------------------------------------------------------------
int ConnectionManager::handle_poll_io  (struct pollfd& pfd, int& nfds, bool rx)
{
    int retval = WHILE_HELL_BURNS;
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
        return retval;
    }

    ConnectionInfo& ci = connections[conn];
    std::queue<io_operation_t>& queue = rx ? ci.rx_queue : ci.tx_queue;
#ifdef UXMPP_IO_CONNECTIONMANAGER_DEBUG
    if (rx)
        uxmpp_log_trace (THIS_FILE, "RX available on ", pfd.fd, ", RX ops in queue: ", queue.size());
    else
        uxmpp_log_trace (THIS_FILE, "TX ready on ", pfd.fd, ", TX ops in queue: ", queue.size());
#endif
    if (!queue.empty()) {
        io_operation_t& op = queue.front ();
        ssize_t result;
        size_t requested_size = op.size;
        if (rx) {
            result = op.connection->do_read (op.buf, op.size, op.offset, op.errnum);
            DEBUG_TRACE (THIS_FILE, "RX result from ", pfd.fd, ": ", result);
        }else{
            result = op.connection->do_write (op.buf, op.size, op.offset, op.errnum);
            DEBUG_TRACE (THIS_FILE, "TX result from ", pfd.fd, ": ", result);
        }
        if (result==-1 && op.errnum==EAGAIN)
            return retval;

        if (op.size>0 && (size_t)result==requested_size) {
            // We assume there is more data to read/write
            DEBUG_TRACE (THIS_FILE, "There are probably bytes left to ", (rx?"read":"write"));
            retval = FASTER_THAN_LIGHT;
        }
        op.result = result;
        op.errnum = errno;

        if (op.callback) {
            ci.cancel_in_callback = false;
            map_mutex.unlock ();
            DEBUG_TRACE (THIS_FILE, "Call RX/TX callback");
            op.callback (*conn, op.buf, op.result, op.errnum);
            map_mutex.lock ();
        }
        if (!queue.empty() && !ci.cancel_in_callback/*don't pop if queue was cancelled*/) {
            queue.pop ();
            DEBUG_TRACE (THIS_FILE, "Remove RX/TX op, ", queue.size(), " operations left");
        }
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
            uxmpp_log_error (THIS_FILE, "Error sending command add_rx");
        }
    }

    return retval;
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
            if (rx)
                fds_rx_more[i] = false;
            else
                fds_tx_more[i] = false;
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
    fds_rx_more[nfds] = false;
    fds_tx_more[nfds] = false;
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
            if (rx)
                fds_rx_more[i] = false;
            else
                fds_tx_more[i] = false;
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
            fds_rx_more[i] = fds_rx_more[nfds-1];
            fds_tx_more[i] = fds_tx_more[nfds-1];
            --nfds;
            break;
        }
    }
    poll_fd_map.erase (fd);
}


UXMPP_END_NAMESPACE2
