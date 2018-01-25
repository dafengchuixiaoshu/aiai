//
//  IMPackageEngine.h
//  imtest
//
//  Created by zhangyang on 14-7-11.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CIMI.h"
#import "IBase.h"

/*
 网络监听
 */
typedef NS_ENUM(NSInteger, IM_NETWORK_STATUSTYPE) {
    IM_NST_NULL = 0,//没网
    IM_NST_WIFI = 1,//wifi
    IM_NST_NETWORK = 2//network
};

// 连接状态类型
typedef NS_ENUM(NSInteger, IM_ConnectionStatusType)
{
    IM_CST_EXIST     = 1,  // 连接存在
    IM_CST_NOTEXIST  = 2   // 连接不存在
};

/*
 发送消息的类型
 */
typedef NS_ENUM(NSInteger, IM_CATEGORY_TYPE){
    IM_SENDMSG_TYPE_PLAIN_TEXT = 1,  //文本
	IM_SENDMSG_TYPE_RICH_TEXT  = 2,  //富文本
	IM_SENDMSG_TYPE_IMAGE      = 3,  //图片
	IM_SENDMSG_TYPE_AUDIO      = 4,  //语音
	IM_SENDMSG_TYPE_VIDEO      = 5   //视频
};

typedef NS_ENUM(NSInteger, IM_MESSAGE_TYPE){
	IM_MESSAGE_TYPE_PERSONAL = 1,   //个人消息
	IM_MESSAGE_TYPE_GROUP    = 2,   //群落消息
    IM_MESSAGE_TYPE_CUSTOM   = 3,   //客服消息
    IM_MESSAGE_TYPE_BUYER    = 4    //买手消息
};

typedef NS_ENUM(NSInteger, IM_KIND_TYPE){
	IM_KIND_TYPE_ORDINARY     = 1,  //普通消息
	IM_KIND_TYPE_NOTIFICATION = 2,  //通知消息
    IM_KIND_TYPE_DRAFT        = 3,  //草稿消息
};

typedef NS_ENUM(NSInteger, IM_DIRECTION_TYPE){
	IM_DIRECTION_TYPE_SEND  = 1, //发送
	IM_DIRECTION_TYPE_RECV  = 2  //接收
};

typedef NS_ENUM(NSInteger, IM_UNREADSTATUS_TYPE){
	IM_UNREADSTATUS_TYPE_UNREAD = 0, //未读
	IM_UNREADSTATUS_TYPE_READ   = 1  //已读
};

typedef NS_ENUM(NSInteger, IM_SENDSTATUS_TYPE){
	IM_SENDSTATUS_TYPE_UNSENT   = 0,    //未发送
	IM_SENDSTATUS_TYPE_SENT     = 1,    //已发送
	IM_SENDSTATUS_TYPE_SENTING  = 2,    //发送中
};

typedef NS_ENUM(NSInteger, IM_PlayedStatus) {
    IM_AUDIO_UNPALY     = 0,         //未播放
    IM_AUDIO_PLAYED     = 1        //已播放
};


typedef NS_ENUM(NSInteger, IM_MESSAGE_ONLINE){
	IM_MESSAGE_TYPE_OFFLINE     = 0,  //离线消息
	IM_MESSAGE_TYPE_ONLINE      = 1   //在线消息
};

// 草稿变化
typedef NS_ENUM(NSInteger, IM_DraftChanged)
{
    IM_DC_UNCHANGED = 0, // 草稿消息未变化
    IM_DC_CHANGED   = 1  // 草稿消息有变化
};

//操作群组
typedef NS_ENUM(NSInteger, IM_OperateGroup){
    IM_GROUP_ADDGROUP                       = 300,  //添加群组
    IM_GROUP_REMOVEGROUP                    = 301,  //删除群组
    IM_GROUP_MODIFYGROUP                    = 302,  //修改群组
    IM_GROUP_INVITEGROUP                    = 303,  //邀请
    IM_GROUP_GETGROUPLIST                   = 304,  //获取群组列表
    IM_GROUP_GETLOCALGROUPLIST              = 305,  //获取本地群组列表
    IM_GROUP_GETLOCALGROUPMEMBERLIST        = 306   //获取本地群组人员列表
};

