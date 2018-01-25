    //
//  IMPackageEngine.m
//  imtest
//
//  Created by zhangyang on 14-7-11.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "IMPackageEngine.h"
#import "IMPackageData.h"
#import "IMPackageGlobal.h"
#import <AudioToolbox/AudioToolbox.h>

#pragma mark IM相关所有回调
class UILoginObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify)
	{
		RspParams* rspParams = (RspParams*)pNotify;
		enum RspType eRspType = rspParams->eRspType;
		switch (eRspType)
		{
            case ST_LOGIN_SUCCESS:
            {
                // 如果本地无缓存，是第一次登陆需要手动调用这2接口，把所有缓存下载到本地。返回值和回调在这里无需处理。
//                [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetBuddyList]; //现在不需要 登录之后load联系人和群组了
//                [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_getGroupList];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyAllDelegateWithdetaileType:kIM_MsgType_Login_sucess_Key];
            }
                break;
            case ST_LOGIN_FAILURE:
            {
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyAllDelegateWithdetaileType:kIM_MsgType_Login_faild_Key];
            }
                break;
            case ST_NETWORK_FAILURE:
            {
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyNetWork_Failure];
            }
                break;
            case ST_LOGIN_ELSEWHERE:
			{
                dispatch_async(dispatch_get_main_queue(), ^{
//                    [LoginHandler requestLogoutWithReady:^{
//                        
//                    } csuccess:^(id obj) {
//                        [LoginMsgCenter loginResultStatus:LoginStatus_Logout loginType:LoginType_Logout loginEntity:nil loginSucessUserinfo:nil];
//                    } failed:^(id obj) {
//                        
//                    }];
//                    ALERT(@"您的海选账号已在其它设备登录，请注意您的账号安全。如果这不是您的操作，您的海选密码可能已经泄露，建议您尽快进行修改密码的操作。");
                });
			}
                break;
            case ST_LOGIN_VERSIONLOW:
            {
//                ALERT(@"版本过低,建议升级使用,否则可能发生未知问题");
            }
                break;
            default:
                break;
		}
		return S_OK;
	}
};

class UIMessageObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify)
	{
		RspParams* pRspParams = (RspParams*)pNotify;
		enum RspType eRspType = pRspParams->eRspType;
		switch (eRspType)
		{
            case ST_SEND_MESSAGE_SUCCESS:
            {
                MessageParams * pMessageParams = (MessageParams*)pRspParams->pMessageParams;
                
                NSString *sessionID = [NSString stringWithUTF8String:pMessageParams->pSessionId.c_str()];
                NSString *msgID = [NSString stringWithUTF8String:pMessageParams->pMsgId.c_str()];
                IMPackageMessageData *model = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetOneMessageWithSessionID:sessionID msgID:msgID andIsNeedUpdateUnreadNum:NO];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifySendMessageSuccessWithMsgModel:model];
            }
                break;
            case ST_SEND_MESSAGE_PROCESS:   //发送中
            {
                MessageParams * pMessageParams = (MessageParams*)pRspParams->pMessageParams;
                IMPackageMessageData* model = [[IMPackageMessageData alloc] initWithMessageParams:pMessageParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifySendMessageProcessWithMsgModel:model];
            }
                break;
            case ST_SEND_MESSAGE_FAILURE:
            {
                MessageParams * pMessageParams = (MessageParams*)pRspParams->pMessageParams;
                IMPackageMessageData* model = [[IMPackageMessageData alloc] initWithMessageParams:pMessageParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifySendMessageFailWithMsgModel:model];
            }
                break;
                
            case ST_RECV_MESSAGE:
			{
				MessageParams * pMessageParams = (MessageParams*)pRspParams->pMessageParams;
                IMPackageMessageData* data = [[IMPackageMessageData alloc] initWithMessageParams:pMessageParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyRecvMessageType:kIM_MsgType_MsgCenter_recv_Key msgData:data];
                
                IMPackageSessionData *sessionData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSession:data.pSessionId];
                if (sessionData.eRemind) {
                    [IMPackageGlobal ringtone];
                    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
                }
			}
                break;
            case ST_RECV_CUSTOM_SMALLNUM:
            {
                IqParams * pCustomParams = (IqParams*)pRspParams->pIqParams;
                IMPackageIqData* iqData = [[IMPackageIqData alloc] initWithIqParams:pCustomParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyReceiveCustomerIdWithModel:iqData];
            }
                break;
            case ST_RECV_OFFLINE_MESSAGE:
            {
                OfflineMessageParams *offLineData = (OfflineMessageParams*)pRspParams->pOfflineMessageParams;
                OfflineMessageData *pMsgData = [[OfflineMessageData alloc]initWithOfflineMessageParams:offLineData];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyOfflineMsgListWithOfflineData:pMsgData];
                //判断 是否有需要提醒的session, 如果有震动 没有就不震动
                BOOL ring = NO;
                for (NSString *sessionID in pMsgData.psessionIDs) {
                    IMPackageSessionData *sessionData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSession:sessionID];
                    if (sessionData.eRemind) {
                        ring = YES;
                        break;
                    }
                }
                if (ring) {
                    [IMPackageGlobal ringtone];
                    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
                }

            }
                break;
            default:
                break;
		}
        
		return S_OK;
	}
};

class UIGroupObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify)
	{
		RspParams* pRspParams = (RspParams*)pNotify;
        id groupData = nil;
		enum RspType eRspType = pRspParams->eRspType;
        enum RspErrCode errcode = pRspParams->errcode;
        int extendcode = pRspParams->extendcode;
		switch (eRspType)
		{
			case ST_ADD_GROUP_SUCCESS:
            {
                groupData = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyGroupDetaileType:kIM_MsgType_Group_addSuccess_Key groupData:groupData];
			}
				break;
            case ST_ADD_GROUP_FAILURE:
            {
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyGroupFailureType:(IM_GroupErrCode)errcode AndGroupMemCount:extendcode];
            }
                break;
			case ST_JOINGRUOP_SUCCESS:
            {
                groupData = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyGroupDetaileType:kIM_MsgType_Group_notifyCreate_Key groupData:groupData];
			}
				break;
            case ST_JOINGROUP_FAILURE:
            {    
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyGroupFailureType:(IM_GroupErrCode)errcode AndGroupMemCount:extendcode];
            }
                break;
			case ST_MODIFY_GROUPNAME_SUCCESS:
            {
                groupData = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyChangeGroupName:kIM_MsgType_Group_modifyNameSuccess_Key];
			}
				break;
            case ST_MODIFY_GROUPNAME_FAILURE:
            {
                AIAI_LOG(@"修改群名称失败");
            }
                break;
            case ST_MODIFY_GROUPMEMBERNAME_SUCCESS:
            {
                groupData = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyChangeCardName:kIM_MsgType_Group_modifyMemberNameSuccess];
            }
                break;
            case ST_MODIFY_GROUPMEMBERNAME_FAILURE:
            {
                AIAI_LOG(@"修改群名片失败");
            }
                break;
			case ST_QUIT_GROUP_SUCCESS:
			{
                groupData = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyGroupDetaileType:kIM_MsgType_Group_quitSuccess_Key groupData:groupData];
			}
				break;
			case ST_GETGROUPLIST_SUCCESS:
			{
                //新库不需要 自己处理逻辑了
//                IMPackageEngine *engine = [IMPackageEngine sharedInstanceIMPackageEngine];
//                NSArray *groupArray = [engine IMPackage_getGroupList];
//                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyAllGroupList:groupArray];
			}
                break;
			case ST_NOTIFY_CREATE_GROUP:
			{
				IMPackageGroupData *data = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyGroupDetaileType:kIM_MsgType_Group_notifyCreate_Key groupData:data];
			}
                break;
			case ST_NOTIFY_REFRESH_GROUP:
			{
				groupData = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyAllDelegate:IM_MsgType_Group detaileType:kIM_MsgType_Group_notifyRefresh_Key result:groupData];
			}
                break;
			case ST_NOTIFY_MODIFY_GROUP:
			{
				groupData = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyAllDelegate:IM_MsgType_Group detaileType:kIM_MsgType_Group_notifyModifyGroupName_Key result:groupData];
			}
                break;
			case ST_NOTIFY_QUIT_GROUP:
			{
                groupData = [[IMPackageGroupData alloc] initWithGroupParams:pRspParams->pGroupParams];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyAllDelegate:IM_MsgType_Group detaileType:kIM_MsgType_Group_notifyQuitGroup_Key result:groupData];
			}
                break;
            case ST_NETWORK_FAILURE:
			{
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyGroupDetaileType:kIM_MsgType_Group_addFailed_Key groupData:groupData];
			}
                break;
			default:
            {
                
            }
		}
        return 0;
	}
};

class UIBuddyObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify)
	{
		RspParams* rspParams = (RspParams*)pNotify;
		enum RspType eRspType = rspParams->eRspType;
        RspType detaileType;
		switch (eRspType)
		{
            case ST_LOAD_BUDDY:
            {
                
                IMPackageBuddyData *memberData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetContactWithUserId:[IMPackageEngine sharedInstanceIMPackageEngine].buddyUserName isFromServer:NO];
                
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyLoadSingleBuddySuccessWithBuddyData:memberData];
                return S_OK;
            }
                break;
            case ST_LOAD_BUDDY_LIST:
            {
                detaileType = ST_LOAD_BUDDY_LIST;
            }
                break;
            case ST_NOTIFY_INVITE_BUDDY_AGREE:
            {
                detaileType = ST_NOTIFY_INVITE_BUDDY_AGREE;
            }
                break;
            case ST_NOTIFY_INVITE_BUDDY:
            {
                detaileType = ST_NOTIFY_INVITE_BUDDY;
            }
                break;
            case ST_NOTIFY_DELETE_BUDDY:
            {
                detaileType = ST_NOTIFY_DELETE_BUDDY;
            }
                break;
            case ST_SEARCH_BUDDY_SUCCESS:
            {
                detaileType = ST_SEARCH_BUDDY_SUCCESS;
                NSArray *searchList = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSearchList];
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifySearchListWithDataArray:searchList];
            }
                break;
            case ST_SEARCH_BUDDY_FAILURE:
            {
                detaileType = ST_SEARCH_BUDDY_FAILURE;
            }
                break;
            case ST_ADD_BUDDY_SUCCESS:      //添加关注成功
            {
                detaileType = ST_ADD_BUDDY_SUCCESS;
            }
                break;
            case ST_ADD_BUDDY_FAILURE:       //添加关注失败
            {
                detaileType = ST_ADD_BUDDY_FAILURE;
            }
                break;
            case ST_DELETE_BUDDY_SUCCESS:
            {
                detaileType = ST_DELETE_BUDDY_SUCCESS;  //取消关注成功
            }
                break;
            case ST_DELETE_BUDDY_FAILURE:
            {
                detaileType = ST_DELETE_BUDDY_SUCCESS;  //取消关注失败
            }
                break;
            case ST_NOTIFY_HUDONG_NEWS:
            {
                detaileType = ST_NOTIFY_HUDONG_NEWS;
            }
                break;
            default:
            {
                printf("Failure");
            }
                return S_FALSE;
		}
        
        [[IMPackageEngine sharedInstanceIMPackageEngine] notifyBuddyListHasChange:detaileType];
        
        return S_OK;
	}
};

//粉丝的 监听
class UIFansObserver : public IObserver
{
    virtual HRESULT OnNotify (VOID* pNotify)
    {
        RspParams* rspParams = (RspParams*)pNotify;
        enum RspType eRspType = rspParams->eRspType;
        RspType detaileType;
        switch (eRspType)
        {
            case ST_LOAD_BUDDY:
            {
                IMPackageBuddyData *memberData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetFansWithUserID:[IMPackageEngine sharedInstanceIMPackageEngine].buddyUserName NeedLoadFromServer:NO];
                
                [[IMPackageEngine sharedInstanceIMPackageEngine] notifyLoadSingleFansSuccessWithBuddyData:memberData];
                return S_OK;
            }
                break;
            case ST_LOAD_FANS_LIST:
            {
                detaileType = ST_LOAD_FANS_LIST;
            }
                break;
            case ST_NOTIFY_ADD_FANS:
            {
                detaileType = ST_NOTIFY_ADD_FANS;
            }
                break;
            case ST_NOTIFY_DELETE_FANS:
            {
                detaileType = ST_NOTIFY_DELETE_FANS;
            }
                break;
            case ST_NOTIFY_MODIFY_FANS:
            {
                detaileType = ST_NOTIFY_MODIFY_FANS;
            }
                break;
                
            default:
            {
                
            }
                return S_FALSE;
                break;
        }
        
        [[IMPackageEngine sharedInstanceIMPackageEngine] notifyFansListHasChange:detaileType];
        
        return S_OK;
    }
};

@interface IMPackageEngine () {
    CIMI* pCimiObj;
    UILoginObserver     *pUILoginObserver;
    UIMessageObserver   *pUIMessageObserver;
    UIGroupObserver     *pUIGroupObserver;
    UIBuddyObserver     *pUIBuddyObserver;
    UIFansObserver      *pUIFansObserver;
    
    NSMutableSet* delegateSet;
    
    dispatch_queue_t queue;
    
    BOOL isLoginSucess;
}
@property(nonatomic,readwrite,assign)IM_ConnectionStatusType connectionType;

@end

@implementation IMPackageEngine
@synthesize userName = _userName;

+ (IMPackageEngine*)sharedInstanceIMPackageEngine
{
    static dispatch_once_t pred = 0;
    __strong static IMPackageEngine* sharedObject = nil;
    dispatch_once(&pred, ^{
        sharedObject = [[IMPackageEngine alloc] init];
    });
    
    return sharedObject;
}

- (id)init {
    if (self = [super init]) {
        delegateSet = [[NSMutableSet alloc] init];
        
        queue = dispatch_queue_create("hx.im.engine", DISPATCH_QUEUE_SERIAL);
    }
    return self;
}

- (void)initElement {
    if (NULL == pCimiObj) {
        pCimiObj = CIMI::GetInstance();
        NSString* path = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/CMMP"];
        pCimiObj->SetRootDir([path UTF8String]);
        pCimiObj->SetServerAddr(CMMP_BASE_IP, CMMP_BASE_PORT);
        //savePath
        NSString *path2 = [NSHomeDirectory() stringByAppendingString:@"/Library/IM_savePath/"];
        [[NSFileManager defaultManager] createDirectoryAtPath:path2 withIntermediateDirectories:YES attributes:nil error:nil];
        pCimiObj->SetSavePath([path2 UTF8String]);
        pCimiObj->SetFileServer(CMMP_IMG_IP,CMMP_IMG_PORT);
        //set观察者
        pUILoginObserver = new UILoginObserver();
        pUIMessageObserver = new UIMessageObserver();
        pUIGroupObserver = new UIGroupObserver();
        pUIBuddyObserver = new UIBuddyObserver();
        pUIFansObserver = new UIFansObserver();
        
        //set自己需要的数据
        isLoginSucess = NO;
    }
}

-(void)setUserName:(NSString *)userName {
    if (nil == userName) {
        return;
    }
    
    _userName = [userName copy];
    
    pCimiObj->SetUserId([_userName UTF8String]);
}

- (void)clearIMPackageEngine {
    _userName = nil;
    for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
        [self removeDelegate:tempDel];
    }
}

- (void)IMPackage_DisConnect {
    pCimiObj->CloseConnection();
}

