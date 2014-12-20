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
#ifndef UXMPP_SEMAPHORE_HPP
#define UXMPP_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>
#include <chrono>


namespace uxmpp {

    /**
     * A semaphore.
     */
    class Semaphore {
    public:
        /**
         * Constructor.
         * @param initial_value The initial value of the semaphore.
         */
        Semaphore (int initial_value=0);

        /**
         * Increment (unlock) the semaphore.
         */
        void post ();

        /**
         * Decrement (lock) the semaphore.
         */
        void wait ();

        /**
         * Decrement (lock) the semaphore,
         * but don't block longer than <code>duration</code>.
         * @return true if successful, false if unsuccessful.
         */
        template<typename Rep, typename Period>
        bool wait (const std::chrono::duration<Rep, Period>& duration) {
            std::unique_lock<std::mutex> lock (value_mutex);
            if (cond.wait_for(lock, duration, [this]{return value;})) {
                --value;
                return true;
            }else{
                return false;
            }
        }

        /**
         * Try to decrement (lock) the semaphore.
         * @return true if successful, false if unsuccessful.
         */
        bool try_wait ();

        /**
         * Return the current value of the semaphore.
         */
        unsigned get_value ();


    private:
        unsigned value;
        std::mutex value_mutex;
        std::condition_variable cond;
    };

};


#endif
