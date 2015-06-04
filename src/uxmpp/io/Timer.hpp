/*
 *  Copyright (C) 2014,2015 Ultramarin Design AB <dan@ultramarin.se>
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

#include <functional>
#include <chrono>
#include <uxmpp/types.hpp>

namespace uxmpp { namespace io {


/**
 * A timer.
 */
class Timer {
public:

    using hours        = std::chrono::hours;
    using minutes      = std::chrono::minutes;
    using seconds      = std::chrono::seconds;
    using milliseconds = std::chrono::milliseconds;
    using microseconds = std::chrono::microseconds;

    static std::chrono::microseconds now;
    static std::chrono::microseconds zero;

    /**
     * Constructor.
     */
    Timer ();

    /**
     * Destructor.
     */
    ~Timer ();

    /**
     * Set a timeout.
     * The supplied callback will be called after a specified time duration.
     * @param duration The duration after which the callback will be called.
     *                 If this is set to zero, the callback will be called
     *                 as soon as possible.
     * @param callback This function will be called when the timer expires.
     */
    template <typename A>
    void set (A duration, std::function<void()> callback) {
        set_impl (std::chrono::duration_cast<std::chrono::microseconds>(duration),
                  Timer::microseconds(0),
                  callback);
    }

    /**
     * Set a timeout.
     * The supplied callback will be called after a specified time duration.
     * After that it will be called periodically at a specified time interval.
     * @param duration The duration after which the callback will be called.
     * @param period The time period between callbacks. Disabled if zero.
     * @param callback This function will be called when the timer expires.
     */
    template <typename A, typename B>
    void set (A duration, B period, std::function<void()> callback) {
        set_impl (std::chrono::duration_cast<std::chrono::microseconds>(duration),
                  std::chrono::duration_cast<std::chrono::microseconds>(period),
                  callback);
    }

    /**
     * Cancel the timeout.
     */
    void cancel ();

    /**
     * Get the current number of timeout overruns.
     */
    unsigned get_overrun ();


private:
    void set_impl (std::chrono::microseconds duration,
                   std::chrono::microseconds period,
                   std::function<void()> callback);
};


}}
#endif
