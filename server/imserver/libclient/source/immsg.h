#ifndef _LIBCLIENT_IMMSG_H_
#define _LIBCLIENT_IMMSG_H_

#include "im.pb.h"
#include "protoparse.h"

string create_coreid();

string get_cur_time(int type = 0);

int file_read(char* &outbuf, const char* filename);

int file_write(const char* filename, const char* inbuf, int inlen);

enum
{
    IM_ROOT = 0,
    IM_ACK,
    IM_CHAT,
    IM_LOGIN,
    IM_LOGOUT,
    IM_KICKOUT,
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

    IM_MESSAGE,
    IM_MAPPOINT,

    IM_DELETE,

    IM_BLACKLIST
};

class CIm
{
public:

    CIm();

    virtual ~CIm(){}

    string GetMsgId() { return m_id; }

    virtual bool ToBlock(const im::im& inbuf){ return true; }

    virtual bool ToBlock(CProtoByte* protobyte){ return true; }

    virtual bool ToBuffer(TString& outstr, string& prototype){ return false; }

    int m_imtype;

    string m_id;
};

class CImAck : public CIm
{
public:

    CImAck();

    CImAck(string& id);

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    virtual bool ToBuffer(TString& outstr, string& prototype);

    bool ToLanchatBuffer(TString& outstr, string& prototype);
    string m_time;
};

class CImChat : public CIm
{
	void Copy(const CImChat &r)
	{
		m_id = r.m_id;
        m_imtype = r.m_imtype;

        m_to = r.m_to;
        m_body = r.m_body;
        m_from = r.m_from;
        m_type = r.m_type;
        m_time = r.m_time;
        m_category = r.m_category;

        m_issave = r.m_issave;
        m_isshare = r.m_isshare;
	}

public:

    CImChat();

    CImChat(const CImChat &r){ Copy(r); }

    CImChat& operator= (const CImChat& r){ if(this != &r) Copy(r); return *this; }

    CImChat(string& from, string& to, string& type, string& category, string& body);

    CImChat(string& from, string& to, string& type, string& category, string& body, string& id);

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    virtual bool ToBuffer(TString& outstr, string& prototype);

    string m_to;

    string m_body;

    string m_from;

    string m_type;

    string m_time;

    string m_category;

    string m_subname;

    int m_issave;

    int m_isshare;
};

class CImLogin : public CIm
{
public:

    CImLogin();

    CImLogin(string& from, string& domain, string& resource, string& platfrom);

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    virtual bool ToBuffer(TString& outstr, string& prototype);

    int m_status;

    string m_from;

    string m_domain;

    string m_resource;

    string m_platform;
};

class CImLogout : public CIm
{
public:

    CImLogout();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    virtual bool ToBuffer(TString& outstr, string& prototype);

    int m_status;
};

class CImKickout : public CIm
{
public:

    CImKickout();
};

class CImKeepAlive: public CIm
{
public:

    CImKeepAlive();

    virtual bool ToBuffer(TString& outstr, string& prototype);
};

class CImPGCreate : public CIm
{
public:

    CImPGCreate();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    int m_type;

    string m_gid;

    string m_time;

    string m_name;
};

class CImPGInvite : public CIm
{
public:

    CImPGInvite();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    int m_type;

    string m_gid;

    string m_time;

    string m_name;

    string m_invites;
};

class CImPGQuit : public CIm
{
public:

    CImPGQuit();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    int m_type;

    string m_uid;

    string m_gid;

    string m_gkey;
    
    string m_time;

    string m_name;
};

class CImPGModify : public CIm
{
public:

    CImPGModify();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    int m_type;

    string m_gid;

    string m_gname;
    
    string m_time;

    string m_name;
};

class CImPGCard: public CIm
{
public:

    CImPGCard();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    string m_uid;

    string m_gid;
    
    string m_time;

    string m_cardname;
};

class CImSns: public CIm
{
public:

    CImSns();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    string m_subtype;
};

class CImFansModify: public CIm
{
public:

    CImFansModify();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    string m_uid;

    string m_name;

    string m_key;

    string m_info;
};

class CImFansAppend: public CIm
{
public:

    CImFansAppend();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    string m_uid;

    string m_key;

    string m_subtype;

    string m_name;

    string m_info;

    string m_cmsid;

    string m_time;
};

class CImFansDelete: public CIm
{
public:

    CImFansDelete();

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    string m_uid;

    string m_time;
};

bool file_decode(string& filename, string& savepath, string& file);

class CImMessage : public CIm
{
	void Copy(const CImMessage &r)
	{
		m_id = r.m_id;
        m_imtype = r.m_imtype;

        //common
        m_touser = r.m_touser;
        m_msgtype = r.m_msgtype;
        m_fromuser = r.m_fromuser;
        m_imagekey = r.m_imagekey;
        m_category = r.m_category;
        m_txtbuffer = r.m_txtbuffer;

        //omage
        m_imagewidth = r.m_imagewidth;
        m_imageheight = r.m_imageheight;

        //video
        m_videokey = r.m_videokey;
        m_videotitle = r.m_videotitle;
        m_videolength = r.m_videolength;

        m_filekey = r.m_filekey;
	}

public:

	CImMessage();

	CImMessage(string& filekey);

	CImMessage(const CImMessage &r){ Copy(r); }

	CImMessage(string& fromuser, string& touser, string& imagepath,
			string& txtbuffer, int imagewidth, int imageheight, string& msgtype);

	CImMessage(string& fromuser, string& touser, string& videopath,
			string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, string& msgtype);

	CImMessage& operator= (const CImMessage& r){ if(this != &r) Copy(r); return *this; }

    bool ToLanchatBuffer(TString& outstr, string& prototype);

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBlock(CProtoByte* protobyte);

    //common
    string m_touser;
    string m_msgtype;
    string m_fromuser;
    string m_category;
    string m_imagekey;
    string m_txtbuffer;

    //image
    string m_imagewidth;
    string m_imageheight;

    //video
    string m_videolength;
    string m_videokey;
    string m_videotitle;

    //delete file
    string m_filekey;
};

class CImMapPoint : public CIm
{
public:

	CImMapPoint();

	CImMapPoint(string& xpoint, string& ypoint);

    virtual bool ToBuffer(TString& outstr, string& prototype);

    string m_xpoint;

    string m_ypoint;
};

class CImDelete: public CIm
{
public:

	CImDelete();

	CImDelete(string& msgid, string& imagekey, string& videokey);

    virtual bool ToBuffer(TString& outstr, string& prototype);

    string m_msgid;

    string m_imagekey;

    string m_videokey;
};

class CImBlackList: public CIm
{
public:

	CImBlackList();

	CImBlackList(string& blackid, int type);

    virtual bool ToBlock(const im::im& inbuf);

    virtual bool ToBuffer(TString& outstr, string& prototype);

    int m_type;

    string m_blackid;
};

#endif //_LIBCLIENT_IMMSG_H_
