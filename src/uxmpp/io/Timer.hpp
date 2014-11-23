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
#ifndef UXMPP_IO_TIMER_HPP
#define UXMPP_IO_TIMER_HPP

#include <uxmpp/types.hpp>
#include <mutex>
#include <functional>
#include <signal.h>
#include <time.h>


namespace uxmpp { namespace io {


/**
 *
 */
class Timer {
public:

    /**
     *
     */
    Timer (std::function<void (Timer*)> cb, int signal_number=SIGRTMIN);

    /**
     *
     */
    virtual ~Timer ();

    /**
     * Set the timer expration time.
     */
    void set (unsigned initial, unsigned interval=0);

    /**
     * Cancel the timer.
     */
    void cancel () {
        set (0, 0);
    }


private:
    timer_t id;
    std::mutex set_mutex;
    std::mutex destructor_mutex;
    std::function<void (Timer* t)> callback;
    static void initialize (int signal_number);
};


}}
#endif
