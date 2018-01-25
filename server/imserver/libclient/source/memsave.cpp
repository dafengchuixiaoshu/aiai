#include "memsave.h"

const int mem_save_version = 0;

extern string get_cur_time(int type = 0);

message::message()
{
	type = 0;
	kind = 0;
	direction = 0;
	unread = 0;
	played = 0;
	send = 0;
	category = 0;
	online = 1;
	create_time = get_cur_time();
}

contact::contact()
{
	create_time = get_cur_time();
}

relation::relation()
:relation_type(-1)
{
	create_time = get_cur_time();
}

CMemSave::CMemSave()
: m_isok(false)
{

}

CMemSave::~CMemSave()
{

}

void CMemSave::CleanMemSave()
{
	if(m_isok)
	{

	}
}

bool CMemSave::OpenMemSave(const string usersavefile)
{
	CleanMemSave();

	m_usersavefile = usersavefile + "memsave";

	m_isok = true;

	//mem_save_version
	LOGD ("%s_%d %s %s usersavefile:%s\n\n",
			__FILE__, __LINE__, __FUNCTION__, m_isok ? "success" : "failed", usersavefile.c_str());
}

string CMemSave::GetUpdateTime(enum UpdateTimeType type)
{
	if(m_isok)
	{
		if(UTT_BUDDY == type)
		{
			return m_updatetime.buddys_updatetime;
		}
		else if(UTT_GROUP == type)
		{
			return m_updatetime.group_updatetime;
		}
	}

	return "";
}

int CMemSave::SetUpdateTime(enum UpdateTimeType type, const string& strUpdateTime)
{
	if(UTT_BUDDY == type)
	{
		m_updatetime.buddys_updatetime = strUpdateTime;
	}
	else if(UTT_GROUP == type)
	{
		m_updatetime.group_updatetime = strUpdateTime;
	}

	if(m_isok)
	{

	}

	return 0;
}

int CMemSave::GetGroupInfo(group& info)
{
	if(m_isok)
	{
		group_id_map::iterator it = m_groupidmap.find(info.group_id);

		if(m_groupidmap.end() != it)
		{
			info = *(it->second);
		}
	}

	return 0;
}

int CMemSave::GetGroupList(vector<group>& infolist)
{
	if(m_isok)
	{
		for(group_time_map::iterator it = m_grouptimemap.begin(); it != m_grouptimemap.end(); ++it)
		{
			group info;

			info = *(it->second);

			infolist.push_back(info);
		}

		return infolist.size();
	}

	return 0;
}

int CMemSave::IsGroupExist(const string& groupid)
{
	if(m_isok)
	{
		group_id_map::iterator it = m_groupidmap.find(groupid);

		if(m_groupidmap.end() != it)
		{
			return 1;
		}
	}

	return 0;
}

void CMemSave::UpdateGroupTempName(const string& groupid)
{
	if(m_isok)
	{
		group_id_map::iterator it = m_groupidmap.find(groupid);

		if(m_groupidmap.end() != it)
		{
			it->second->group_id = groupid;
		}
	}
}

bool CMemSave::InsertGroupInfo(group& info)
{
	if(m_isok)
	{
		group* newgroup = new group();

		if(NULL != newgroup)
		{
			*newgroup = info;
			m_groupidmap[info.group_id] = newgroup;
			m_grouptimemap[info.cur_time] = newgroup;

			return true;
		}
	}

	return false;
}

void CMemSave::UpdateGroupInfoForInvite(const string& groupid, string key, string value)
{
	if(m_isok)
	{
		group_id_map::iterator it = m_groupidmap.find(groupid);

		if(m_groupidmap.end() != it)
		{
			if(key.empty())
			{
				group_time_map::iterator ittime = m_grouptimemap.find(it->second->cur_time);

				if(m_grouptimemap.end() == ittime)
				{

				}
				else
				{
					delete it->second;
					m_groupidmap.erase(it);
					m_grouptimemap.erase(ittime);
				}
			}
			else if("portraitKey" == key)
			{
				it->second->portraitKey = value;
			}
			else if("group_name" == key)
			{
				it->second->group_name = value;
			}
			else if("member_max_count" == key)
			{
				it->second->member_max_count = value;
			}
			else
			{

			}
		}
	}
}

bool CMemSave::InsertMessage(message& msg)
{
	if(m_isok)
	{
		m_messagemap[msg.msg_id] = msg;

		return true;
	}

	return false;
}

void CMemSave::UpdateMessage(const string& msgid, string key, string value)
{
	message_map::iterator it = m_messagemap.find(msgid);

	if(it != m_messagemap.end())
	{
		if("send" == key)
		{
			it->second.send = atoi(value.c_str());
		}
		else if("server_time" == key)
		{
			it->second.server_time = value;
		}
		else
		{

		}
	}
}

bool CMemSave::InsertSession(session& ssn)
{
	if(m_isok)
	{
		m_sessionmap[ssn.msg_id] = ssn;

		return true;
	}

	return false;
}

void CMemSave::UpdateSession(const string& ssnid, string key, string value)
{
	session_map::iterator it = m_sessionmap.find(ssnid);

	if(it != m_sessionmap.end())
	{
		if("update_time" == key)
		{
			it->second.update_time = value;
		}
		else
		{

		}
	}
}

int CMemSave::GetGlobal(string key)
{
	int ret = 0;

	global_map::iterator it = m_globalmap.find(key);

	if(it != m_globalmap.end())
	{
		if("newfans" == key)
		{
			ret = it->second.values.size();
		}
		else
		{
			ret = it->second.value;
		}
	}

	return ret;
}

void CMemSave::UpdateGlobal(string key, int value)
{

}

void CMemSave::UpdateGlobal(string key, string value, bool isapp)
{
	global_map::iterator it = m_globalmap.find(key);

	if(it == m_globalmap.end())
	{
		static global temp;

		m_globalmap[key] = temp;
	}

	if("newfans" == key)
	{
		if(value.empty())
		{
			m_globalmap[key].values.clear();
		}
		else if(isapp)
		{
			m_globalmap[key].values.insert(value);
		}
		else
		{
			set<string>::iterator it = m_globalmap[key].values.find(value);

			if(it != m_globalmap[key].values.end())
			{
				m_globalmap[key].values.erase(it);
			}
		}
	}
}

bool CMemSave::InsertContact(contact& info)
{

}

bool CMemSave::UpdataContact(string userid, string key, string value)
{

}

int CMemSave::IsContactExist(const string& userid)
{

}

bool CMemSave::GetContact(contact& info, string userid)
{
	return true;
}

