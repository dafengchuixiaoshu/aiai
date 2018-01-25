#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "base64.h"

static char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 
static char base64_decode_map[256] = {

        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

        64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64,

        64, 0, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,

        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, 64, 26, 27, 28,

        29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,

        49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
    };



int base64_encode(const char *src, int src_len, char *dst)
{
    unsigned char* ptr = (unsigned char*)src;

    unsigned char oval = 0 ;
    char *out = dst;
    
    while (src_len >= 3) {
         *out++ = basis_64[ptr[0] >> 2];
         *out++ = basis_64[((ptr[0] << 4) & 0x30) | (ptr[1] >> 4)];
         *out++ = basis_64[((ptr[1] << 2) & 0x3C) | (ptr[2] >> 6)];
         *out++ = basis_64[ptr[2] & 0x3F];
         ptr += 3;
         src_len -= 3;
     }
     
    if (src_len > 0) {
         *out++ = basis_64[ptr[0] >> 2];
         oval = (ptr[0] << 4) & 0x30 ;
         if (src_len > 1) oval |= ptr[1] >> 4;
         *out++ = basis_64[oval];
         *out++ = (src_len < 2) ? '=' : basis_64[(ptr[1] << 2) & 0x3C];
         *out++ = '=';
    }
    *out = '\0';
    return 0;
} 


int base64_decode(const char *src, int src_len, char *dst)
{
 
    unsigned char* ptr = (unsigned char*)dst;

    int i = 0, j = 0;
    for (; i < src_len; i += 4) {

        ptr[j++] = base64_decode_map[src[i]] << 2 |

            base64_decode_map[src[i + 1]] >> 4;

        ptr[j++] = base64_decode_map[src[i + 1]] << 4 |

            base64_decode_map[src[i + 2]] >> 2;

        ptr[j++] = base64_decode_map[src[i + 2]] << 6 |

            base64_decode_map[src[i + 3]];

    }

    if(src_len >= 4)
    {
        if(src[src_len-1] == '=')
            j -=1 ;

        if(src[src_len-2] == '=')
            j -=1 ;
    }
    ptr[j] = '\0';
    return j;
}


int base64_test()
{
    char src[512];
    
    for(int i=0;i<30;i++)
    {
        sprintf(src,"123456789",i);

        char en_dst[512];
        base64_encode(src, strlen(src), en_dst);

        char de_dst[512];

        base64_decode(en_dst,strlen(en_dst),de_dst);
        printf("%s\n",de_dst);
    }
    return 0;
}