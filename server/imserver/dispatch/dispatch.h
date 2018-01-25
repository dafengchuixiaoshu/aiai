#ifndef _GD_MODULE_H_
#define _GD_MODULE_H_

#include "BaseModule.h"

class TLogFile;
class TDBManager;

typedef vector<string> module_info;//module info

typedef map<string, module_info> module_map;//name,info

typedef map<string, module_map> pool_map;//version,modules

class TGDModule : public TBaseModule
{
public:

    TGDModule(){}

    virtual ~TGDModule(){}

    virtual void OnRun();

    virtual void ProcessTimer(TModuleMsg* pMsg);

    virtual void ProcessHttpMsg(TModuleMsg* pMsg);

private:

    void GetPubInfo(string& pubinfo, const int& workindex);

    void GetModules(pool_map& poolmap, const int& workindex);

    bool GetPoolInfo(string& poolinfo, int hash, string& version);

private:

    string m_pubinfo;

    pool_map m_poolmap;

    TLogFile* m_logfile;

    TDBManager** m_dbmgrs;

    kernel::CTMutex m_mutex;
};

#endif //_GD_MODULE_H_
