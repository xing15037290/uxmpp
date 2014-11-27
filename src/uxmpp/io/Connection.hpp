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
#ifndef UXMPP_IO_CONNECTION_HPP
#define UXMPP_IO_CONNECTION_HPP

#include <uxmpp/types.hpp>
#include <functional>


namespace uxmpp { namespace io {


/**
 *
 */
class Connection {
public:

    typedef std::function<void (Connection&, void*, ssize_t, int)> io_callback_t;

    /**
     * Default constructor.
     */
    Connection ();

    /**
     * Destructor.
     */
    virtual ~Connection ();

    /**
     * Queue a read operation.
     */
    virtual void read (void* buf, size_t size, off_t offset, io_callback_t rx_cb=nullptr);

    /**
     * Queue a read operation.
     */
    virtual void read (void* buf, size_t size, io_callback_t rx_cb=nullptr) {
        read (buf, size, -1, rx_cb);
    }

    /**
     * Queue a write operation.
     */
    virtual void write (void* buf, size_t size, off_t offset, io_callback_t tx_cb=nullptr);

    /**
     * Queue a write operation.
     */
    virtual void write (void* buf, size_t size, io_callback_t tx_cb=nullptr) {
        write (buf, size, -1, tx_cb);
    }

    /**
     * Cancel all queued I/O operations.
     */
    virtual void cancel ();

    /**
     * Close the file descriptor.
     */
    virtual void close ();

    /**
     * Return the file descriptor.
     */
    int get_fd () {
        return fd;
    }

    /**
     * Set RX callback.
     */
    void set_rx_cb (io_callback_t callback);

    /**
     * Set TX callback.
     */
    void set_tx_cb (io_callback_t callback);


protected:

    int fd;              /**< File descriptor. */
    io_callback_t rx_cb; /**< RX callback. */
    io_callback_t tx_cb; /**< TX callback. */

private:

    //class ConnectionManager;
};


}}
#endif
