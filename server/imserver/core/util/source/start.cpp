#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#endif

void  GetInfomation();
void  CreateFifo();
void  PeepProcess();
void  StartProcess(int index = -1);
void  WriteErrorLog(char*  reason);
void  catchSigCLD();

char m_sStartHome[256];
#define CHECK_TIMER 30 

struct TProcessInfo
{
    int   m_nReadAccessFD;
    int   m_nPid;
    char  m_sFifoName[256];
    char  m_sExePath[256];
    char  m_sExeName[256];
    int   m_LastSecond;
    
    TProcessInfo()
    {
        m_nReadAccessFD = 0;
        m_nPid = 0;
        m_sFifoName[0] =0;
        m_sExePath[0] =0;
        m_sExeName[0] =0;
        m_LastSecond = 0;
    }
};

#define MAX_PROCESS  10
TProcessInfo _ProcessInfo[MAX_PROCESS];
int          _ProcessCount = 1;

char*  getExePath()
{
    static  char  buf[1024];
    memset(buf,0,sizeof(buf));

    int   rslt   =   readlink("/proc/self/exe",buf,1024);
    if(rslt < 0 || rslt>= 1024 )
    {
          return   "../";
    }

    int j = 0;
    for(int i=rslt;i>=0;i--)
    {
         if(buf[i] == '/')
         {
               j++;
	       if(j == 2)
	       {
  	           buf[i] = 0;
                   return buf; 
	       }
         }
    }
    return   "../";
}


main(int argc, char** argv)
{
    if(argc>1 && strcmp(argv[1],"-v")==0)
    {
        printf("\n\tstart program for presence.");
        printf("\n\tVersion 1.00.");
        printf("\n\tCopyRight feinno, All Rights reserved.\n\n");
        exit(0);
    }
    
    if (fork()!=0)
    {
        exit(0);
    }
   	
#ifdef _SUN_SYSTEM
    signal(SIGCHLD,SIG_IGN);
#else   	
    signal(SIGCLD, (void(*)(int))catchSigCLD);      //dispose the signal that child process be terminated.
#endif
    signal(SIGHUP, SIG_IGN);                        //ignore  the signal that control terminal telephone line be disconnected.
    signal(SIGINT, SIG_IGN);                        //ignore  the signal that control terminal be terminated.Type delete key or ctrl-c key. 

    strcpy(m_sStartHome,getExePath());

    GetInfomation();
    CreateFifo();
    WriteErrorLog("start[passive]");
    StartProcess();
    PeepProcess();
}


void GetInfomation()
{
    int  count = 0;
    char sExeName[256]="";
    
    FILE *fp;
    char sFileName[512];
    sprintf(sFileName,"%s/config/config.start",m_sStartHome);
    if((fp=fopen(sFileName,"r"))==NULL)
    {
        printf("The file %s is not exist\n",sFileName);
        exit(0);
    }
    fscanf(fp,"%s %d",sExeName,&count);
    fclose(fp);
    fp=NULL;
    
    if(count > 0)
    {
        if(count > MAX_PROCESS)
        {
            _ProcessCount = MAX_PROCESS;
        }
        else
        {
            _ProcessCount = count;
        }
    }
    
    if(strlen(sExeName)>18 || strlen(sExeName)<1)
    {
        printf("The content of file %s is invalid\n",sFileName);
        exit(0);
    }
    
    for(int i=0; i< _ProcessCount;i++)
    {
        strcpy(_ProcessInfo[i].m_sExeName,	sExeName);
        sprintf(_ProcessInfo[i].m_sFifoName,"fifo.%s_%d",sExeName,i);
        sprintf(_ProcessInfo[i].m_sExePath,"%s/bin/%s",m_sStartHome,sExeName);
    }
    
    
}
void StartProcess(int index)
{
    for(int i = 0;i<	_ProcessCount;i++)
    {
        if(index == -1 || index == i)
        {
            sleep(1);
            
            if((_ProcessInfo[i].m_nPid=fork()) ==0 )
            {
                char index[64];
                sprintf(index,"%d",i);
                if(execl(_ProcessInfo[i].m_sExePath,_ProcessInfo[i].m_sExeName,"-fifo",_ProcessInfo[i].m_sFifoName,"-i",index,"-p","1",(char*)0)<0)
                {
                    printf("the %s don't execute.\n",_ProcessInfo[i].m_sExePath);
                    char buffer[1024];
                    sprintf(buffer,"Execl fail-%s",strerror(errno));
                    WriteErrorLog(buffer);

                    kill(getppid(),SIGTERM);
                }
               
                WriteErrorLog("Start success");
                exit(0);
            }
            else
            {
                char buffer[1024];
                sprintf(buffer,"Start fail-%s",strerror(errno));
                WriteErrorLog(buffer);
            }
        }
    }
        
}
void  CreateFifo()
{
    char   sFileName[256];
    char   sSystem[256];

    sprintf(sSystem,"mkdir %s/fifo",m_sStartHome);
    system(sSystem);
   
    for(int i = 0;i<	_ProcessCount;i++)
    { 
        sprintf(sFileName,"%s/fifo/%s",m_sStartHome,_ProcessInfo[i].m_sFifoName);
        remove(sFileName);
        sprintf(sSystem,"mkfifo %s",sFileName);
        system(sSystem);
        
        if((_ProcessInfo[i].m_nReadAccessFD=open(sFileName,O_RDONLY|O_NDELAY))<0)
        {
            printf("Open fifo failed.\n");
            exit(0);
        }
    }    
}

void PeepProcess()
{
    char   sReadData[256];
    time_t currentSecond;

    currentSecond=time(NULL);
    
    for(int i = 0;i<	_ProcessCount;i++)
    {
        _ProcessInfo[i].m_LastSecond = currentSecond;
    }

    for(;;)
    {
        for(int i =  0; i < 	_ProcessCount;i++)
        {
            currentSecond=time(NULL);
            if(currentSecond-_ProcessInfo[i].m_LastSecond>=CHECK_TIMER)
            {
                WriteErrorLog("start[auto]");
                kill(_ProcessInfo[i].m_nPid,SIGTERM);
                StartProcess(i);
                _ProcessInfo[i].m_LastSecond=currentSecond;
            }
       	    if(read(_ProcessInfo[i].m_nReadAccessFD,sReadData,255)>0)
       	    {
                _ProcessInfo[i].m_LastSecond=currentSecond;
            }
        }
        sleep(2);
    }
}

void  WriteErrorLog(char*  reason)
{
    time_t	atimer;
    struct	tm   *area;
    FILE        *fp;

    char  fileName[128];
    sprintf(fileName,"%s/log/start.log",m_sStartHome);
    
    atimer = time( NULL );
    area = localtime( &atimer );
    fp=fopen(fileName,"a+");
    fprintf(fp,"%s:%04d%02d%02d%02d%02d%02d\n",reason,area->tm_year+1900,area->tm_mon + 1, area->tm_mday,area->tm_hour,area->tm_min,area->tm_sec);
    fclose(fp);
    fp=NULL;	
}

void catchSigCLD()
{
    waitpid(-1, (int*)0, WNOHANG);
}
