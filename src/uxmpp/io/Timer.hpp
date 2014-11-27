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
#include <uxmpp/io/TimerException.hpp>
#include <mutex>
#include <functional>
#include <signal.h>
#include <time.h>


namespace uxmpp { namespace io {


/**
 * Class used to represent a timer.
 * The timer is implemented using POSIX per-process timers.
 */
class Timer {
public:

    /**
     * Callback that is called when a timer expires.
     * @param timer The timed that has expired.
     */
    typedef std::function<void (Timer& timer)> timer_callback_t;

    /**
     * Constructor.
     * @param signal_number The signal timer that will be used by the process to
     *                      handle the timer. Default is SIGRTMIN.
     *                      One worker thread will be created for each signal to
     *                      manage all threads that use the same signal number.
     * @param cb Callback to be called when the timer expires. The parameter to
     *           the callback is a reference to the timer that expired.
     * @throws TimerException If unsuccessful to install a signal handler or
     *                        create a system timer object.
     */
    Timer (int signal_number=SIGRTMIN, timer_callback_t cb=nullptr) throw (TimerException);

    /**
     * Constructor.
     * @param cb Callback to be called when the timer expires. The parameter to
     *           the callback is a reference to the timer that expired.
     * @throws TimerException If unsuccessful to install a signal handler or
     *                        create a system timer object.
     */
    Timer (timer_callback_t cb) throw (TimerException) : Timer (SIGRTMIN, cb) {}

    /**
     * Destructor.
     * Stops an deinitializes the timer.
     */
    virtual ~Timer ();

    /**
     * Set the timer expration time.
     * @param initial The initial time in milliseconds until the times shall expire.
     * @param interval The inteval in milliseconds that the timer
     *                 shall expire after the initial expiration.
     *                 Set this to zero for a one-shot timer.
     */
    void set (unsigned initial, unsigned interval=0);

    /**
     * Cancel the timer.
     * After this call the callback will not be called until the timer is set again.
     */
    void cancel () {
        set (0, 0);
    }

    /**
     * Set the callback that will be called when the timer expires.
     */
    void set_callback (timer_callback_t new_callback);

    /**
     * Get the current overrun count for the timer.
     * @return The current overrun count for the timer.
     */
    int get_overrun ();


private:
    timer_t id;
    int signum;
    std::mutex set_mutex;
    timer_callback_t callback;
    unsigned initial;
    unsigned interval;
    static void initialize_controller (int signal_number) throw (TimerException);
};


}}
#endif
