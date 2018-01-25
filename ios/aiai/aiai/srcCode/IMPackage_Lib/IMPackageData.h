//
//  IMPackageData.h
//  imtest
//
//  Created by zhangyang on 14-7-12.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IMPackageEngine.h"

@class IMPackageBuddyData;
@class IMPackageMessageData;
/*
 消息中心的session数据结构
 */
@interface IMPackageSessionData : NSObject

@property(nonatomic,copy)NSString* pPortraitKey;            // 头像Key
@property(nonatomic,copy)NSString* pMsgId;                  // 消息ID
@property(nonatomic,copy)NSString* pSessionId;              // 会话ID
@property(nonatomic,copy)NSString* pSessionName;            // 会话名
@property(nonatomic,assign)NSInteger nUnreadNum;            // 未读消息个数
@property(nonatomic,assign)IM_MESSAGE_TYPE eType;           // 消息类型（1:个人消息,2:群组消息）
@property(nonatomic,assign)BOOL eTop;                       // 置顶状态（0:未置顶,1:已置顶）
@property(nonatomic,assign)BOOL eRemind;                    // 新消息提醒(0:关闭,1:开启)
@property(nonatomic,copy)NSString *pDraftContent;           //草稿内容
@property(nonatomic,assign)IM_DraftChanged eDraftChanged;   //草稿是否有变化

-(id)initWithSessionParams:(SessionParams*)params;

-(id)initWithIMPackageBuddyData:(IMPackageBuddyData *)buddyData;

-(id)initWithIMPackageMessageData:(IMPackageMessageData *)msgData;

-(id)initWithIMPackageGroupData:(IMPackageGroupData *)groupData;

@end

/*
 消息中心的聊天数据结构
 */
@interface IMPackageMessageData : NSObject

@property(nonatomic,copy)NSString* pMsgId;
@property(nonatomic,copy)NSString* pSessionId;
@property(nonatomic,copy)NSString* pSender;
@property(nonatomic,copy)NSString* pSenderKey;
@property(nonatomic,copy)NSString* pSenderName;
@property(nonatomic,copy)NSString* pReceiver;
@property(nonatomic,copy)NSString* ptext;
@property(nonatomic,copy)NSString* ptitle;      //语音的title
@property(nonatomic,copy)NSString* pImgKey;
@property(nonatomic,copy)NSString* pVideoKey;
@property(nonatomic,copy)NSString* pTime;
@property(nonatomic,assign)int pIsSave;         //是否可保存         可保存（1：可分享，其他不分享）
@property(nonatomic,assign)int pIsShare;        //是否可分享         可分享（1：可分享，其他不分享）

//客户端扩充属性
@property(nonatomic, assign)double longitude;       //发送消息时的经度
@property(nonatomic, assign)double latitude;       //发送消息时的纬度
@property(nonatomic, assign)CGSize pImageSize;      //图片尺寸
@property(nonatomic,assign)int pVideoLength;        //语音时长
@property(nonatomic,assign)int pAudioLength;        //语音时长
@property(nonatomic, copy)NSString* pGoodsName;     //商品名称
@property(nonatomic, copy)NSString* pGoodsImgUrl;   //商品图片
@property(nonatomic, copy)NSString* pGoodsPrice;    //商品价格
@property(nonatomic, copy)NSString* pGoodsSeriesID; //商品系列ID
@property(nonatomic, assign)int isOnSale;           //商品上下架状态 1或4为上架

@property(nonatomic,assign)IM_MESSAGE_TYPE eType;
@property(nonatomic,assign)IM_KIND_TYPE eKind;
@property(nonatomic,assign)IM_CATEGORY_TYPE eCategory;
@property(nonatomic,assign)IM_DIRECTION_TYPE eDirection;
@property(nonatomic,assign)IM_UNREADSTATUS_TYPE eUnread;        //已读未读
@property(nonatomic,assign)IM_SENDSTATUS_TYPE eSend;
@property(nonatomic,assign)IM_PlayedStatus ePlay;
@property(nonatomic,assign)IM_MESSAGE_ONLINE eonLine;       //是否是连线消息