typedef NS_ENUM(NSInteger, IM_GroupErrCode){
    IM_GROUP_CREAT_ERRCODE  = 513,      // 创建群群人数超出上限
    IM_GROUP_INVITE_ERRCODE = 521,      //邀请超出人数
};

typedef NS_ENUM(NSInteger, IM_SessionTopStatus){
    
	IM_SESSION_UNTOPPED     = 0,        // 未置顶
    IM_SESSION_TOP          = 1         // 置顶
};

typedef NS_ENUM(NSInteger, IM_SessionReceiveMessage){
    
	IM_SESSION_UNRECEIVE     = 0,        // 关闭
    IM_SESSION_RECEIVE          = 1         // 开启
};

//好友关系
typedef NS_ENUM(NSInteger, IM_IsBuddyType){
    IM_BUDDY_NOTBUDDY     = 0,  // 非好友（为前向兼容）
    IM_BUDDY_BUDDY        = 1,  // 好友
    IM_BUDDY_FANS         = 2,  // 粉丝
    IM_BUDDY_BUDDYFANS    = 3,  // 好友及粉丝
    IM_BUDDY_NOTBUDDYFANS = 4   // 非好友非粉丝
};

//添加黑名单
typedef NS_ENUM(NSInteger, IM_BlackListType){
    IM_BL_DELETE         = 0,    //删除
    IM_BL_ADD            = 1,    //添加
};

//iq类型
typedef NS_ENUM(NSInteger, IM_IqType){
	IM_IT_PG            = 1,    //群组
	IM_IT_CONTACT       = 2,    //联系人
    IM_IT_CUSTOMCHAT    = 3,    //客服
    IM_IT_DISCONNECT    = 4     //网络断线
};

// 消息的特定字段类型
typedef NS_ENUM(NSInteger, IM_GivenFieldType)
{
    IM_GFT_CONTENT = 0  // 消息内容字段
};

// 消息的特定条件类型
typedef NS_ENUM(NSInteger, IM_GivenConditionType)
{
    IM_GCT_IMAGE = 0  // 图片
};

typedef NS_ENUM(NSInteger, IM_SNSTYPE)
{
    IM_HUDONGCOUNT = 1 ,            //互动消息
    IM_RecommendationFriend = 2     //好友推荐
};

typedef NS_ENUM(NSInteger, IM_OPTIONTYPE)
{
    IM_UNRECEIVE    =   0,  //不接收推送
    IM_RECEIVE      =   1   //接收推送
};

//NSObject
@protocol IMPackageEngineDelegate;
@class IMPackageSessionData;
@class IMPackageBuddyData;
@class IMPackageGroupData;
@class IMPackageGroupMemberData;
@class IMPackageMessageData;
@class FriendData;
@class IMPackageIqData;
@class OfflineMessageData;
@interface IMPackageEngine : NSObject

//interface
+ (IMPackageEngine*)sharedInstanceIMPackageEngine;

//清除所有数据
-(void)clearIMPackageEngine;

//userName，必须设置，这个是im的入口
@property(nonatomic,copy)NSString* userName;

//好友的cmsid 需要它在回调的时候获取好友信息
@property(nonatomic,copy)NSString* buddyUserName;

//im连接状态
@property(nonatomic,readonly,assign)IM_ConnectionStatusType connectionType;

//添加代理
-(void)addDelegate:(id<IMPackageEngineDelegate>)delegate;
//移除代理
-(void)removeDelegate:(id<IMPackageEngineDelegate>)delegate;

/*
 网络监听
 */
-(void)IMPackage_NotifyNetwork:(IM_NETWORK_STATUSTYPE)networkType;
/*
 忽略SIGPIPE信号
 */
-(void)IMPackage_IgnoreSIGPIPE;
/*
 预设IM数据库路径
 */
- (void)IMPackage_Init;
/*
 return: -1标识没设置cmsid,其他标识正常请求
 */
-(int)IMPackage_Login;

/*注销*/
-(int)IMPackage_Logout;

