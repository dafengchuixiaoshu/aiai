#ifndef __FRAMEWORK_LOGGER_H_
#define __FRAMEWORK_LOGGER_H_

#include "mutex.h"
#include "queue.h"
#include "thread.h"

enum
{
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_COUNT
};

class CLog
{
    friend class CLogger;

    CLog(string logname);

public:

    ~CLog();

    void DoLog(int level, const char* file, int line, const char* func, const char* fmt, ...);

    void DoLog(int level, const char* file, int line, const char* func, const char* fmt, va_list& args);

private:

    int m_logsize;

    FILE* m_logfile;

    string m_logname;
};

struct LogParam
{
    LogParam(CLog* log)
        : m_log(log)
    {
        m_prebuf[1024] = '\0';

        m_logbuf[2 * 1024] = '\0';
    }

    CLog* m_log;

    char m_prebuf[1024 + 1];

    char m_logbuf[2 * 1024 + 1];
};

class CLogger
{
    friend class CLog;

    friend class CSingleton<CLogger>;

    typedef map<string, CLog*> log_map;

    friend THREAD_HANDLE(logger_printlog, param);

    CLogger();

    void PrintLog();

    bool PrintLog(LogParam* logparam);

public:

    ~CLogger();

    CLog* GetLog(string logname);

private:

    bool m_istart;

    CMutex m_mutex;

    CThread m_thread;

    log_map m_logmap;

    CQueue<LogParam*> m_logqueue;
};

#define term(...) sys_log(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define GET_LOG(logname) CSingleton<CLogger>::Instance().GetLog(logname)

void sys_log(const char* file, int line, const char* func, const char* fmt, ...);

#define LOG_INFO(plog, ...) plog->DoLog(LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_ERROR(plog, ...) plog->DoLog(LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_NOTICE(plog, ...) plog->DoLog(LOG_LEVEL_NOTICE, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#endif //__FRAMEWORK_LOGGER_H_