-(id)initWithMessageParams:(MessageParams*)params;
-(MessageParams)getMessageParams;

@end

//离线消息的数据格式
@interface OfflineMessageData : NSObject
@property(nonatomic, assign) int pOfflineMsgCount;
@property(nonatomic, assign) int pSessionCount;
@property(nonatomic, strong) NSArray *psessionIDs;
@property(nonatomic, strong) NSMutableDictionary *pMsgListDic;

-(id)initWithOfflineMessageParams:(OfflineMessageParams*)params;
@end

/*
 一个群成员数据
 */
@interface IMPackageGroupMemberData : NSObject

@property(nonatomic,copy)NSString* memberUserName;    // 成员用户名
@property(nonatomic,copy)NSString* memberCMSID; // 成员cmsID
@property(nonatomic,copy)NSString* memberNickName;  //成员昵称
@property(nonatomic,copy)NSString* GroupCardName;//群名片
@property(nonatomic,copy)NSString* portraitKey; //头像
@property(nonatomic,copy)NSString* videoKey; //头像
@property(nonatomic,copy)NSString* EmotionMood; //心情短语

-(id)initWithGroupMemberParams:(MemberParams*)params;
-(MemberParams)getGroupMemberParams;

@end

/*
 群组的数据结构
 */
@interface IMPackageGroupData : NSObject

@property(nonatomic,copy)NSString* groupID;// 群ID
@property(nonatomic,copy)NSString* groupName;//群名称
@property(nonatomic,copy)NSString* groupPortraitKey; //群头像
@property(nonatomic,copy)NSString* ownerID;//群主ID
@property(nonatomic,copy)NSString* groupMemberCount; //群成员个数
@property(nonatomic,copy)NSString* groupMemberMax;//群最大上限
@property(nonatomic,copy)NSString* groupCardName;//群名片
@property(nonatomic,strong)IMPackageGroupMemberData* groupMemberData;//群主
@property(nonatomic,strong)NSArray* groupMemberList;//列表

-(id)initWithGroupParams:(GroupParams*)params;

@end

/*好友数据*/
@interface IMPackageBuddyData : NSObject

@property(nonatomic,copy)NSString *buddyCMSID;      // CMSID
@property(nonatomic,copy)NSString *buddyUserName;         // 联系人ID
@property(nonatomic,copy)NSString *buddyNickName;       //联系人名字
@property(nonatomic,copy)NSString *buddyEmail;          //好友邮箱
@property(nonatomic,copy)NSString *buddyMobile;         //好友手机号
@property(nonatomic,copy)NSString *buddyPortraitKey;    //头像Key
@property(nonatomic,copy)NSString *buddyVideoKey;       //头像视频
@property(nonatomic,copy)NSString *buddySubname;        //备注名
@property(nonatomic,copy)NSString *buddyIsSave;         //是否可保存         可保存（1：可分享，其他不分享）
@property(nonatomic,copy)NSString *buddyIsShare;        //是否可分享         可分享（1：可分享，其他不分享）
@property(nonatomic,copy)NSString *buddyEmotionMood;    //心情短语
@property(nonatomic,copy)NSString *buddyQrerUrl;        //圈儿URL
@property(nonatomic,copy)NSString *buddyCommunityUrl;   //群落URL
@property(nonatomic,assign)IM_IsBuddyType buddyIsBuddy;           //是否是好友

-(id)initWithBuddyParams:(BuddyParams*)BuddyParams;

//排序
+(NSDictionary*)firstLetterContactDictionaryWithBuddyList:(NSArray*)buddyList;
+(NSArray*)firstLetterArray:(NSDictionary*)contactBuddyDic;
@end

//IQ结构体
@interface IMPackageIqData : NSObject
@property(nonatomic, copy) NSString* pSmallNum;         // 小号
@property(nonatomic, copy) NSString* pBody;             // Iq体
@property(nonatomic, copy) NSString* pTalkId;           // 商品ID
@property(nonatomic, assign) IM_IqType etype;           //iq类型

-(id)initWithIqParams:(IqParams*)iQParams;
@end


