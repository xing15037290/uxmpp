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
#include <uxmpp/io/FileConnection.hpp>
#include <uxmpp/Logger.hpp>

#include <unistd.h>
#include <cstring>


UXMPP_START_NAMESPACE2(uxmpp, io)

using namespace std;
using namespace uxmpp;

#define THIS_FILE "FileConnection"



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
FileConnection::FileConnection (const std::string& file, int flags)
{
    int fd;
    if (flags | O_CREAT)
        fd = ::open (file.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    else
        fd = ::open (file.c_str(), flags);
    if (fd < 0) {
        uxmpp_log_warning (THIS_FILE, "Unable to open file ", file);
    }else{
        set_fd (fd);

        int result = 0;
        int flags  = fcntl (fd, F_GETFL, 0);
        if (flags != -1)
            result = fcntl (fd, F_SETFL, flags | O_NONBLOCK);
        if (flags==-1 || result==-1) {
            uxmpp_log_warning (THIS_FILE,
                               "Unable to set file handle in non-blocking mode: ",
                               string(strerror(errno)));
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
FileConnection::FileConnection (const std::string& file, int flags, int mode)
{
    int fd = open (file.c_str(), flags, mode);
    if (fd < 0) {
        uxmpp_log_warning (THIS_FILE, "Unable to open file ", file);
    }else{
        set_fd (fd);

        int result = 0;
        int flags  = fcntl (fd, F_GETFL, 0);
        if (flags != -1)
            result = fcntl (fd, F_SETFL, flags | O_NONBLOCK);
        if (flags==-1 || result==-1) {
            uxmpp_log_warning (THIS_FILE,
                               "Unable to set file handle in non-blocking mode: ",
                               string(strerror(errno)));
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
FileConnection::~FileConnection ()
{
}


UXMPP_END_NAMESPACE2
