#include "logger.h"

int g_logsync = 0;

int g_loglevel = LOG_LEVEL_INFO;

static const int max_log_file_size = 5 * 1024 * 1024;

void sys_log(const char* file, int line, const char* func, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    static CLog* syslog = GET_LOG("system");

    syslog->DoLog(LOG_LEVEL_NOTICE, file, line, func, fmt, args);

    va_end(args);
}

#ifdef LINUX

void set_sys_func(int signo)
{
    if(SIGTERM == signo)
    {
        kill(getppid(), SIGKILL);

        exit(0);
    }
    else if(SIGUSR1 == signo)
    {
        ++g_loglevel;

        g_loglevel %= LOG_LEVEL_COUNT;

        term("system loglevel: %s", (LOG_LEVEL_INFO == g_loglevel) ? "info" : ((LOG_LEVEL_ERROR == g_loglevel) ? "error" : "notice"));
    }
    else if(SIGUSR2 == signo)
    {
        ++g_logsync;

        g_logsync %= 2;

        term("system synclog is: %d", g_logsync);
    }
    else if(SIGHUP == signo || SIGPIPE == signo)
    {
        //SIG_IGN
    }
}

bool set_signal_handler(int signo)
{
    struct sigaction act, oact;

    act.sa_flags = 0; act.sa_handler = set_sys_func;

    sigemptyset(&act.sa_mask);

    return sigaction(signo, &act, &oact) >= 0;
}

#endif

CLog::CLog(string logname)
: m_logsize(0)
, m_logfile(NULL)
, m_logname(logname)
{
}

CLog::~CLog()
{
    if(NULL != m_logfile)
    {
        fclose(m_logfile);
    }
}

void CLog::DoLog(int level, const char* file, int line, const char* func, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    DoLog(level, file, line, func, fmt, args);

    va_end(args);
}

#define slevel (LOG_LEVEL_INFO == level) ? "info" : ((LOG_LEVEL_ERROR == level) ? "error" : "notice")

void CLog::DoLog(int level, const char* file, int line, const char* func, const char* fmt, va_list& args)
{
    if(g_loglevel < level)
    {
        LogParam* logparam = new LogParam(this);

        static CLogger& logger = CSingleton<CLogger>::Instance();

        if(NULL == logparam)
        {
            return;
        }
        else
        {
#ifdef WIN32
            const char* subfile = strrchr(file, '\\');
#else
            const char* subfile = strrchr(file, '/');
#endif

            snprintf(logparam->m_prebuf, 1024, "-->Date:%s,Level:%s,File:%s,Line:%d,Func:%s,Log:",
                get_cur_time().c_str(), slevel, (NULL == subfile) ? file : subfile + 1, line, func);

            split_to_string(logparam->m_logbuf, 2 * 1024, fmt, args);

#if defined(WIN32) || defined(PRINTF_LOG)
            printf("%s[%s]\n\n", logparam->m_prebuf, logparam->m_logbuf);
#endif

            if(1 == g_logsync)
            {
                logger.PrintLog(logparam);
            }
            else
            {
                logger.m_logqueue.Push(logparam);
            }
        }
    }
}

THREAD_HANDLE(logger_printlog, param)
{
    CSingleton<CLogger>::Instance().PrintLog();

    return 0;
}

CLogger::CLogger()
: m_istart(false)
{
}

CLogger::~CLogger()
{
    if(m_istart)
    {
        m_istart = false;

        sleep_s(1);
    }

    LogParam* logparam = NULL;

    while(m_logqueue.Pop(logparam, -1) >= 0)
    {
        PrintLog(logparam);

        delete logparam;
    }

    erase_map<log_map>(m_logmap);
}

void CLogger::PrintLog()
{
    LogParam* logparam;

    while(m_istart)
    {
        if(m_logqueue.Pop(logparam, 0, false) >= 0)
        {
            if(PrintLog(logparam))
            {
                m_logqueue.Pop(logparam);

                delete logparam;
            }
        }
    }
}

