#include "header.h"

static const char pad = '=';

static const char table64[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

static const char alphabet64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(string& outbuf, const char* inbuf, int inlen)
{
    if(NULL != inbuf && inlen > 0)
    {
        int outlen = 0;

        outbuf.resize(inlen + (inlen >> 1), 0);

        while(inlen >= 3)
        {
            outbuf[outlen++] = alphabet64[(inbuf[0]>>2) & 0x3f];

            outbuf[outlen++] = alphabet64[((inbuf[0]<<4) & 0x30) | ((inbuf[1]>>4) & 0x0f)];

            outbuf[outlen++] = alphabet64[((inbuf[1]<<2) & 0x3c) | ((inbuf[2]>>6) & 0x03)];

            outbuf[outlen++] = alphabet64[inbuf[2] & 0x3f];

            inlen -= 3; inbuf += 3;
        }

        if(inlen > 0)
        {
            outbuf[outlen++] = alphabet64[(inbuf[0]>>2) & 0x3f];

            unsigned char c = (inbuf[0]<<4) & 0x30;

            if(2 == inlen)
            {
                c |= (inbuf[1]>>4) & 0x0f;
            }

            outbuf[outlen++] = alphabet64[c];

            outbuf[outlen++] = (1 == inlen) ? pad : alphabet64[(inbuf[1]<<2) & 0x3c];

            outbuf[outlen++] = pad;
        }

        outbuf[outlen] = '\0';

        return outlen;
    }

    return 0;
}

int base64_decode(string& outbuf, const char* inbuf, int inlen)
{
    if(NULL != inbuf && inlen >= 4)
    {
        int outlen = 0;

        outbuf.resize(inlen, 0);

        int offset = (pad == inbuf[inlen - 1]) ? ((pad == inbuf[inlen - 2]) ? 2 : 1) : 0;

        while(inlen > 0)
        {
            outbuf[outlen++] = (table64[(unsigned char)inbuf[0]]<<2) | ((table64[(unsigned char)inbuf[1]]>>4) & 0x3);

            outbuf[outlen++] = ((table64[(unsigned char)inbuf[1]]<<4) & 0xf0) | ((table64[(unsigned char)inbuf[2]]>>2) & 0xf);

            outbuf[outlen++] = ((table64[(unsigned char)inbuf[2]]<<6) & 0xc0) | table64[(unsigned char)inbuf[3]];

            inlen -= 4; inbuf += 4;
        }

        outlen -= offset;

        outbuf[outlen] = '\0';

        return outlen;
    }

    return 0;
}

void base64_test()
{
    int enlen; string enbuf;

    if(0 == (enlen = base64_encode(enbuf, alphabet64, strlen(alphabet64))))
    {
        printf("base64_encode failed\n");
    }

    int delen; string debuf;

    if(0 == (delen = base64_decode(debuf, enbuf.c_str(), enlen)))
    {
        printf("base64_decode failed\n");
    }

    if(alphabet64 == debuf)
    {
        printf("base64_test successful\n");
    }
    else
    {        
        printf("base64_test failed\n");
    }
}
