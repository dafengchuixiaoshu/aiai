/*
 * Copyright (c) 2011, D-Media Communication Technology Inc.
 * All rights reserved.
 *
 *
 * ---
 * Author: Yichi Zhang <zyichi@gmail.com>
 *
 * Simple encrypt/decrypt function. Algorithm description:
 *
 * Encrypt:
 * . Reverse bytes to be encrypted.
 * . Reverse bits for each byte.
 * . XOR magic buffer.
 * . Encrypted buffer to hex format.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static const char* __magic_str = "imsdbp";


unsigned char*
dbp_bin2hex(const unsigned char* src, int len, unsigned char* dst)
{
    static unsigned char hex[] = "0123456789ABCDEF";

    while (len--) {
        *dst++ = hex[*src >> 4];
        *dst++ = hex[*src++ & 0xF];
    }

    return dst;
}


unsigned char
dbp_hex2char(unsigned char l, unsigned char h);


unsigned char*
dbp_hex2bin(const unsigned char* src, int len, unsigned char* dst)
{
    int i;
    const unsigned char *p;
    int n = len / 2;
    for (i = 0; i < n; ++i) {
        p = src + 2*i;

        dst[i] = dbp_hex2char(p[1], p[0]);
    }

    return dst;
}


unsigned char
dbp_hex2char(unsigned char l, unsigned char h)
{
    unsigned int ih = (unsigned int)h;
    unsigned int il = (unsigned int)l;

    if (ih >= 48 && ih <= 57) {
        ih -= 48;
    } else if (ih >= 65 && ih <= 70) {
        ih -= 65;
        ih += 10;
    }

    if (il >= 48 && il <= 57) {
        il -= 48;
    } else if (il >= 65 && il <= 70) {
        il -= 65;
        il += 10;
    }

    return (unsigned char)(ih << 4 | il);
}


unsigned char*
dbp_reverse_str_byte(unsigned char* s, int len)
{
    unsigned char c;
    int i = 0;
    int j = len - 1;

    while (i < j) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
        ++i; --j;
    }

    return s;
}


unsigned char
dbp_reverse_char_bit(unsigned char c)
{
    int i = 8;
    int j = 0;
    unsigned char val = 0;
    static int mask[] = {1, 2, 4, 8, 16, 32, 64, 128};
    while (i--) {
        val |= (((c & mask[i]) >> i) << j);
        j++;
    }

    return val;
}


unsigned char*
dbp_xor_buffer(unsigned char* l_p, int l_len,
               const unsigned char* r_p, int r_len)
{
    int j = 0;
    int i = 0;
    for (i = 0; i < l_len; ++i) {
        l_p[i] ^= r_p[j];
        ++j;
        if (j == r_len) {
            j = 0;
        }
    }
    return l_p;
}


int
dbp_encrypt(const char* key, const unsigned char* in, int in_len,
            unsigned char** out, int* out_len)
{
    (void)key;
    unsigned char* p;
    unsigned char* hex;
    int len = in_len * 2;

    p = (unsigned char*)calloc(1, in_len +1);
    if (p == NULL) {
        return -1;
    }
    hex = (unsigned char*)calloc(1, len +1);
    if (hex == NULL) {
        free(p);
        return -1;
    }
    memcpy(p, in, in_len);
    dbp_reverse_str_byte(p, in_len);

    int i;
    for (i = 0; i < in_len; ++i) {
        p[i] = dbp_reverse_char_bit(p[i]);
    }

    dbp_xor_buffer(p, in_len, (unsigned char*)__magic_str, strlen(__magic_str));

    dbp_bin2hex(p, in_len, hex);
    free(p);

    *out = hex;
    if (out_len) {
        *out_len = len;
    }

    hex[len] =0;
    return 0;
}

int
dbp_decrypt(const char* key, const unsigned char* in, int in_len,
            unsigned char** out, int* out_len)
{
    (void)key;
    unsigned char* p;
    int len = in_len / 2;

    p = (unsigned char*)calloc(1, len +1);
    if (p == NULL) {
        return -1;
    }
    dbp_hex2bin(in, in_len, p);

    dbp_xor_buffer(p, len, (unsigned char*)__magic_str, strlen(__magic_str));

    int i;
    for (i = 0; i < len; ++i) {
        p[i] = dbp_reverse_char_bit(p[i]);
    }

    dbp_reverse_str_byte(p, len);

    *out = p;


    if (out_len) {
        *out_len = len;
    }
    p[len] =0;
    return 0;
}


