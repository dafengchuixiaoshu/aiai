#ifndef _COMMON_H
#define _COMMON_H

#include "Defines.h"

#define USE_LOG
#if defined(USE_LOG)
#define USE_LOG_TO_CONSOLE
#define USE_LOG_TO_FILE
#define USE_CRASH_INFO
#include "CLog.h"
#include "NativeCrashHandler.h"
#define LOGP if (g_bOpenLog) \
                 CLog::Logp
#define LOGD if (g_bOpenLog) \
                 CLog::Logd
#define INIT_CRASH_REPORT_FOR_ANDROID(strRootDir) if (g_bOpenLog) \
                                         InitCrashReport(strRootDir)
#else
#define LOGP
#define LOGD
#define INIT_CRASH_REPORT_FOR_ANDROID(strRootDir)
#endif

#ifndef IMI_GD_VERSION
#define IMI_GD_VERSION          "2"
#endif

enum
{
	IMI_DB_VERSION_0 = 0,
	IMI_DB_VERSION_1 = 1,
	IMI_DB_VERSION_2 = 2,
	IMI_DB_VERSION_3 = 3
};

#ifndef IMI_DB_VERSION
#define IMI_DB_VERSION          IMI_DB_VERSION_3
#endif


// The major/minor/patch version (up to 3 digits each).
#define CPPDOM_VERSION_MAJOR     0
#define CPPDOM_VERSION_MINOR     8
#define CPPDOM_VERSION_PATCH     6

// These helper macros are used to stringify a given macro
#define CPPDOM_STR(s)             # s
#define CPPDOM_XSTR(s)            CPPDOM_STR(s)

// These helper macros are used to build up the CPPDOM_VERSION_STRING macro.
#define CPPDOM_DOT(a,b)           a ## . ## b
#define CPPDOM_XDOT(a,b)          CPPDOM_DOT(a,b)

// Create the CPPDOM_VERSION_STRING macro
#define CPPDOM_VERSION_STRING \
	CPPDOM_XDOT( \
			CPPDOM_XDOT(CPPDOM_VERSION_MAJOR, CPPDOM_VERSION_MINOR), \
			CPPDOM_VERSION_PATCH \
			)

#if defined(ANDROID)
#define IMI_RESOURCE      "mobile"
#define IMI_PLATFORM      "android"
#elif defined(IOS)
#define IMI_RESOURCE      "mobile"
#define IMI_PLATFORM      "ios"
#else
#define IMI_RESOURCE      "mobile"
#define IMI_PLATFORM      "android"
//#define IMI_RESOURCE      "cygwin"
//#define IMI_PLATFORM      "cygwin"
#endif

#ifndef IMI_DEFAULT_DOMAIN
#define IMI_DEFAULT_DOMAIN      "im.ii.com"
#endif

#define MAX_BODY_LEN            1536//1024+512 //2048  // bytes

#ifndef _T
#define _T(a) a
#endif

#ifndef STDMETHOD
#define STDMETHOD(method) virtual HRESULT /*__stdcall*/ method
#endif

#ifndef STDMETHODIMP
#define STDMETHODIMP            HRESULT
#endif

#ifndef FAILED
#define FAILED(hr) ((HRESULT)(hr) < 0)
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)
#endif

#ifndef ATLASSERT
#define ATLASSERT(a)
#endif

#ifndef _wtoi
#define _wtoi my_wtoi
#endif
extern int my_wtoi (wchar_t* num);

#ifndef _ttoi
#define _ttoi atoi
#endif

#ifndef _tcsncicmp
#define _tcsncicmp strncasecmp
#endif

#ifndef _tcschr
#define _tcschr strchr
#endif

#ifndef _tcsnccmp
#define _tcsnccmp strncmp
#endif

#ifndef _tcsncmp
#define _tcsncmp strncmp
#endif

#ifndef _tcsncat
#define _tcsncat strncat
#endif