/*断开连接*/
- (void)IMPackage_DisConnect;

/*释放*/
- (void)IMPackage_Release;

//重连
-(void)IMPackage_Reconnect;

//检查网络状态
-(IM_ConnectionStatusType)IMPackage_CheckConnection;

#pragma mark ---------- session
/*
 获取session列表,类型为IMPackageSessionData
 */
- (IMPackageSessionData*)IMPackage_GetSession:(NSString*)sessionID;
-(NSArray*)IMPackage_GetSessionList;//列表
-(int)IMPackage_RemoveSession:(NSString*)sessionID;//移除session
-(int)IMPackage_SetSessionToTopWithSessionID:(NSString*)sessionID AndSessionOptionType:(IM_SessionTopStatus)optionType;     //session置顶
-(int)IMPackage_SetSessionNewMsgRemindWithSessionID:(NSString*)sessionID AndSessionOptionType:(IM_SessionReceiveMessage)optionType;     //session 接收新消息
-(int)IMPackage_ClearSession:(NSString*)sessionID;  //清空session


#pragma mark -----------消息
/*
 receiverID:发送到的对方id
 type:发送得类型
 content:具体内容，文本和富文本是内容字符串，图片语音和视频是文件服务器的url
 ps:接受消息在回调函数中，是及时性的 msgType = IM_MsgType_MsgCenter 且父dic的key为kIM_MsgType_MsgCenter_Key，子key为kIM_MsgType_MsgCenter_recv_Key
 */

//普通发消息 senderId传nil  与客服聊天senderId 传 客服小号
//-(int)IMPackage_SendMessageToID:(NSString*)receiverID fromID:(NSString*)senderId sendType:(IM_CATEGORY_TYPE)type msgType:(IM_MESSAGE_TYPE)msgType content:(NSString*)content;

//发送一条图片消息
-(void)IMPackage_SendImgMsgWithToUser:(NSString*)toUser imgPath:(NSString*)imgPath textBuffer:(NSString*)text imgWidth:(int)width imgHeight:(int)height msgType:(IM_MESSAGE_TYPE)msgType;

//发送一段视频
-(void)IMPackage_SendVideoMsgWithToUser:(NSString*)toUser videoPath:(NSString*)videoPath faceImagePath:(NSString*)imgPath textBuffer:(NSString*)text videoTitle:(NSString*)title videoLength:(int)length msgType:(IM_MESSAGE_TYPE)msgType;
/*
 用来保存通知消息和草稿
 return 0:ok  其他:错误
 */
//-(NSString*)IMPackage_SaveMessage:(IMPackageMessageData*)packageMessageData;

//更新一条消息
//-(int)IMPackage_UpdateMessageWithMsgID:(NSString*)msgID SessionID:(NSString*)sessionID AndUpdateContent:(NSString*)content;

/*
 获取一条消息
 */
-(IMPackageMessageData*)IMPackage_GetOneMessageWithSessionID:(NSString*)sessionID msgID:(NSString*)msgID andIsNeedUpdateUnreadNum:(BOOL)needUpDate;

/*
 重发消息
 */
//-(int)IMPackage_ReSendMessageWithId:(NSString*)msgID andSectionID:(NSString*)sessionID;

/*
 删除一条消息
 */
-(int)IMPackage_DeleteOneMessageWithSessionID:(NSString*)sessionID msgID:(NSString*)msgID imageKey:(NSString*)imageKey vedioKey:(NSString*)vedioKey;

/*
 根据sessionid获取聊天记录列表，类型为IMPackageData
 */
- (NSArray*)IMPackage_GetMessageListWithSessionID:(NSString*)sessionID LastMsgID:(NSString*)msgId pageSize:(NSInteger)pageSize;


//更新草稿消息
- (void)IMPackage_UpdateDraftWithSessionID:(NSString*)sessionID AndMsgContent:(NSString*)msgContent;

//设置语音未已播放状态
-(int)IMPackage_SetAudioPlayerWithSessionID:(NSString*)sessionId AndMsgID:(NSString*)msgID;

