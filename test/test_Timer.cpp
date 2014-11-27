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
#include <uxmpp/utils.hpp>
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
    //block_signal (SIGRTMIN);
    //block_signal (SIGRTMIN+1);

    try {
        uxmpp_set_log_level (LogLevel::trace);

        Timer t1 ([](Timer& t){
                uxmpp_log_info (THIS_FILE, "t1 expired - overrun: ", t.get_overrun());
            });

        Timer t3 ([](Timer& t){
                uxmpp_log_info (THIS_FILE, "t3 expired - overrun: ", t.get_overrun());
                //t.cancel ();
            });

        Timer t2 (SIGRTMIN+1, [&t3](Timer& t){
                uxmpp_log_info (THIS_FILE, "t2 expired - overrun: ", t.get_overrun());
                this_thread::sleep_for (chrono::milliseconds(400));
                uxmpp_log_info (THIS_FILE, "t2 expired - done");
            });

        t3.set (1000, 40);
        t1.set (1000, 500);
        t2.set (1000, 500);

        this_thread::sleep_for (chrono::seconds(4));
    }
    catch (UxmppException& ue) {
        uxmpp_log_fatal (THIS_FILE, ue.what());
    }
    catch (...) {
        uxmpp_log_fatal (THIS_FILE, "Unknown exception caught");
    }

    return 0;
}
