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
#include <uxmpp.hpp>
#include <iostream>
#include <thread>

using namespace std;
using namespace uxmpp;

#define THIS_FILE "test_Semaphore"


int main (int argc, char* argv[])
{
    Semaphore sem;

    uxmpp_log_info (THIS_FILE, "Wait for semaphore");
    sem.wait (chrono::milliseconds(500));
    //sem.wait ();
    uxmpp_log_info (THIS_FILE, "Done waiting");

    uxmpp_log_info (THIS_FILE, "Wait for semaphore");
    sem.post ();
    sem.post ();
    sem.wait ();
    uxmpp_log_info (THIS_FILE, "Done waiting");
    uxmpp_log_info (THIS_FILE, "Wait for semaphore");
    sem.wait (chrono::milliseconds(500));
    uxmpp_log_info (THIS_FILE, "Done waiting");

    return 0;
}
