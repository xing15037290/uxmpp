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
#ifndef UXMPP_IO_FILECONNECTION_HPP
#define UXMPP_IO_FILECONNECTION_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/io/Connection.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


namespace uxmpp { namespace io {

    /**
     * File I/O connection.
     */
    class FileConnection : public Connection {
    public:

        /**
         * Constructor.
         */
        FileConnection (const std::string& file, int flags);

        /**
         * Constructor.
         */
        FileConnection (const std::string& file, int flags, int mode);

        /**
         * Destructor.
         */
        virtual ~FileConnection ();
    };


}}
#endif
