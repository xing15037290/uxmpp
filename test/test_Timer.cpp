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
#include <uxmpp/Logger.hpp>
#include <uxmpp/io/Timer.hpp>


#include <thread>
#include <iostream>


using namespace std;
using namespace uxmpp;
using namespace uxmpp::io;

#define THIS_FILE "test_Timer"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main (int argc, char* argv[])
{
    uxmpp_set_log_level (LogLevel::trace);

    Timer t1 ([](Timer* t){
            uxmpp_log_info (THIS_FILE, "t expired");
        });

    Timer t3 ([](Timer* t){
            uxmpp_log_info (THIS_FILE, "t3 expired");
        });

    Timer t2 ([&t3](Timer* t){
            uxmpp_log_info (THIS_FILE, "t2 expired");
        });

    t3.set (2000);
    t1.set (500, 50);
    t2.set (5000, 5000);

    this_thread::sleep_for (chrono::seconds(11));

    return 0;
}