bool CLogger::PrintLog(LogParam* logparam)
{
    char curtime[20] = { 0 };

    const char* start = strchr(logparam->m_prebuf, ':');

    const char* end = strchr(logparam->m_prebuf, ',');

    memcpy(curtime, start + 1, end - start - 1);

    curtime[13] = '\0';

    char command[128] = { 0 };

    sprintf(command, "%s-%s.log", logparam->m_log->m_logname.c_str(), curtime);

    if(NULL == logparam->m_log->m_logfile || 0 != access(command, 0))
    {
        if(NULL != logparam->m_log->m_logfile)
        {
            fclose(logparam->m_log->m_logfile);
        }

        logparam->m_log->m_logfile = fopen(command, "a+");

        if(NULL == logparam->m_log->m_logfile)
        {
            return false;
        }

        struct stat statbuf;

        if(0 == stat(command, &statbuf))
        {
            logparam->m_log->m_logsize = statbuf.st_size;
        }

        logparam->m_log->m_logsize += fprintf(logparam->m_log->m_logfile, "=====================================================\n");

        logparam->m_log->m_logsize += fprintf(logparam->m_log->m_logfile, "====================    start    ====================\n");

        logparam->m_log->m_logsize += fprintf(logparam->m_log->m_logfile, "=====================================================\n\n");
    }

    logparam->m_log->m_logsize += fprintf(logparam->m_log->m_logfile,
        "%s%s[%s]\n\n", get_cur_time().c_str(), logparam->m_prebuf, logparam->m_logbuf);

    fflush(logparam->m_log->m_logfile);

    if(max_log_file_size <= logparam->m_log->m_logsize)
    {
        if(NULL != logparam->m_log->m_logfile)
        {
            fclose(logparam->m_log->m_logfile);

            logparam->m_log->m_logfile = NULL;
        }

        curtime[13] = '-';

        curtime[16] = '-';

        char mvcmd[1024] = { 0 };

#ifdef WIN32
        sprintf(mvcmd, "move %s %s-%s.log", command, logparam->m_log->m_logname.c_str(), curtime);
#else
        sprintf(mvcmd, "mv %s %s-%s.log", command, logparam->m_log->m_logname.c_str(), curtime);
#endif

        system(mvcmd);
    }

    return true;
}

static bool parse_exepath(const char* argv_0, string& exepath, string& exename)
{
    exename = "";

    exepath = argv_0;

    replace_separator(exepath, "\\", "/");

    string::size_type pos1 = exepath.rfind("/");

    if(string::npos != pos1)
    {
        string::size_type pos2 = exepath.rfind(".");

        if(string::npos == pos2)
        {
            exename = exepath.substr(pos1 + 1);
        }
        else
        {
            exename = exepath.substr(pos1 + 1, pos2 - pos1 - 1);
        }

        exepath.erase(pos1);

        return true;
    }

    return false;
}

static void get_exe_param(char* exepath, char* exename)
{
#ifdef WIN32
    if(!GetModuleFileName(NULL, exepath, 255))
    {
        printf("get exepath failed\n");

        exit(0);
    }
#else
    int rslt = readlink("/proc/self/exe", exepath, 255);

    if(rslt < 0 || rslt >= 255)
    {
        printf("get exepath failed\n");

        exit(0);
    }
#endif

    string path, name;

    if(parse_exepath(exepath, path, name))
    {
        strcpy(exepath, path.c_str());

        strcpy(exename, path.c_str());
    }
}

CLog* CLogger::GetLog(string logname)
{
    char exepath[256] = { 0 };

    char exename[256] = { 0 };

    get_exe_param(exepath, exename);

    if(!m_istart)
    {
#ifdef LINUX
        set_signal_handler(SIGHUP);

        set_signal_handler(SIGPIPE);

        set_signal_handler(SIGTERM);

        set_signal_handler(SIGUSR1);

        set_signal_handler(SIGUSR2);
#endif

        if(!(m_istart = m_thread.DoStart(logger_printlog, this)))
        {
            printf("start logger failed\n");

            exit(0);
        }
    }

    m_mutex.Lock();

    CLog* log = NULL;

    char filename[1024] = { 0 };

    sprintf(filename, "%s/../log/%s-%s", exepath, exename, logname.c_str());

    log_map::const_iterator it = m_logmap.find(filename);

    if(m_logmap.end() == it)
    {
        log = new CLog(filename);

        if(NULL == log)
        {
            if(0 == stricmp(filename, "system"))
            {
                printf("start log:%s failed\n", filename);
            }
            else
            {
                term("start log:%s failed\n", filename);
            }

            exit(0);
        }
        else
        {
            m_logmap[filename] = log;
        }
    }
    else
    {
        log = it->second;
    }

    m_mutex.Unlock();

    return log;
}
