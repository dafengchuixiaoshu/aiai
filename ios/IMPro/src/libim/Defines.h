#ifndef _DEFINES_H
#define _DEFINES_H

using namespace std;

#include <string>
#include <vector>

#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char BYTE;
#endif // _BYTE_DEFINED

#ifndef _INT_DEFINED
#define _INT_DEFINED
typedef unsigned int INT;
#endif // _INT_DEFINED

#ifndef _UINT_DEFINED
#define _UINT_DEFINED
typedef unsigned int UINT;
#endif // _UINT_DEFINED

#ifndef _INT32_DEFINED
#define _INT32_DEFINED
typedef int INT32;
#endif // _INT32_DEFINED

#ifndef _UINT32_DEFINED
#define _UINT32_DEFINED
typedef unsigned int UINT32;
#endif // _UINT32_DEFINED

#ifndef _LONG_DEFINED
#define _LONG_DEFINED
typedef long LONG;
#endif // _LONG_DEFINED

#ifndef _ULONG_DEFINED
#define _ULONG_DEFINED
typedef unsigned long ULONG;
#endif // _ULONG_DEFINED

#ifndef _ULONG_DEFINED
#define _ULONG_DEFINED
typedef unsigned long long ULONG64;
#endif // _ULONG_DEFINED

#ifndef _VOID_DEFINED
#define _VOID_DEFINED
typedef void VOID;
#endif // _VOID_DEFINED

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef long HRESULT;
#endif // _HRESULT_DEFINED

#ifndef _CHAR_DEFINED
#define _CHAR_DEFINED
typedef char CHAR;
#endif // _CHAR_DEFINED

#ifndef _BOOL_DEFINED
#define _BOOL_DEFINED

#if !defined(IOS)
typedef signed char BOOL;
#else
#include <objc/objc.h>
#endif

#endif // _BOOL_DEFINED

#ifndef _BSTR_DEFINED
#define _BSTR_DEFINED
typedef wchar_t* BSTR;
#endif // _BSTR_DEFINED

#ifndef VARIANT_BOOL
#define VARIANT_BOOL short
#endif

#ifndef NULL
#define NULL  '\0'
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define S_OK           0
#define S_FALSE       -1
#define E_NOTIMPL     -2
#define E_NOINTERFACE -3
#define E_ABORT       -4
#define E_FAIL        -5
#define E_INVALIDARG  -6

// 业务请求类型
enum ReqType
{
	RT_LOGIN                     = 100,
	RT_LOGOUT                    = 101,

	RT_SEND_MESSAGE              = 200,
	RT_RESEND_MESSAGE            = 201,

	RT_ADD_GROUP                 = 300,
	RT_REMOVE_GROUP              = 301,
	RT_MODIFY_GROUP              = 302,
	RT_INVITE_GROUP              = 303,
	RT_GET_GROUPLIST             = 304,
	RT_GET_LOCAL_GROUPLIST       = 305,
	RT_GET_LOCAL_GROUPMEMBERLIST = 306,
	RT_MODIFY_GROUPMEMBER_NAME   = 307
};

// 业务回应结果
enum RspType
{
	ST_NETWORK_FAILURE              = 10,   // 网络错误

	ST_LOGIN_SUCCESS                = 100,  // 登录成功
	ST_LOGIN_FAILURE                = 101,  // 登录失败
	ST_LOGIN_PROCESS                = 102,  // 登录进行中
	ST_LOGIN_ELSEWHERE              = 103,  // 同账号在其他终端上登录
	ST_LOGIN_VERSIONLOW             = 104,  // 协议版本低，需要强制升级
	ST_LOGOUT_SUCCESS               = 105,  // 登出成功
	ST_LOGOUT_FAILURE               = 106,  // 登出失败

	ST_SEND_MESSAGE_SUCCESS         = 200,  // 发送消息成功
	ST_SEND_MESSAGE_FAILURE         = 201,  // 发送消息失败
	ST_SEND_MESSAGE_PROCESS         = 202,  // 发送消息进行中
	ST_RECV_MESSAGE                 = 203,  // 接收到在线新消息
	ST_RECV_OFFLINE_MESSAGE         = 204,  // 接收到离线消息
	ST_RECV_CUSTOM_SMALLNUM         = 205,  // 收到客服小号
	ST_WRITE_MESSAGE_SUCCESS        = 206,  // 写消息成功
	ST_WRITE_MESSAGE_FAILURE        = 207,  // 写消息失败

//	ST_lANCHAT_WRITE_BINARY 		= 290,  // 接收局域网文件

