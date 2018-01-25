#ifndef _DB_MANAGER_H
#define _DB_MANAGER_H

#include "mysqlagent.h"
#include "ResourceBase.h"
#include "BaseQueue.h"

#define DB_TRHEAD_COUNT 20
struct TDBAccessMsg
{
    void*           m_nParam;
    TMySQLParam*    m_pMySQLParam;
};

typedef void TDBReportResult_CallBack(TMySQLParam* pMySQLParam,void* nParam);

class TLogFile;
class TDBManager;

int sync_exec_sql(TDBManager* pDBManager,TMySQLParam* sqlparam,const char* sqlText, TLogFile* logFile = NULL);
int init_dbmgr(TDBManager** &pDBManager,string dbip,string dbUser,string dbPin,string dbName,int dbPort,int threadcount);

class TDBManager  
{
public:
    TDBManager(char* mysqldatabaseip, 
           char* mysqldatabasename, 
           char* userName,
           char* password,
           int   port,
           int   threadCount
           );
    virtual ~TDBManager();

    int  Start(TDBReportResult_CallBack* pResFunc);
    void Run(TMySQLAgent* pSession);
    void Stop();
    
    void ExecSql(TMySQLParam* pMySQLParam,void* nParam);

    int SynStart();
    int SynExecSql(TMySQLParam* pMySQLParam);

private:
    char       m_sMysqlDatabaseIp[64];
    char       m_sUserName[64];
    char       m_sPassword[64];
    char       m_sMysqlDatabaseName[64];
    int        m_nPort;

    kernel::BaseQueue<TDBAccessMsg*> m_MsgQueue;
    int CheckConnectStatus();
    int        m_bCanRun;
    kernel::CTThread*  m_pThread[DB_TRHEAD_COUNT];
    TMySQLAgent*  m_mysqlagent[DB_TRHEAD_COUNT];
    
    TDBReportResult_CallBack* m_pResultFunc;
    
    int        m_nThreadCount;
};

#include <list>
using std::list;

class CCheckSyncMysql
{
public:

    CCheckSyncMysql();
    ~CCheckSyncMysql();

    void Run();

    void Remove(TMySQLAgent* p);

    list<TMySQLAgent*>  m_checklist;
    kernel::CTMutex m_mutex;

    kernel::CTThread*  m_pThread;
};

#endif
