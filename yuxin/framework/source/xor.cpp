#include "header.h"

static const char xor_buf[] = "I LOVE MY FAMILY";

static void swap_str(char* inbuf, int inlen)
{
    int begin = 0;

    int end = inlen - 1;

    while(begin < end)
    {
        inbuf[begin] += inbuf[end];

        inbuf[end] = inbuf[begin] - inbuf[end];

        inbuf[begin] -= inbuf[end];

        --end; ++begin;
    }
}

static void swap_bit(char* inbuf, int inlen)
{
    static const int mask[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

    for(int index = 0; index < inlen; ++index)
    {
        char val = 0; int end = 0; int begin = 8;

        while(begin--)
        {
            val |= (((inbuf[index] & mask[begin]) >> begin) << end);

            ++end;
        }

        inbuf[index] = val;
    }
}

static void xor_str(char* inbuf, int inlen, const char* xorbuf)
{
    if(NULL == xorbuf)
    {
        xorbuf = xor_buf;
    }

    const int xorbuflen = strlen(xorbuf);

    for(int index = 0; index < inlen; ++index)
    {
        inbuf[index] ^= xorbuf[index % xorbuflen];
    }
}

int xor_encode(string& outbuf, const char* inbuf, int inlen, const char* xorbuf)
{    
    int outlen = 0;

    if(NULL != inbuf && inlen > 0)
    {
        char* temp = new char[inlen];
 
        if(NULL != temp)
        {
            memcpy(temp, inbuf, inlen);

            swap_str(temp, inlen);

            swap_bit(temp, inlen);

            xor_str(temp, inlen, xorbuf);

            outlen = base64_encode(outbuf, temp, inlen);
            
            delete[] temp;
        }
    }

    return outlen;
}

int xor_decode(string& outbuf, const char* inbuf, int inlen, const char* xorbuf)
{
    int outlen = base64_decode(outbuf, inbuf, inlen);

    if(NULL != inbuf && inlen > 0 && outlen > 0)
    {
        char* temp = new char[outlen];

        if(NULL != temp)
        {
            memcpy(temp, outbuf.c_str(), outlen);

            xor_str(temp, outlen, xorbuf);

            swap_bit(temp, outlen);

            swap_str(temp, outlen);

            outbuf.append(temp, outlen);

            delete[] temp;
        }
    }

    return outlen;
}

void xor_test()
{
    const char test[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    int enlen; string enbuf;

    if(0 == (enlen = xor_encode(enbuf, test, strlen(test), NULL)))
    {
        printf("xor_encode failed\n");
    }

    int delen; string debuf;

    if(0 == (delen = xor_decode(debuf, enbuf.c_str(), enlen, NULL)))
    {
        printf("xor_decode failed\n");
    }

    if(test == debuf)
    {
        printf("xor_test successful\n");
    }
    else
    {
        printf("xor_test failed\n");
    }
}
