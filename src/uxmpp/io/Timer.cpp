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
#include <uxmpp/io/Timer.hpp>
#include <uxmpp/Logger.hpp>
#include <condition_variable>
#include <functional>
#include <map>
#include <set>
#include <mutex>
#include <thread>

UXMPP_START_NAMESPACE2(uxmpp, io)


/**
 *
 */
struct timer_entry_t {
    timer_entry_t (Timer* t=nullptr) : timer{t}, callback{nullptr} {} // Constructor

    Timer* timer;
    std::chrono::steady_clock::time_point timeout;
    Timer::microseconds repeat;
    std::function<void()> callback;
};

/**
 * Functional object used for comparing timer_entry_t instances.
 */
struct timer_entry_cmp {
    bool operator () (const timer_entry_t& lhs, const timer_entry_t& rhs) {
        return lhs.timeout < rhs.timeout;
    }
};

/**
 * Functional object used for comparing Timer instances.
 */
struct timer_cmp {
    bool operator () (const Timer& lhs, const Timer& rhs) {
        return &lhs < &rhs;
    }
};


// Timer instances with timer entries
static std::map<std::reference_wrapper<Timer>, timer_entry_t, timer_cmp> timer_map;

// Scheduled timer entries
static std::multiset<std::reference_wrapper<timer_entry_t>, timer_entry_cmp>  timer_set;

static std::mutex resource_lock;
std::condition_variable resource_cond;
static std::thread worker_thread;
static bool worker_done {true};

std::chrono::microseconds Timer::now  {Timer::microseconds (0)};
std::chrono::microseconds Timer::zero {Timer::microseconds (0)};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void worker_function ()
{
    std::unique_lock<std::mutex> lock (resource_lock);

    while (!worker_done) {
        // Look at first entry in timer_set
        auto timeout = std::chrono::steady_clock::now() + Timer::minutes(30);
        auto iter = timer_set.begin ();
        if (iter != timer_set.end())
            timeout = iter->get().timeout;

        // sleep until entry.timeout or set is modified
        if (resource_cond.wait_until(lock, timeout) != std::cv_status::timeout)
            continue; // Set is modified, start again

        auto& entry = iter->get ();
        if (entry.repeat == Timer::zero) {
            timer_set.erase (iter);
        }else{
            entry.timeout = entry.timeout + entry.repeat;
            timer_set.erase (iter);
            timer_set.insert (entry);
        }

        if (entry.callback != nullptr) {
            lock.unlock ();
            entry.callback ();
            lock.lock ();
        }
    }
    resource_lock.unlock ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void start_worker_thread ()
{
    if (worker_done) {
        worker_done = false;
        std::atexit ([](){
                resource_lock.lock ();
                worker_done = true;
                resource_lock.unlock ();
                resource_cond.notify_all ();
                worker_thread.join ();
            });
        worker_thread = std::thread ([](){
                worker_function ();
            });
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Timer::Timer ()
{
    std::lock_guard<std::mutex> lock (resource_lock);
    timer_map.emplace (*this, this);
    start_worker_thread ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Timer::~Timer ()
{
    cancel ();
    std::lock_guard<std::mutex> lock (resource_lock);
    timer_map.erase (*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::set_impl (std::chrono::microseconds duration,
                      std::chrono::microseconds period,
                      std::function<void()> callback)
{
    auto timeout = std::chrono::steady_clock::now() + duration;
    if (!callback) {
        uxmpp_log_warning ("Timer::set", "No callback, do nothing");
        return;
    }

    std::lock_guard<std::mutex> lock (resource_lock);

    auto i = timer_map.find (*this);
    if (i == timer_map.end()) {
        uxmpp_log_error ("Timer::set", "Unknown timer instance!!!");
        return;
    }

    i->second.timeout  = timeout;
    i->second.repeat   = period;
    i->second.callback = callback;

    auto pos = timer_set.find (timer_map[*this]);
    if (pos != timer_set.end())
        timer_set.erase (pos);
    timer_set.insert (i->second);

    resource_cond.notify_all ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::cancel ()
{
    std::lock_guard<std::mutex> lock (resource_lock);
    auto pos = timer_set.find (timer_map[*this]);
    if (pos != timer_set.end()) {
        timer_set.erase (pos);
        resource_cond.notify_all ();
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned Timer::get_overrun ()
{
    return 0;
}


UXMPP_END_NAMESPACE2
