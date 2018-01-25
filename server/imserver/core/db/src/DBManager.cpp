#ifdef WIN32
#include <Afx.h>
#include <Winsock2.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Log.h"
#include "DBManager.h"

#ifdef WIN32
#define sleep(n) Sleep(1000 *n)
#endif

CCheckSyncMysql g_checkSyncMysql;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct TDBThreadParam
{
    TDBManager*   pManager;
    TMySQLAgent*  pSqlAgent;
};

THREAD_HANDLER_DECL(_DBRun, lpParam )
{
    TDBThreadParam* pInfo = (TDBThreadParam*)lpParam;
    TDBManager* pDBManager = pInfo->pManager;
    TMySQLAgent* pMysqlAgent = pInfo->pSqlAgent;
    delete pInfo;
    
    pDBManager->Run(pMysqlAgent);
    return 0;
}

int sync_exec_sql(TDBManager* pDBManager,TMySQLParam* sqlparam,const char* sqlText, TLogFile* logFile)
{
    if(logFile)
    {
        LOG_INFO(logFile, "%s", sqlText);
    }

    sqlparam->SetCommandText(sqlText);
    return pDBManager->SynExecSql(sqlparam);
}

int init_dbmgr(TDBManager** &pDBManager,string dbip,string dbUser,string dbPin,string dbName,int dbPort,int threadcount)
{
    pDBManager = new TDBManager*[threadcount];
    for(int i=0;i<threadcount;i++)
    {
        pDBManager[i] = new TDBManager((char*)dbip.c_str(),(char*)dbName.c_str(),(char*)dbUser.c_str(),(char*)dbPin.c_str(),dbPort,threadcount);
        if(0 != pDBManager[i]->SynStart())
        {
            term("Connect db fail[%d-%s]",i,dbName.c_str());
            
            sleep_s(1);
            
            exit(0);
        }
    }
    return 1;
}

TDBManager::TDBManager(char* mysqldatabaseip, 
                   char* mysqldatabasename, 
                   char* userName,
                   char* password,
                   int   port,
                   int   threadCount)
{
    strcpy(m_sMysqlDatabaseName,mysqldatabasename);
    strcpy(m_sMysqlDatabaseIp,mysqldatabaseip);
    strcpy(m_sUserName,userName);
    strcpy(m_sPassword,password);
    m_nPort = port;
    
    m_nThreadCount = threadCount;
    if(m_nThreadCount < 1)
    {
        m_nThreadCount = 1;
    }
    else if(m_nThreadCount > DB_TRHEAD_COUNT)
    {
        m_nThreadCount = DB_TRHEAD_COUNT;
    }
    
    m_bCanRun = 0;
    for(int i=0;i<m_nThreadCount;i++)
    {
        m_pThread[i]=NULL;
        m_mysqlagent[i]=NULL;
    }
}

TDBManager::~TDBManager()
{
    Stop();
}

int TDBManager::Start(TDBReportResult_CallBack* pResFunc)
{
    m_bCanRun = 1;
    m_pResultFunc = pResFunc;    

    for(int i = 0; i < m_nThreadCount; i++)
    {
        m_mysqlagent[i]=new TMySQLAgent;
        m_mysqlagent[i]->initconnect(m_sMysqlDatabaseName,
                              m_sMysqlDatabaseIp,
                              m_sUserName,
                              m_sPassword,
                              m_nPort);

        TDBThreadParam* pInfo = new TDBThreadParam;
        pInfo->pManager = this;
        pInfo->pSqlAgent = m_mysqlagent[i];
		

        if (m_mysqlagent[i]->connectMysqlDB())
        {
            m_pThread[i] = new kernel::CTThread( _DBRun,pInfo);
        }
        else
        {
            return -1;
        }
    }

    return 0;
}



