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
#include <uxmpp/io/Connection.hpp>
#include <uxmpp/io/ConnectionManager.hpp>
#include <uxmpp/Logger.hpp>

#include <unistd.h>


UXMPP_START_NAMESPACE2(uxmpp, io)

#define THIS_FILE "Connection"


using namespace std;
using namespace uxmpp;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Connection::Connection ()
    :
    rx_cb {nullptr},
    tx_cb {nullptr},
    fd {-1}
{
    ConnectionManager::getInstance().register_connection (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Connection::~Connection ()
{
    close ();
    ConnectionManager::getInstance().unregister_connection (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::read_offset (void* buf, size_t size, off_t offset, io_callback_t rx_cb)
{
    io_callback_t cb = rx_cb==nullptr ? this->rx_cb : rx_cb;
    ConnectionManager::getInstance().read (*this, buf, size, offset, cb);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::write_offset (void* buf, size_t size, off_t offset, io_callback_t tx_cb)
{
    io_callback_t cb = tx_cb==nullptr ? this->tx_cb : tx_cb;
    ConnectionManager::getInstance().write (*this, buf, size, offset, cb);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::cancel ()
{
    ConnectionManager::getInstance().cancel (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::close ()
{
    if (fd != -1) {
        cancel ();
        ::close (fd);
        fd = -1;
        ConnectionManager::getInstance().update_fd (*this);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::set_fd (int fd)
{
    this->fd = fd;
    ConnectionManager::getInstance().update_fd (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Connection::io_callback_t Connection::set_rx_cb (io_callback_t callback)
{
    io_callback_t old_cb {rx_cb};
    rx_cb = callback;
    return old_cb;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Connection::io_callback_t Connection::set_tx_cb (io_callback_t callback)
{
    io_callback_t old_cb {tx_cb};
    tx_cb = callback;
    return tx_cb;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ssize_t Connection::do_read (void* buf, size_t size, off_t offset, int& errnum)
{
    if (offset != -1) {
        auto result = ::lseek (fd, offset, SEEK_SET);
        if (result < 0) {
            errnum = errno;
            return -1;
        }
    }

    ssize_t result = ::read (fd, buf, size);
    errnum = result<0 ? errno : 0;
    return result;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ssize_t Connection::do_write (void* buf, size_t size, off_t offset, int& errnum)
{
    if (offset != -1) {
        auto result = ::lseek (fd, offset, SEEK_SET);
        if (result < 0) {
            errnum = errno;
            return -1;
        }
    }

    ssize_t result = ::write (fd, buf, size);
    errnum = result<0 ? errno : 0;
    return result;
}



UXMPP_END_NAMESPACE2
