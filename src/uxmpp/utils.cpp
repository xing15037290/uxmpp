#include <uxmpp/utils.hpp>
#include <uxmpp/Logger.hpp>

#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <signal.h>
#include <cstring>

#include <thread>
#include <sys/syscall.h>
#include <unistd.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


UXMPP_START_NAMESPACE1(uxmpp)

#define THIS_FILE "utils"

using namespace std;


static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char reverse_table[128] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string base64_encode (const std::string &bindata)
{
    if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u)
        throw length_error ("Input buffer too large");

    const size_t binlen = bindata.size ();
    string retval ((((binlen + 2) / 3) * 4), '='); // Make sure the output is padded with '='
    size_t outpos = 0;
    int bits_collected = 0;
    unsigned int accumulator = 0;
    const string::const_iterator binend = bindata.end ();

    for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
        accumulator = (accumulator << 8) | (*i & 0xffu);
        bits_collected += 8;
        while (bits_collected >= 6) {
            bits_collected -= 6;
            retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }
    if (bits_collected > 0) { // Any trailing bits that are missing.
        assert (bits_collected < 6);
        accumulator <<= 6 - bits_collected;
        retval[outpos++] = b64_table[accumulator & 0x3fu];
    }
    assert (outpos >= (retval.size() - 2));
    assert (outpos <= retval.size());
    return retval;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string base64_decode (const std::string &ascdata)
{
    string retval;
    const string::const_iterator last = ascdata.end ();
    int bits_collected = 0;
    unsigned int accumulator = 0;

    for (string::const_iterator i = ascdata.begin(); i != last; ++i) {
        const int c = *i;
        if (isspace(c) || c == '=') {
            // Skip whitespace and padding. Be liberal in what you accept.
            continue;
        }
        if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
            throw invalid_argument("Not base64 encoded");
        }
        accumulator = (accumulator << 6) | reverse_table[c];
        bits_collected += 6;
        if (bits_collected >= 8) {
            bits_collected -= 8;
            retval += (char)((accumulator >> bits_collected) & 0xffu);
        }
    }
    return retval;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool block_signal (int signal_number)
{
    sigset_t mask;
    sigemptyset (&mask);
    sigaddset (&mask, signal_number);
    if (sigprocmask(SIG_BLOCK, &mask, NULL)) {
        uxmpp_log_error (THIS_FILE, "Unable to block signal ", signal_number, " - ", string(strerror(errno)));
        return false;
    }
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool unblock_signal (int signal_number)
{
    sigset_t mask;
    sigemptyset (&mask);
    sigaddset (&mask, signal_number);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL)) {
        uxmpp_log_error (THIS_FILE, "Unable to unblock signal ", signal_number, " - ", string(strerror(errno)));
        return false;
    }
    return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned long get_thread_id ()
{
    return static_cast<unsigned long> (syscall(SYS_gettid));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned get_num_cores ()
{
#ifdef _WIN32
    SYSTEM_INFO si;
    GetSystemInfo (&si);
    return static_cast<unsigned> (si.dwNumberOfProcessors);
#else
#ifndef _SC_NPROCESSORS_ONLN
    uxmpp_log_warning (THIS_FILE, "Unable to determine number of processor cores");
    return 1;
#else
    auto cores = sysconf (_SC_NPROCESSORS_ONLN);
    if (cores <= 0) {
        uxmpp_log_warning (THIS_FILE, "Unable to get number of processor cores");
        return 1;
    }
    return static_cast<unsigned> (cores);
#endif
#endif
}


UXMPP_END_NAMESPACE1