- (void)IMPackage_Release {
    self.userName = @"";

    pCimiObj->Release();
    pCimiObj = NULL;
    
    delete pUILoginObserver;
    pUILoginObserver = NULL;
    
    delete pUIMessageObserver;
    pUIMessageObserver = NULL;
    
    delete pUIGroupObserver;
    pUIGroupObserver = NULL;
    
    delete pUIBuddyObserver;
    pUIBuddyObserver = NULL;
    
    delete pUIFansObserver;
    pUIFansObserver = NULL;
}

- (void)dealloc {
    [self IMPackage_Release];
}

-(IM_ConnectionStatusType)IMPackage_CheckConnection{
    ConnectionStatusType type = pCimiObj->GetConnectionStatus();
    return (IM_ConnectionStatusType)type;
}

-(void)IMPackageOpenLog:(BOOL)openLog{
    pCimiObj->OpenLog(openLog);
}

#pragma mark 网络监听
-(void)IMPackage_NotifyNetwork:(IM_NETWORK_STATUSTYPE)networkType{
    pCimiObj->NotifyNetworkStatus((NetWorkStatusType)networkType);
}

-(void)IMPackage_IgnoreSIGPIPE{
    signal(SIGPIPE, SIG_IGN);
}

#pragma mark --Login

- (void)IMPackage_Init {
    [self initElement];
    
    pCimiObj->Init();
}

- (int)IMPackage_Login {
    
#ifdef AIAI_DEBUG
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackageOpenLog:YES];
#else
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackageOpenLog:YES];
#endif
    
    if ([self.userName length]) {
        pCimiObj->SetObserver(OT_LOGIN, pUILoginObserver);
        pCimiObj->SetObserver(OT_MESSAGE, pUIMessageObserver);
        pCimiObj->SetObserver(OT_GROUP, pUIGroupObserver);
        pCimiObj->SetObserver(OT_BUDDY, pUIBuddyObserver);
        pCimiObj->SetObserver(OT_FANS, pUIFansObserver);
        
        //登录放在子线程
        dispatch_async(queue, ^{
            ReqParams reqParams;
            reqParams.eReqType = RT_LOGIN;
            pCimiObj->Login(&reqParams);
        });
        
        return S_OK;
    }
    return S_FALSE;
}

//重新连接
-(void)IMPackage_Reconnect{
    ReqParams reqParams;
    reqParams.eReqType = RT_LOGIN;
    pCimiObj->Login(&reqParams);
}

- (int)IMPackage_Logout {
    if (!self.userName) {
        return S_FALSE;
    }
    ReqParams reqParams;
    reqParams.eReqType = RT_LOGOUT;
    pCimiObj->Logout(&reqParams);
    return S_OK;
}

#pragma mark --MessageCenter
/*
- (int)IMPackage_SendMessageToID:(NSString*)receiverID fromID:(NSString*)senderId sendType:(IM_CATEGORY_TYPE)type msgType:(IM_MESSAGE_TYPE)msgType content:(NSString*)content {
    if (!self.userName) {
        return S_FALSE;
    }
    if (isLoginSucess == NO) {
        AIAI_LOG(@"未登录");
        return S_FALSE;
    }
    // send message
    ReqParams reqParams;
	reqParams.eReqType = RT_SEND_MESSAGE;
	MessageParams messageParams = {};
    
    if (senderId) {
        messageParams.pSender = (CHAR*)[senderId UTF8String];
    }else{
        messageParams.pSender = (CHAR*)[[self userName] UTF8String];
    }
    if (IM_MESSAGE_TYPE_CUSTOM == msgType && senderId == nil){
        messageParams.pSender = (CHAR*)[@"" UTF8String];
    }
    
    if (![receiverID isKindOfClass:[NSString class]]) {
        receiverID = [NSString stringWithFormat:@"%@",receiverID];
    }
	messageParams.pReceiver = (CHAR*)[receiverID UTF8String];
    
    NSString* baseInfoString = [AppUtils base64StringEncode:content];
	messageParams.pContent = (CHAR*)[baseInfoString UTF8String];
	messageParams.eType = (MessageType)msgType;
	messageParams.eCategory = (CategoryType)type;
    messageParams.eKind = KT_ORDINARY;
	reqParams.pMessageParams = &messageParams;
	pCimiObj->SendMessage(&reqParams);
    //底层已经保存记录所以不需要保存了
    return S_OK;
}
*/
//发送一个图片消息
-(void)IMPackage_SendImgMsgWithToUser:(NSString*)toUser imgPath:(NSString*)imgPath textBuffer:(NSString*)text imgWidth:(int)width imgHeight:(int)height msgType:(IM_MESSAGE_TYPE)msgType{
    
    //fromUser
    NSString *fromUser = [IMPackageEngine sharedInstanceIMPackageEngine].userName;
    char *from = (CHAR*)malloc(strlen([fromUser UTF8String])+1);
    memset(from, 0, strlen([fromUser UTF8String])+1);
    strcpy(from, [fromUser UTF8String]);
    string from_user = from;
    
    //toUser
    if (!toUser) {
        AIAI_LOG(@"接收消息人id错误");
        return;
    }
    char *to = (CHAR*)malloc(strlen([toUser UTF8String])+1);
    memset(to, 0, strlen([toUser UTF8String])+1);
    strcpy(to, [toUser UTF8String]);
    string to_user = to;
    
    //imgPath
    if (!imgPath) {
        AIAI_LOG(@"消息路径错误");
        return;
    }
    char *path = (CHAR*)malloc(strlen([imgPath UTF8String])+1);
    memset(path, 0, strlen([imgPath UTF8String])+1);
    strcpy(path, [imgPath UTF8String]);
    string img_path = path;
    
    //text
    if (!text) text = @"";
    char *sendText = (CHAR*)malloc(strlen([text UTF8String])+1);
    memset(sendText, 0, strlen([text UTF8String])+1);
    strcpy(sendText, [text UTF8String]);
    string send_text = sendText;
    
    //imgSize
    if (width == 0) {
        width = 480;
    }
    if (height == 0) {
        height = 480;
    }
    
    pCimiObj->SendImage(from_user, to_user, img_path, send_text, width, height, msgType);
    
}

//发送一个视频消息
-(void)IMPackage_SendVideoMsgWithToUser:(NSString*)toUser videoPath:(NSString*)videoPath faceImagePath:(NSString*)imgPath textBuffer:(NSString*)text videoTitle:(NSString*)title videoLength:(int)length msgType:(IM_MESSAGE_TYPE)msgType{
    
    //fromUser
    NSString *fromUser = [IMPackageEngine sharedInstanceIMPackageEngine].userName;
    char *from = (CHAR*)malloc(strlen([fromUser UTF8String])+1);
    memset(from, 0, strlen([fromUser UTF8String])+1);
    strcpy(from, [fromUser UTF8String]);
    string from_user = from;
    
    //toUser
    if (!toUser) {
        AIAI_LOG(@"接收消息人id错误");
        return;
    }
    char *to = (CHAR*)malloc(strlen([toUser UTF8String])+1);
    memset(to, 0, strlen([toUser UTF8String])+1);
    strcpy(to, [toUser UTF8String]);
    string to_user = to;
    
    //videoPath
    if (!videoPath) {
        AIAI_LOG(@"消息路径错误");
        return;
    }
    char *path = (CHAR*)malloc(strlen([videoPath UTF8String])+1);
    memset(path, 0, strlen([videoPath UTF8String])+1);
    strcpy(path, [videoPath UTF8String]);
    string video_path = path;
    
    //imgPath
    if (!imgPath) {
        AIAI_LOG(@"消息路径错误");
        return;
    }
    char *imagepath = (CHAR*)malloc(strlen([imgPath UTF8String])+1);
    memset(imagepath, 0, strlen([imgPath UTF8String])+1);
    strcpy(imagepath, [imgPath UTF8String]);
    string img_path = imagepath;
    
    //text
    if (!text) text = @"";
    char *sendText = (CHAR*)malloc(strlen([text UTF8String])+1);
    memset(sendText, 0, strlen([text UTF8String])+1);
    strcpy(sendText, [text UTF8String]);
    string send_text = sendText;
    
    //title
    if (!title) title = @"";
    char *sendTitle = (CHAR*)malloc(strlen([title UTF8String])+1);
    memset(sendTitle, 0, strlen([title UTF8String])+1);
    strcpy(sendTitle, [title UTF8String]);
    string send_title = sendTitle;
    
    pCimiObj->SendVideo(from_user, to_user, video_path, img_path, send_text, send_title, length, msgType);
    
}
/*
//重发消息
-(int)IMPackage_ReSendMessageWithId:(NSString*)msgID andSectionID:(NSString*)sessionID{
    
    ReqParams reqParams;
	reqParams.eReqType = RT_RESEND_MESSAGE;
    
    MessageParams pMessageParams = pCimiObj->GetMessage([sessionID UTF8String], [msgID UTF8String], NO);
    reqParams.pMessageParams = &pMessageParams;
    
    HRESULT hResult = pCimiObj->ResendMessage(&reqParams);
    if (hResult == S_OK) {
        return S_OK;
    }
    return S_FALSE;
}
*/
//更新草稿消息
- (void)IMPackage_UpdateDraftWithSessionID:(NSString*)sessionID AndMsgContent:(NSString*)msgContent{
    if (!sessionID || [sessionID isEqual:[NSNull class]] || !msgContent) {
        return;
    }
    pCimiObj->UpdateDraft([sessionID UTF8String], [msgContent UTF8String]);
    
}

