#ifndef _HASH_FUN_H_
#define _HASH_FUN_H_

/* 5 */
unsigned int RSHash(const char *str);
/* 4 */
unsigned int JSHash(const char *str);
/* 2 */
unsigned int APHash(const char *str);
/* 7 */
unsigned int PJWHash(const char *str);
/* 8 */
unsigned int ELFHash(const char *str);
/* 3 */
unsigned int DJBHash(const char *str);
/* 6 */
unsigned int SDBMHash(const char *str);
/* 1 */
unsigned int BKDRHash(const char *str);

#endif /* _HASH_FUN_H_ */
