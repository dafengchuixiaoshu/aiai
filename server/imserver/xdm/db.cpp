//数据库公共函数
#ifdef WIN32
#include <afx.h>
#include <Winsock2.h>
#define snprintf _snprintf
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "xml.h"
#include "Log.h"
#include "DBManager.h"
#include "tcpsocket.h"
#include "httpmsg.h"
#include "xdm.h"

#define DB_ERR_LOG(...) LOG_ERROR(pglogfile, __VA_ARGS__)

bool getsqltime(TDBManager* pManager, string& ver)
{
    TMySQLParam sqlparam;
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    static const char sqltxt[] = "SELECT sysdate(6)";

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt))
    {
        ver = sqlparam.GetStrValue(0, 0);

        return true;
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);

        return false;
    }

}

int appendcontact(TDBManager* pManager, ContactInfo& frominfo, ContactInfo& toinfo)
{
    TMySQLParam sqlparam;
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 };

    static const char fmt[] = "INSERT INTO users_friend(uid,friend_id) VALUES(%s,%s)";

    sprintf(sqltxt, fmt, frominfo._id.c_str(), toinfo._id.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        TMySQLParam sqlparam;

        char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO users_friend_update(uid,friend_id,change_time,op_type) VALUES(%s,%s,sysdate(6),0)";

        sprintf(sqltxt, fmt, frominfo._id.c_str(), toinfo._id.c_str());

        return sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile);
    }

    return -1;
}

int deletecontact(TDBManager* pManager, ContactInfo& frominfo, ContactInfo& toinfo)
{
    TMySQLParam sqlparam;

    char sqltxt[1024] = { 0 };
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    static const char fmt[] = "DELETE FROM users_friend WHERE uid=%s AND friend_id=%s";

    sprintf(sqltxt, fmt, frominfo._id.c_str(), toinfo._id.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        TMySQLParam sqlparam;

        char sqltxt[1024] = { 0 };

        static const char fmt[] = "REPLACE INTO users_friend_update(uid,friend_id,change_time,op_type) VALUES(%s,%s,sysdate(6),1)";

        sprintf(sqltxt, fmt, frominfo._id.c_str(), toinfo._id.c_str());

        return sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile);
    }

    return -1;
}

void getmembername(TDBManager* pManager, string& cardname, string gid, string uid)
{
    TMySQLParam sqlparam;

    char sqltxt[1024] = { 0 };

    static TLogFile* dblogfile = GET_LOG(db);

    static TLogFile* pglogfile = GET_LOG(pg);

    static const char fmt[] = "SELECT cardname FROM im_pub_groupuser WHERE gid='%s' AND uid='%s' AND status=1";

    sprintf(sqltxt, fmt, gid.c_str(), uid.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        if(1 == sqlparam.GetRecordCount())
        {
            cardname = sqlparam.GetStrValue(0, 0);
        }
    }
}