/*
- (NSString*)IMPackage_SaveMessage:(IMPackageMessageData*)packageMessageData {
    MessageParams msgParams = [packageMessageData getMessageParams];
	NSString *msgID = [[NSString alloc]initWithUTF8String:pCimiObj->WriteMessage(msgParams).c_str()];
	if (msgID){
		return msgID;
	}
	return nil;
}
*/
/*
-(int)IMPackage_UpdateMessageWithMsgID:(NSString*)msgID SessionID:(NSString*)sessionID AndUpdateContent:(NSString*)content{
    MessageParams msgParams = pCimiObj->GetMessage([sessionID UTF8String], [msgID UTF8String], NO);
    NSString* baseInfoString = [AppUtils base64StringEncode:content];
    msgParams.pContent = (CHAR*)[baseInfoString UTF8String];
    
    HRESULT hResult = pCimiObj->UpdateMessage(msgParams);
    if (hResult == S_OK) {
        return S_OK;
    }
    return S_FALSE;
    
}
*/
- (IMPackageMessageData*)IMPackage_GetOneMessageWithSessionID:(NSString*)sessionID msgID:(NSString*)msgID andIsNeedUpdateUnreadNum:(BOOL)needUpDate {
//    if (!self.cmsId) {
//        return nil;
//    }
//    if (isLoginSucess == NO) {
//        return nil;
//    }
    
    MessageParams pMessageParams = pCimiObj->GetMessage([sessionID? :@"" UTF8String], [msgID? :@"" UTF8String], needUpDate);
    IMPackageMessageData* data = [[IMPackageMessageData alloc] initWithMessageParams:&pMessageParams];
    
    return data;
}

