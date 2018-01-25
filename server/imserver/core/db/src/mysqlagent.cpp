#ifdef WIN32
#include <Afx.h>
#include <Winsock2.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysqlagent.h"
#include "../mysql/include/errmsg.h"

#include "Control.h"
#include "Log.h"
TControl* GetCtrl();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

unsigned int GetCurSec()
{
#ifndef WIN32
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned int)tv.tv_sec;
#else
    return GetTickCount()/1000;
#endif
}

TMySQLParam::TMySQLParam()
{
    sqlcommandtext_ = NULL;
    pMySqlRes_ = NULL;
    pMySqlRows = NULL;
    nMySqlRowCount = 0;
    nMySqlFieldCount = 0;
    nMysqlAffectedRows =0;
    nResult = -1;
}

TMySQLParam::~TMySQLParam()
{
    if(sqlcommandtext_)
    {
        delete sqlcommandtext_;
        sqlcommandtext_ = NULL;
    }

    if(pMySqlRows)
    {
        delete [] pMySqlRows;
    }
    
    if(pMySqlRes_)
    {
        mysql_free_result(pMySqlRes_);
    }
}

void TMySQLParam::SetCommandText(const char* CommandText)
{
    int length = strlen(CommandText);

    sqlcommandtext_ = new char[length+1];   
	  memset(sqlcommandtext_, 0, length+1);
    strcpy(sqlcommandtext_, CommandText);
}

int TMySQLParam::GetFieldsSize()
{
    return nMySqlFieldCount;
}

int TMySQLParam::GetFieldsCount()
{
    return nMySqlFieldCount;
}

string TMySQLParam::GetFieldName(int index)
{
    if(index<0 || index >= nMySqlFieldCount)
        return "";
    
    MYSQL_FIELD* pField = mysql_fetch_field_direct(pMySqlRes_,index);
    if(pField == NULL)
        return "";
    
    return pField->name;
}

int TMySQLParam::GetRecordCount()
{
    return nMySqlRowCount;
}

void TMySQLParam::create_res_info()
{
    if(!pMySqlRes_)
        return ;
    
    nMySqlRowCount = mysql_num_rows(pMySqlRes_);
    nMySqlFieldCount = mysql_num_fields(pMySqlRes_);
    
    if(nMySqlRowCount >0 )
    {
        pMySqlRows =  new MYSQL_ROW[nMySqlRowCount];
        for(int i=0;i<nMySqlRowCount;i++)
        {
            pMySqlRows[i] = mysql_fetch_row(pMySqlRes_);
        }
    }
}

string TMySQLParam::GetStrValue(int row_index, int colum_index)
{
    if(row_index <0 || row_index>= nMySqlRowCount)
        return "";
    
    if(colum_index <0 || colum_index>= nMySqlFieldCount)
        return "";

    char* temp = pMySqlRows[row_index][colum_index];
    if(NULL == temp) return "";

    return temp;
}

int TMySQLParam::GetIntValue(int row_index, int colum_index)
{
    string s = GetStrValue(row_index, colum_index);

    return atoi(s.c_str());
}

int TMySQLParam::GetAffectedRows()
{
    return nMysqlAffectedRows;
}
//////////////////////////////////////////////////////////////////////////

TMySQLAgent::TMySQLAgent(TMySQLParam* tparam):mysqlparam_(tparam)
{
    memset(db_name_, 0, MAX_DB_LEN);
    memset(remote_db_ip_, 0, MAX_DB_LEN);
    memset(user_name_, 0, MAX_DB_LEN);
    memset(user_pwd_, 0, MAX_DB_LEN);
    dbport = 0;
}
    
TMySQLAgent::~TMySQLAgent()
{
    disconnectMysqlDB();
}

int TMySQLAgent::initconnect(const char* db_name, 
               const char* remote_db_ip, 
               const char* user_name, 
               const char* user_pwd,
               int port)
{
    assert(db_name && remote_db_ip && user_name && user_pwd);
    
    strcpy(db_name_, db_name);
    strcpy(remote_db_ip_, remote_db_ip);
    strcpy(user_name_, user_name);
    strcpy(user_pwd_, user_pwd);
    dbport = port;
    
    
    return 1;
}

