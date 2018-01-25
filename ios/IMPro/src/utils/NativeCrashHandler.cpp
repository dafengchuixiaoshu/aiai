
#include "NativeCrashHandler.h"

#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>

#include "Defines.h"
#include "BizCommon.h"
#include "File.h"
#include "CIMI.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_CRASH_INFO)

static string g_strRootDir = "";
typedef struct map_info_t map_info_t;
typedef struct {
	uintptr_t absolute_pc;
	uintptr_t stack_top;
	size_t stack_size;
} backtrace_frame_t;
typedef struct {
	uintptr_t relative_pc;
	uintptr_t relative_symbol_addr;
	char* map_name;
	char* symbol_name;
	char* demangled_name;
} backtrace_symbol_t;
typedef ssize_t (*t_unwind_backtrace_signal_arch)(siginfo_t* si, void* sc, const map_info_t* lst, backtrace_frame_t* bt, size_t ignore_depth, size_t max_depth);
static t_unwind_backtrace_signal_arch unwind_backtrace_signal_arch;
typedef map_info_t* (*t_acquire_my_map_info_list)();
static t_acquire_my_map_info_list acquire_my_map_info_list;
typedef void (*t_release_my_map_info_list)(map_info_t* milist);
static t_release_my_map_info_list release_my_map_info_list;
typedef void (*t_get_backtrace_symbols)(const backtrace_frame_t* backtrace, size_t frames, backtrace_symbol_t* symbols);
static t_get_backtrace_symbols get_backtrace_symbols;
typedef void (*t_free_backtrace_symbols)(backtrace_symbol_t* symbols, size_t frames);
static t_free_backtrace_symbols free_backtrace_symbols;

void nativeCrashHandler_sigaction(int signal, siginfo_t *siginfo, void *sigcontext)
{
	if (g_bOpenLog)
	{
		LOGD ("Create a crash infomation begin\r\n");

		if (acquire_my_map_info_list != NULL
				&& unwind_backtrace_signal_arch != NULL
				&& get_backtrace_symbols != NULL
				&& free_backtrace_symbols != NULL
				&& release_my_map_info_list != NULL)
		{
			LOGD ("Writing crash info\r\n");

			const string strRootDir = g_strRootDir;
	//		const string strRootDir = "/storage/emulated/0/Android/data/com.izp.f2c/file/izpmmp";
			if (strRootDir.empty())
			{
				LOGD ("Can't get the root dir\r\n");
				return;
			}

			string strDateTime = MkIntDateTime2();
			string strCrashDir = strRootDir + "/crash";
			string strCrashFilename = strCrashDir + "/" + strDateTime + ".log";
			LOGD ("strCrashFilename=%s\r\n", strCrashFilename.c_str());

			// if the directory is not exist, so create it immediately.
			File_Create_Dir (strCrashDir.c_str());

			FILE_HANDLE* FileHandle = NULL;
			unsigned long WriteCount = 0;

			File_Open (&FileHandle, strCrashFilename.c_str(), FO_OWRITE, FO_CREATE);
			if (FileHandle != NULL)
			{
				char szDump[256] = {'\0'};

				snprintf (szDump, 255, "%s\r\n", strDateTime.c_str());
				LOGD ("%s", szDump);
				File_Write (FileHandle, szDump, strlen(szDump), &WriteCount);

				snprintf (szDump, 255, "%s\r\n", strsignal(signal));
				LOGD ("%s", szDump);
				File_Write (FileHandle, szDump, strlen(szDump), &WriteCount);

				map_info_t *map_info = acquire_my_map_info_list();
				backtrace_frame_t frames[256] = {0,};
				backtrace_symbol_t symbols[256] = {0,};
				const ssize_t size = unwind_backtrace_signal_arch(siginfo, sigcontext, map_info, frames, 1, 255);
				get_backtrace_symbols(frames,  size, symbols);
				for (int i = 0; i < size; ++i)
				{
					snprintf (szDump, 255, "%s\r\n", (symbols[i].demangled_name ? symbols[i].demangled_name : symbols[i].symbol_name));
					LOGD ("%s", szDump);
					File_Write (FileHandle, szDump, strlen(szDump), &WriteCount);
				}
				free_backtrace_symbols(symbols, size);
				release_my_map_info_list(map_info);

				File_Close (FileHandle);
			}
		}

		LOGD ("Create a crash infomation end\r\n");
	}
}

void nativeCrashHandler_onLoad()
{
	if (g_bOpenLog)
	{
		void * libcorkscrew = dlopen("libcorkscrew.so", RTLD_LAZY | RTLD_LOCAL);
		if (libcorkscrew) {
			unwind_backtrace_signal_arch = (t_unwind_backtrace_signal_arch) dlsym(libcorkscrew, "unwind_backtrace_signal_arch");
			acquire_my_map_info_list = (t_acquire_my_map_info_list) dlsym(libcorkscrew, "acquire_my_map_info_list");
			release_my_map_info_list = (t_release_my_map_info_list) dlsym(libcorkscrew, "release_my_map_info_list");
			get_backtrace_symbols  = (t_get_backtrace_symbols) dlsym(libcorkscrew, "get_backtrace_symbols");
			free_backtrace_symbols = (t_free_backtrace_symbols) dlsym(libcorkscrew, "free_backtrace_symbols");
		}
	}
}

static struct sigaction old_sa[NSIG];

void android_sigaction(int signal, siginfo_t *info, void *reserved)
{
	if (g_bOpenLog)
	{
		if (signal >= NSIG)
			return;

		if (old_sa[signal].sa_handler)
			old_sa[signal].sa_handler(signal);

		nativeCrashHandler_sigaction(signal, info, reserved);

	//	old_sa[signal].sa_handler(signal);
	}
}

void InitCrashReport(const string strRootDir)
{
	if (g_bOpenLog)
	{
		LOGD ("InitCrashReport\r\n");

		g_strRootDir = strRootDir;
		LOGD ("Crash Report File will be saved in %s\r\n", g_strRootDir.c_str());

		nativeCrashHandler_onLoad ();

		// Try to catch crashes...
		struct sigaction handler;
		memset(&handler, 0, sizeof(struct sigaction));

		handler.sa_sigaction = android_sigaction;
		handler.sa_flags = SA_RESETHAND | SA_SIGINFO;

	/*
		SIGTERM	 终止请求，发送给程序
		SIGSEGV	 无效的内存访问（分割故障）
		SIGINT	 外部中断，通常是由用户发起的
		SIGILL	 无效的程序图像，如无效的指令
		SIGABRT	 异常终止的条件，例如，发起abort()
		SIGFPE	 如被零除错误的算术运算
		SIGTRAP	 实现相关的硬件异常。一般是调试异常
	*/
	#define CATCHSIG(X) sigaction(X, &handler, &old_sa[X])
		CATCHSIG(SIGTERM);
		CATCHSIG(SIGSEGV);
		CATCHSIG(SIGINT);
		CATCHSIG(SIGILL);
		CATCHSIG(SIGABRT);
		CATCHSIG(SIGFPE);
		CATCHSIG(SIGBUS);
		CATCHSIG(SIGPIPE);
		CATCHSIG(SIGTRAP);
	}
}

#endif

#ifdef __cplusplus
}
#endif