- (int)IMPackage_DeleteOneMessageWithSessionID:(NSString*)sessionID msgID:(NSString*)msgID  imageKey:(NSString*)imageKey vedioKey:(NSString*)vedioKey{
    if (!self.userName) {
        return S_FALSE;
    }
//    if (isLoginSucess == NO) {
//        return S_FALSE;
//    }
    
    HRESULT hResult2 = pCimiObj->DeleteMessage([sessionID UTF8String], [msgID UTF8String],[imageKey UTF8String],[vedioKey UTF8String]);
	if (hResult2 == S_OK){
#ifdef BadgeNumberZero
        [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
#else
        [UIApplication sharedApplication].applicationIconBadgeNumber -= 1;
#endif
		return S_OK;
	}
	return S_FALSE;
}


- (NSArray*)IMPackage_GetMessageListWithSessionID:(NSString*)sessionID LastMsgID:(NSString*)msgId pageSize:(NSInteger)pageSize {
//    if (!self.cmsId) {
//        return nil;
//    }
//    if (isLoginSucess == NO) {
//        return nil;
//    }
    if ([msgId isEmpty] || msgId == nil || [msgId isEqual:[NSNull null]]) {
        msgId = @"";
    }
    MessageList pMessageList = pCimiObj->GetMessageList([sessionID UTF8String], [msgId UTF8String], (int)pageSize);
    if (pMessageList.nCount > 0)
    {
        NSMutableArray* messageArray = [[NSMutableArray alloc] init];
        for (int i=0; i<pMessageList.nCount; i++)
        {
            MessageParams pMessageParams = pMessageList.ppMessageParams[i];
            IMPackageMessageData* data = [[IMPackageMessageData alloc] initWithMessageParams:&pMessageParams];
            [messageArray addObject:data];
        }
        return messageArray;
    }
    return nil;
}

//设置语音未已播放状态
-(int)IMPackage_SetAudioPlayerWithSessionID:(NSString*)sessionId AndMsgID:(NSString*)msgID{
    if (sessionId && msgID) {
        
        HRESULT hResult = pCimiObj->SetAudioPlayed([sessionId UTF8String], [msgID UTF8String]);
        if (hResult == S_OK) {
            return S_OK;
        }
        return S_FALSE;
    }
    return S_FALSE;
}

//获取离线消息
-(NSArray*)IMPackage_GetOfflineMsgListWithSessionID:(NSString*)sessionID{
    if (!sessionID) {
        return [NSArray array];
    }
    
    NSMutableArray *dataArray = [[NSMutableArray alloc] init];
    MessageList msgList = pCimiObj->GetOfflineMessageList([sessionID UTF8String]);
    for (int i = 0; i<msgList.nCount; i++) {
        MessageParams msgParams = msgList.ppMessageParams[i];
        IMPackageMessageData *data = [[IMPackageMessageData alloc] initWithMessageParams:&msgParams];
        [dataArray addObject:data];
    }
    return dataArray;
}

#pragma mark -- Session

- (IMPackageSessionData*)IMPackage_GetSession:(NSString*)sessionID {
    if (!self.userName) {
        return nil;
    }
    if (isLoginSucess == NO) {
        return nil;
    }
    
    SessionParams pSessionParams = pCimiObj->GetSession([sessionID UTF8String]);
    
    IMPackageSessionData* data = [[IMPackageSessionData alloc] initWithSessionParams:&pSessionParams];
    
    return data;
}

- (NSArray*)IMPackage_GetSessionList {
    if ([self.userName isEmpty]) {
        return nil;
    }
    
    SessionList pSessionList = pCimiObj->GetSessionList();
    if (pSessionList.nCount > 0)
    {
        
        NSMutableArray* sessionArray = [[NSMutableArray alloc] init];
        //在这里统一设置消息未读数
        NSInteger badgeNumber = 0;
        for (int i=0; i<pSessionList.nCount; i++)
        {
            SessionParams pSessionParams = pSessionList.ppSessionParams[i];
            IMPackageSessionData* data = [[IMPackageSessionData alloc] initWithSessionParams:&pSessionParams];
            badgeNumber += data.nUnreadNum;
            [sessionArray addObject:data];
        }
#ifdef BadgeNumberZero
        [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
#else
        [UIApplication sharedApplication].applicationIconBadgeNumber = badgeNumber;
#endif
        return sessionArray;
    }
    return nil;
}

- (int)IMPackage_RemoveSession:(NSString*)sessionID {
    
	HRESULT hResult2 = pCimiObj->DeleteSession([sessionID UTF8String],NO);
	if (hResult2 == S_OK){
        ///在这里统一设置消息未读数
        NSInteger badgeNumber = [UIApplication sharedApplication].applicationIconBadgeNumber;
        IMPackageSessionData *session = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSession:sessionID];
        badgeNumber -= session.nUnreadNum;
#ifdef BadgeNumberZero
        [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
#else
        [UIApplication sharedApplication].applicationIconBadgeNumber = badgeNumber;
#endif
		return S_OK;
	}
	return S_FALSE;
}

- (int)IMPackage_ClearSession:(NSString*)sessionID {
    HRESULT hResult = pCimiObj->ClearSession(sessionID.UTF8String);
    if (hResult == S_OK) {
        return S_OK;
    }
    return S_FALSE;
}

//置顶设置
-(int)IMPackage_SetSessionToTopWithSessionID:(NSString*)sessionID AndSessionOptionType:(IM_SessionTopStatus)optionType{
    
    SessionParams pSessionParams = pCimiObj->GetSession([sessionID UTF8String]);
    if (pSessionParams.pSessionId.empty() == true) {
        return S_OK;
    }
    pSessionParams.pSessionId = [sessionID UTF8String];
    pSessionParams.eTop = (TopStatus)optionType;
    
    HRESULT result = pCimiObj->SetSessionOption(SOT_TOP, pSessionParams);
    
    if (result == S_OK) {
        return S_OK;
    }
	return S_FALSE;
}

//新消息设置
-(int)IMPackage_SetSessionNewMsgRemindWithSessionID:(NSString*)sessionID AndSessionOptionType:(IM_SessionReceiveMessage)optionType{
    SessionParams pSessionParams = pCimiObj->GetSession([sessionID UTF8String]);
    if (pSessionParams.pSessionId.empty() == true) {
        return S_OK;
    }
    pSessionParams.pSessionId = [sessionID UTF8String];
    pSessionParams.eRemind = (RemindStatus)optionType;
    
    HRESULT result = pCimiObj->SetSessionOption(SOT_REMIND, pSessionParams);
    
    if (result == S_OK) {
        return S_OK;
    }
	return S_FALSE;
}

-(NSArray*)IMPackage_GetGivenFieldListWithSessionID:(NSString*)sessionId IM_GivenFieldType:(IM_GivenFieldType)fieldType IM_GivenConditionType:(IM_GivenConditionType)conditionType{
    
    MessageList pMessageList = pCimiObj->GetGivenFieldList([sessionId UTF8String],GivenFieldType(fieldType),GivenConditionType(conditionType));
    if (pMessageList.nCount > 0)
    {
        NSMutableArray* messageArray = [[NSMutableArray alloc] init];
        for (int i=0; i<pMessageList.nCount; i++)
        {
            MessageParams pMessageParams = pMessageList.ppMessageParams[i];
            IMPackageMessageData* data = [[IMPackageMessageData alloc] initWithMessageParams:&pMessageParams];
            [messageArray addObject:data];
        }
        return messageArray;
    }
    return nil;
}

-(void)setMapPointWithLongitude:(NSString*)longitude Latitude:(NSString*)latitude{
    
    if ([longitude intValue] == 0 && [latitude intValue] == 0) {
        //如果没有定到位就设置为天安门坐标
        longitude = @"116.3972282409668";
        latitude = @"39.90960456049752";
    }
    
    if (longitude && latitude) {
        
        char *xpoint = (CHAR*)malloc(strlen([longitude UTF8String])+1);
        memset(xpoint, 0, strlen([longitude UTF8String])+1);
        strcpy(xpoint, [longitude UTF8String]);
        string x = xpoint;
        
        char *ypoint = (CHAR*)malloc(strlen([latitude UTF8String])+1);
        memset(ypoint, 0, strlen([latitude UTF8String])+1);
        strcpy(ypoint, [latitude UTF8String]);
        string y = ypoint;
        
        pCimiObj->SetMapPoint(x, y);
        
    }
    
}

#pragma mark --Group
//创建群组
-(void)IMPackage_AddNewGroupWithAddUserID:(NSString*)userID groupPortraitKey:(NSString*)groupPortrait addContactList:(NSArray*)addContactList{
    if (!self.userName) {
        return;
    }
//    if (isLoginSucess == NO) {
//        return;
//    }
    
    //test add group
    ReqParams reqParams;
	reqParams.eReqType = RT_ADD_GROUP;
    
	GroupParams pGroupCreateParams;
    pGroupCreateParams.GroupName = "groupName";
    pGroupCreateParams.GroupID = "";
	pGroupCreateParams.OwnerID = [userID UTF8String];

	pGroupCreateParams.GroupPortraitKey = [groupPortrait UTF8String];
	pGroupCreateParams.GroupMemberCount = "";
	pGroupCreateParams.GroupMaxMember = "";
    
    IMPackageBuddyData *buddyData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetContactWithUserId:userID isFromServer:NO];
    MemberParams memberinfo = pCimiObj->GetGroupMemberInfo(userID.UTF8String,[@"" UTF8String]);
    memberinfo.MemberID = [userID UTF8String];
    memberinfo.MemberName = [buddyData.buddyUserName UTF8String];
    NSString *faceStr = buddyData.buddyPortraitKey? :@"";
    memberinfo.PortraitKey = [faceStr UTF8String];
    memberinfo.EmotionMood = [@"" UTF8String];
    pGroupCreateParams.MemberInfo = memberinfo;
    
    std::vector<MemberParams>members;
    for (IMPackageBuddyData* useridObj in addContactList) {
        
        if (useridObj.buddyUserName == nil) {
            continue;
        }
        
        IMPackageGroupMemberData *data = [[IMPackageGroupMemberData alloc] init];
        data.memberCMSID = useridObj.buddyCMSID;
        data.memberUserName = useridObj.buddyUserName;
        data.memberNickName = useridObj.buddyNickName;
        data.portraitKey = useridObj.buddyPortraitKey;
        data.EmotionMood = useridObj.buddyEmotionMood;
        
        MemberParams memberParams = [data getGroupMemberParams];
        members.push_back(memberParams);
    }
	pGroupCreateParams.Userlist = members;
	reqParams.pGroupParams = &pGroupCreateParams;
    
	pCimiObj->OperateGroup(&reqParams);
}

//退出群组 0成功  -1失败
-(int)IMPackage_leavingGroupWithUserID:(NSString*)userID andGroupID:(NSString*)groupID groupPortraitKey:(NSString*)groupPortrait{
    
    if (!self.userName || !userID || !groupID) {
        return S_FALSE;
    }
//    if (isLoginSucess == NO) {
//        return S_FALSE;
//    }
    
    ReqParams reqParams;
	reqParams.eReqType = RT_REMOVE_GROUP;
    
    //获取群组信息
	GroupParams pGroupLeavingParams = pCimiObj->GetGroupInfo(groupID.UTF8String);
    if (groupPortrait && [groupPortrait length]>0) {
        pGroupLeavingParams.GroupPortraitKey = [groupPortrait UTF8String];
    }
    
    //获取退出者信息
    MemberParams groupMemberInfo = pCimiObj->GetGroupMemberInfo(userID.UTF8String,[groupID UTF8String]);
    groupMemberInfo.MemberID = [userID UTF8String];
    if (groupMemberInfo.MemberName.length() == 0) {
//        groupMemberInfo.MemberName = [[UserInfoEntity sharedInstanceUserInfoEntity].user_NickName? :@"" UTF8String];
    }
    if (groupMemberInfo.PortraitKey.length() == 0) {
//        groupMemberInfo.PortraitKey = [[UserInfoEntity sharedInstanceUserInfoEntity].user_HeadLogoUrl? :@"" UTF8String];
    }
    if (groupMemberInfo.EmotionMood.length() == 0) {
//        groupMemberInfo.EmotionMood = [[UserInfoEntity sharedInstanceUserInfoEntity].user_Signature? :@"" UTF8String];
    }
    pGroupLeavingParams.MemberInfo = groupMemberInfo;
    
    
	reqParams.pGroupParams = &pGroupLeavingParams;
    HRESULT result = pCimiObj->OperateGroup(&reqParams);
    if (result == S_FALSE) {
        return S_FALSE;
    }
    return S_OK;
}

//邀请好友加入群组  0成功  -1失败
-(int)IMPackage_InviteGroupWithGroupID:(NSString*)groupID WithInviterID:(NSString*)inviterId groupPortraitKey:(NSString*)groupPortrait andAddContactList:(NSArray*)addContactList{
    
    if (!self.userName || !groupID) {
        return S_FALSE;
    }
    
    ReqParams reqParams;
	reqParams.eReqType = RT_INVITE_GROUP;
    
    //获取群组信息
	GroupParams pGroupLeavingParams = pCimiObj->GetGroupInfo(groupID.UTF8String);
    if (groupPortrait && [groupPortrait length]>0) {
        pGroupLeavingParams.GroupPortraitKey = [groupPortrait UTF8String];
    }
    
    std::vector<MemberParams>members = pGroupLeavingParams.Userlist;
    for (IMPackageBuddyData *useridObj in addContactList) {
        
        IMPackageGroupMemberData *data = [[IMPackageGroupMemberData alloc] init];
        data.memberCMSID = useridObj.buddyCMSID;
        data.memberUserName = useridObj.buddyUserName;
        data.memberNickName = useridObj.buddyNickName;
        data.portraitKey = useridObj.buddyPortraitKey;

        MemberParams memberParams = [data getGroupMemberParams];
        members.push_back(memberParams);
    }
    
    pGroupLeavingParams.Userlist = members;
    
    MemberParams memberinfo = pCimiObj->GetGroupMemberInfo(inviterId.UTF8String,[groupID UTF8String]);
    pGroupLeavingParams.MemberInfo = memberinfo;
    
	reqParams.pGroupParams = &pGroupLeavingParams;
    HRESULT result = pCimiObj->OperateGroup(&reqParams);
    if (result == S_FALSE) {
        return S_FALSE;
    }
    return S_OK;
}

//修改群名字
-(int)IMPackage_ModifyGroupNameWithGroupID:(NSString*)groupID WithModifyerID:(NSString*)modifyerID AndGroupName:(NSString*)groupName{
    
    if (!self.userName || !groupID) {
        return S_FALSE;
    }
    if (isLoginSucess == NO) {
        return S_FALSE;
    }
    
    ReqParams reqParams;
	reqParams.eReqType = RT_MODIFY_GROUP;
    
    GroupParams pGroupCreateParams = pCimiObj->GetGroupInfo(groupID.UTF8String);
    pGroupCreateParams.GroupName = [groupName UTF8String];
    
    MemberParams memberinfo = pCimiObj->GetGroupMemberInfo(modifyerID.UTF8String,[groupID UTF8String]);
    pGroupCreateParams.MemberInfo = memberinfo;
    reqParams.pGroupParams = &pGroupCreateParams;
    
    HRESULT result = pCimiObj->OperateGroup(&reqParams);
    if (result == S_FALSE) {
        return S_FALSE;
    }
    return S_OK;
}

//获取群组列表
-(NSArray*)IMPackage_getGroupList{
    std::vector<GroupParams> grouplist = pCimiObj->GetGroupList();
    
    NSInteger grouplistsize = grouplist.size();

    if (grouplistsize <= 0) {
        return nil;
    }
    
    std::vector<GroupParams>::iterator iter;
    
    NSMutableArray* groupslist = [[NSMutableArray alloc] init];
    for(iter = grouplist.begin();iter != grouplist.end();iter++)
    {
        IMPackageGroupData* groupData = [[IMPackageGroupData alloc] initWithGroupParams:&(*iter)];
        [groupslist addObject:groupData];
    }
    return groupslist;
}

//通过groupid获取群组信息
-(IMPackageGroupData*)IMPackage_getGroupInfoWithGroupId:(NSString*)groupID{
    
    GroupParams groupParams = pCimiObj->GetGroupInfo(groupID.UTF8String);
    
    IMPackageGroupData* groupData = [[IMPackageGroupData alloc]initWithGroupParams:&groupParams];
    
    return groupData;
}

//获取群组成员列表
-(NSArray*)IMPackage_GetGroupMemberListWithGroupID:(NSString*)groupID{
    std::vector<MemberParams> groupMemberList = pCimiObj->GetGroupMemberList(groupID.UTF8String);
    NSInteger groupMember = groupMemberList.size();
    if (groupMember <= 0) {
        return nil;
    }
    
    std::vector<MemberParams>::iterator iter;
    NSMutableArray* memberList = [[NSMutableArray alloc] init];
    for (iter = groupMemberList.begin(); iter != groupMemberList.end(); iter++) {
        IMPackageGroupMemberData* memberData = [[IMPackageGroupMemberData alloc]initWithGroupMemberParams:&(*iter)];
        [memberList addObject:memberData];
    }
    return memberList;
}

// 获取成员信息
-(IMPackageGroupMemberData*)IMPackage_getGroupMemberInfoWithMemberId:(NSString*)memberID groupId:(NSString*)groupId isFromServer:(BOOL)fromServer{
    
    self.buddyUserName = memberID;
    if (memberID == nil) {
        return nil;
    }
    
    if (!groupId) {
        groupId = @"";
    }
    MemberParams getGroupMemberInfo = pCimiObj->GetGroupMemberInfo(memberID.UTF8String,[groupId UTF8String],fromServer);
    
    IMPackageGroupMemberData* memberData = [[IMPackageGroupMemberData alloc]initWithGroupMemberParams:&getGroupMemberInfo];
    
    return memberData;
}

//修改群名片
-(int)IMPackage_ModifyMemberCardNameWithGroupID:(NSString*)groupID WithModifyerID:(NSString*)modifyerID AndCardName:(NSString*)cardName{
    
    if (!self.userName || !groupID) {
        return S_FALSE;
    }
//    if (isLoginSucess == NO) {
//        return S_FALSE;
//    }
    
    ReqParams reqParams;
	reqParams.eReqType = RT_MODIFY_GROUPMEMBER_NAME;
    
    GroupParams pGroupCreateParams = pCimiObj->GetGroupInfo(groupID.UTF8String);
    
    MemberParams memberinfo = pCimiObj->GetGroupMemberInfo(modifyerID.UTF8String,[groupID UTF8String]);
    if (cardName && [cardName length]>0) {
        memberinfo.MemberCardName = [cardName UTF8String];
    }
    pGroupCreateParams.MemberInfo = memberinfo;
    reqParams.pGroupParams = &pGroupCreateParams;
    
    HRESULT result = pCimiObj->OperateGroup(&reqParams);
    if (result == S_FALSE) {
        return S_FALSE;
    }
    return S_OK;
}

//获取互动消息数
-(int)IMpackage_GetHudongCount{
    if (!self.userName) {
        return 0;
    }
    int count = pCimiObj->GetSnsCount(IT_HUDONG);
    return count;
}

//清空互动消息
-(int)IMpackage_ClearHudongCount{
    return pCimiObj->ClearSnsCount(IT_HUDONG);
}

//获取推荐好友数
-(int)IMPackage_GetRecommendationFriend{
    if (!self.userName) {
        return 0;
    }
    int count = pCimiObj->GetSnsCount(IT_RECOMMEND);
    return count;
}

//清空推荐好友数
-(int)IMPackage_ClearRecommendationFriend{
    return pCimiObj->ClearSnsCount(IT_RECOMMEND);
}

//获取推送设置
-(int)IMPackage_GetReceiveMessageOption{
    return pCimiObj->GetReceiveMessageOption();
    return 0;
}

-(long)IMPackage_SetReceiveMessageOption:(IM_OPTIONTYPE)option{
    return pCimiObj->SetReceiveMessageOption(option);
    return 0;
}

#pragma mark -- 获取互动数
-(int)getMsgCount{
    //消息数是互动消息和聊天消息数的和
    int msgCount = [[IMPackageEngine sharedInstanceIMPackageEngine] IMpackage_GetHudongCount];
    
    NSArray* sessionArray = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSessionList];
    if (nil == sessionArray) {
        return msgCount;
    }
    for (IMPackageSessionData* data in sessionArray) {
        //不计算未开启的消息数
        if (data.eRemind == YES) {
            msgCount += data.nUnreadNum;
        }
    }
    return msgCount;
}