int TMySQLAgent::connectMysqlDB()
{
    pmysql = mysql_init( NULL );
    if(pmysql == NULL)
    {
        term( "Failed to init mysql error");
        return 0;
    }
    
    char value = 1;
    mysql_options(pmysql, MYSQL_OPT_RECONNECT, (char*)&value);
    mysql_options(pmysql, MYSQL_SET_CHARSET_NAME, "utf8");

    value = 1;
    int ret = mysql_options(pmysql, MYSQL_OPT_READ_TIMEOUT, (char*)&value);
    ret = mysql_options(pmysql, MYSQL_OPT_WRITE_TIMEOUT, (char*)&value);

    if ( NULL == mysql_real_connect( pmysql, 
                              remote_db_ip_, 
                              user_name_,
                              user_pwd_,
                              db_name_,
                              dbport,
                              NULL,
                              CLIENT_MULTI_STATEMENTS | CLIENT_FOUND_ROWS ) )
    {
        term( "Failed to connect to mysql! err=%s",mysql_error(pmysql));
        return 0;
    }
    
    bhasconnect_ = true;

    return 1;
}

int TMySQLAgent::checkconnectstatus(string& sql)
{
 	//unsigned int starttime = GetCurSec();
    int ret = 0;
    //ret = mysql_ping(pmysql);
    //if(0 != ret)
    {
        //unsigned int endtime = GetCurSec();
        //GetCtrl()->GetSystemLog()->WriteLog("checkconnectstatus error,time is[%u][%u][%u].err=[%d]%s\n",endtime,starttime,endtime-starttime,ret,mysql_error(pmysql));
        //bhasconnect_ = false;

        TMySQLParam* pMysqlCommand=new TMySQLParam();			
        pMysqlCommand->SetCommandText(sql.c_str());
        setMySQLParam(pMysqlCommand);
        excute();
        delete pMysqlCommand;
    }
    
    return ret;
}

int TMySQLAgent::Execute_SQL(TMySQLParam* pParam, const char* Parase_Sql)
{
 	unsigned int starttime = GetCurSec();
    unsigned int endtime;

    int result = mysql_real_query(pmysql,Parase_Sql,(unsigned long)strlen(Parase_Sql));

    if(result != 0)
	{
        term("the first execute sql error,result: %d, err=%s, SQL is : %s",result,mysql_error(pmysql),Parase_Sql);

        int ret = mysql_ping(pmysql);
        if(0 != ret)
		{
            term("mysql_ping error,err=[%d]%s, SQL is : %s",ret,mysql_error(pmysql),Parase_Sql);
			bhasconnect_ = false;
			return 0;
		}
		else
		{
            result = mysql_real_query(pmysql,Parase_Sql,(unsigned long)strlen(Parase_Sql));

			if(result != 0)
			{
                term("the second execute sql error,result: %d,err=%s, SQL is : %s",result,mysql_error(pmysql),Parase_Sql);
                bhasconnect_ = false;
                return 0;
			}
		}
	}
    
	endtime = GetCurSec();
	if (endtime-starttime > 20) 
	{
        term("outtime exception(%u,%u,%u). SQL is : %s",starttime,endtime,endtime-starttime, Parase_Sql);
	}

    pParam->pMySqlRes_ = mysql_store_result(pmysql);
    pParam->nMysqlAffectedRows = mysql_affected_rows(pmysql);
    if( pParam->pMySqlRes_  != NULL)
    {
        pParam->create_res_info();    
    }
    return 1;
}

unsigned int TMySQLAgent::GetExcuteTime()
{
    return m_excutetime;
}

int TMySQLAgent::excute()
{
    m_mutex.Lock();

    m_excutetime = GetCurSec();

    if ( !bhasconnect_)
    {
        mysql_close(pmysql);
        bhasconnect_ = false; 
        int reconnect_count = RECONNECT_COUNT;
        while(reconnect_count--)
        {
            int ret = connectMysqlDB();
            if(ret == 1)
                break;
        }
    }
	
    if(!bhasconnect_)
    {
        term("Can't connect db, SQL is : %s", mysqlparam_->sqlcommandtext_);
        m_mutex.Unlock();
        return 0;
    }
    else
    {
        int ret = Execute_SQL(mysqlparam_,mysqlparam_->sqlcommandtext_);
        m_mutex.Unlock();
        return ret;
    }
}

void TMySQLAgent::disconnectMysqlDB()
{
    if (bhasconnect_)
    {
        mysql_close(pmysql);
        bhasconnect_ = false;
    }
}