//获取离线消息
-(NSArray*)IMPackage_GetOfflineMsgListWithSessionID:(NSString*)sessionID;

//获取消息列表内的 指定消息类型列表
-(NSArray*)IMPackage_GetGivenFieldListWithSessionID:(NSString*)sessionId IM_GivenFieldType:(IM_GivenFieldType)fieldType IM_GivenConditionType:(IM_GivenConditionType)conditionType;

//获取经纬度
-(void)setMapPointWithLongitude:(NSString*)longitude Latitude:(NSString*)latitude;

#pragma mark ------ group

// 创建一个群聊
-(void)IMPackage_AddNewGroupWithAddUserID:(NSString*)userID groupPortraitKey:(NSString*)groupPortrait addContactList:(NSArray*)addContactList;

//通过groupid获取群组信息
-(IMPackageGroupData*)IMPackage_getGroupInfoWithGroupId:(NSString*)groupID;


//获取群组成员列表
-(NSArray*)IMPackage_GetGroupMemberListWithGroupID:(NSString*)groupID;

// 获取成员信息
-(IMPackageGroupMemberData*)IMPackage_getGroupMemberInfoWithMemberId:(NSString*)memberID groupId:(NSString*)groupId isFromServer:(BOOL)fromServer;

//加人
-(int)IMPackage_InviteGroupWithGroupID:(NSString*)groupID WithInviterID:(NSString*)inviterId groupPortraitKey:(NSString*)groupPortrait andAddContactList:(NSArray*)addContactList;

//退出群组
-(int)IMPackage_leavingGroupWithUserID:(NSString*)userID andGroupID:(NSString*)groupID groupPortraitKey:(NSString*)groupPortrait;

//修改群名字
-(int)IMPackage_ModifyGroupNameWithGroupID:(NSString*)groupID WithModifyerID:(NSString*)modifyerID AndGroupName:(NSString*)groupName;

//修改群名片
-(int)IMPackage_ModifyMemberCardNameWithGroupID:(NSString*)groupID WithModifyerID:(NSString*)modifyerID AndCardName:(NSString*)cardName;

#pragma mark --------- 好友接口
//获取一个好友的信息
-(IMPackageBuddyData*)IMPackage_GetSingleBuddyWithUserID:(NSString*)userID NeedLoadFromServer:(BOOL)loadFromServer;

//获取联系人
-(IMPackageBuddyData*)IMPackage_GetContactWithUserId:(NSString*)userID isFromServer:(BOOL)fromServer;

//获取好友列表
-(NSArray*)IMPackage_GetBuddyList;

//获取粉丝列表
-(NSArray*)IMPackage_GetFansList;

//获取一个粉丝的信息
-(IMPackageBuddyData*)IMPackage_GetFansWithUserID:(NSString*)userID NeedLoadFromServer:(BOOL)loadFromServer;

//添加关注
-(int)IMPackage_AddBuddyWithUserID:(NSString*)userId;

//添加黑名单
-(void)IMPackage_SetBlackListWithId:(NSString*)buddyId IM_BlackListType:(IM_BlackListType)type;

//修改某个好友信息  ..这个接口没处用
-(int)IMPackage_ModifyBuddyWihBuddyData:(IMPackageBuddyData*)buddyData;

//取消关注
-(int)IMPackage_DeleteBuddyWihUserID:(NSString*)userID;

//搜索好友
-(int)IMPackage_SearchContactWithuserID:(NSString*)userID;

//获取搜索好友列表  搜索完成后会在回调里调用 只需取搜索后的回调数据即可
-(NSArray*)IMPackage_GetSearchList;

//判断是否 我已经关注了
-(BOOL)IMPackage_IsFriendWithUserID:(NSString*)userID;

//添加好友信息到本地数据库
-(void)IMPackage_AddContentWithUserID:(NSString*)userID;

//获取新粉丝个数
-(int)IMPackage_GetNewFansCount;
-(void)IMPackage_ClearFansCount;

//互动消息
-(int)IMpackage_GetHudongCount;
-(int)IMpackage_ClearHudongCount;
//推荐好友
-(int)IMPackage_GetRecommendationFriend;
-(int)IMPackage_ClearRecommendationFriend;

