#include <uxmpp/utils.hpp>
#include <uxmpp/Logger.hpp>

#include <string>
#include <signal.h>
#include <cstring>
#include <sstream>
#include <random>

#include <thread>
#include <sys/syscall.h>
#include <unistd.h>


UXMPP_START_NAMESPACE1(uxmpp)

static const std::string log_unit {"utils"};

using namespace std;


static std::array<char, 64> b64_alphabet {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};
static std::array<char, 80> reverse_b64 = {
    62, 65, 65, 65, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 65,
    65, 65, 65, 65, 65, 65,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    65, 65, 65, 65, 65, 65, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_base64 (const unsigned char* buf, size_t len)
{
    // Sanity check
    if (buf == nullptr)
        return "";

    string result (((len+2) / 3) * 4, '=');
    unsigned buf_pos = 0;
    unsigned result_pos = 0;
    while (len >= 3) {
        result[result_pos++] = b64_alphabet[buf[buf_pos]>>2];
        result[result_pos++] = b64_alphabet[(buf[buf_pos]<<4 & 0x3f) | (buf[buf_pos+1]>>4)];
        ++buf_pos;
        result[result_pos++] = b64_alphabet[(buf[buf_pos]<<2 & 0x3f) | (buf[buf_pos+1]>>6)];
        ++buf_pos;
        result[result_pos++] = b64_alphabet[buf[buf_pos++] & 0x3f];
        len -= 3;
    }
    if (len == 2) {
        result[result_pos++] = b64_alphabet[buf[buf_pos]>>2];
        result[result_pos++] = b64_alphabet[(buf[buf_pos]<<4 & 0x3f) | (buf[buf_pos+1]>>4)];
        ++buf_pos;
        result[result_pos++] = b64_alphabet[buf[buf_pos]<<2 & 0x3f];
    }
    else if (len == 1) {
        result[result_pos++] = b64_alphabet[buf[buf_pos]>>2];
        result[result_pos++] = b64_alphabet[buf[buf_pos]<<4 & 0x3f];
    }

    return std::move (result);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_base64 (const string& text)
{
    return to_base64 (reinterpret_cast<const unsigned char*>(text.c_str()), text.length());
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
size_t from_base64 (const string& encoded_string, char* buf, size_t buf_len)
{
    size_t result_len = 0;
    char data = 0;
    int index = 0;
    char code;
    for (auto c : encoded_string) {
        code = 65;
        if (isspace(c)) {
            // Ignore whitespace
            continue;
        }else if (c>='+' && c<='z') {
            code = reverse_b64[c-'+'];
        }
        if (code == 65) {
            // Stop at invalid base64 character
            break;
        }

        switch (index) {
        case 0:
            data = code << 2;
            ++index;
            break;
        case 1:
            data |= code >> 4;
            buf[result_len++] = data;
            data = (code & 0x0f) << 4;
            ++index;
            break;
        case 2:
            data |= code >> 2;
            buf[result_len++] = data;
            data = (code & 0x03) << 6;
            ++index;
            break;
        case 3:
            data |= code;
            buf[result_len++] = data;
            index = 0;
            break;
        }
    }
    if (index != 0)
        buf[result_len++] = data;

    return result_len;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string from_base64 (const string& encoded_string)
{
    size_t expected_len = ((encoded_string.length()+3) / 4) * 3;
    string result (expected_len, '\0');
    auto actual_len = from_base64 (encoded_string,
                                   const_cast<char*>(result.c_str()),
                                   expected_len);
    result.resize (actual_len);
    return result;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string make_uuid_v4 ()
{
    static std::random_device rd;
    static std::default_random_engine re (rd());
    static std::uniform_int_distribution<int> random_digit (0, 15);  // Random between 0 .. 15

    std::stringstream ss;
    ss << std::hex;

    // ........-....-4...-a...-............
    // ^^^^^^^^
    for (auto i=0; i<8; i++)
        ss << random_digit (re);
    ss << '-';

    // ........-....-4...-a...-............
    //          ^^^^
    for (auto i=0; i<4; i++)
        ss << random_digit (re);
    ss << '-';

    // ........-....-4...-a...-............
    //                ^^^
    ss << '4';
    for (auto i=0; i<3; i++)
        ss << random_digit (re);
    ss << '-';

    // ........-....-4...-a...-............
    //                     ^^^
    ss << 'a';
    for (auto i=0; i<3; i++)
        ss << random_digit (re);
    ss << '-';

    // ........-....-4...-a...-............
    //                         ^^^^^^^^^^^^
    for (auto i=0; i<12; i++)
        ss << random_digit (re);

    return std::move (ss.str());
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool block_signal (int signal_number)
{
    sigset_t mask;
    sigemptyset (&mask);
    sigaddset (&mask, signal_number);
    if (sigprocmask(SIG_BLOCK, &mask, NULL)) {
        uxmpp_log_error (log_unit, "Unable to block signal ", signal_number, " - ", string(strerror(errno)));
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
        uxmpp_log_error (log_unit, "Unable to unblock signal ", signal_number, " - ", string(strerror(errno)));
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
    return std::thread::hardware_concurrency ();
}


UXMPP_END_NAMESPACE1
