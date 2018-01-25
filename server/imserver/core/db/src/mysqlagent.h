#ifndef __MYSQL_AGENT_H__
#define __MYSQL_AGENT_H__

#include <stdio.h>
#include <string>
#include <assert.h>
#include <string.h>

#include "../mysql/include/mysql.h"

#include "ResourceBase.h"

using namespace std;

#define MAX_TXT_LEN 200000 
#define MAX_DB_LEN 256
#define RECONNECT_COUNT 3

extern const char* lost_server_con_str;

class TMySQLAgent;

class TMySQLParam
{
    friend class TMySQLAgent;
public:
    TMySQLParam();
    ~TMySQLParam();

    void SetCommandText(const char* CommandText);
    const char* getCommandText(){return sqlcommandtext_;};
    
    int GetFieldsSize();
    int GetFieldsCount();
    string GetFieldName(int index);;
    int GetRecordCount();
    string GetStrValue(int row_index, int colum_index);
    int   GetIntValue(int row_index, int colum_index);
    int  GetAffectedRows();

    int  nResult;

private:
    void  create_res_info();
    char* sqlcommandtext_;
    MYSQL_RES* pMySqlRes_;
    MYSQL_ROW* pMySqlRows;
    int        nMySqlRowCount;
    int        nMySqlFieldCount;            
    int        nMysqlAffectedRows;
};

class TMySQLAgent
{
public:
    TMySQLAgent(TMySQLParam* tparam = 0);
    ~TMySQLAgent();
    
public:
    int initconnect(const char* db_name, 
       const char* remote_db_ip, 
       const char* user_name, 
       const char* user_pwd,
       int port);
    
    int connectMysqlDB();
    int excute();
    void disconnectMysqlDB();
    void setMySQLParam(TMySQLParam* tparam){mysqlparam_ = tparam;};
    int  checkconnectstatus(string& sql);
    unsigned int GetExcuteTime();;

private:

    int  Execute_SQL(TMySQLParam* pParam, const char* Parase_Sql);
    
private:
    MYSQL* pmysql;

    TMySQLParam* mysqlparam_;
    char db_name_[MAX_DB_LEN];
    char remote_db_ip_[MAX_DB_LEN];
    char user_name_[MAX_DB_LEN];
    char user_pwd_[MAX_DB_LEN];
    int  dbport;
    int  bhasconnect_;

    kernel::CTMutex m_mutex;
    unsigned int m_excutetime;
};

#endif