	ST_ADD_GROUP_SUCCESS            = 300,
	ST_ADD_GROUP_FAILURE            = 301,
	ST_QUIT_GROUP_SUCCESS           = 302,
	ST_QUIT_GROUP_FAILURE           = 303,
	ST_MODIFY_GROUPNAME_SUCCESS     = 304,
	ST_MODIFY_GROUPNAME_FAILURE     = 305,
	ST_JOINGRUOP_SUCCESS            = 306,
	ST_JOINGROUP_FAILURE            = 307,
	ST_GETGROUPLIST_SUCCESS         = 308,
	ST_GETGROUPLIST_FAILURE         = 309,
	ST_GETGROUPMEMBER_SUCCESS       = 310,
	ST_GETGROUPMEMBER_FAILURE       = 311,
	ST_NOTIFY_CREATE_GROUP          = 312,  // 收到邀请加入群通知
	ST_NOTIFY_REFRESH_GROUP         = 313,  // 收到其他人加入群通知，刷新群成员列表
	ST_NOTIFY_MODIFY_GROUP          = 314,  // 收到修改群名称通知
	ST_NOTIFY_QUIT_GROUP            = 315,  // 收到修改群名称通知
	ST_MODIFY_GROUPMEMBERNAME_SUCCESS = 316,//修改自己在某个群名片
	ST_MODIFY_GROUPMEMBERNAME_FAILURE = 317,
	ST_NOTIFY_MODIFY_GROUPMEMBERNAME  = 318,

	ST_LOAD_BUDDY                   = 400,  // 网络获取好友
	ST_LOAD_BUDDY_LIST              = 401,  // 网络获取好友列表
	ST_NOTIFY_INVITE_BUDDY          = 402,  // 收到邀请添加好友
	ST_NOTIFY_INVITE_BUDDY_AGREE    = 403,  // 对方同意添加好友
	ST_NOTIFY_INVITE_BUDDY_REJECT   = 404,  // 对方拒绝添加好友
	ST_NOTIFY_DELETE_BUDDY          = 405,  // 对方删除好友
	ST_ADD_BUDDY_SUCCESS            = 406,  // 添加好友成功
	ST_ADD_BUDDY_FAILURE            = 407,  // 添加好友失败
	ST_DELETE_BUDDY_SUCCESS         = 408,  // 删除好友成功
	ST_DELETE_BUDDY_FAILURE         = 409,  // 删除好友失败
	ST_SEARCH_BUDDY_SUCCESS         = 410,  // 搜索好友成功
	ST_SEARCH_BUDDY_FAILURE         = 411,  // 搜索好友失败
	ST_NOTIFY_MODIFY_BUDDY          = 412,  // 收到修改好友通知
	ST_NOTIFY_MODIFY_GROUPMEMBER    = 413,  // 收到修改群成员通知
	//以下界面不需要处理
	ST_INNER_NOTIFY_GETBUDDYLIST_SUCCESS = 501,  // 登录获取联系人成功
	ST_INNER_NOTIFY_GETGROUPLIST_SUCCESS = 502,  // 登录获取群组成功

	ST_LOAD_FANS                    = 600,  // 网络获取粉丝
	ST_LOAD_FANS_LIST               = 601,  // 网络获取粉丝列表
	ST_NOTIFY_ADD_FANS              = 602,  // 收到添加粉丝通知
	ST_NOTIFY_DELETE_FANS           = 603,  // 收到删除粉丝通知
	ST_NOTIFY_MODIFY_FANS           = 604,  // 收到修改粉丝通知
	ST_NOTIFY_NEW_FANS              = 605,  // 收到新增粉丝通知
	ST_NOTIFY_HUDONG_NEWS           = 606,  // 收到互动消息通知
	ST_NOTIFY_RECOMMEND_BUDDY       = 607   // 收到推荐好友通知
};

// 网络状态类型
enum NetWorkStatusType
{
	NST_NULL = 0,//没网
	NST_WIFI = 1,//wifi
	NST_NETWORK = 2,//network
	NST_NETWORK_DOWNLOAD = 3//设置自动下载图片在3G
};

// 连接状态类型
enum ConnectionStatusType
{
	CST_EXIST     = 1,  // 连接存在
	CST_NOTEXIST  = 2   // 连接不存在
};

// 消息类型
enum MessageType
{
	MT_PERSONAL = 1,  // 个人消息
	MT_GROUP    = 2,  // 群组消息
	//MT_CUSTOM   = 3,  // 客服消息
	//MT_MAISHOU  = 4   // 买手消息
};

// 方式类型
enum KindType
{
	KT_ORDINARY     = 1,  // 普通消息
	KT_NOTIFICATION = 2   // 通知消息
};

// 内容种类
enum CategoryType
{
	CT_PLAIN_TEXT = 1,  // 纯文本
	CT_RICH_TEXT  = 2,  // 富文本
	CT_IMAGE      = 3,  // 图片
	CT_AUDIO      = 4,  // 音频
	CT_VIDEO      = 5   // 视频
};

