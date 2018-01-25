#ifndef _BASE_PARAM_H
#define _BASE_PARAM_H

#include "im.pb.h"

class TString;
class TXMLElem;

class TBaseParam
{
public:

    int m_msgtype;

    string m_prototype;

    TBaseParam(string prototype = "XML");

    virtual ~TBaseParam(){}

    virtual bool ToBuffer(TString& outstr){ return false; }

    virtual bool ToBlock(TXMLElem* xmlelem){ return true; }

    virtual bool ToBlock(const im::im& inbuf){ return true; }
};

bool parse_recvmsg(TBaseParam* &baseparam, im::im& inbuf);

bool parse_recvmsg(TBaseParam* &baseparam, TXMLElem* xmlelem);

enum ImType
{
    IM_ROOT = 0,
    IM_ACK,
    IM_CHAT,
    IM_CLOSE,
    IM_LOGIN,
    IM_LOGOUT,
    IM_KICKOUT,
    IM_UPSTREAM,
    IM_KEEPALIVE,

    IM_PGCREATE,
    IM_PGINVITE,
    IM_PGQUIT,
    IM_PGMODIFY,
    IM_PGCARD,

    IM_SNS,

    IM_FANSMODIFY,
    IM_FANSAPPEND,
    IM_FANSDELETE,

    IM_MAPPOINT,

    IM_DELETE,

    IM_BLACKLIST
};

class TImAck : public TBaseParam
{
public:

    TImAck(string prototype = "XML");

    TImAck(string& id, string time, string prototype = "XML");

    TImAck(string& id, string time, string& name, string& addr, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_id;

    string m_addr;

    string m_time;

    string m_name;
};

class TImChat : public TBaseParam
{
public:

    TImChat(string prototype = "XML");

    TImChat(string& id, string addr = "", string prototype = "XML");

    TImChat(string& id, string& from, string& to, string& type, string& category, string recvtime, string& body, int issave, int isshare, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_id;

    string m_to;

    string m_addr;

    string m_body;

    string m_from;

    string m_type;

    string m_time;

    string m_category;

    int m_issave;

    int m_isshare;
};

class TImClose: public TBaseParam
{
public:

    TImClose(string prototype = "XML");

    TImClose(string& user, string addr, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_user;

    string m_addr;
};

class TImLogin : public TBaseParam
{
public:

    TImLogin(string prototype = "XML");

    TImLogin(int status, string prototype = "XML");

    TImLogin(string& from, string& domain, string& resource, string& platfrom, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    int m_status;

    string m_from;

    string m_domain;

    string m_resource;

    string m_platform;
};

class TImLogout : public TBaseParam
{
public:

    TImLogout(string prototype = "XML");

    TImLogout(int status, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    int m_status;
};

class TImKickout : public TBaseParam
{
public:

    TImKickout(string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);
};

class TImKeepAlive: public TBaseParam
{
public:

    TImKeepAlive(string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);
};

class TImUpStream : public TBaseParam
{
public:

    TImUpStream(string prototype = "XML");

    TImUpStream(string& from, string& domain, string& resource, string& platfrom, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_from;

    string m_domain;

    string m_resource;

    string m_platform;
};

class TImPGCreate : public TBaseParam
{
public:

    TImPGCreate(string prototype = "XML");

    TImPGCreate(string& id, string& gid, string& name, int type, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    int m_type;

    string m_id;

    string m_gid;

    string m_time;

    string m_name;
};

class TImPGInvite: public TBaseParam
{
public:

    TImPGInvite(string prototype = "XML");

    TImPGInvite(string& id, string& gid, string& name, int type, string& invites, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    int m_type;

    string m_id;

    string m_gid;

    string m_time;

    string m_name;

    string m_invites;
};

class TImPGQuit : public TBaseParam
{
public:

    TImPGQuit(string prototype = "XML");

    TImPGQuit(string& id, string& gid, string& uid, string& name, int type, string& gkey, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    int m_type;

    string m_id;

    string m_uid;

    string m_gid;

    string m_gkey;

    string m_time;

    string m_name;
};

class TImPGModify : public TBaseParam
{
public:

    TImPGModify(string prototype = "XML");

    TImPGModify(string& id, string& gid, string& name, int type, string& gname, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    int m_type;

    string m_id;

    string m_gid;

    string m_time;

    string m_name;

    string m_gname;
};

class TImPGCard : public TBaseParam
{
public:

    TImPGCard(string prototype = "XML");

    TImPGCard(string& id, string& gid, string& uid, string& cardname, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_id;

    string m_uid;

    string m_gid;

    string m_time;

    string m_cardname;
};

class TImSns: public TBaseParam
{
public:

    TImSns(string prototype = "XML");

    TImSns(string& id, string& subtype, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_id;

    string m_subtype;
};

class TImFansModify: public TBaseParam
{
public:

    TImFansModify(string prototype = "XML");

    TImFansModify(string& uid, string& name, string& key, string& info, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_id;

    string m_uid;

    string m_name;

    string m_key;

    string m_info;
};

class TImFansAppend : public TBaseParam
{
public:

    TImFansAppend(string prototype = "XML");

    TImFansAppend(string& id, string& subtype, string& uid, string& name, string& key, string& info, string& cmsid, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_id;

    string m_uid;

    string m_key;

    string m_subtype;

    string m_name;

    string m_info;

    string m_cmsid;

    string m_time;
};

class TImFansDelete: public TBaseParam
{
public:

    TImFansDelete(string prototype = "XML");

    TImFansDelete(string& id, string& uid, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_id;

    string m_uid;

    string m_time;
};

class TImMapPoint: public TBaseParam
{
public:

    TImMapPoint(string prototype = "XML");

    virtual bool ToBlock(TXMLElem* xmlelem);

    virtual bool ToBlock(const im::im& inbuf);

    string m_xpoint;

    string m_ypoint;
};

class TImDelete: public TBaseParam
{
public:

    TImDelete(string prototype = "XML");

    virtual bool ToBlock(const im::im& inbuf);

    string m_msgid;

    string m_imagekey;

    string m_videokey;
};

class TImBlackList: public TBaseParam
{
public:

    TImBlackList(string prototype = "XML");

    TImBlackList(string& blackid, int type, string prototype = "XML");

    virtual bool ToBuffer(TString& outstr);

    virtual bool ToBlock(const im::im& inbuf);

    int m_type;

    string m_blackid;
};

#endif //_BASE_PARAM_H