/**
获取用户信息
*/
int getcontactinfo(TDBManager* pUserManager, TDBManager* pImpresaManager, string dbkey, ContactInfo& contactinfo, string uid)
{
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

    static const char fmt[] ="SELECT id,userName,nickName,email,face_url,info,mobile,face_vedio FROM users WHERE %s=%s%s%s AND status=1";

    sprintf(sqltxt, fmt,
        ("id" == dbkey) ? dbkey.c_str() : "userName",
        ("id" == dbkey) ? "" : "'",
        ("id" == dbkey) ? contactinfo._id.c_str() : contactinfo._loginame.c_str(),
        ("id" == dbkey) ? "" : "'");

    if(0 != sync_exec_sql(pImpresaManager, &sqlparam, sqltxt, dblogfile))
    {
        if(1 == sqlparam.GetRecordCount())
        {
            contactinfo._id = sqlparam.GetStrValue(0, 0);
            contactinfo._loginame = sqlparam.GetStrValue(0, 1);
            contactinfo._nickname = sqlparam.GetStrValue(0, 2);
            contactinfo._email = sqlparam.GetStrValue(0, 3);
            contactinfo._portraitcrc = sqlparam.GetStrValue(0, 4);
            contactinfo._impresa = sqlparam.GetStrValue(0, 5);
            contactinfo._mobile = sqlparam.GetStrValue(0, 6);
            contactinfo._facevideo = sqlparam.GetStrValue(0, 7);

            if(contactinfo._nickname.empty())
            {
                contactinfo._nickname = contactinfo._loginame;
            }

            if(!uid.empty())
            {
                TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                static const char fmt[] = "SELECT issave,isshare,subname FROM users_friend WHERE uid=%s AND friend_id=%s";

                sprintf(sqltxt, fmt, uid.c_str(), contactinfo._id.c_str());

                if(0 != sync_exec_sql(pImpresaManager, &sqlparam, sqltxt, dblogfile))
                {
                    if(sqlparam.GetRecordCount() > 0)
                    {
                        contactinfo._issave = sqlparam.GetStrValue(0, 0);
                        contactinfo._isshare = sqlparam.GetStrValue(0, 1);
                        contactinfo._subname = sqlparam.GetStrValue(0, 2);         
                    }
                }
            }

            return 1;
        }
    }

    return -1;
}

int getcontactlist(TDBManager* pUserManager, TDBManager* pImpresaManager, string uid, string ver, vector<ContactInfo>& contactinfolist, string type, bool isbuddy)
{
    int contactcount = 0;
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);
    ContactInfo myself; myself._loginame = uid;
    if(1 == getcontactinfo(pUserManager, pImpresaManager, "name", myself, ""))
    {
        if("uship" == type)
        {
            TMySQLParam sqlparam;

            char sqltxt[1024] = { 0 };

            if(isbuddy)
                sprintf(sqltxt, "SELECT friend_id,op_type FROM users_friend_update WHERE uid='%s' AND change_time>'%s'", myself._id.c_str(), ver.c_str());
            else
                sprintf(sqltxt, "SELECT uid,op_type FROM users_friend_update WHERE friend_id='%s' AND change_time>'%s'", myself._id.c_str(), ver.c_str());

            if(0 != sync_exec_sql(pImpresaManager, &sqlparam, sqltxt, dblogfile))
            {
                int recordcount = sqlparam.GetRecordCount();

                for(int index = 0; index < recordcount; ++index)
                {
                    ContactInfo contactinfo;

                    contactinfo._id = sqlparam.GetStrValue(index, 0);

                    contactinfo._optype = sqlparam.GetIntValue(index, 1);

                    contactinfolist.push_back(contactinfo);
                }
            }
            else
            {
                DB_ERR_LOG("err sqltxt: %s", sqltxt);
            }
        }
        else 
        {
            TMySQLParam sqlparam;

            char sqltxt[1024] = { 0 };

            if(isbuddy)
                sprintf(sqltxt, "SELECT friend_id FROM users_friend WHERE uid='%s' GROUP BY friend_id", myself._id.c_str());
            else
                sprintf(sqltxt, "SELECT uid FROM users_friend WHERE friend_id='%s' GROUP BY uid", myself._id.c_str());

            if(0 != sync_exec_sql(pImpresaManager, &sqlparam, sqltxt, dblogfile))
            {
                int recordcount = sqlparam.GetRecordCount();

                for(int index = 0; index < recordcount; ++index)
                {
                    ContactInfo contactinfo;

                    contactinfo._id = sqlparam.GetStrValue(index, 0);

                    if(!ver.empty())
                    {
                        TMySQLParam sqlparam;

                        char sqltxt[1024] = { 0 };

                        sprintf(sqltxt, "SELECT userid FROM im_user WHERE userid='%s' AND last_updatetime>'%s'", contactinfo._id.c_str(), ver.c_str());

                        if(0 != sync_exec_sql(pUserManager, &sqlparam, sqltxt, dblogfile))
                        {
                            if(0 == sqlparam.GetRecordCount())
                            {
                                continue;
                            }
                        }
                        else
                        {
                            DB_ERR_LOG("err sqltxt: %s", sqltxt);
                        }
                    }

                    contactinfolist.push_back(contactinfo);
                }
            }
            else
            {
                DB_ERR_LOG("err sqltxt: %s", sqltxt);
            }
        }

        contactcount = contactinfolist.size();

        for(int index = 0; index < contactcount; ++index)
        {
            getcontactinfo(pUserManager, pImpresaManager, "id", contactinfolist[index], myself._id);
        }
    }

    return contactcount;
}

