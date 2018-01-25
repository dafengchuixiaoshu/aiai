
#ifndef __NATIVE_CRASH_HANDLER__
#define __NATIVE_CRASH_HANDLER__

#include "BizCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_CRASH_INFO)
void InitCrashReport(const string strRootDir);
#endif

#ifdef __cplusplus
}
#endif

#endif // !__NATIVE_CRASH_HANDLER__
