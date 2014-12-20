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
#include <cstdio>


namespace uxmpp { namespace io {


// Forward declarations
class ConnectionManager;


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
     * @param buf The buffer where to store the data.
     * @param size The number of bytes to read.
     * @param rx_cb An optional callback to be called when the data is read.
     *              If this is a nullptr the callback that is registered
     *              with set_rx_cb is used.
     */
    virtual void read (void* buf, size_t size, io_callback_t rx_cb=nullptr) {
        read_offset (buf, size, -1, rx_cb);
    }

    /**
     * Queue a read operation from a specific offset.
     * @param buf The buffer where to store the data.
     * @param size The number of bytes to read.
     * @param offset An offset from where to read.
     *               This may not be relevant for all types of connections.
     *               If -1 then the data will be read from the current
     *               position.
     * @param rx_cb An optional callback to be called when the data is read.
     *              If this is a nullptr the callback that is registered
     *              with set_rx_cb is used.
     */
    virtual void read_offset (void* buf, size_t size, off_t offset, io_callback_t rx_cb=nullptr);

    /**
     * Queue a write operation.
     */
    virtual void write (void* buf, size_t size, io_callback_t tx_cb=nullptr) {
        write_offset (buf, size, -1, tx_cb);
    }

    /**
     * Queue a write operation.
     */
    virtual void write_offset (void* buf, size_t size, off_t offset, io_callback_t tx_cb=nullptr);

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
     * Set the file descriptor.
     */
    void set_fd (int fd);

    /**
     * Set RX callback.
     * @return The old callback.
     */
    io_callback_t set_rx_cb (io_callback_t callback);

    /**
     * Set TX callback.
     * @return The old callback.
     */
    io_callback_t set_tx_cb (io_callback_t callback);

    /**
     * Do the actual reading from the file descriptor.
     * This method should not be called directly and should
     * be overridden by classes that needs to process
     * the data. For exmple to implement support for encryption.
     * @param buf A pointer to the memory area where data should be read.
     * @param size The number of bytes to read.
     * @param offset The offset in the file where to read.
     * @param errnum The value of errno after the read operation.
     */
    virtual ssize_t do_read (void* buf, size_t size, off_t offset, int& errnum);

    /**
     * Do the actual writing to the file descriptor.
     * This method should not be called directly and should
     * be overridden by classes that needs to process
     * the data. For exmple to implement support for encryption.
     * @param buf A pointer to the memory area that should be written.
     * @param size The number of bytes to write.
     * @param offset The offset in the file where to write.
     * @param errnum The value of errno after the write operation.
     */
    virtual ssize_t do_write (void* buf, size_t size, off_t offset, int& errnum);


protected:

    io_callback_t rx_cb; /**< RX callback. */
    io_callback_t tx_cb; /**< TX callback. */


private:

    int fd; /**< File descriptor. */
};


}}
#endif