static void get_contact_update(TDBManager* pUserManager, TDBManager* pImpresaManager, ContactUpdate& contactupdate, bool isbuddy)
{
    TMySQLParam sqlparam;

    char sqltxt[1024] = { 0 };

    static TLogFile* dblogfile = GET_LOG(db);

    static TLogFile* pglogfile = GET_LOG(pg);

    if(isbuddy)
        sprintf(sqltxt, "SELECT uid FROM users_friend WHERE friend_id='%s' GROUP BY uid", contactupdate._myselfinfo._id.c_str());
    else
        sprintf(sqltxt, "SELECT friend_id FROM users_friend WHERE uid='%s' GROUP BY friend_id", contactupdate._myselfinfo._id.c_str());

    if(0 != sync_exec_sql(pImpresaManager, &sqlparam, sqltxt, dblogfile))
    {
        int recordcount = sqlparam.GetRecordCount();

        for(int index = 0; index < recordcount; ++index)
        {
            ContactInfo contactinfo;

            contactinfo._id = sqlparam.GetStrValue(index, 0);

            if(1 == getcontactinfo(pUserManager, pImpresaManager, "id", contactinfo, contactupdate._myselfinfo._id))
            {
                contactupdate._contactinfos.push_back(contactinfo);
            }
        }
    }
}

void getcontactupdate(TDBManager* pUserManager, TDBManager* pImpresaManager, string ver, vector<ContactUpdate>& contactupdates, bool isinfo)
{
    int result;

    TMySQLParam sqlparam;

    char sqltxt[1024] = { 0 };

    static TLogFile* dblogfile = GET_LOG(db);

    static TLogFile* pglogfile = GET_LOG(pg);

    static const char cfmt[] = "SELECT last_updatetime,userid FROM im_user WHERE last_updatetime>'%s' GROUP BY userid ORDER BY last_updatetime DESC";

    static const char ifmt[] = "SELECT last_updatetime,userid FROM im_user WHERE last_updatetime>'%s' GROUP BY userid ORDER BY last_updatetime DESC";

    if(isinfo)
    {
        sprintf(sqltxt, ifmt, ver.c_str());

        result = sync_exec_sql(pImpresaManager, &sqlparam, sqltxt);
    }
    else
    {
        sprintf(sqltxt, cfmt, ver.c_str());

        result = sync_exec_sql(pUserManager, &sqlparam, sqltxt);
    }

    if(0 != result)
    {
        int recordcount = sqlparam.GetRecordCount();

        for(int index = 0; index < recordcount; ++index)
        {
            ContactUpdate contactupdate;

            contactupdate._time = sqlparam.GetStrValue(index, 0);

            contactupdate._myselfinfo._id = sqlparam.GetStrValue(index, 1);

            if(1 == getcontactinfo(pUserManager, pImpresaManager, "id", contactupdate._myselfinfo, ""))
            {
                get_contact_update(pUserManager, pImpresaManager, contactupdate, true);

                get_contact_update(pUserManager, pImpresaManager, contactupdate, false);
            }

            contactupdates.push_back(contactupdate);
        }
    }
}

