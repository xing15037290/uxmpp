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
#include <uxmpp/Logger.hpp>


UXMPP_START_NAMESPACE2(uxmpp, io)

#define THIS_FILE "Connection"


using namespace std;
using namespace uxmpp;


/**
 *
 */
/*
class Connection::ConnectionManager {
public:
    ConnectionManager () = default;
    ~ConnectionManager () = default;

    static ConnectionManager& getInstance () {
        if (!instance)
            instance = new ConnectionManager;
        return *instance;
    }

private:
    static ConnectionManager* instance;
};

Connection::ConnectionManager::instance = nullptr;
*/


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Connection::Connection ()
    :
    fd {-1},
    rx_cb {nullptr},
    tx_cb {nullptr}
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Connection::~Connection ()
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::read (void* buf, size_t size, off_t offset, io_callback_t rx_cb)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::write (void* buf, size_t size, off_t offset, io_callback_t tx_cb)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::cancel ()
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::close ()
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::set_rx_cb (io_callback_t callback)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Connection::set_tx_cb (io_callback_t callback)
{
}



UXMPP_END_NAMESPACE2