#pragma mark buddy
//获取单个好友
-(IMPackageBuddyData*)IMPackage_GetSingleBuddyWithUserID:(NSString*)userID NeedLoadFromServer:(BOOL)loadFromServer{
    if (!userID) {
        self.buddyUserName = nil;
        return nil;
    }
    self.buddyUserName = userID;
    BuddyParams params = pCimiObj->GetBuddy(userID.UTF8String, loadFromServer);

    if (params.pUserId.empty() == true) {
        return nil;
    }
    IMPackageBuddyData* buddyData = [[IMPackageBuddyData alloc] initWithBuddyParams:&params];
    return buddyData;
}

//获取联系人 在回调中使用
-(IMPackageBuddyData*)IMPackage_GetContactWithUserId:(NSString*)userID isFromServer:(BOOL)fromServer{
    if (!userID) {
        self.buddyUserName = nil;
        return nil;
    }
    self.buddyUserName = userID;
    BuddyParams params = pCimiObj->GetContact([userID UTF8String],fromServer);
    if (params.pUserId.empty() == true) {
        return nil;
    }
    IMPackageBuddyData *contentData = [[IMPackageBuddyData alloc] initWithBuddyParams:&params];
    return contentData;
}


//获取好友列表
-(NSArray*)IMPackage_GetBuddyList{
    BuddyList paramsList = pCimiObj->GetBuddyList();
    if (paramsList.nCount > 0) {
        NSMutableArray* buddyList = [[NSMutableArray alloc] init];
        for (int i = 0; i<paramsList.nCount; i++) {
            IMPackageBuddyData* buddyData = [[IMPackageBuddyData alloc]initWithBuddyParams:&paramsList.ppBuddyParams[i]];
            [buddyList addObject:buddyData];
        }
        return buddyList;
    }
    return nil;
}