bool ismember(TDBManager* pManager, string gid, string uid)
{
    TMySQLParam sqlparam;

    static const char fmt[] = "SELECT status FROM im_pub_groupuser WHERE gid='%s' AND uid='%s' AND status=1";
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, gid.c_str(), uid.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        if(1 == sqlparam.GetRecordCount())
        {
            return true;
        }
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);
    }

    return false;
}

bool insertmember(TDBManager* pManager, string gid, string uid, int type)
{
    TMySQLParam sqlparam;

    static const char fmt[] = "DELETE FROM im_pub_groupuser WHERE gid='%s' AND uid='%s'";
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, gid.c_str(), uid.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        TMySQLParam sqlparam; 

        static const char fmt[] = "INSERT INTO im_pub_groupuser(gid,uid,type,status,createtime,modifytime) VALUES('%s','%s',%d,1,sysdate(6),sysdate(6))";

        char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, gid.c_str(), uid.c_str(), type);

        if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
        {
            return true;
        }
        else
        {
            DB_ERR_LOG("err sqltxt: %s", sqltxt);
        }
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);
    }

    return false;
}

bool deletemember(TDBManager* pManager, string gid, string gkey, string uid)
{
    TMySQLParam sqlparam;
    static const char fmt[] = "UPDATE im_pub_group SET gkey='%s',modifytime=sysdate(6) WHERE gid='%s'";
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, gkey.c_str(), gid.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        TMySQLParam sqlparam;

        static const char fmt[] = "UPDATE im_pub_groupuser SET status=0,modifytime=sysdate(6) WHERE gid='%s' AND uid='%s'";

        char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, gid.c_str(), uid.c_str());

        if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
        {
            return true;
        }
        else
        {
            DB_ERR_LOG("err sqltxt: %s", sqltxt);
        }
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);
    }

    return false;
}

bool upmemberinfo(TDBManager* pManager, string gid, string uid, string sqlappend)
{
    TMySQLParam sqlparam;

    static const char fmt[] = "UPDATE im_pub_groupuser SET %s WHERE gid='%s' AND uid='%s'";
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, sqlappend.c_str(), gid.c_str(), uid.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        return true;
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);

        return false;
    }
}

/**
获取群成员信息
*/
int getmemberlist(TDBManager* pUserManager, TDBManager* pImpresaManager, vector<GroupMemberInfo>& members, string sqlappend)
{
    TMySQLParam sqlparam;

    static const char fmt[] = "SELECT gid,uid,status,cardname,createtime FROM im_pub_groupuser WHERE %s";
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, sqlappend.c_str());

    if(0 != sync_exec_sql(pImpresaManager, &sqlparam, sqltxt, dblogfile))
    {
        int recordcount = sqlparam.GetRecordCount();

        for(int index = 0; index < recordcount; ++index)
        {
            GroupMemberInfo member;
            member._gid = sqlparam.GetStrValue(index, 0);
            member._uid = sqlparam.GetStrValue(index, 1);
            member._status = sqlparam.GetStrValue(index, 2);
            member._cardname = sqlparam.GetStrValue(index, 3);
            member._createtime = sqlparam.GetStrValue(index, 4);
            member._contactinfo._loginame = member._uid;

            if(1 != getcontactinfo(pUserManager, pImpresaManager, "name", member._contactinfo, ""))
            {
                return -1;
            }

            members.push_back(member);
        }

        return recordcount;
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);
    }

    return -2;
}

bool insertgroup(TDBManager* pManager, GroupInfo& groupinfo)
{
    TMySQLParam sqlparam;

    static const char fmt[] = "INSERT INTO im_pub_group(gid,owner,gkey,gmaxcount,createtime,modifytime) VALUES('%s','%s','%s',%d,sysdate(6),sysdate(6))";
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, groupinfo._gid.c_str(), groupinfo._gcreator.c_str(), groupinfo._gportraitcrc.c_str(), groupinfo._gmaxcount);

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        return true;
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);

        return false;
    }
}

