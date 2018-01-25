#include <time.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>

#ifdef WIN32
#include <io.h>
#define snprintf _snprintf
#pragma warning(disable : 4786)
#else
#include<unistd.h>
#endif

#include "Log.h"

extern char g_exepath[256];

extern char g_exename[256];

int g_logprintf = 0;

int g_isclose = 0;

int g_logsign = LOGLEVEL_INFO;

static const int max_log_file_size = 5 * 1024 * 1024;

void sleep_s(int sec)
{
#ifdef WIN32
    Sleep(sec * 1000);
#else
    sleep(sec);
#endif
}

void sleep_ms(int msec)
{
#ifdef WIN32
    Sleep(msec);
#else
    usleep(msec * 1000);
#endif
}

#ifndef WIN32
typedef void signal_func(int);

void set_log_flag(int signo)
{
    ++g_logsign;
    
    g_logsign %= LOGLEVEL_COUNT;
    
    term("logsignal is: %s", (LOGLEVEL_INFO == g_logsign) ? "info" : ((LOGLEVEL_ERROR == g_logsign) ? "error" : "notice"));
}

void set_sys_close(int signo)
{
    kill(getppid(), SIGKILL);

    kill(getpid(), SIGKILL);
}

signal_func* set_signal_handler(int signo, signal_func *func)
{
    struct sigaction act, oact;

    act.sa_flags = 0;

    act.sa_handler = func;
    
    sigemptyset(&act.sa_mask);
    
    if (sigaction(signo, &act, &oact) < 0)
    {
        return SIG_ERR;
    }
    else
    {
        return oact.sa_handler;
    }
}

int set_signal(int signo)
{
    if(SIGHUP == signo || SIGPIPE == signo)
    {
        struct sigaction act;

        act.sa_flags = 0;

        act.sa_handler = SIG_IGN;
        
        sigemptyset(&act.sa_mask);

        sigaction(SIGPIPE, &act, NULL);
    }
    else if(SIGTERM == signo)
    {
        if(SIG_ERR == set_signal_handler(signo, set_sys_close))
            return -1;
    }
    else if(SIGUSR1 == signo)
    {
        if(SIG_ERR == set_signal_handler(signo, set_log_flag))
            return -2;
    }
    else
    {
        return -3;
    }

    return 0;
}

#endif

void DoLog(TLogFile* logfile, int level, const char* file, int line, const char* func, const char* fmt, va_list& args);

void sys_log(const char* file, int line, const char* func, const char* fmt, ...)
{
    static TLogFile* syslog = GET_LOG(system);

    if(NULL == syslog)
    {
        printf("start syslog failed"); exit(0);
    }

    va_list args;
    
    va_start(args, fmt);

    DoLog(syslog, LOGLEVEL_NOTICE, file, line, func, fmt, args);

    va_end(args);
}

string split_to_string(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    char inbuf[1024 + 1] = { 0 };

    vsnprintf(inbuf, 1024, fmt, args);

    va_end(args);

    return inbuf;
}

const char* split_to_string(char* inbuf, int inlen, const char* fmt, va_list& args)
{
    vsnprintf(inbuf, inlen, fmt, args);

    return inbuf;
}

void sleep_s(int sec);

void sleep_ms(int msec);

