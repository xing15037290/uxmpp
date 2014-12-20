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
#include <uxmpp/io/Timer.hpp>
#include <uxmpp/Logger.hpp>

#include <thread>
#include <queue>
#include <semaphore.h>
#include <cstring>
#include <map>


UXMPP_START_NAMESPACE2(uxmpp, io)

#define THIS_FILE "Timer"


using namespace std;
using namespace uxmpp;




/**
 *
 */
struct timer_controller_t {
    sem_t sig_sem;
    std::thread worker_thread;
    bool quit_worker_thread;
    std::queue<Timer*> timer_queue;
};


static void signal_handler (int sig, siginfo_t* si, void* data);
static std::mutex timer_controller_mutex;
static map<int, timer_controller_t> signal_timer_map;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Timer::Timer (int signal_number, timer_callback_t cb) throw (TimerException)
    :
    signum   {signal_number},
    callback {cb},
    initial  {0},
    interval {0}
{
    initialize_controller (signal_number);

    struct sigevent se;
    se.sigev_notify = SIGEV_SIGNAL;
    se.sigev_signo  = signal_number;
    se.sigev_value.sival_ptr = this;
    if (timer_create(CLOCK_REALTIME, &se, &id)) {
        int errnum = errno;
        uxmpp_log_error (THIS_FILE, "Unable to create timer using signal ", signal_number);
        throw TimerException (string("Unable to create timer: ") + string(strerror(errnum)));
        callback = nullptr;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Timer::~Timer ()
{
    cancel ();
    timer_delete (id);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::set (unsigned initial, unsigned interval, timer_callback_t callback)
{
    struct itimerspec ts;

    // Trigger immediately ?
    if (initial==0 && interval==0) {
        set (ts, false, true, callback);
        return;
    }

    if (initial == 0) {
        // Make it trigger as soon as we can
        ts.it_value.tv_sec  = 0;
        ts.it_value.tv_nsec = 1;
    }else{
        ts.it_value.tv_sec  = (initial / 1000);
        ts.it_value.tv_nsec = (initial % 1000) * 1000000;
    }
    ts.it_interval.tv_sec  = (interval / 1000);
    ts.it_interval.tv_nsec = (interval % 1000) * 1000000;
    this->initial  = initial;
    this->interval = interval;
    set (ts, false, false, callback);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::uset (unsigned initial, unsigned interval, timer_callback_t callback)
{
    struct itimerspec ts;

    // Trigger immediately ?
    if (initial==0 && interval==0) {
        set (ts, false, true, callback);
        return;
    }

    if (initial == 0) {
        // Make it trigger as soon as we can
        ts.it_value.tv_sec  = 0;
        ts.it_value.tv_nsec = 1;
    }else{
        ts.it_value.tv_sec  = (initial / 1000000);
        ts.it_value.tv_nsec = (initial % 1000000) * 1000;
    }
    ts.it_interval.tv_sec  = (interval / 1000000);
    ts.it_interval.tv_nsec = (interval % 1000000) * 1000;
    this->initial  = initial;
    this->interval = interval;
    set (ts, false, false, callback);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::set (struct itimerspec& ts, bool stop, bool now, timer_callback_t new_callback)
{
    timer_controller_mutex.lock ();
    timer_controller_t& tc = signal_timer_map[signum];
    timer_controller_mutex.unlock ();

    bool need_lock = this_thread::get_id() != tc.worker_thread.get_id();
    if (need_lock)
        set_mutex.lock ();

    // Cancel the timer?
    //
    if (stop) {
        initial = interval = 0;
        timer_settime (id, 0, &ts, NULL);
        if (need_lock)
            set_mutex.unlock ();
        return;
    }

    // Trigger timer now?
    //
    if (now) {
        // Cancel the POSIX timer first
        ts.it_value.tv_sec  = 0;
        ts.it_value.tv_nsec = 0;
        ts.it_interval.tv_sec  = 0;
        ts.it_interval.tv_nsec = 0;
        timer_settime (id, 0, &ts, NULL);
        if (new_callback)
            callback = new_callback;
        // Make sure the timer isn't ignored in the worker thread
        initial = 1;
        // Do a 'manual' trigger
        tc.timer_queue.push (this);
        sem_post (&tc.sig_sem);

        if (need_lock)
            set_mutex.unlock ();
        return;
    }

    if (new_callback)
        callback = new_callback;

    // Arm the timer
    //
    auto result = timer_settime (id, 0, &ts, NULL);
    if (need_lock)
        set_mutex.unlock ();

    if (result) {
        if (initial==0 && interval==0)
            uxmpp_log_error (THIS_FILE, "Unable to cancel timer #", reinterpret_cast<unsigned long>(id));
        else
            uxmpp_log_error (THIS_FILE, "Unable to set timer #", reinterpret_cast<unsigned long>(id));
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::cancel ()
{
    struct itimerspec ts;
    ts.it_value.tv_sec  = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec  = 0;
    ts.it_interval.tv_nsec = 0;
    set (ts, true, false, nullptr);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::set_callback (timer_callback_t new_callback)
{
    timer_controller_mutex.lock ();
    timer_controller_t& tc = signal_timer_map[signum];
    timer_controller_mutex.unlock ();

    bool need_lock = this_thread::get_id() != tc.worker_thread.get_id();
    if (need_lock)
        set_mutex.lock ();

    callback = new_callback;

    if (need_lock)
        set_mutex.unlock ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int Timer::get_overrun ()
{
    int overrun;

    overrun = timer_getoverrun (id);
    if (overrun < 0) {
        uxmpp_log_error (THIS_FILE, "Unable to get timer overrun for timer #",
                         reinterpret_cast<unsigned long>(id));
        return 0;
    }
    return overrun;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//void Timer::signal_handler (int sig, siginfo_t* si, void* data)
void signal_handler (int sig, siginfo_t* si, void* data)
{
    Timer* timer = reinterpret_cast<Timer*> (si->si_value.sival_ptr);
    if (!timer)
        return;

    timer_controller_mutex.lock ();
    if (signal_timer_map.find(sig) == signal_timer_map.end()) {
        timer_controller_mutex.unlock ();
        return;
    }
    timer_controller_t& tc = signal_timer_map[sig];
    timer_controller_mutex.unlock ();

    tc.timer_queue.push (timer);
    sem_post (&tc.sig_sem);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::initialize_controller (int signal_number) throw (TimerException)
{
    lock_guard<mutex> lock (timer_controller_mutex);

    if (signal_timer_map.find(signal_number) != signal_timer_map.end())
        return;

    uxmpp_log_trace (THIS_FILE, "Initialize timer signal handler for signal ", signal_number);

    signal_timer_map[signal_number].quit_worker_thread = false;
    timer_controller_t& tc = signal_timer_map[signal_number];

    // Initialize the signal semaphore
    //
    if (sem_init(&tc.sig_sem, 0, 0)) {
        int errnum = errno;
        uxmpp_log_error (THIS_FILE, "Unable to initialize semaphore");
        signal_timer_map.erase (signal_number);
        throw TimerException (string("Unable to initialize semaphore: ") + string(strerror(errnum)));
    }

    // Block the signal
    //
    sigset_t mask;
    sigemptyset (&mask);
    sigaddset (&mask, signal_number);
    if (sigprocmask(SIG_BLOCK, &mask, NULL)) {
        int errnum = errno;
        uxmpp_log_error (THIS_FILE, "Unable to block signal ", signal_number);
        signal_timer_map.erase (signal_number);
        throw TimerException (string("Unable to block signal: ") + string(strerror(errnum)));
    }

    // Install the signal handler
    //
    struct sigaction sa;
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigemptyset (&sa.sa_mask);
    if (sigaction(signal_number, &sa, NULL)) {
        int errnum = errno;
        uxmpp_log_error (THIS_FILE, "Unable to install signal handler for signal ", signal_number);
        signal_timer_map.erase (signal_number);
        throw TimerException (string("Unable to install signal handler: ") + string(strerror(errnum)));
    }

    // Start the timer worker thread
    //
    tc.worker_thread = thread ([signal_number, &tc](){
            // Unblock signal
            sigset_t mask;
            sigemptyset (&mask);
            sigaddset (&mask, signal_number);
            if (sigprocmask(SIG_UNBLOCK, &mask, NULL)) {
                int errnum = errno;
                uxmpp_log_error (THIS_FILE, "Unable to unblock signal ", signal_number);
                throw TimerException (string("Unable to unblock signal: ") + string(strerror(errnum)));
            }

            while (!tc.quit_worker_thread) {
                if (sem_wait(&tc.sig_sem)) {
                    continue;
                }
                if (tc.quit_worker_thread)
                    break;

                Timer* timer = tc.timer_queue.front ();
                tc.timer_queue.pop ();

                if (timer) {
                    timer->set_mutex.lock ();
                    if (timer->callback && (timer->initial || timer->interval)) {
                        // Block the signal while the timer callback runs
                        if (sigprocmask(SIG_BLOCK, &mask, NULL))
                            uxmpp_log_warning (THIS_FILE, "Unable to block signal ", signal_number);

                        // Call the timer callback
                        timer->callback (*timer);

                        // Unblock the signal after the timer callback finished
                        if (sigprocmask(SIG_UNBLOCK, &mask, NULL))
                            uxmpp_log_warning (THIS_FILE, "Unable to unblock signal ", signal_number);
                    }
                    timer->set_mutex.unlock ();
                }
            }
        });

    // Quit the worker thread(s) at program exit
    //
    if (signal_timer_map.size() <= 1) {
        uxmpp_log_trace (THIS_FILE, "Install 'atexit' function to clean up timer handlers");
        atexit ([](){
                // Signal the worker threads to stop
                for (auto& i : signal_timer_map) {
                    uxmpp_log_trace (THIS_FILE "(atexit)", "Stop timer signal handler for signal ", i.first);
                    i.second.quit_worker_thread = true;
                    sem_post (&i.second.sig_sem);
                }
                // Wait for the worker threads to stop
                for (auto& i : signal_timer_map) {
                    i.second.worker_thread.join ();
                }
            });
    }
}


UXMPP_END_NAMESPACE2
