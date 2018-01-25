#ifndef _PG_MODULE_H_
#define _PG_MODULE_H_

#include <map>
using std::map;
#include <string>
using std::string;
#include <vector>
using std::vector;

#include "BaseModule.h"

bool getsqltime(TDBManager* pManager, string& ver);

struct ContactInfo
{
    ContactInfo()
        : _optype(-1)
    {
    }

    ContactInfo(const ContactInfo& r)
    {
        Copy(r);
    }

    ContactInfo& operator=(const ContactInfo& r)
    {
        if(&r != this)
        {
            Copy(r);
        }

        return *this;
    }

    string _id;
    string _loginame;
    string _nickname;
    string _realname;
    string _email;
    string _mobile;
    string _facevideo;
    string _portraitcrc;//头像
    string _impresa;//箴言,签名,心情短语
    int _optype;//操作类型：0添加，1删除
    string _issave;
    string _isshare;
    string _subname;

private:

    void Copy(const ContactInfo& r)
    {
        _id = r._id;
        _loginame = r._loginame;
        _nickname = r._nickname;
        _realname = r._realname;
        _email = r._email;
        _mobile = r._mobile;
        _portraitcrc = r._portraitcrc;
        _impresa = r._impresa;
        _optype = r._optype;
        _facevideo = r._facevideo;
        _issave = r._issave;
        _isshare = r._isshare;
        _subname = r._subname;
    }
};

void getmembername(TDBManager* pManager, string& cardname, string gid, string uid);

int appendcontact(TDBManager* pManager, ContactInfo& frominfo, ContactInfo& toinfo);

int deletecontact(TDBManager* pManager, ContactInfo& frominfo, ContactInfo& toinfo);

int getcontactinfo(TDBManager* pUserManager, TDBManager* pImpresaManager, string dbkey, ContactInfo& contactinfo, string uid);

int getcontactlist(TDBManager* pUserManager, TDBManager* pImpresaManager, string uid, string ver, vector<ContactInfo>& contactinfolist, string type, bool isbuddy);

struct ContactUpdate
{
    ContactUpdate()
    {
    }

    ContactUpdate(const ContactUpdate& r)
    {
        Copy(r);
    }

    ContactUpdate& operator=(const ContactUpdate& r)
    {
        if(&r != this)
        {
            Copy(r);
        }

        return *this;
    }

    string _time;

    ContactInfo _myselfinfo;

    vector<ContactInfo> _contactinfos;

private:

    void Copy(const ContactUpdate& r)
    {
        _time = r._time;
        _myselfinfo = r._myselfinfo;
        _contactinfos = r._contactinfos;
    }
};

void getcontactupdate(TDBManager* pUserManager, TDBManager* pImpresaManager, string ver, vector<ContactUpdate>& contactupdates, bool isinfo);
int SearchContact(TDBManager* pUserManager,TDBManager* pImpresaManager,string dbkey, vector<ContactInfo>& contactinfolist);
struct GroupMemberInfo
{
    GroupMemberInfo()
    {
    }

    GroupMemberInfo(const GroupMemberInfo& r)
    {
        Copy(r);
    }

    GroupMemberInfo& operator=(const GroupMemberInfo& r)
    {
        if(&r != this)
        {
            Copy(r);
        }

        return *this;
    }

    string _gid;
    string _uid;
    string _status;
    string _cardname;//备注
    string _createtime;
    ContactInfo _contactinfo;

private:

    void Copy(const GroupMemberInfo& r)
    {
        _gid = r._gid;
        _uid = r._uid;
        _status = r._status;
        _cardname = r._cardname;
        _createtime = r._createtime;
        _contactinfo = r._contactinfo;
    }
};
//离线消息
struct OfflineMSG
{
    OfflineMSG()
    {
    }

    OfflineMSG(const OfflineMSG& r)
    {
        Copy(r);
    }

    OfflineMSG& operator=(const OfflineMSG& r)
    {
        if(&r != this)
        {
            Copy(r);
        }

        return *this;
    }

    string _recvtime;//服务器接收消息时间
    string _msg;//消息
   

private:

    void Copy(const OfflineMSG& r)
    {
        _recvtime = r._recvtime;
        _msg = r._msg;
    }
};
//获取离线消息
int getofflinemsg(TDBManager* pManager,vector<OfflineMSG>& offmsgs ,string uid);
//change offline messsage
int getofflinemsg(TDBManager* pManager,string sql);