//推送消息
-(int)IMPackage_GetReceiveMessageOption;
-(long)IMPackage_SetReceiveMessageOption:(IM_OPTIONTYPE)option;

//获取所有未读消息个数
-(int)getMsgCount;

@end


/*
 IMPackageEngine的请求类型
 */
typedef NS_ENUM(NSInteger, IM_MsgType){
    IM_MsgType_Login,
    IM_MsgType_MsgCenter,
    IM_MsgType_Group,
    IM_MsgType_Buddy
};
/*
 取数据的时候使用的key
 */
static const NSString* kIM_MsgType_Login_Key =      @"kIM_MsgType_Login_Key";
static const NSString* kIM_MsgType_MsgCenter_Key =  @"kIM_MsgType_MsgCenter_Key";
static const NSString* kIM_MsgType_Group_Key =      @"kIM_MsgType_Group_Key";
static const NSString* kIM_MsgType_Buddy_Key =      @"kIM_MsgType_Buddy_Key";

/*
 delegate第二个参数的数值
 */
//登录子字段
static const NSInteger kIM_MsgType_Login_sucess_Key =   0;
static const NSInteger kIM_MsgType_Login_faild_Key =    1;
static const NSInteger kIM_MsgType_Login_noExist_Key =  2;

//消息子字段
static const NSInteger kIM_MsgType_MsgCenter_sucess_Key =   0;
static const NSInteger kIM_MsgType_MsgCenter_faild_Key =    1;
static const NSInteger kIM_MsgType_MsgCenter_recv_Key =     2;

//群落子字段
static const NSInteger kIM_MsgType_Group_addSuccess_Key =               0;
static const NSInteger kIM_MsgType_Group_joinSuccess_Key =              1;
static const NSInteger kIM_MsgType_Group_modifyNameSuccess_Key =        2;
static const NSInteger kIM_MsgType_Group_quitSuccess_Key =              3;
static const NSInteger kIM_MsgType_Group_getGroupList_Key =             4;
static const NSInteger kIM_MsgType_Group_notifyCreate_Key =             5;//收到邀请加入群通知
static const NSInteger kIM_MsgType_Group_notifyRefresh_Key =            6;//收到其他人加入群通知，刷新群成员列表
static const NSInteger kIM_MsgType_Group_notifyModifyGroupName_Key =    7;//收到修改群名称通知
static const NSInteger kIM_MsgType_Group_notifyQuitGroup_Key =          8;//通知退群
static const NSInteger kIM_MsgType_Group_addFailed_Key =                9;//群加人失败
static const NSInteger kIM_MsgType_Group_modifyMemberNameSuccess =      10;//修改群名片

//好友字段
static const NSInteger kIM_MsgType_Buddy_LOAD_BUDDY=            0;
static const NSInteger kIM_MsgType_Buddy_LOAD_BUDDY_LIST=       1;

@protocol IMPackageEngineDelegate <NSObject>

@optional
///*********************************************************      登 录 相 关      *********************************************************//
-(void)IMPackageLoginStatus:(int)detaileType;
-(void)IMPackageOpenLog:(BOOL)openLog;

///*********************************************************      群 组 相 关      *********************************************************//

/*
 参数说明:
    IMEngine:当前IMPackageEngine对象
    msgType:消息返回类型
    msgDic:详细数据，根据key值取不同的内容
 */
-(void)IMPackageMsgType:(IM_MsgType)msgType detaileType:(int)detaileType result:(id)result;

/*
 参数说明:
 detaileType:消息返回类型
 groupData:当前的Group
 */
- (void)IMPackageGroupDetaileType:(int)detaileType groupData:(IMPackageGroupData*)groupData;

//创建和邀请人失败
- (void)IMPackageNotifyGroupFaildWithtype:(IM_GroupErrCode)errType AndGroupMemCount:(int)memCount;

/*
 参数说明:
 detaileType:消息返回类型
 groupArray:所有群组的列表
 */
- (void)IMPackageGetAllGroupList:(NSArray*)groupArray;
- (void)IMPackageChangeGroupName:(int)detaileType;
- (void)IMPackageChangeCardName:(int)detaileType;