string get_cur_time(bool space)
{
    struct tm* timeinfo;

    time_t now = time(NULL);

#ifdef WIN32
    timeinfo = localtime(&now);
#else
    struct tm t; timeinfo = localtime_r(&now, &t);
#endif

    char curtime[20] = { 0 };

    if(space)
    {
        snprintf(curtime, sizeof(curtime), "%04d-%02d-%02d %02d:%02d:%02d",
            timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }
    else
    {
        snprintf(curtime, sizeof(curtime), "%04d-%02d-%02dT%02d:%02d:%02d",
            timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }

    return curtime;
}

TLogFile::TLogFile(const char* logname)
: m_filesize(0)
, m_logfile(NULL)
{
    snprintf(m_logname, 128, "%s/../log/%s-%s", g_exepath, g_exename, logname);
}

TLogFile::~TLogFile()
{
    if(NULL != m_logfile) fclose(m_logfile);
}

void TLogFile::DoLog(int level, const char* file, int line, const char* func, const char* fmt, ...)
{
    if(level < g_logsign) return;

    va_list args;

    va_start(args, fmt);

    ::DoLog(this, level, file, line, func, fmt, args);

    va_end(args);
}

void DoLog(TLogFile* logfile, int level, const char* file, int line, const char* func, const char* fmt, va_list& args)
{
    if(level < g_logsign) return;

    LogParam* logparam = new LogParam(logfile);

    if(NULL == logparam) return;

#ifdef WIN32
    const char* subfile = strrchr(file, '\\');
#else
    const char* subfile = strrchr(file, '/');
#endif

    const char* subfunc = strrchr(func, ':');

    snprintf(logparam->_prebuf, 1024, "-->Date:%s,Level:%s,File:%s,Line:%d,Func:%s,Log:", get_cur_time().c_str(),
        (LOGLEVEL_INFO == level) ? "info" : ((LOGLEVEL_ERROR == level) ? "error" : "notice"),
        (NULL == subfile) ? file : subfile + 1, line, (NULL == subfunc) ? func : subfunc + 1);

    split_to_string(logparam->_logbuf, 2 * 1024, fmt, args);

    static TLogControl& logcontrol = CTSingleton<TLogControl>::Instance();

#ifndef WIN32
    if(1 == g_logprintf)
#endif
    {
        printf("%s", logparam->_prebuf);
        printf("[%s]\n\n", logparam->_logbuf);
    }

    logcontrol.m_logparams.TryPut(logparam);
}

THREAD_HANDLER_DECL(logcontrol_printlog, lpParam)
{
    CTSingleton<TLogControl>::Instance().PrintLog();

    return 0;
}

TLogControl::TLogControl()
: m_istart(false)
{
}

TLogControl::~TLogControl()
{
    if(m_istart)
    {
        m_istart = false; sleep_s(1); delete m_thread;
    }

    //EraseMap<log_map>(m_logmap);
}

void TLogControl::PrintLog()
{
    while(m_istart)
    {
        LogParam* logparam;

        if(0 == m_logparams.TryGet(logparam))
        {
            char curtime[20] = { 0 };

            const char* start = strchr(logparam->_prebuf, ':');

            const char* end = strchr(logparam->_prebuf, ',');

            memcpy(curtime, start + 1, end - start - 1);

            curtime[13] = '\0';

            char command[128] = { 0 };

            sprintf(command, "%s-%s.log", logparam->_log->m_logname, curtime);

            if(NULL == logparam->_log->m_logfile || 0 != access(command, 0))
            {
                if(NULL != logparam->_log->m_logfile) fclose(logparam->_log->m_logfile);

                logparam->_log->m_logfile = fopen(command, "a+");

                if(NULL == logparam->_log->m_logfile)
                {
                    m_logparams.TryPut(logparam);

                    sleep_ms(1); continue;
                }

                struct stat statbuf;

                if(0 == stat(command, &statbuf)) logparam->_log->m_filesize = statbuf.st_size;

                logparam->_log->m_filesize += fprintf(logparam->_log->m_logfile, "=====================================================\n");

                logparam->_log->m_filesize += fprintf(logparam->_log->m_logfile, "====================    start    ====================\n");

                logparam->_log->m_filesize += fprintf(logparam->_log->m_logfile, "=====================================================\n\n");
            }

            logparam->_log->m_filesize += fprintf(logparam->_log->m_logfile, "%s", logparam->_prebuf);

            logparam->_log->m_filesize += fprintf(logparam->_log->m_logfile, "[%s]\n\n", logparam->_logbuf);

            fflush(logparam->_log->m_logfile);

            if(max_log_file_size <= logparam->_log->m_filesize)
            {
                if(NULL != logparam->_log->m_logfile)
                {
                    fclose(logparam->_log->m_logfile);

                    logparam->_log->m_logfile = NULL;
                }

                char mvcmd[1024] = { 0 };

                curtime[13] = '-'; curtime[16] = '-';

#ifdef WIN32
                sprintf(mvcmd,"move %s %s-%s.log", command, logparam->_log->m_logname, curtime);
#else    
                sprintf(mvcmd,"mv %s %s-%s.log", command, logparam->_log->m_logname, curtime);
#endif

                system(mvcmd);
            }

            delete logparam;
        }
    }
}

TLogFile* TLogControl::GetLog(const char* filename)
{
    if(!m_istart)
    {
#ifndef WIN32
        set_signal(SIGHUP);
        set_signal(SIGPIPE);
        set_signal(SIGUSR1);
#endif

        m_thread = new kernel::CTThread(logcontrol_printlog, this);

        if(!(m_istart = m_thread->IsValid()))
        {
            printf("start logger failed\n");

            exit(0);
        }
    }

    TLogFile* log = NULL;

    m_mutex.Lock();

    log_map::iterator it = m_logmap.find(filename);

    if(m_logmap.end() != it)
    {
        log = (*it).second;
    }
    else
    {
        log = new TLogFile(filename);

        if(NULL == log)
        {
            term("new log: failed", filename);

            sleep_s(1);

            exit(0);
        }
        else
        {
            m_logmap[filename] = log;
        }
    }

    m_mutex.Unlock();

    return log;
}
