#ifndef _ZLIB_GZIP_H
#define _ZLIB_GZIP_H

char*  gzip_decompress(char  *source,int  length,int& de_len) ;
char*  gzip_compress(char *source,int length, int& en_len);
void   gzip_free(char* content);

#endif