//获取单个粉丝数据
-(IMPackageBuddyData*)IMPackage_GetFansWithUserID:(NSString*)userID NeedLoadFromServer:(BOOL)loadFromServer{
    if (!userID) {
        self.buddyUserName = nil;
        return nil;
    }
    self.buddyUserName = userID;
    BuddyParams params = pCimiObj->GetFans(userID.UTF8String, loadFromServer);
    
    if (params.pUserId.empty() == true) {
        return nil;
    }
    IMPackageBuddyData* buddyData = [[IMPackageBuddyData alloc] initWithBuddyParams:&params];
    return buddyData;
}

//获取粉丝列表
-(NSArray*)IMPackage_GetFansList{
    BuddyList paramsList = pCimiObj->GetFansList();
    if (paramsList.nCount > 0) {
        NSMutableArray* buddyList = [[NSMutableArray alloc] init];
        for (int i = 0; i<paramsList.nCount; i++) {
            IMPackageBuddyData* buddyData = [[IMPackageBuddyData alloc]initWithBuddyParams:&paramsList.ppBuddyParams[i]];
            [buddyList addObject:buddyData];
        }
        return buddyList;
    }
    return nil;
}

//添加关注
-(int)IMPackage_AddBuddyWithUserID:(NSString*)userId{
    if (userId == nil) {
        return S_FALSE;
    }
    /*
    BuddyParams pBuddyParams = {};
    
    pBuddyParams.pUserId =[friendData.userName UTF8String];
    
    pBuddyParams.pUserName = [friendData.userName UTF8String];
    
    pBuddyParams.pCmsId = [friendData.cmsID UTF8String];
    
    if (friendData.faceKey) {
        pBuddyParams.pPortraitKey = [friendData.faceKey UTF8String];
    }
    
    if (friendData.Signature) {
        pBuddyParams.pEmotionMood = [friendData.Signature UTF8String];
    }

    pBuddyParams.eIsBuddy = IBT_BUDDY;
     */
    
    HRESULT result2 = pCimiObj->AddBuddy([userId UTF8String]);
    if (result2 == S_OK) {
        return S_OK;
    }
    return S_FALSE;
}

//添加黑名单
-(void)IMPackage_SetBlackListWithId:(NSString*)buddyId IM_BlackListType:(IM_BlackListType)type{
    
    //uid
    char *uid = (CHAR*)malloc(strlen([buddyId UTF8String])+1);
    memset(uid, 0, strlen([buddyId UTF8String])+1);
    strcpy(uid, [buddyId UTF8String]);
    string blackId = uid;
    
    pCimiObj->SetBlackListFlag(blackId, type);
}

//修改某个好友信息
-(int)IMPackage_ModifyBuddyWihBuddyData:(IMPackageBuddyData*)buddyData{
    /*
    if (buddyData == nil) {
        return S_FALSE;
    }
    //从服务器获取好友信息
    BuddyParams pBuddyParams = pCimiObj->GetBuddy(buddyData.buddyUserId.UTF8String, YES);
    if (pBuddyParams.pUserId.empty() == true) {
        return S_FALSE;
    }
    //修改信息  后传给服务器   这里待定 不知道能修改哪些信息
    if (buddyData.buddyUserId) {
        pBuddyParams.pUserId = buddyData.buddyUserId.UTF8String;
    }
    if (buddyData.buddyUserName) {
        pBuddyParams.pUserName = buddyData.buddyUserName.UTF8String;
    }
    if (buddyData.buddyPortraitKey) {
        pBuddyParams.pPortraitKey = buddyData.buddyPortraitKey.UTF8String;
    }
    if (buddyData.buddyEmotionMood) {
        pBuddyParams.pEmotionMood = buddyData.buddyEmotionMood.UTF8String;
    }
    if (buddyData.buddyQrerUrl) {
        pBuddyParams.pQrerUrl = buddyData.buddyQrerUrl.UTF8String;
    }
    if (buddyData.buddyCommunityUrl) {
        pBuddyParams.pCommunityUrl = buddyData.buddyCommunityUrl.UTF8String;
    }
//    pBuddyParams->eIsBuddy = buddyData.buddyIsBuddy;
    
    HRESULT result2 = pCimiObj->ModifyBuddy(pBuddyParams);
    if (result2 == S_OK) {
        return S_OK;
     }
     */
    return S_FALSE;
}

//取消关注
-(int)IMPackage_DeleteBuddyWihUserID:(NSString*)userID{
    if (userID == nil) {
        return S_FALSE;
    }
    HRESULT result = pCimiObj->DeleteBuddy(userID.UTF8String);
    if (result == S_OK) {
        return S_OK;
    }
    return S_FALSE;
}

//搜索联系人
-(int)IMPackage_SearchContactWithuserID:(NSString*)userID{
    if (userID == nil) {
        return S_FALSE;
    }
    HRESULT result = pCimiObj->SearchContact(userID.UTF8String);
    if (result == S_OK) {
        return S_OK;
    }
    return S_FALSE;
}

//获取搜索人列表
-(NSArray*)IMPackage_GetSearchList{
    BuddyList paramsList = pCimiObj->GetSearchContact();
    if (paramsList.nCount > 0) {
        NSMutableArray* searchList = [[NSMutableArray alloc] init];
        for (int i = 0; i<paramsList.nCount; i++) {
            IMPackageBuddyData* buddyData = [[IMPackageBuddyData alloc]initWithBuddyParams:&paramsList.ppBuddyParams[i]];
            [searchList addObject:buddyData];
        }
        return searchList;
    }
    return nil;
}