// 消息方向
enum DirectionType
{
	DT_SEND  = 1, // 发送
	DT_RECV  = 2  // 接收
};

// 未读状态
enum UnreadStatus
{
	US_UNREAD = 0, // 未读
	US_READ   = 1  // 已读
};

// 播放状态
enum PlayedStatus
{
	PS_UNPLAY = 0, // 未播放
	PS_PLAYED = 1  // 已播放
};

// 发送状态
enum SendStatus
{
	SS_UNSENT = 0, // 未发送
	SS_SENT   = 1  // 已发送
};

// 在线状态
enum OnlineStatus
{
	OS_OFFLINE = 0, // 离线消息
	OS_ONLINE  = 1  // 在线消息
};

// 置顶状态
enum TopStatus
{
	TS_UNTOPPED = 0, // 未置顶
	TS_TOPPED   = 1  // 已置顶
};

// 新消息提醒
enum RemindStatus
{
	RS_CLOSE = 0, // 关闭
	RS_OPEN  = 1  // 开启
};

// 会话属性类型
enum SessionOptionType
{
	SOT_TOP     = 1, // 会话置顶类型
	SOT_REMIND  = 2  // 新消息提醒类型
};

// 消息的特定字段类型
enum GivenFieldType
{
	GFT_CONTENT = 0  // 消息内容字段
};

// 消息的特定条件类型
enum GivenConditionType
{
	GCT_IMAGE = 0  // 图片
};

// 草稿变化
enum DraftChanged
{
	DC_UNCHANGED = 0, // 未变化
	DC_CHANGED   = 1  // 有变化
};

// 是否好友类型
enum IsBuddyType
{
	IBT_NOTBUDDY     = 0,  // 非好友（为前向兼容）
	IBT_BUDDY        = 1,  // 好友
	IBT_FANS         = 2,  // 粉丝
	IBT_MEMBER       = 4   // 成员
};

// Iq类型
enum IqType
{
	IT_PG         = 1,   // 群组
	IT_CONTACT    = 2,   // 联系人
	IT_CUSTOMCHAT = 3,   // 客服
	IT_DISCONNECT = 4,   // 网络断线
	IT_HUDONGNEWS = 5    // 互动消息
};

enum SnsType
{
	IT_HUDONG    = 1, //互动消息
	IT_RECOMMEND = 2  //推荐好友
};

// 消息数据
typedef struct
{
	string             pMsgId;       // 消息ID
	string             pSessionId;   // 会话ID
	string             pSender;      // 消息发送人
	string             pSenderName;  // 消息发送人的名字
	string             pSenderKey;   // 消息发送人的头像
	string             pReceiver;    // 消息接收人
	string             pContent;     // 消息内容（文本）
	string             pTime;        // 消息时间（发送或者接收）
	enum MessageType   eType;        // 消息类型（1:个人消息,2:群组消息,3:客服消息,4:买手消息）
	enum KindType      eKind;        // 消息方式（1:普通消息,2:通知消息）
	enum CategoryType  eCategory;    // 内容种类（1:纯文本(plain-text),2:富文本(rich-text),3:图片(image),4:音频(audio),5:视频(video)）
	enum DirectionType eDirection;   // 消息方向（1:发送,2:接收）
	enum UnreadStatus  eUnread;      // 未读状态（0:未读,1:已读）
	enum PlayedStatus  ePlayed;      // 播放状态（0:未播放,1:已播放）
	enum SendStatus    eSend;        // 发送状态（0:未发送,1:已发送）
	enum OnlineStatus  eOnline;      // 在线状态（0:离线,1:在线）

	int pIsSave; //可保存（1：保存，其他不保存）
	int pIsShare; //可分享（1：可分享，其他不分享）

} MessageParams;

struct LanChatBinary
{
	LanChatBinary(string& key, string& binary, string& filename)
		: m_key(key)
		, m_binary(binary)
		, m_filename(filename)
	{
	}

	string& m_key;
	string& m_binary;
	string& m_filename;
};

// 消息列表
typedef struct
{
	int                   nCount;
	vector<MessageParams> ppMessageParams;

} MessageList;

// 会话数据
typedef struct
{
	string            pSessionId;    // 会话ID
	string            pSessionName;  // 会话名
	string            pPortraitKey;  // 头像Key
	string            pMsgId;        // 消息ID
	enum MessageType  eType;         // 消息类型（1:个人消息,2:群组消息,3:客服消息,4:买手消息）
	enum KindType     eKind;         // 消息方式（1:普通消息,2:通知消息）
	int               nUnreadNum;    // 未读消息个数
	enum TopStatus    eTop;          // 置顶状态（0:未置顶,1:已置顶）
	enum RemindStatus eRemind;       // 新消息提醒(0:关闭,1:开启)
	string            pDraftContent; // 草稿内容
	enum DraftChanged eDraftChanged; // 草稿变化

} SessionParams;

