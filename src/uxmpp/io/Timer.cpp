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


UXMPP_START_NAMESPACE2(uxmpp, io)

#define THIS_FILE "Timer"


using namespace std;
using namespace uxmpp;


static void signal_handler (int sig, siginfo_t* si, void* data);
static bool initialized = false;
static std::mutex initialize_mutex;
static sem_t sig_sem;
static std::thread worker_thread;
static bool quit_worker_thread = false;
static std::queue<Timer*> timer_queue;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Timer::Timer (function<void (Timer*)> cb, int signal_number)
    : callback (cb)
{
    if (!initialized)
        initialize (signal_number);

    struct sigevent se;
    se.sigev_notify = SIGEV_SIGNAL;
    se.sigev_signo  = signal_number;
    se.sigev_value.sival_ptr = this;
    if (timer_create(CLOCK_REALTIME, &se, &id)) {
        uxmpp_log_error (THIS_FILE, "Unable to create timer using signal ", signal_number);
        callback = nullptr;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Timer::~Timer ()
{
    destructor_mutex.lock ();
    timer_delete (id);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::set (unsigned initial, unsigned interval)
{
    struct itimerspec ts;

    set_mutex.lock ();

    ts.it_value.tv_sec  = (initial / 1000);
    ts.it_value.tv_nsec = (initial % 1000) * 1000000;
    ts.it_interval.tv_sec  = (interval / 1000);
    ts.it_interval.tv_nsec = (interval % 1000) * 1000000;

    auto result = timer_settime (id, 0, &ts, NULL);
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
//void Timer::signal_handler (int sig, siginfo_t* si, void* data)
void signal_handler (int sig, siginfo_t* si, void* data)
{
    Timer* timer = reinterpret_cast<Timer*> (si->si_value.sival_ptr);
    if (!timer)
        return;

    timer_queue.push (timer);
    sem_post (&sig_sem);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Timer::initialize (int signal_number)
{
    std::lock_guard<std::mutex> lock (initialize_mutex);
    if (initialized)
        return;

    // Initialize the signal semaphore
    //
    if (sem_init(&sig_sem, 0, 0)) {
        uxmpp_log_error (THIS_FILE, "Unable to initialize semaphore");
        return;
    }

    // Block the signal
    //
    sigset_t mask;
    sigemptyset (&mask);
    sigaddset (&mask, signal_number);
    if (sigprocmask(SIG_BLOCK, &mask, NULL)) {
        uxmpp_log_error (THIS_FILE, "Unable to block signal ", signal_number);
        return;
    }

    // Install the signal handler
    //
    struct sigaction sa;
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigemptyset (&sa.sa_mask);
    if (sigaction(signal_number, &sa, NULL)) {
        uxmpp_log_error (THIS_FILE, "Unable to install signal handler for signal ", signal_number);
        return;
    }

    // Start the timer worker thread
    //
    worker_thread = thread ([signal_number](){
            // Unblock signal
            sigset_t mask;
            sigemptyset (&mask);
            sigaddset (&mask, signal_number);
            if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
                uxmpp_log_error (THIS_FILE, "Unable to unblock signal ", signal_number);
                return;
            }

            while (!quit_worker_thread) {
                if (sem_wait(&sig_sem)) {
                    continue;
                }
                if (quit_worker_thread)
                    break;

                Timer* timer = timer_queue.front ();
                timer_queue.pop ();

                if (timer && timer->callback) {
                    if (timer->destructor_mutex.try_lock()) {
                        timer->callback (timer);
                        timer->destructor_mutex.unlock ();
                    }
                }
            }
        });

    // Quit the worker thread at program exit
    //
    atexit ([](){
            quit_worker_thread = true;
            sem_post (&sig_sem);
            worker_thread.join ();
        });

    initialized = true;
}


UXMPP_END_NAMESPACE2
