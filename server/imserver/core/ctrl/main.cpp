#ifndef WIN32
#include<sys/types.h>
#include<sys/wait.h>
#endif

#include "string.h"

#include <string>
using std::string;

#include "Log.h"
#include "Control.h"

extern char* Version;
extern int g_logprintf;

#ifndef WIN32
int set_signal(int signo);
#endif

char g_exepath[256] = { 0 };

char g_exename[256] = { 0 };

static TControl m_pControl;

TControl* GetCtrl()
{
    return &m_pControl;
}

void parse_exe_line(const char* argv_0)
{
#ifdef WIN32
    const char* delim1 = strrchr(argv_0, '\\');
#else
    const char* delim1 = strrchr(argv_0, '/');
#endif

    if(NULL == delim1)
    {
        printf("can't find delim in argv_0\n"); getchar();
    }

    strncpy(g_exepath, argv_0, delim1 - argv_0);

    ++delim1;

    const char* delim2 = strrchr(delim1, '.');

    if(NULL == delim2)
    {
        strcpy(g_exename, delim1);
    }
    else
    {
        strncpy(g_exename, delim1, delim2 - delim1);
    }
}

#include <fstream>

int file_read(char* &outbuf, const char* filename)
{
    int outlen = -1;

    std::ifstream is(filename, std::ios::binary);

    if(is)
    {
        is.seekg(0, is.end);

        outlen = is.tellg();

        is.seekg(0, is.beg);

        if(outlen > 0)
        {
            outbuf = new char[outlen + 1];

            if(NULL == outbuf)
            {
                outlen = -2;
            }
            else
            {
                is.read(outbuf, outlen);

                if(is)
                {
                    outbuf[outlen] = '\0';
                }
                else
                {
                    delete outbuf;

                    outlen = -3;
                }
            }
        }

        is.close();
    }

    return outlen;
}

int file_write(const char* filename, const char* inbuf, int inlen, bool isapp)
{
    std::ios_base::openmode mode = std::ios::binary;

    if(isapp)
    {
        mode |= std::ios_base::app;
    }

    std::ofstream os(filename, mode);

    if(os)
    {
        os.write(inbuf, inlen);

        os.flush();

        os.close();

        return inlen;
    }

    return -1;
}

int main(int argc,char* argv[])
{
    if(argc > 1) g_logprintf = 1;

    parse_exe_line(argv[0]);

    char* outbuf = NULL;

    if(file_read(outbuf, split_to_string("%s/pid.log", g_exepath).c_str()) > 0)
    {
        char cmd[1024] = { 0 };

        sprintf(cmd, "kill -15 %s", outbuf);

        system(cmd);

        sleep_s(1);

        delete outbuf;
    }

#ifndef WIN32
    set_signal(SIGTERM);

    while(1)
    {
        pid_t pid = 0;

        if((pid = fork()) < 0)
        {
            term("fork failed\n"); exit(0);
        }
        else if(pid > 0)
        {
            if(wait(NULL) < 0)
            {
                kill(pid, SIGKILL);

                sleep_s(10);
            }
        }
        else
        {
            break;
        }
    }

    if(file_write(split_to_string("%s/pid.log", g_exepath).c_str(),
        split_to_string("%u", getpid()).c_str(), split_to_string("%u", getpid()).size(), false) <= 0)
    {
        term("printf pid.log failed");

        return 0;
    }

    term("program start. ppid: %u, pid: %u", getppid(), getpid());

#endif

#ifdef _USE_BIT64
    term("use 64bit os system");
#else
    term("use 32bit os system");
#endif

    term("Version: %s", Version);

    _System_Run();

    m_pControl.InitModule();

    return 0;
}
