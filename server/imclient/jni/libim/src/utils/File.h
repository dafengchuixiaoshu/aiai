#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_HANDLE FILE

#define FO_OREAD  1
#define FO_OWRITE 2
#define FO_RW     3

#define FO_CREATE 1
#define FO_APPEND 2

unsigned short File_Open(FILE_HANDLE** FileHandle, const char* name, unsigned short flag,unsigned short mode);

unsigned short File_Close(FILE_HANDLE* FileHandle);

unsigned short File_GetSize(FILE_HANDLE* FileHandle, unsigned long* FileSize);

unsigned short File_Read(FILE_HANDLE* FileHandle, char* buf, unsigned long count,unsigned long* ReadCount);

unsigned short File_Write(FILE_HANDLE* FileHandle, char* buf, unsigned long count,unsigned long* WriteCount);

unsigned short File_Seek(FILE_HANDLE* FileHandle, long offset, short origin,unsigned long* SeekLen);

unsigned short File_Delete(char* name);

unsigned short File_ISExist(char* path);

unsigned short File_Create_Dir(const char* dirName);

unsigned short File_Delete_Dir(char* dirName);

#ifdef __cplusplus
}
#endif