//判断是否是好友
-(BOOL)IMPackage_IsFriendWithUserID:(NSString*)userID{
    if (!userID) {
        return NO;
    }
    HRESULT result = pCimiObj->IsBuddyExist([userID UTF8String]);
    if (1 == result) {
        return YES;
    }else{
        return NO;
    }
}

//添加联系人到本地数据库
-(void)IMPackage_AddContentWithUserID:(NSString*)userID{
//    if (!userID) {
//        self.buddyCmsId = nil;
//        return ;
//    }
//    self.buddyCmsId = userID;
//    
//    BuddyParams params = pCimiObj->GetContact([userID UTF8String]);
//    pCimiObj->AddContact(params);
}

//获取新粉丝个数
-(int)IMPackage_GetNewFansCount{
    int count = pCimiObj->GetNewFansCount();
    return count;
}

//清空粉丝数
-(void)IMPackage_ClearFansCount{
    pCimiObj->ClearNewFansCount();
}

#pragma mark IMPackageEngineDelegate Method

-(void)addDelegate:(id<IMPackageEngineDelegate>)delegate{
    dispatch_async(queue, ^{
         [delegateSet addObject:delegate];
    });
}

-(void)removeDelegate:(id<IMPackageEngineDelegate>)delegate{
    dispatch_async(queue, ^{
        [delegateSet removeObject:delegate];
    });
}

-(void)notifyAllDelegate:(IM_MsgType)msgType detaileType:(int)detaileType result:(id)data{
   
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageMsgType:detaileType:result:)]) {
                [tempDel IMPackageMsgType:msgType detaileType:detaileType result:data];
            }
        }
    });
}

//登录成功或失败
-(void)notifyAllDelegateWithdetaileType:(int)detaileType{
    if (kIM_MsgType_Login_sucess_Key == detaileType) {
        isLoginSucess = YES;
        [IMPackageEngine sharedInstanceIMPackageEngine].connectionType = IM_CST_EXIST;
    }else if (kIM_MsgType_Login_faild_Key == detaileType){
        isLoginSucess = NO;
        [IMPackageEngine sharedInstanceIMPackageEngine].connectionType = IM_CST_NOTEXIST;
    }
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageLoginStatus:)]) {
                [tempDel IMPackageLoginStatus:detaileType];
            }
        }
    });
}

- (void)notifyGroupDetaileType:(int)detaileType groupData:(IMPackageGroupData*)groupData {
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageGroupDetaileType:groupData:)]) {
                [tempDel IMPackageGroupDetaileType:detaileType groupData:groupData];
            }
        }
    });
}

//建群或邀请人失败
-(void)notifyGroupFailureType:(IM_GroupErrCode)errType AndGroupMemCount:(int)memCount{
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageNotifyGroupFaildWithtype:AndGroupMemCount:)]) {
                [tempDel IMPackageNotifyGroupFaildWithtype:errType AndGroupMemCount:memCount];
            }
        }
    });
}

- (void)notifyAllGroupList:(NSArray*)groupArray {
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageGetAllGroupList:)]) {
                [tempDel IMPackageGetAllGroupList:groupArray];
            }
        }
    });
}

//修改群名称
- (void)notifyChangeGroupName:(int)detaileType{
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageChangeGroupName:)]) {
                [tempDel IMPackageChangeGroupName:detaileType];
            }
        }
    });
    
}

//修改群名片
- (void)notifyChangeCardName:(int)detaileType{
    for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
        if (tempDel && [tempDel respondsToSelector:@selector(IMPackageChangeCardName:)]) {
            [tempDel IMPackageChangeCardName:detaileType];
            return;
        }
    }
}

//接收到新消息
- (void)notifyRecvMessageType:(int)detaileType msgData:(IMPackageMessageData *)msgModel {
    if (msgModel == nil) {
        return;
    }
    dispatch_async(queue, ^{
#ifdef BadgeNumberZero
        [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
#else
        [UIApplication sharedApplication].applicationIconBadgeNumber += 1;
#endif
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageRecvMessageType:msgData:)]) {
                [tempDel IMPackageRecvMessageType:detaileType msgData:msgModel];
            }
        }
    });
}

//消息发送失败
-(void)notifySendMessageFailWithMsgModel:(IMPackageMessageData *)msgModel{
    
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageSendMsgFailWithMsgModel:)]) {
                [tempDel IMPackageSendMsgFailWithMsgModel:msgModel];
            }
        }
    });
}

//消息发送成功
- (void)notifySendMessageSuccessWithMsgModel:(IMPackageMessageData *)msgModel{
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageSendMsgSuccessWithMsgModel:)]) {
                [tempDel IMPackageSendMsgSuccessWithMsgModel:msgModel];
            }
        }
    });
}

//消息发送中
- (void)notifySendMessageProcessWithMsgModel:(IMPackageMessageData *)msgModel{
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackageSendMsgProcessWithMsgModel:)]) {
                [tempDel IMPackageSendMsgProcessWithMsgModel:msgModel];
            }
        }
    });
}

//接收到客服小号
- (void)notifyReceiveCustomerIdWithModel:(IMPackageIqData *)iqData {
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackage_ReSendCustomMessageWithIQCustomData:)]) {
                [tempDel IMPackage_ReSendCustomMessageWithIQCustomData:iqData];
            }
        }
    });
}

//接收离线消息
- (void)notifyOfflineMsgListWithOfflineData:(OfflineMessageData*)offlineData{
    dispatch_async(queue, ^{
        for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
            if (tempDel && [tempDel respondsToSelector:@selector(IMPackage_RecevieOfflineDataWithOfflineModel:)]) {
                [tempDel IMPackage_RecevieOfflineDataWithOfflineModel:offlineData];
            }
        }
    });
}

//获取单个联系人
-(void)notifyLoadSingleBuddySuccessWithBuddyData:(IMPackageBuddyData*)memberData{
    for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
        if (tempDel && [tempDel respondsToSelector:@selector(IMPackageGetBuddyDataSuccess:)]) {
            [tempDel IMPackageGetBuddyDataSuccess:memberData];
        }
    }
}

//获取粉丝
-(void)notifyLoadSingleFansSuccessWithBuddyData:(IMPackageBuddyData *)buddyData{
    for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
        if (tempDel && [tempDel respondsToSelector:@selector(IMPackageGetFansDataSuccess:)]) {
            [tempDel IMPackageGetFansDataSuccess:buddyData];
        }
    }
}

//联系人有变化
- (void)notifyBuddyListHasChange:(RspType)changeType {
     for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
         if (tempDel && [tempDel respondsToSelector:@selector(IMPackageBuddyListDataHasChange:)]) {
             [tempDel IMPackageBuddyListDataHasChange:changeType];
         }
     }
}

//粉丝有变化
-(void)notifyFansListHasChange:(RspType)changeType{
    for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
        if (tempDel && [tempDel respondsToSelector:@selector(IMPackageFansListDataHasChange:)]) {
            [tempDel IMPackageFansListDataHasChange:changeType];
        }
    }
}

//获取搜索列表
- (void)notifySearchListWithDataArray:(NSArray*)searchList{
    for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
        if (tempDel && [tempDel respondsToSelector:@selector(IMPackage_GetSearchList:)]) {
            [tempDel IMPackage_GetSearchList:searchList];
        }
    }
}

//网络连接错误回调
- (void)notifyNetWork_Failure{
    [IMPackageEngine sharedInstanceIMPackageEngine].connectionType = IM_CST_NOTEXIST;
    for (id<IMPackageEngineDelegate> tempDel in delegateSet) {
        if (tempDel && [tempDel respondsToSelector:@selector(IMPackage_NetWork_Failure)]) {
            [tempDel IMPackage_NetWork_Failure];
        }
    }
}

@end