#ifndef _tcscat
#define _tcscat strcat
#endif

#ifndef _tcslen
#define _tcslen strlen
#endif

#ifndef _tcstol
#define _tcstol strtol
#endif

#ifndef _tcscpy
#define _tcscpy strcpy
#endif

#ifndef _stprintf
#define _stprintf sprintf
#endif

#ifndef _tcsstr
#define _tcsstr strstr
#endif

#define _tcsncpy strncpy

#ifndef _ttoi64
#define _ttoi64 atoi
#endif

#ifndef LPCTSTR
#define LPCTSTR const char*
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

enum
{
	HTTP_METHOD_PUT    = 0,
	HTTP_METHOD_DELETE = 1,
	HTTP_METHOD_GET    = 2,
	HTTP_METHOD_POST   = 3

};

#define CONFIG_DIR			"/configure"
#define CONFIG_APP_XML		"config.xml"
#define CONFIG_RUNTIME_XML	"runtime.xml"
#define CONFIG_PUSH_APPS    "runtime2.xml"
#define USER_DIR			"/users"
#define CONFIG_USER			"user.xml"
#define CONFIG_CUSTOMHEAD 	"/portraits"
#define CONFIG_PSWD_XML     "password.xml"
#define CONFIG_SERIAL_XML   "serial.xml"
#define DATABASE_DIR		"/databases"
#define CONFIG_DATABASE		"im.db"
#define CRASH_DIR			"/crash"
#define LOG_DIR			    "/log"
#define LOG_FILENAME		"log.txt"

using namespace std;

#include <map>
#include <list>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "shared_ptr.h"
#include "MutexHelper.h"
#include "sqlite3.h"
#include "sqliteimpl.h"
#include "thread_pool.h"
#include "IConfigure.h"

extern "C" void on_thread_closed (int tid);

// 工作者类型
enum WorkerType
{
	WT_LOGIN   = 1,
	WT_LOGOUT  = 2,
	WT_MESSAGE = 3,
	WT_GROUP   = 4,
	WT_CONTACT = 5
};

class GroupInfo
{
public:
	GroupInfo()
	{
		strGroupName = "";
		strGroupId = "";
		strOwnerId = "";
		memberMaxcount = "";
		strGroupIcon = "";
		strGroupTmpName = "";
	}
	virtual ~GroupInfo()
	{

	}

	string strGroupName;//群名字
	string strGroupTmpName;//群临时名字
	string strGroupId;//群唯一id
	string strOwnerId;//群主id
	string strGroupIcon;//群头像key
	string memberMaxcount;//群成员上限
	vector<MemberParams> memberlist;//参数为成员id

	GroupInfo& operator =(const GroupInfo& groupinfo)
	{
		strGroupName = groupinfo.strGroupName;
		strGroupId   = groupinfo.strGroupId;
		strOwnerId   = groupinfo.strOwnerId;
		memberMaxcount  = groupinfo.memberMaxcount;
		memberlist   = groupinfo.memberlist;
		strGroupIcon = groupinfo.strGroupIcon;
		strGroupTmpName = groupinfo.strGroupTmpName;
		return *this;
	}
	GroupInfo(const GroupInfo& groupinfo)
	{
		strGroupName = groupinfo.strGroupName;
		strGroupId   = groupinfo.strGroupId;
		strOwnerId   = groupinfo.strOwnerId;
		memberMaxcount  = groupinfo.memberMaxcount;
		memberlist   = groupinfo.memberlist;
		strGroupIcon = groupinfo.strGroupIcon;
		strGroupTmpName = groupinfo.strGroupTmpName;
	}
};


typedef struct _HTTP_PROXY_INFO
{
	BOOL bUseProxy;
	string strProxyServer;
	string strProxyPort;
	string strProxyUser;
	string strProxyPwd;
}HTTP_PROXY_INFO;

