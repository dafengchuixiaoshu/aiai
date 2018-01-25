/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.izp.imsdkjni;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class IMSDKJni
{
	private static IMSDKJni g_imsdkjni = null;

    public static IMSDKJni getInstance()  
    {  
        if (g_imsdkjni == null)  
        {  
        	g_imsdkjni = new IMSDKJni();  
        }  
        return g_imsdkjni;  
    }

	// 函数返回值常量
    public static final int S_OK          = 0;
    public static final int E_FALSE       = -1;
    public static final int E_NOTIMPL     = -2;
    public static final int E_NOINTERFACE = -3;
    public static final int E_ABORT       = -4;
    public static final int E_FAIL        = -5;
    public static final int E_INVALIDARG  = -6;
    
	// 观察者类型
	public enum ObserverType
	{
		OT_LOGIN   (1),
		OT_MESSAGE (2),
		OT_GROUP   (3),
		OT_BUDDY   (4),
		OT_FANS    (5);
		
		private int value;
		private ObserverType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 业务请求类型
	public enum ReqType
	{
		RT_LOGIN                     (100),
		RT_LOGOUT                    (101),
		
		RT_SEND_MESSAGE              (200),
		RT_RESEND_MESSAGE            (201),
		
		RT_ADD_GROUP                 (300),
		RT_REMOVE_GROUP              (301),
		RT_MODIFY_GROUP              (302),
		RT_INVITE_GROUP              (303),
		RT_GET_GROUPLIST             (304),
		RT_GET_LOCAL_GROUPLIST       (305),
		RT_GET_LOCAL_GROUPMEMBERLIST (306),
		RT_MODIFY_GROUPMEMBER_NAME   (307);
		
		private int value;
		private ReqType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 业务回应结果
	public enum RspType
	{
		ST_NETWORK_FAILURE              (10) ,  // 网络错误

		ST_LOGIN_SUCCESS                (100),  // 登录成功
		ST_LOGIN_FAILURE                (101),  // 登录失败
		ST_LOGIN_PROCESS                (102),  // 登录进行中
		ST_LOGIN_ELSEWHERE              (103),  // 同账号在其他终端上登录
		ST_LOGIN_VERSIONLOW             (104),  // 协议版本低，需要强制升级
		ST_LOGOUT_SUCCESS               (105),  // 登出成功
		ST_LOGOUT_FAILURE               (106),  // 登出失败

		ST_SEND_MESSAGE_SUCCESS         (200),  // 发送消息成功
		ST_SEND_MESSAGE_FAILURE         (201),  // 发送消息失败
		ST_SEND_MESSAGE_PROCESS         (202),  // 发送消息进行中
		ST_RECV_MESSAGE                 (203),  // 接收到新消息
		ST_RECV_OFFLINE_MESSAGE         (204),  // 接收到离线消息
		ST_RECV_CUSTOM_SMALLNUM         (205),  // 收到客服小号
		ST_WRITE_MESSAGE_SUCCESS        (206),  // 写消息成功
		ST_WRITE_MESSAGE_FAILURE        (207),  // 写消息失败

		ST_ADD_GROUP_SUCCESS            (300),
		ST_ADD_GROUP_FAILURE            (301),
		ST_QUIT_GROUP_SUCCESS           (302),
		ST_QUIT_GROUP_FAILURE           (303),
		ST_MODIFY_GROUPNAME_SUCCESS     (304),
		ST_MODIFY_GROUPNAME_FAILURE     (305),
		ST_JOINGRUOP_SUCCESS            (306),
		ST_JOINGROUP_FAILURE            (307),
		ST_GETGROUPLIST_SUCCESS         (308),
		ST_GETGROUPLIST_FAILURE         (309),
		ST_GETGROUPMEMBER_SUCCESS       (310),
		ST_GETGROUPMEMBER_FAILURE       (311),
		ST_NOTIFY_CREATE_GROUP          (312),//收到邀请加入群通知
		ST_NOTIFY_REFRESH_GROUP         (313),//收到其他人加入群通知，刷新群成员列表
		ST_NOTIFY_MODIFY_GROUP          (314),//收到修改群名称通知
		ST_NOTIFY_QUIT_GROUP            (315), //收到修改群名称通知
		ST_MODIFY_GROUPMEMBERNAME_SUCCESS (316),//修改自己在某个群名片
		ST_MODIFY_GROUPMEMBERNAME_FAILURE (317),
		ST_NOTIFY_MODIFY_GROUPMEMBERNAME  (318),
		
		ST_LOAD_BUDDY                   (400),
		ST_LOAD_BUDDY_LIST              (401),
		ST_NOTIFY_INVITE_BUDDY          (402), //收到邀请添加好友
		ST_NOTIFY_INVITE_BUDDY_AGREE    (403), //对方同意添加好友
		ST_NOTIFY_INVITE_BUDDY_REJECT   (404), //对方拒绝添加好友
		ST_NOTIFY_DELETE_BUDDY          (405),  //对方删除好友
		ST_ADD_BUDDY_SUCCESS            (406),   // 添加好友成功
		ST_ADD_BUDDY_FAILURE            (407),   // 添加好友成功
		ST_DELETE_BUDDY_SUCCESS         (408),   // 删除好友成功
		ST_DELETE_BUDDY_FAILURE         (409),   // 删除好友成功
		ST_SEARCH_BUDDY_SUCCESS         (410),   // 搜索好友成功
		ST_SEARCH_BUDDY_FAILURE         (411),   // 搜索好友成功

		ST_LOAD_FANS                    (600),  // 网络获取粉丝
		ST_LOAD_FANS_LIST               (601),  // 网络获取粉丝列表
		ST_NOTIFY_ADD_FANS              (602),  // 收到添加粉丝通知
		ST_NOTIFY_DELETE_FANS           (603),  // 收到删除粉丝通知
		ST_NOTIFY_MODIFY_FANS           (604),  // 收到修改粉丝通知
		ST_NOTIFY_NEW_FANS              (605),  // 收到新增粉丝通知
		ST_NOTIFY_HUDONG_NEWS           (606),  // 收到互动消息通知
		ST_NOTIFY_RECOMMEND_BUDDY       (607);  // 收到推荐好友通知

		private int value;
		private RspType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
		public static RspType valueOf(int value) {
	        switch (value) {
	        
	        case 10:
	            return ST_NETWORK_FAILURE;
	            
	        case 100:
	            return ST_LOGIN_SUCCESS;
	        case 101:
	            return ST_LOGIN_FAILURE;
	        case 102:
	            return ST_LOGIN_PROCESS;
	        case 103:
	        	return ST_LOGIN_ELSEWHERE;
	        case 104:
	        	return ST_LOGOUT_SUCCESS;
	        case 105:
	        	return ST_LOGOUT_FAILURE;
  
	        case 200:
	            return ST_SEND_MESSAGE_SUCCESS;
	        case 201:
	            return ST_SEND_MESSAGE_FAILURE;
	        case 202:
	            return ST_SEND_MESSAGE_PROCESS;
	        case 203:
	        	return ST_RECV_MESSAGE;
	        case 204:
	        	return ST_RECV_OFFLINE_MESSAGE;
	        case 205:
	        	return ST_RECV_CUSTOM_SMALLNUM;
	        case 206:
	        	return ST_WRITE_MESSAGE_SUCCESS;
	        case 207:
	        	return ST_WRITE_MESSAGE_FAILURE;

	        case 300:
	            return ST_ADD_GROUP_SUCCESS;
	        case 301:
	            return ST_ADD_GROUP_FAILURE;
	        case 302:
	            return ST_QUIT_GROUP_SUCCESS;
	        case 303:
	            return ST_QUIT_GROUP_FAILURE;
	        case 304:
	            return ST_MODIFY_GROUPNAME_SUCCESS;
	        case 305:
	            return ST_MODIFY_GROUPNAME_FAILURE;
	        case 306:
	            return ST_JOINGRUOP_SUCCESS;
	        case 307:
	            return ST_JOINGROUP_FAILURE;
	        case 308:
	            return ST_GETGROUPLIST_SUCCESS;
	        case 309:
	            return ST_GETGROUPLIST_FAILURE;
	        case 310:
	            return ST_GETGROUPMEMBER_SUCCESS;
	        case 311:
	            return ST_GETGROUPMEMBER_FAILURE;
	        case 312:
	            return ST_NOTIFY_CREATE_GROUP;
	        case 313:
	            return ST_NOTIFY_REFRESH_GROUP;
	        case 314:
	            return ST_NOTIFY_MODIFY_GROUP;
	        case 315:
	            return ST_NOTIFY_QUIT_GROUP;
	        case 316:
	        	return ST_MODIFY_GROUPMEMBERNAME_SUCCESS;
	        case 317:
	        	return ST_MODIFY_GROUPMEMBERNAME_FAILURE;
	        case 318:
	        	return ST_NOTIFY_MODIFY_GROUPMEMBERNAME;
	        	
	        case 400:
	        	return ST_LOAD_BUDDY;
	        case 401:
	        	return ST_LOAD_BUDDY_LIST;
	        case 402:
	        	return ST_NOTIFY_INVITE_BUDDY;
	        case 403:
	        	return ST_NOTIFY_INVITE_BUDDY_AGREE;
	        case 404:
	        	return ST_NOTIFY_INVITE_BUDDY_REJECT;
	        case 405:
	        	return ST_NOTIFY_DELETE_BUDDY;
	        case 406:
	        	return ST_ADD_BUDDY_SUCCESS;
	        case 407:
	        	return ST_ADD_BUDDY_FAILURE;
	        case 408:
	        	return ST_DELETE_BUDDY_SUCCESS;
	        case 409:
	        	return ST_DELETE_BUDDY_FAILURE;
	        case 410:
	        	return ST_SEARCH_BUDDY_SUCCESS;
	        case 411:
	        	return ST_SEARCH_BUDDY_FAILURE;	        	
	        	
	        case 600:
	        	return ST_LOAD_FANS;
	        case 601:
	        	return ST_LOAD_FANS_LIST;
	        case 602:
	        	return ST_NOTIFY_ADD_FANS;
	        case 603:
	        	return ST_NOTIFY_DELETE_FANS;
	        case 604:
	        	return ST_NOTIFY_MODIFY_FANS;
			case 605:
				return ST_NOTIFY_NEW_FANS;
			case 606:
				return ST_NOTIFY_HUDONG_NEWS;
	
	        default:
	            return null;
	        }
		}
	}
	
	// 网络状态类型
	public enum NetWorkStatusType
	{
		NST_GOOD  (1),  // 网络正常
		NST_ERROR (2);  // 网络错误

		private int value;
		private NetWorkStatusType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	// 连接状态类型
	public enum ConnectionStatusType
	{
		CST_EXIST     (1),  // 连接存在
		CST_NOTEXIST  (2);  // 连接不存在

		private int value;
		private ConnectionStatusType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	// 消息类型
	public enum MessageType
	{
		MT_PERSONAL (1),  // 个人消息
		MT_GROUP    (2),  // 群组消息
		MT_CUSTOM   (3),  // 客服消息
		MT_MAISHOU  (4);  // 买手消息

		private int value;
		private MessageType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 方式类型
	public enum KindType
	{
		KT_ORDINARY     (1),  // 普通消息
		KT_NOTIFICATION (2);  // 通知消息

		private int value;
		private KindType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	// 内容种类
	public enum CategoryType
	{
		CT_PLAIN_TEXT (1),  // 纯文本
		CT_RICH_TEXT  (2),  // 富文本
		CT_IMAGE      (3),  // 图片
		CT_AUDIO      (4),  // 音频
		CT_VIDEO      (5);  // 视频

		private int value;
		private CategoryType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	// 消息方向
	public enum DirectionType
	{
		DT_SEND  (1), // 发送
		DT_RECV  (2); // 接收

		private int value;
		private DirectionType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	// 未读状态
	public enum UnreadStatus
	{
		US_UNREAD (0),  // 未读
		US_READ   (1);  // 已读
				
		private int value;
		private UnreadStatus(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	// 播放状态
	public enum PlayedStatus
	{
		PS_UNPLAY (0),  // 未播放
		PS_PLAYED (1);  // 已播放

		private int value;
		private PlayedStatus(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 发送状态
	public enum SendStatus
	{
		SS_UNSENT (0),  // 未发送
		SS_SENT   (1);  // 已发送

		private int value;
		private SendStatus(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 在线状态
	public enum OnlineStatus
	{
		OS_OFFLINE (0), // 离线消息
		OS_ONLINE  (1);  // 在线消息

		private int value;
		private OnlineStatus(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 置顶状态
	public enum TopStatus
	{
		TS_UNTOPPED (0),  // 未置顶
		TS_TOPPED   (1);  // 已置顶

		private int value;
		private TopStatus(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 新消息提醒
	public enum RemindStatus
	{
		RS_CLOSE (0),  // 关闭
		RS_OPEN  (1);  // 开启

		private int value;
		private RemindStatus(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 会话属性类型
	public enum SessionOptionType
	{
		SOT_TOP    (1),  // 会话置顶类型
		SOT_REMIND (2);  // 新消息提醒类型

		private int value;
		private SessionOptionType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 消息的特定字段类型
	public enum GivenFieldType
	{
		GFT_CONTENT (0);  // 消息内容字段

		private int value;
		private GivenFieldType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	// 消息的特定条件类型
	public enum GivenConditionType
	{
		GCT_IMAGE (0);  // 图片

		private int value;
		private GivenConditionType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 草稿变化
	public enum DraftChanged
	{
		DC_UNCHANGED (0),  // 未变化
		DC_CHANGED   (1);  // 有变化

		private int value;
		private DraftChanged(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}
	
	// 是否好友类型
	public enum IsBuddyType
	{
		IBT_NOTBUDDY     (0),  // 非好友（为前向兼容）
		IBT_BUDDY        (1),  // 好友
		IBT_FANS         (2),  // 粉丝
		IBT_MEMBER       (4);  // 成员

		private int value;
		private IsBuddyType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	// Iq类型
	public enum IqType
	{
		IT_PG         (1),   // 群组
		IT_CONTACT    (2),   // 联系人
		IT_CUSTOMCHAT (3),   // 客服
		IT_DISCONNECT (4);   // 网络断线

		private int value;
		private IqType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	}

	public enum SnsType
	{
		IT_HUDONG (1),//互动消息
		IT_RECOMMEND(2);//推荐好友
		private int value;
		private SnsType(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
	};
	// 消息数据
	public static class MessageParams
	{
		public String pMsgId;        // 消息ID
		public String pSessionId;    // 会话ID
		public String pSender;       // 消息发送人
		public String pSenderName;   // 消息发送人的名字
		public String pSenderKey;    // 消息发送人的头像
		public String pReceiver;     // 消息接收人
		public String pContent;      // 消息内容（文本）
		public String pTime;         // 消息时间（发送或者接收）
		public int    eType;         // 消息类型（1:个人消息,2:群组消息）
		public int    eKind;         // 消息方式（1:普通消息,2:通知消息）
		public int    eCategory;     // 内容种类（1:纯文本(plain-text),2:富文本(rich-text),3:图片(image),4:音频(audio),5:视频(video)）
		public int    eDirection;    // 消息方向（1:发送,2:接收）
		public int    eUnread;       // 未读状态（0:已读,1:未读）
		public int    ePlayed;       // 播放状态（0:未播放,1:已播放）
		public int    eSend;         // 发送状态（0:未发送,1:已发送）
		public int    eOnline;       // 在线状态（0:离线,1:在线）

	}
	
	// 会话数据
	public static class SessionParams
	{
		public String pSessionId;    // 会话ID
		public String pSessionName;  // 会话ID
		public String pPortraitKey;  // 头像Key
		public String pMsgId;        // 消息ID
		public int    eType;         // 消息类型（1:个人消息,2:群组消息）
		public int    nUnreadNum;    // 未读消息个数
		public int    eTop;          // 置顶状态（0:未置顶,1:已置顶）
		public int    eRemind;       // 新消息提醒(0:关闭,1:开启)
		public String pDraftContent; // 草稿内容
		public int    eDraftChanged; // 草稿变化

	}
	
	// 好友数据
	public static class BuddyParams
	{
		public String pUserId;        // 联系人ID
		public String pUserName;      // 联系人名字
		public String pPortraitKey;   // 头像Key
		public String pEmotionMood;   // 心情短语
		public String pCmsId;         // CmsId
		public String pQrerUrl;       // 圈儿URL
		public String pCommunityUrl;  // 群落URL
		public int    eIsBuddy;       // 是否好友
		public String pGroupId;       // 群昵称所在群组ID
		public String pCardName;      // 群昵称
		public String pEmail;         //邮箱
		public String pMobile;        //手机号
	}

	// Iq数据
	public static class IqParams
	{
		public int    eType;      // Iq类型
		public String pBody;      // Iq体
		public String pTalkId;    // 商品ID
		public String pSmallNum;  // 小号

	}

	// 离线消息数据
	public static class OfflineMessageParams
	{
		public int      nOfflineTotal;  // 离线消息总条数
		public int      nSessionCount;  // 离线消息会话个数
		public Object   ppSessionId;    // 离线消息会话列表
	}
	
	// 群成员数据结构
	public static class MemberParams
	{
		public String        MemberID;         // 成员ID
		public String        MemberName;       //成员昵称
		public String        MemberCardName;   //群名片
		public String        PortraitKey;      //头像
		public String        EmotionMood;      //心情短语
		public String        CmsID;
		public int           IsBuddy;          //是否是好友
	}
	
	// 群数据结构
	public static class GroupParams
	{
		public String        GroupID;          // 群ID
		public String        GroupName;        //群名称
		public String        GroupPortraitKey; //群头像
		public String        GroupCardName;    //群名片
		public String        GroupMaxMember;   //群人数上限
		public String        OwnerID;          //群主ID
		public String        GroupMemberCount; //群成员人数
		public Object        MemberInfo;       // 群主ID,邀请者ID,修改者ID,退出者ID
		public Object        Userlist;         // 邀请者列表,被邀请者列表,群成员列表,群列表
	
	} 

	// 业务请求数据
	public static class ReqParams {
		public int eReqType;
		public Object objReq;
	}
	
	//业务返回错误码
	public enum RspErrCode
	{
		CREATE_GROUP_RSP_ERR_CODE(513),//创建群群人数超出上限
		INVITE_GROUP_RSP_ERR_CODE(521);//创建群群人数超出上限
		private int value;
		private RspErrCode(int value) {
			this.value = value;
		}
		public int value() {
			return this.value;
		}
		
	};
	
	// 业务回应数据
	public static class RspParams {
		public int eRspType;
		public int eRspErrCode;
		public int eRspExtendCode;
		public Object objRsp;
	}
	
	public native int Init ();
	public native int Release ();

	public native int    SetRootDir (String rootDir);
	public native int    SetServerAddr (String serverAddr, String serverPort);
	public native int    SetUserId (String userID);
	public native String GetUserId ();
	public native int    SetObserver (int observerType, Object uiObserver);
	public native int    NotifyNetworkStatus (int networkStatusType);
	public native int    GetConnectionStatus ();
	public native int    CloseConnection ();
	public native int    OpenLog (int openLog);

	// login
	public native int Login (Object reqParams);
	public native int Logout (Object reqParams);
	
	// message
	public native int      SendMessage (Object reqParams);
	public native int      ResendMessage (Object reqParams);
	public native String   WriteMessage (Object messageParams);
	public native int      UpdateMessage (Object messageParams);
	public native Object[] GetSessionList ();
	public native Object   GetSession (String sessionId);
	public native int      DeleteSession (String sessionId, int deleteMessageLogs);
	public native int      DeleteSessionList (Object sessionList, int deleteMessageLogs);
	public native int      ClearSession (String sessionId);
	public native int      SetSessionOption (int optionType, Object sessionParams);
	public native int      SetSessionListReaded (Object sessionList);
	public native int      GetTotalUnreadNum ();
	public native int      UpdateDraft (String sessionId, String draftContent);
	public native Object[] GetMessageList (String sessionId, String lastMsgId, int pageSize);
	public native Object   GetMessage (String sessionId, String msgId, int updateUnreadNum);
	public native int      DeleteMessage (String sessionId, String msgId);
	public native int      SetAudioPlayed (String sessionId, String msgId);
	public native Object[] GetGivenFieldList (String sessionId, int givenField, int givenCondition);
	public native Object[] GetOfflineMessageList (String sessionId);

	//group
	public native int      OperateGroup (Object reqParams);
	public native Object[] GetGroupList ();
	public native Object   GetGroupInfo (String groupID);
	public native Object[] GetGroupMemberList (String groupID);
	public native Object   GetGroupMemberInfo (String memberID, String groupID,int loadFromServer);

	// buddy
	public native Object   GetBuddy (String userId, int loadFromServer);
	public native Object[] GetBuddyList ();
	public native int      AddBuddy (String userId);
	public native int      ModifyBuddy (Object buddyParams);
	public native int      DeleteBuddy (String userId);
	public native int      IsBuddyExist (String userId);
	public native int      SearchContact (String key);
	public native Object[] GetSearchContact ();
	public native Object   GetContact (String userId, int loadFromServer);

	// fans
	public native Object   GetFans (String userId, int loadFromServer);
	public native Object[] GetFansList ();
	public native int      GetNewFansCount();
	public native int      ClearNewFansCount();
	public native int      GetSnsCount(int type);
	public native int      ClearSnsCount(int type);
	
	static {
		System.loadLibrary("imsdk-jni");
	}
	
	public static void test1() { 
		Log.i("libim-jni.so", "test1"); 
	}
}
