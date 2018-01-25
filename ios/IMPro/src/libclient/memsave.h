#ifndef _LIBCLIENT_MEMSAVE_H_
#define _LIBCLIENT_MEMSAVE_H_

#include "header.h"

#ifdef NO_LANCHAT
#define LOGD printf
#else
#include "BizCommon.h"
#endif

struct updatetime
{
	string group_updatetime;

	string buddys_updatetime;
};

struct group
{
	group(){}
	group(const group &r){ Copy(r); }
	group& operator=(const group& r){ if(this != &r) Copy(r); return *this; }

	string cur_time;
	string group_id;
	string group_name;
	string portraitKey;
	string owner_id;
	string member_max_count;
	string group_tmp_name;

private:

    void Copy(const group& r)
	{
    	cur_time = r.cur_time;
		group_id = r.group_id;
		group_name = r.group_name;
		portraitKey = r.portraitKey;
		owner_id = r.owner_id;
		member_max_count = r.member_max_count;
		group_tmp_name = r.group_tmp_name;
	}
};

typedef map<string, group*> group_id_map;
typedef map<string, group*> group_time_map;

struct message
{
	message();//{ type = 0; kind = 0; direction = 0; unread = 0; played = 0; send = 0; category = 0; online = 1; create_time = get_cur_time(); }
	message(const message &r){ Copy(r); }
	message& operator=(const message& r){ if(this != &r) Copy(r); return *this; }

	string msg_id;
	string session_id;
	string sender;
	string receiver;
	int type;
	int kind;
	int direction;
	int unread;
	int send;
	int category;
	string content;
	string client_time;
	string server_time;
	string create_time;
	int online;
	int played;

private:

    void Copy(const message& r)
	{
    	msg_id = r.msg_id;
    	session_id = r.session_id;
    	sender = r.sender;
    	receiver = r.receiver;
    	type = r.type;
    	kind = r.kind;
    	direction = r.direction;
    	unread = r.unread;
    	send = r.send;
    	category = r.category;
    	content = r.content;
    	client_time = r.client_time;
    	server_time = r.server_time;
    	create_time = r.create_time;
    	online = r.online;
    	played = r.played;
	}
};

typedef map<string, message> message_map;

struct session
{
	session(){ type = 0; kind = 1; unread_num = 0; top = 0; remind = 1; draft_changed = 0; }
	session(const session &r){ Copy(r); }
	session& operator=(const session& r){ if(this != &r) Copy(r); return *this; }

	string session_id;
	string msg_id;
	int type;
	string update_time;
	int kind;
	int unread_num;
	int draft_changed;
	int top;
	int remind;
	string draft_content;

private:

    void Copy(const session& r)
	{
    	session_id = r.session_id;
    	msg_id = r.msg_id;
    	type = r.type;
    	update_time = r.update_time;
    	kind = r.kind;
    	unread_num = r.unread_num;
    	draft_changed = r.draft_changed;
    	top = r.top;
    	remind = r.remind;
    	draft_content = r.draft_content;
	}
};

typedef map<string, session> session_map;

struct contact
{
	contact();
	contact(const contact &r){ Copy(r); }
	contact& operator=(const contact& r){ if(this != &r) Copy(r); return *this; }

	string user_id;
	string user_name;
	string user_name_gbk;
	string portrait_key;
	string emotion_mood;
	string cms_id;
	string qrer_url;
	string community_url;
	string create_time;

private:

    void Copy(const contact& r)
	{
    	user_id = r.user_id;
    	user_name = r.user_name;
    	user_name_gbk = r.user_name_gbk;
    	portrait_key = r.portrait_key;
    	emotion_mood = r.emotion_mood;
    	cms_id = r.cms_id;
    	qrer_url = r.qrer_url;
    	community_url = r.community_url;
    	create_time = r.create_time;
	}
};

typedef map<string, contact*> contact_map;

struct relation
{
	relation();
	relation(const relation &r){ Copy(r); }
	relation& operator=(const relation& r){ if(this != &r) Copy(r); return *this; }

	string user_id;
	string relation_id;
	int relation_type;
	string group_cardname;
	string create_time;

private:

    void Copy(const relation& r)
	{
    	user_id = r.user_id;
    	relation_id = r.relation_id;
    	relation_type = r.relation_type;
    	group_cardname = r.group_cardname;
    	create_time = r.create_time;
	}
};

#include <set>
using std::set;

struct global
{
	global():value(0){}
	global(const global &r){ Copy(r); }
	global& operator=(const global& r){ if(this != &r) Copy(r); return *this; }

	int value;
	set<string> values;

private:

    void Copy(const global& r)
	{
    	value = r.value;
    	values = r.values;
	}
};

typedef map<string, global> global_map;

class CMemSave
{
	CMemSave();

	friend class CSingleton<CMemSave>;

public:

	~CMemSave();
	void CleanMemSave();
	bool OpenMemSave(const string usersavefile);

	//updatetime
	string GetUpdateTime(enum UpdateTimeType type);
	int SetUpdateTime(enum UpdateTimeType type, const string& strUpdateTime);

	//group
	int GetGroupInfo(group& info);
    int GetGroupList(vector<group>& infolist);
    int IsGroupExist(const string& groupid);
    void UpdateGroupTempName(const string& groupid);
    bool InsertGroupInfo(group& info);
    void UpdateGroupInfoForInvite(const string& groupid, string key, string value);

    bool InsertMessage(message& msg);
    void UpdateMessage(const string& msgid, string key, string value);

    bool InsertSession(session& ssn);
    void UpdateSession(const string& ssnid, string key, string value);

    int GetGlobal(string key);
    void UpdateGlobal(string key, int value);
    void UpdateGlobal(string key, string value, bool isapp);

    bool InsertContact(contact& info);
    bool UpdataContact(string userid, string key, string value);
    int IsContactExist(const string& userid);
    bool GetContact(contact& info, string userid);

private:

	bool m_isok;

	string m_usersavefile;

	global_map m_globalmap;

	updatetime m_updatetime;

	contact_map m_contactmap;

	message_map m_messagemap;

	session_map m_sessionmap;

	group_id_map m_groupidmap;

	group_time_map m_grouptimemap;
};

#define sMemSave CSingleton<CMemSave>::Instance()

#endif //_LIBCLIENT_MEMSAVE_H_