// 会话列表
typedef struct
{
	int                   nCount;
	vector<SessionParams> ppSessionParams;

} SessionList;

// 好友数据
typedef struct
{
	string           pCmsId;         // CmsId
	string           pUserId;        // 联系人ID
	string           pUserName;      // 联系人名字
	string           pPortraitKey;   // 头像Key
	string           pEmotionMood;   // 心情短语
	string           pQrerUrl;       // 圈儿URL
	string           pCommunityUrl;  // 群落URL
	enum IsBuddyType eIsBuddy;       // 是否好友
	string           pGroupId;       // 群昵称所在群组ID
	string           pCardName;      // 群昵称
	string           pEmail;         //邮箱
	string           pMobile;        //手机号
	string			 pSubname;		 //备注
	string  		 pIsSave;		 //可保存
	string			 pIsShare;		 //可分享


} BuddyParams;

// 好友列表
typedef struct
{
	int                 nCount;
	vector<BuddyParams> ppBuddyParams;

} BuddyList;

// Iq数据
typedef struct
{
	enum IqType   eType;      // Iq类型
	string        pBody;      // Iq体
	string        pTalkId;    // 商品ID
	string        pSmallNum;  // 小号

} IqParams;

// 离线消息数据
typedef struct
{
	int                  nOfflineTotal;  // 离线消息总条数
	int                  nSessionCount;  // 离线消息会话个数
	vector<string>       ppSessionId;    // 离线消息会话列表

} OfflineMessageParams;

// 群成员数据结构
typedef struct
{
	string        MemberID;         // 成员ID
	string        MemberName;       //成员昵称
	string        MemberCardName;   //成员名片
	string        PortraitKey;      //头像
	string        EmotionMood;      //心情短语
	string        CmsID;
	int           IsBuddy;          //是否是好友
} MemberParams;

// 群数据结构
typedef struct
{
	string            GroupID;          // 群ID
	string            GroupName;        // 群名称
	string            GroupPortraitKey; // 群头像
	string            OwnerID;          // 群主ID
	string            GroupMemberCount; // 群成员人数
	string            GroupMaxMember;   // 群成员人数上限
	string            GroupCardName;    // 群名片
	MemberParams      MemberInfo;       // 群主ID,邀请者ID,修改者ID,退出者ID
	vector<MemberParams> Userlist;      // 邀请者列表,被邀请者列表,群成员列表
} GroupParams;

// 关系数据结构
typedef struct
{
	enum IsBuddyType eIsBuddyType;    // 是否好友类型
	string           pUserId;         // 用户ID
	string           pRelationId;     // 关系ID
	string           pGroupCardName;  //群名片

} RelationParams;

// 关系列表
typedef struct
{
	int                 nCount;
	vector<RelationParams> ppRelationParams;

} RelationList;

// 业务请求数据
typedef struct
{
	enum ReqType  eReqType;

	union
	{
		MessageParams* pMessageParams;
		GroupParams*   pGroupParams;
	};

} ReqParams;

struct NewChat
{
	NewChat(string& to, string& buffer, bool ischat)
	{
		touser = to;

		if(ischat)
		{
			chatmsg = buffer;
		}
		else
		{
			filename = buffer;
		}
	}

	string touser;
	string chatmsg;
	string filename;
};

//业务返回错误码
enum RspErrCode
{
	CREATE_GROUP_RSP_ERR_CODE            = 513,   // 创建群群人数超出上限
	INVITE_GROUP_RSP_ERR_CODE            = 521,   // 邀请群群人数超出上限
};

// 业务回应数据
typedef struct RspParams
{
	enum RspType       eRspType;
	enum RspErrCode    errcode;
	int                extendcode;

	MessageParams*        pMessageParams;
	GroupParams*          pGroupParams;
	BuddyParams*          pBuddyParams;
	IqParams*             pIqParams;
	OfflineMessageParams* pOfflineMessageParams;
	LanChatBinary*		  pLanchatBinary;


	RspParams()
	{
		pMessageParams = NULL;
		pGroupParams = NULL;
		pBuddyParams = NULL;
		pIqParams = NULL;
		pOfflineMessageParams = NULL;
		pLanchatBinary = NULL;
	}
} ;


// 观察者类型
enum ObserverType
{
	OT_LOGIN   = 1,
	OT_MESSAGE = 2,
	OT_GROUP   = 3,
	OT_BUDDY   = 4,
	OT_FANS    = 5

};

#endif // _DEFINES_H
