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

using namespace std;
using namespace uxmpp;
using namespace uxmpp::io;

#define THIS_FILE "main"


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int main (int argc, char* argv[])
{
    uxmpp_set_log_level (LogLevel::silent);
    if (argc < 3) {
        cerr << "Usage: test_FileConnection <in_file> <out_file>" << endl;
        return 1;
    }

    //uxmpp_set_log_level (LogLevel::trace);

    FileConnection in_file (argv[1], O_RDONLY|O_NONBLOCK);
    FileConnection out_file (argv[2], O_WRONLY|O_CREAT|O_NONBLOCK, S_IRWXU|S_IRWXG);

    bool rx_done = false;
    bool tx_done = true;
    array<char*, 4096> rx_buf;

    out_file.set_tx_cb ([&](Connection& c, void* b, ssize_t r, int e){
            //uxmpp_log_info (THIS_FILE, "Wrote ", r, " bytes");
            if (r > 0) {
                //delete[] static_cast<char*> (b);
                in_file.read (rx_buf.data(), rx_buf.size());
            }else{
                tx_done = true;
            }
        });
    
    in_file.set_rx_cb ([&](Connection& c, void* b, ssize_t r, int e){
            //uxmpp_log_info (THIS_FILE, "Read ", r, " bytes");
            if (r > 0) {
                /*
                char* tx_buf = new char[r];
                memcpy (tx_buf, b, r);
                tx_done = false;
                out_file.write (tx_buf, r, -1);
                c.read (rx_buf.data(), rx_buf.size(), -1);
                */
                out_file.write (b, r);
            }else{
                tx_done = false;
                out_file.write (nullptr, 0);
                rx_done = true;
            }
        });

    //uxmpp_log_info (THIS_FILE, "Start read");
    in_file.read_offset (rx_buf.data(), rx_buf.size(), 6);

    while (!rx_done || !tx_done)
        this_thread::sleep_for (chrono::microseconds(10));

    //uxmpp_log_info (THIS_FILE, "done");

    //this_thread::sleep_for (chrono::milliseconds(100));

    return 0;
}