bool upgroupinfo(TDBManager* pManager, string gid, string sqlappend)
{
    TMySQLParam sqlparam;
    static const char fmt[] = "UPDATE im_pub_group SET %s WHERE gid='%s'";
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, sqlappend.c_str(), gid.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        return true;
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);

        return false;
    }
}

int getgrouplist(TDBManager* pManager, vector<GroupInfo>& groups, string uid, string sqlappend)
{
    static TLogFile* dblogfile = GET_LOG(db);

    static TLogFile* pglogfile = GET_LOG(pg);

    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

    static const char fmt[] = "SELECT gid,gname,owner,gmaxcount,gkey FROM im_pub_group WHERE owner='%s' %s";

    sprintf(sqltxt, fmt, uid.c_str(), sqlappend.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        int recordcount = sqlparam.GetRecordCount();

        for(int index = 0; index < recordcount; ++index)
        {
            GroupInfo group;

            group._gid = sqlparam.GetStrValue(0, 0);
            group._gname = sqlparam.GetStrValue(0, 1);
            group._gcreator = sqlparam.GetStrValue(0, 2);
            group._gmaxcount = sqlparam.GetIntValue(0, 3);
            group._gportraitcrc = sqlparam.GetStrValue(0, 4);

            groups.push_back(group);
        }

        return recordcount;
    }

    return -1;
}

