#ifndef UXMPP_IO_ANDROID_RESOLVER_GLUE_H
#define UXMPP_IO_ANDROID_RESOLVER_GLUE_H

//
// Add declarations that are missing in android
//
extern "C" {
    int res_query (const char* dname, int class, int type, unsigned char* answer, int answer_len);
    int dn_expand (unsigned char* msg, unsigned char* eomorig, unsigned char* comp_dn, char* exp_dn, int length);

    struct HEADER {
        unsigned id      : 16;
#if (BYTE_ORDER == BIG_ENDIAN)
        unsigned qr      :  1;
        unsigned opcode  :  4;
        unsigned aa      :  1;
        unsigned tc      :  1;
        unsigned rd      :  1;
        unsigned ra      :  1;
        unsigned unused  :  1;
        unsigned ad      :  1;
        unsigned cd      :  1;
        unsigned rcode   :  4;
#else
        unsigned rd      :  1;
        unsigned tc      :  1;
        unsigned aa      :  1;
        unsigned opcode  :  4;
        unsigned qr      :  1;
        unsigned rcode   :  4;
        unsigned cd      :  1;
        unsigned ad      :  1;
        unsigned unused  :  1;
        unsigned ra      :  1;
#endif
        unsigned qdcount : 16;
        unsigned ancount : 16;
        unsigned nscount : 16;
        unsigned arcount : 16;
    };
    typedef struct HEADER HEADER;
}
#define C_IN      1
#define T_SRV    33
#define QFIXEDSZ  4


#endif