///*********************************************************      消 息 相 关      *********************************************************//

/*
 参数说明:
 detaileType:消息返回类型
 msgData:所有群组的列表
 */
- (void)IMPackageRecvMessageType:(int)detaileType msgData:(IMPackageMessageData*)msgModel;
- (void)IMPackageSendMsgSuccessWithMsgModel:(IMPackageMessageData*)msgModel;
- (void)IMPackageSendMsgFailWithMsgModel:(IMPackageMessageData*)msgModel;
- (void)IMPackageSendMsgProcessWithMsgModel:(IMPackageMessageData*)msgModel;
//收到客服小号
-(void)IMPackage_ReSendCustomMessageWithIQCustomData:(IMPackageIqData*)data;
//收到离线消息
-(void)IMPackage_RecevieOfflineDataWithOfflineModel:(OfflineMessageData*)data;

- (void)IMPackage_NetWork_Failure;


///*********************************************************      联 系 人 相 关   *********************************************************//
-(void)IMPackageGetBuddyDataSuccess:(IMPackageBuddyData*)buddyData;

- (void)IMPackageBuddyListDataHasChange:(RspType)changeType;

//获取粉丝接口
-(void)IMPackageGetFansDataSuccess:(IMPackageBuddyData*)buddyData;

- (void)IMPackageFansListDataHasChange:(RspType)changeType;

//获取搜索后的联系人列表
-(void)IMPackage_GetSearchList:(NSArray*)searchList;

@end

@interface IMPackageEngine (PraviteFuntion)

/*
 获取群组列表，只能在收到kIM_MsgType_Group_getGroupList_Key消息之后调用，否则为空
 */
-(NSArray*)IMPackage_getGroupList;

//通知所有代理新消息到了
-(void)notifyAllDelegate:(IM_MsgType)msgType detaileType:(int)detaileType result:(id)data;

///*********************************************************      登 录 相 关      *********************************************************//
-(void)notifyAllDelegateWithdetaileType:(int)detaileType;

///*********************************************************      群 组 相 关      *********************************************************//

//通知显示群组列表界面，有新的群组创建了
- (void)notifyGroupDetaileType:(int)detaileType groupData:(IMPackageGroupData*)groupData;

//通知群组创建 或 邀请人失败
-(void)notifyGroupFailureType:(IM_GroupErrCode)errType AndGroupMemCount:(int)memCount;
//通知显示群组列表界面，已经获取到了所有群组列表
- (void)notifyAllGroupList:(NSArray*)groupArray;

//通知修改群名
- (void)notifyChangeGroupName:(int)detaileType;

//通知修改群名片
- (void)notifyChangeCardName:(int)detaileType;

///*********************************************************      消 息 相 关      *********************************************************//
//成功收到消息
- (void)notifyRecvMessageType:(int)detaileType msgData:(IMPackageMessageData*)msgModel;
//成功发送消息
- (void)notifySendMessageSuccessWithMsgModel:(IMPackageMessageData*)msgModel;
//发送消息失败
- (void)notifySendMessageFailWithMsgModel:(IMPackageMessageData*)msgModel;
//消息发送中
- (void)notifySendMessageProcessWithMsgModel:(IMPackageMessageData*)msgModel;

//获取客服小号
-(void)notifyReceiveCustomerIdWithModel:(IMPackageIqData*)iqData;
//接收离线消息
- (void)notifyOfflineMsgListWithOfflineData:(OfflineMessageData*)offlineData;

- (void)notifyNetWork_Failure;

///*********************************************************      联 系 人 相 关   *********************************************************//
- (void)notifyLoadSingleBuddySuccessWithBuddyData:(IMPackageBuddyData*)buddyData;
- (void)notifyLoadSingleFansSuccessWithBuddyData:(IMPackageBuddyData*)buddyData;
- (void)notifyBuddyListHasChange:(RspType)changeType;
- (void)notifyFansListHasChange:(RspType)changeType;

- (void)notifySearchListWithDataArray:(NSArray*)searchList;
@end