/**
获取群信息
*/
int getgroupinfo(TDBManager* pManager, GroupInfo& group, string gid, string sqlappend)
{
    TMySQLParam sqlparam;

    static const char fmt[] = "SELECT gid,gname,owner,gmaxcount,gkey FROM im_pub_group WHERE gid='%s' %s";
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, gid.c_str(), sqlappend.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        if(1 == sqlparam.GetRecordCount())
        {
            group._gid = sqlparam.GetStrValue(0, 0);
            group._gname = sqlparam.GetStrValue(0, 1);
            group._gcreator = sqlparam.GetStrValue(0, 2);
            group._gmaxcount = sqlparam.GetIntValue(0, 3);
            group._gportraitcrc = sqlparam.GetStrValue(0, 4);

            return 1;
        }
        else
        {            
            return 0;
        }
    }
    else
    {
        DB_ERR_LOG("err sqltxt: %s", sqltxt);

        return -1;
    }
}
/**
获取离线消息
*/
bool get_save_and_share(TDBManager* dbmgr, const string& from, const string& to, int& issave, int& isshare)
{
    static TLogFile* dblogfile = GET_LOG(db);

    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

    static const char fmt[] ="SELECT id FROM users WHERE userName='%s' AND status=1";

    sprintf(sqltxt, fmt, from.c_str());

    if(0 != sync_exec_sql(dbmgr, &sqlparam, sqltxt, dblogfile))
    {
        if(1 == sqlparam.GetRecordCount())
        {
            string fromid = sqlparam.GetStrValue(0, 0);

            TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

            static const char fmt[] ="SELECT id FROM users WHERE userName='%s' AND status=1";

            sprintf(sqltxt, fmt, to.c_str());

            if(0 != sync_exec_sql(dbmgr, &sqlparam, sqltxt, dblogfile))
            {
                if(1 == sqlparam.GetRecordCount())
                {
                    string toid = sqlparam.GetStrValue(0, 0);

                    TMySQLParam sqlparam; char sqltxt[1024] = { 0 };

                    static const char fmt[] = "SELECT issave,isshare FROM users_friend WHERE uid=%s AND friend_id=%s";

                    sprintf(sqltxt, fmt, fromid.c_str(), toid.c_str());

                    if(0 != sync_exec_sql(dbmgr, &sqlparam, sqltxt, dblogfile))
                    {
                        if(sqlparam.GetRecordCount() > 0)
                        {
                            issave = sqlparam.GetIntValue(0, 0);
                            isshare = sqlparam.GetIntValue(0, 1);

                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

int getofflinemsg(TDBManager* pManager,vector<OfflineMSG>& offmsgs ,string uid)
{
    static TLogFile* dblogfile = GET_LOG(db);

    static TLogFile* pglogfile = GET_LOG(pg);

    static const char fmt[] = "SELECT id,fromuser,touser,togroup,type,category,body,recvtime FROM im_msg_chat WHERE status=0 AND touser='%s' AND type IN('chat', 'groupchat') ORDER BY recvtime";

    TMySQLParam sqlparam; char sqltxt[1024] = { 0 }; sprintf(sqltxt, fmt, uid.c_str());

    if(0 != sync_exec_sql(pManager, &sqlparam, sqltxt, dblogfile))
    {
        int recordcount = sqlparam.GetRecordCount();

        for(int index = 0; index < recordcount; ++index)
        {
            OfflineMSG offmsg;

            char txt[4096] = { 0 };

            string id = sqlparam.GetStrValue(index, 0);
            string fromuser = sqlparam.GetStrValue(index, 1);
            string touser = sqlparam.GetStrValue(index, 2);
            string togroup = sqlparam.GetStrValue(index, 3);
            string type = sqlparam.GetStrValue(index, 4);
            string category = sqlparam.GetStrValue(index, 5);
            string body = sqlparam.GetStrValue(index, 6);
            offmsg._recvtime = sqlparam.GetStrValue(index, 7);

            int issave = 0; int isshare = 0;

            get_save_and_share(pManager, fromuser, touser, issave, isshare);

            sprintf(txt, "<message msgid=\"%s\" from=\"%s\" to=\"%s\" type=\"%s\" category=\"%s\" save=\"%d\" share=\"%d\"><body>%s</body></message>",
                id.c_str(), fromuser.c_str(),
                ("groupchat" == type) ? togroup.c_str() : touser.c_str(),
                type.c_str(), category.c_str(), issave, isshare, body.c_str());

            offmsg._msg = txt;            

            offmsgs.push_back(offmsg);
        }

        return 1;
    }
    else
        DB_ERR_LOG("err sqltxt: %s", sqltxt);

    return -1;
}

//更新已读离线消息
int getofflinemsg(TDBManager* pManager,string sql)
{
    //string offmsgfmt ="UPDATE im_msg_chat SET status=1,sendtime=now() "+sql;
    string offmsgfmt ="DELETE FROM im_msg_chat WHERE "+sql;
    TMySQLParam sqlparam;
    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);

    if(0== sync_exec_sql(pManager, &sqlparam, offmsgfmt.c_str(), dblogfile)){
        DB_ERR_LOG("err sqltxt: %s", offmsgfmt.c_str());
        return -1;
    }
    return 1;

}

int SearchContact(TDBManager* pUserManager,TDBManager* pImpresaManager,string dbkey,vector<ContactInfo>& contactinfolist)
{
    static const char fmt[] = "SELECT userName,mobile,email FROM users WHERE (nickName like '%%%s%%' OR mobile='%s') AND status=1";

    static TLogFile* dblogfile = GET_LOG(db);
    static TLogFile* pglogfile = GET_LOG(pg);
    char sqltxt[1024] = { 0 }; 
    sprintf(sqltxt, fmt, dbkey.c_str(),dbkey.c_str(),dbkey.c_str());
    TMySQLParam base_usersqlparam;
    if(0 != sync_exec_sql(pUserManager, &base_usersqlparam, sqltxt, dblogfile))
    {
        int recordcount = base_usersqlparam.GetRecordCount();
        for(int index = 0; index < recordcount; ++index)
        {
            ContactInfo contactinfo;
            contactinfo._loginame = base_usersqlparam.GetStrValue(index,0);

            if(1 == getcontactinfo(pUserManager, pImpresaManager, "name", contactinfo, ""))
            {
                contactinfolist.push_back(contactinfo);  
            }
        }

        return contactinfolist.size();
    }

    return -1;
}
