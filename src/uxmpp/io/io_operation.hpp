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
#ifndef UXMPP_IO_OPERATION_HPP
#define UXMPP_IO_OPERATION_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/io/Connection.hpp>


namespace uxmpp { namespace io {

/**
 * Input/output operation.
 */
struct io_operation_t {
    Connection*   connection;           /**< The Connection object that is performing the operation. */
    void*         buf;                  /**< Buffer for reading/writing.*/
    size_t        size;                 /**< Number of bytes to read/write. */
    off_t         offset;               /**< Offset from where to read/write. Mainly used for file operations. */
    ssize_t       result;               /**< Result of the operation. If -1 an error ocurred, see errnum. */
    int           errnum;               /**< The value of errno after the operation was performed. */
    Connection::io_callback_t callback; /**< Callback to be called when the operation is done. */
};


}}
#endif