bool ismember(TDBManager* pManager, string gid, string uid);

bool insertmember(TDBManager* pManager, string gid, string uid, int type);

bool deletemember(TDBManager* pManager, string gid, string gkey, string uid);

bool upmemberinfo(TDBManager* pManager, string gid, string uid, string sqlappend);
bool upcontactinfo(TDBManager* pManager, string uid,string to);
int getmemberlist(TDBManager* pUserManager, TDBManager* pImpresaManager, vector<GroupMemberInfo>& members, string sqlappend);

struct GroupInfo
{
    GroupInfo()
    {
    }

    GroupInfo(const GroupInfo& r)
    {
        Copy(r);
    }

    GroupInfo& operator=(const GroupInfo& r)
    {
        if(&r != this)
        {
            Copy(r);
        }

        return *this;
    }

    string _gid;
    string _gname;
    string _gcreator;
    string _gportraitcrc;//群头像
    int _gmaxcount;

private:

    void Copy(const GroupInfo& r)
    {
        _gid = r._gid;
        _gname = r._gname;
        _gcreator = r._gcreator;
        _gmaxcount = r._gmaxcount;
        _gportraitcrc = r._gportraitcrc;
    }
};

bool insertgroup(TDBManager* pManager, GroupInfo& groupinfo);

bool upgroupinfo(TDBManager* pManager, string gid, string sqlappend);

int getgroupinfo(TDBManager* pManager, GroupInfo& group, string gid, string sqlappend = "");

int getgrouplist(TDBManager* pManager, vector<GroupInfo>& groups, string uid, string sqlappend = "");

class TXMLElem;
class TLogFile;
class TUpStream;

class TBoddyUpdate
{
public:

    TBoddyUpdate();

    ~TBoddyUpdate();

    void OnRun();

    void Process();

protected:

    virtual int GetPacklen(const char* inputData,int length,int& startPos);

private:

    bool m_istart;

    TLogFile* m_log;

    TDBManager** m_pUserDB;

    TDBManager** m_wbbnewdbmgrs;

    kernel::CTThread* m_thread;
};

class TPGModule : public TBaseModule
{
public:

    string m_iqproto;

    TPGModule();

    virtual ~TPGModule();

    virtual void OnInit();

    virtual void OnRun();

    virtual void ProcessTimer(TModuleMsg* pMsg);

    virtual void ProcessHttpMsg(TModuleMsg* pMsg);

    void ProcessIQ(int workindex, const char* addrinfo, const char* userid, string msgid, const char* iqbody, const char* type);

private:

    void SendResponse200(TModuleMsg* pMsg, string& respbody);

    void SendResponse200(TModuleMsg* pMsg, const char* respbody = NULL);
    void SendResponse200(TModuleMsg* pMsg,const char* respbody,int resbody_length,int bgzip);

    void SendResponse403(TModuleMsg* pMsg, int warnid, const char* warninfo, const char* app = NULL);

    void SendResponse404(TModuleMsg* pMsg, int warnid, const char* warninfo);

private:

    void ProcessPubGroup(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessPubGroupCreate(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessPubGroupInvite(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessPubGroupQuit(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessPubGroupModify(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessPubGroupInfo(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessPubGroupCardModify(TXMLElem* xmlelem, TModuleMsg* pMsg);

private:

    void ProcessContact(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessContactInfo(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessContactAppend(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessContactDelete(TXMLElem* xmlelem, TModuleMsg* pMsg);

    void ProcessGetOfflinemsg(TXMLElem* xmlelem, TModuleMsg* pMsg);
    
    void ProcessPutOfflinemsgACK(TXMLElem* xmlelem, TModuleMsg* pMsg);
     /**
      search contact 
    */    
    void ProcessContactSearch(TXMLElem* xmlelem, TModuleMsg* pMsg);

    bool GetSaveAndShare(TDBManager* dbmgr, const string& from, const string& to, int& issave, int& isshare);

private:

    int m_gmaxcount;

    string m_upaddr;
        
    string m_version;

    string m_moduleaddr;

    string m_groupdelim;

    TLogFile* m_pglogfile;

    TDBManager** m_pUserDB;

    TDBManager** m_syncdbmgrs;

    TBoddyUpdate m_boddyupdate;

    vector<TUpStream*> m_upstreams;
};

#endif //_PG_MODULE_H_
