#ifndef _BASE64_H
#define _BASE64_H

int base64_encode(const char *src, int src_len, char *dst);
int base64_decode(const char *src, int src_len, char *dst);

#endif