void TDBManager::ExecSql(TMySQLParam* pMySQLParam,void* nParam)
{
    TDBAccessMsg* pMsg = new TDBAccessMsg;
    pMsg->m_nParam = nParam;
    pMsg->m_pMySQLParam = pMySQLParam;
    if(m_MsgQueue.TryPut(pMsg) == -1)
    {
         (*m_pResultFunc)(pMsg->m_pMySQLParam,pMsg->m_nParam);
         delete pMsg;
    }
}

int TDBManager::SynStart()
{
    m_bCanRun = 1;
    m_mysqlagent[0]=new TMySQLAgent;
    m_mysqlagent[0]->initconnect(m_sMysqlDatabaseName,
                              m_sMysqlDatabaseIp,
                              m_sUserName,
                              m_sPassword,
                              m_nPort);

    if (m_mysqlagent[0]->connectMysqlDB())
    {
        g_checkSyncMysql.m_mutex.Lock();
        g_checkSyncMysql.m_checklist.push_back(m_mysqlagent[0]);
        g_checkSyncMysql.m_mutex.Unlock();
        return 0;
    }
    else
    {
        return -1;
    }
    return 0;
}


int TDBManager::SynExecSql(TMySQLParam* pMySQLParam)
{
    m_mysqlagent[0]->setMySQLParam(pMySQLParam);
    return m_mysqlagent[0]->excute();
}

#ifndef WIN32
unsigned int GetCurSec();
#else
#define GetCurSec() GetTickCount()/1000
#endif

void TDBManager::Run(TMySQLAgent* pSession)
{
    TDBAccessMsg* pDBMsg=NULL;

    while (m_bCanRun)
    {
        if(GetCurSec() - pSession->GetExcuteTime() > 3 * 60)
		{
            static string sql = "SELECT 123;";
            pSession->checkconnectstatus(sql);
		}

		pDBMsg=NULL;
        m_MsgQueue.TryGet(pDBMsg, 60*1000);

        if(pDBMsg)
        {		
			pSession->setMySQLParam(pDBMsg->m_pMySQLParam);

            if (0 != pSession->excute())
                pDBMsg->m_pMySQLParam->nResult = 0;
			
            (*m_pResultFunc)(pDBMsg->m_pMySQLParam,pDBMsg->m_nParam);
            
            delete pDBMsg;
        }
		
    }
    
    
}
void TDBManager::Stop()
{
    g_checkSyncMysql.Remove(m_mysqlagent[0]);

    if (!m_bCanRun)
    {
        return;
    }

    m_bCanRun = 0;
    
    for(int i=0;i<m_nThreadCount;i++)
    {
        if(NULL != m_pThread[i])
        {
            delete m_pThread[i];
            m_pThread[i]=NULL;
        }

        if(NULL != m_mysqlagent[i])
        {
            delete m_mysqlagent[i];
            m_mysqlagent[i] = NULL;
        }
    }
}

CCheckSyncMysql::CCheckSyncMysql()
: m_pThread(NULL)
{
}

CCheckSyncMysql::~CCheckSyncMysql()
{
    //if(NULL != m_pThread)
    //{
    //    delete m_pThread;
    //}
}

void CCheckSyncMysql::Remove(TMySQLAgent* p)
{
    m_mutex.Lock();
    
    for(list<TMySQLAgent*>::iterator it = m_checklist.begin(); it != m_checklist.end(); ++it)
    {
        if(p == *it)
        {
            m_checklist.erase(it);
            m_mutex.Unlock();
            return;
        }
    }
    
    m_mutex.Unlock();
}

THREAD_HANDLER_DECL(_CheckSyncMysqlRun, lpParam)
{
    g_checkSyncMysql.Run();

    return 0;
}

void CCheckSyncMysql::Run()
{
    while (1)
    {
        m_mutex.Lock();

        for(list<TMySQLAgent*>::iterator it = m_checklist.begin(); it != m_checklist.end(); ++it)
        {
            TMySQLAgent* p = *it;

            if(GetCurSec() - p->GetExcuteTime() > 3 * 60)
            {
                static string sql = "SELECT 11;";
                p->checkconnectstatus(sql);
            }
        }

        m_mutex.Unlock();

        sleep(20);
    }
}