typedef enum {
	XO_GET,						// 获取群组列表
	XO_DELETE,					// 删除群组
	XO_MODIFY,					// 修改群组
	XO_ADD,						// 创建并邀请人进群
	XO_INVITE,                  // 邀请人进群
	XO_GET_BUDDYLIST,           //获取联系人列表
	XO_GET_BUDDYINFO,           //获取联系人信息
	XO_GET_GROUPMEMBERLIST,     //更新某个群成员列表
	XO_MODIFY_GROUPMEMBERNAME,  //更新群名片
	XO_GET_OFFLINEMSG,           //获取离线消息
	XO_GET_OFFLINEMSG_ACK,        //获取离线消息回应
	XO_ADD_BUDDY,                //添加好友
	XO_DELETE_BUDDY,             //删除好友
	XO_SEARCH_BUDDY,             //查找好友
	XO_SET_RECV_PUSHMSG,         //查找好友
	XO_GET_RECV_PUSHMSG,         //查找好友

} XCAP_OPER;

typedef enum {
	XP_SERVER,					// 服务器地址 IP
	XP_PORT,                    //服务器port
	XP_JID,
	XP_PASSWORD,
	XP_DOM,						// 默认的域名
	XP_RECENT_DIR,				//最近联系人缓存路径
	XP_COMPANY_DIR,			//企业信息缓存路径
	XP_MAX_RECENT,              //最大联系人数
	XP_COMPANY_ETAGS,           //企业etag缓存路径
	XP_IOT_VERSION,
	XP_PROXY_USE,				//是否使用代理
	XP_PROXY_SERVER,			//代理服务器
	XP_PROXY_PORT,				//代理端口
	XP_PROXY_USER,				//代理用户名
	XP_PROXY_PWD,				//代理密码
} XCAP_PROP;

#ifndef INFINITE
#define INFINITE 0x7fffffff
#endif

// 全量或增量更新时间类型
enum UpdateTimeType
{
	UTT_BUDDY = 1,
	UTT_GROUP = 2
};

inline void ZeroMemory (void* p, int nSize) {
	memset (p, 0, nSize);
}

vector<string> split(string str, string pattern);

bool is_dir_exist (const char* dir_name);
bool ensure_path_exist (const char* dir_name);
bool file_exist (const char* path);
string GetUserDir (string& strRootDir, string& id, bool bCreate);
string GetConfigDir (string& strRootDir);
string GetCrashDir (string& strRootDir);
string GetLogDir (string& strRootDir);
string GetDBDir (string& strRootDir, string& strUserId);
string GetRootDir ();

string FromInt (int n);

string RandomString ();
string RandomDigitString ();
int wcslen2 (const wchar_t* s);
int wcscmp2 (const wchar_t* s1, const wchar_t* s2);
int utf8_to_unicode(uint8_t *in, wchar_t **out, int *outsize);
int unicode_to_utf8(wchar_t *in, int insize, uint8_t **out);
void wcscpy2 (wchar_t* dst,const wchar_t* src);

string MkIntDateTime();
string MkIntDateTime2();
string MkIntDateTimeLong();
string DatetimeStrToLong(const string& strTime);

void replace_separator2(string& instr, string srcdelim, string destdelim);

string Base64Encode(const string& source);
string Base64Decode(const string& source);

MessageType  String2Type (const string& type);
CategoryType String2Category (const string& category);
OnlineStatus String2OnlineStatus (const string& offline);
string       Type2String (MessageType eType);
string       Category2String (CategoryType eCategory);

string StringFormat (const string fmt, ...);

BOOL IfContactOrGroupExist(enum MessageType eMessageType, const string& pUserIdOrGroupId);
map<string, string> RetrieveContactOrGroup(enum MessageType eMessageType, const string& pUserIdOrGroupId);
map<string, string> RetrieveContactOrMember (enum MessageType eMessageType,
		const string& pUserId, const string& pGroupId);

#endif // _COMMON_H
