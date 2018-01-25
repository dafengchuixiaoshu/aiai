#ifndef  _LOG_FILE_H 
#define  _LOG_FILE_H

#include <string>
using std::string;

#include "BaseQueue.h"
#include "ResourceBase.h"

void sleep_s(int sec);
void sleep_ms(int msec);

string get_cur_time(bool space = false);
string split_to_string(const char* fmt, ...);

enum LogLevel
{
    LOGLEVEL_INFO = 0,
    LOGLEVEL_ERROR,
    LOGLEVEL_NOTICE,
    LOGLEVEL_NOLOG,
    LOGLEVEL_COUNT
};

class TLogFile
{
    friend class TLogControl;
    
    TLogFile(const char* logname);

public:

    ~TLogFile();

    void DoLog(int level, const char* file, int line, const char* func, const char* fmt, ...);

private:

    FILE* m_logfile;

    int  m_filesize;

    char m_logname[128];
};

struct LogParam
{
    LogParam(TLogFile* log):_log(log){ _prebuf[1024] = '\0'; _logbuf[2 * 1024] = '\0'; }

    TLogFile* _log;

    char _prebuf[1024 + 1];

    char _logbuf[2 * 1024 + 1];
};

template<typename T> class CTSingleton
{
public:

    CTSingleton(){}

    static T& Instance()
    {
        static T instance; return instance;
    }
};

class TLogControl
{
    friend class CTSingleton<TLogControl>;

    friend THREAD_HANDLER_DECL(logcontrol_printlog, lpParam);

    friend void DoLog(TLogFile* logfile, int level, const char* file, int line, const char* func, const char* fmt, va_list& args);

    typedef map<string, TLogFile*> log_map;

    TLogControl();

    void PrintLog();

    bool PrintLog(LogParam* logparam);

public:

    ~TLogControl();

    TLogFile* GetLog(const char* filename);

private:

    bool m_istart;

    log_map m_logmap;

    kernel::CTMutex m_mutex;

    kernel::CTThread* m_thread;

    kernel::BaseQueue<LogParam*> m_logparams;
};

template<typename T> void EraseMap(T& erasemap)
{
    typename T::iterator it = erasemap.begin();

    while(erasemap.end() != it)
    {
        delete (it->second);

        erasemap.erase(it);

        it = erasemap.begin();
    }
}

#define term(...) sys_log(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

void sys_log(const char* file, int line, const char* func, const char* fmt, ...);

#define GET_LOG(name) CTSingleton<TLogControl>::Instance().GetLog(#name)

#define LOG_INFO(plog, ...) plog->DoLog(LOGLEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_ERROR(plog, ...) plog->DoLog(LOGLEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#endif
