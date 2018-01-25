#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>//for system()函数
#include "BizCommon.h"
#include "File.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * File_Open--打开文件
 * flag--读\写\读写\追加
 * mode--如果没有，是否新建
 */
unsigned short File_Open(FILE_HANDLE** FileHandle, const char* name,
		unsigned short flag, unsigned short mode) {

	if (FileHandle == NULL) {
		return 100;
	}
	if (name == NULL) {
		return 100;
	}
	char type[128] = "";

	if (flag == FO_OREAD) {
		if (mode != FO_CREATE) {
			strcpy(type, "r"); //只读，不建文件
		} else {
			strcpy(type, "r+");
		}
	} else if (flag == FO_OWRITE) {
		if (mode != FO_CREATE) {
			strcpy(type, "w"); //只写，不建文件
		} else {
			strcpy(type, "w+");
		}
	} else if (flag == FO_RW) {
		if (mode != FO_CREATE) {
			strcpy(type, "a");
		} else {
			strcpy(type, "a+");
		}
	} else if (flag == FO_APPEND) {
		if (mode != FO_CREATE) {
			strcpy(type, "a");
		} else {
			strcpy(type, "a+");
		}
	}

	*FileHandle = fopen(name, type);
	return 0;
}

/**
 * File_Close--关闭文件
 * 返回值
 * 0--关闭成功;否则失败
 */
unsigned short File_Close(FILE_HANDLE* FileHandle) {
	if (FileHandle == NULL) {
		return 100;
	}
	return fclose(FileHandle);
}

/**
 * File_GetSize--得到文件长度
 * FileSize--返回文件长度
 */
unsigned short File_GetSize(FILE_HANDLE* FileHandle, unsigned long* FileSize) {
	if (FileHandle == NULL) {
		return 100;
	}
	fseek(FileHandle, 0L, SEEK_END);
	*FileSize = ftell(FileHandle);
	return 0;
}

/**
 * File_Read--读取文件到buf
 * count--读取的长度
 * ReadCount--返回已读取的长度
 */
unsigned short File_Read(FILE_HANDLE* FileHandle, char* buf,
		unsigned long count, unsigned long* ReadCount) {
	if (FileHandle == NULL) {
		return 100;
	}
	*ReadCount = fread(buf, 1, count, FileHandle);
	LOGD ("File_Read ReadCount=%d\n", *ReadCount);
	return 0;
}

/**
 * File_Write--从buf中写入文件
 * count--写入的长度
 * WriteCount--返回已写入的长度
 */
unsigned short File_Write(FILE_HANDLE* FileHandle, char* buf,
		unsigned long count, unsigned long* WriteCount) {
	if (FileHandle == NULL) {
		return 100;
	}
	unsigned short write_result = fwrite(buf, count, 1, FileHandle); // 返回值是成功写入的项目数
	if(write_result == 1) {
		*WriteCount = write_result * count;
	}
	return write_result;
}

/**
 * File_Seek--给文件偏移量
 * offset--偏移量
 * origin--偏移方向
 * SeekLen--返回已偏移的长度
 * 返回值0--成功,其他失败
 */
unsigned short File_Seek(FILE_HANDLE* FileHandle, long offset, short origin,
		unsigned long* SeekLen) {
	unsigned short seek_result = fseek(FileHandle, offset, origin);
	if(seek_result == 0) {
		*SeekLen = offset;
	}
	return seek_result;
}

/**
 * File_Delete--删除文件
 * 0--删除成功  -1--删除失败
 */
unsigned short File_Delete(char* name) {
	if (name == NULL) {
		return 100;
	}
	return remove(name);
}

/**
 * File_ISExist--判断文件是否存在
 * 0存在  -1不存在
 */
unsigned short File_ISExist(char* path) {
	if (path == NULL) {
		return 100;
	}
	return access(path, 0);
}

/**
 * File_Create_Dir--创建文件目录
 * 返回0--成功
 */
unsigned short File_Create_Dir(const char* dirName) {
	if (dirName == NULL) {
		return 100;
	}
	return mkdir(dirName, S_IRWXU);
}

/**
 * File_Delete_Dir--删除文件目录
 */
unsigned short File_Delete_Dir(char* dirName) {
	if (dirName == NULL) {
		return 100;
	}
	DIR* dp = NULL;
	DIR* dpin = NULL;
	char *pathname = (char*) malloc(256);
	memset(pathname, 0, 256);
	struct dirent* dirp;
	dp = opendir(dirName);
	if (dp == NULL) {
		LOGD ("File_Delete_Dir your input directory is not exist!\n");
		free(pathname);
		pathname = NULL;
		return 100;
	}
	while ((dirp = readdir(dp)) != NULL) {
		if (strcmp(dirp->d_name, "..") == 0 || strcmp(dirp->d_name, ".") == 0)
			continue;
		strcpy(pathname, dirName);
		strcat(pathname, "/");
		strcat(pathname, dirp->d_name);
		dpin = opendir(pathname);
		if (dpin != NULL) {
			closedir(dpin);
			dpin = NULL;
			File_Delete_Dir(pathname);
		} else {
			remove(pathname);
		}
	}
	rmdir(dirName);
	closedir(dp);
	free(pathname);
	pathname = NULL;
	dirp = NULL;

	return 0;
}

#ifdef __cplusplus
}
#endif
