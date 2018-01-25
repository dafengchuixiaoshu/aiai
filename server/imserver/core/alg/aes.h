#ifndef AES_H
#define AES_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

typedef unsigned char u8;
typedef unsigned int  u32;

class TAesCode
{
public:
    TAesCode(char* key);
    ~TAesCode();

    int  encrypt_cbc(char* data, int length,char* out);
    int  decrypt_cbc(char* data, int length);
    void remove_padding(char* data, int length);

private:
    void*  m_decodeCtx;
    void*  m_encodeCtx;
};

#endif /* AES_H */
