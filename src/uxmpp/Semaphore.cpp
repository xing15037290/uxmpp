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
#include <uxmpp/types.hpp>
#include <uxmpp/Semaphore.hpp>

UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Semaphore::Semaphore (int initial_value)
{
    std::lock_guard<std::mutex> lock (value_mutex);
    value = initial_value;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Semaphore::post ()
{
    lock_guard<mutex> lock (value_mutex);
    ++value;
    cond.notify_all ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Semaphore::wait ()
{
    unique_lock<mutex> lock (value_mutex);
    cond.wait (lock, [this]{return value;});
    --value;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Semaphore::try_wait ()
{
    unique_lock<mutex> lock (value_mutex);
    if (value) {
        --value;
        return true;
    }else{
        return false;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned Semaphore::get_value ()
{
    lock_guard<mutex> lock (value_mutex);
    return value;
}


UXMPP_END_NAMESPACE1
