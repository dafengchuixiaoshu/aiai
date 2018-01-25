//
//  IMPackageData.m
//  imtest
//
//  Created by zhangyang on 14-7-12.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "IMPackageData.h"
#import "IMPackageGlobal.h"

#import "PinyinHelper.h"
#import "HanyuPinyinOutputFormat.h"

@implementation IMPackageSessionData

-(id)initWithSessionParams:(SessionParams*)params{
    if (self = [super init]) {
        if (NULL == params) {
            self.eTop = NO;//是否置顶
            self.eRemind = YES;//新消息提醒(0:关闭,1:开启)
        }
        else {
            self.pMsgId = [[NSString alloc] initWithUTF8String:params->pMsgId.c_str()];
            self.pSessionId = [[NSString alloc] initWithUTF8String:params->pSessionId.c_str()];
            self.nUnreadNum = params->nUnreadNum;   // 未读消息个数
            self.eType = (IM_MESSAGE_TYPE)params->eType;// 消息类型（1:个人消息,2:群组消息）
            self.eTop = params->eTop;//是否置顶
            self.eRemind = params->eRemind;//新消息提醒(0:关闭,1:开启)
            if (params->pSessionName.empty() == false) {
                self.pSessionName = [[NSString alloc] initWithUTF8String:params->pSessionName.c_str()];
            }else{
                self.pSessionName = @"";
            }if (params->pPortraitKey.empty() == false) {
                self.pPortraitKey = [[NSString alloc] initWithUTF8String:params->pPortraitKey.c_str()];
            }else{
                self.pPortraitKey = @"";
            }
            self.pDraftContent = [[NSString alloc] initWithUTF8String:params->pDraftContent.c_str()];
            self.eDraftChanged = (IM_DraftChanged)params->eDraftChanged;
        }
    }
    return self;
}

-(id)initWithIMPackageBuddyData:(IMPackageBuddyData *)buddyData{
    if (self = [super init]) {
        NSString* imageUrl = [AppUtils compatibleImageKeyWithKey:buddyData.buddyPortraitKey];
        NSString* vedioUrl = [AppUtils compatibleImageKeyWithKey:buddyData.buddyVideoKey];
        self.pPortraitKey = [NSString stringWithFormat:@"%@###%@",imageUrl,vedioUrl];
        self.pSessionId = buddyData.buddyUserName;
        self.pSessionName = buddyData.buddyNickName;
        self.nUnreadNum = 0;
        self.eType = IM_MESSAGE_TYPE_PERSONAL;
        self.eTop = YES;
        self.eRemind = YES;
        self.pDraftContent = @"";
        self.eDraftChanged = IM_DC_UNCHANGED;
    }
    return self;
}

-(id)initWithIMPackageMessageData:(IMPackageMessageData *)msgData{
    if (self = [super init]) {
        NSString* imageUrl = [AppUtils compatibleImageKeyWithKey:msgData.pImgKey];
        NSString* vedioUrl = [AppUtils compatibleImageKeyWithKey:msgData.pVideoKey];
        self.pPortraitKey = [NSString stringWithFormat:@"%@###%@",imageUrl,vedioUrl];
        self.pSessionId = msgData.pSessionId;
        self.pMsgId = msgData.pMsgId;
        self.pSessionName = msgData.pSender;
        self.nUnreadNum = 0;
        self.eType = IM_MESSAGE_TYPE_PERSONAL;
        self.eTop = YES;
        self.eRemind = YES;
        self.pDraftContent = @"";
        self.eDraftChanged = IM_DC_UNCHANGED;
    }
    return self;
}

-(id)initWithIMPackageGroupData:(IMPackageGroupData *)groupData{
    if (self = [super init]) {
        self.pPortraitKey = groupData.groupPortraitKey;
        self.pSessionId = groupData.groupID;
        self.pSessionName = groupData.groupName;
        self.pMsgId = nil;
        self.nUnreadNum = 0;
        self.eType = IM_MESSAGE_TYPE_GROUP;
        self.eTop = YES;
        self.eRemind = YES;
        self.pDraftContent = @"";
        self.eDraftChanged = IM_DC_UNCHANGED;
    }
    return self;
}

@end


@implementation IMPackageMessageData

-(id)initWithMessageParams:(MessageParams*)params{
    if (self = [super init]) {
        
        if (params == NULL) {
            return self;
        }
        self.pMsgId = [[NSString alloc] initWithUTF8String:params->pMsgId.c_str()];
        self.pSessionId = [[NSString alloc] initWithUTF8String:params->pSessionId.c_str()];
        if (params->pSender.empty() == false) {
            self.pSender = [[NSString alloc] initWithUTF8String:params->pSender.c_str()];
        }
        if (params->pReceiver.empty() == false) {
            self.pReceiver = [[NSString alloc] initWithUTF8String:params->pReceiver.c_str()];
        }
        if (params->pTime.empty() == false) {
            self.pTime = [[NSString alloc] initWithUTF8String:params->pTime.c_str()];
        }
        if (params->pSenderName.empty() == false) {
            self.pSenderName = [[NSString alloc] initWithUTF8String:params->pSenderName.c_str()];
        }
        if (params->pSenderKey.empty() == false) {
            self.pSenderKey = [[NSString alloc]initWithUTF8String:params->pSenderKey.c_str()];
        }
        self.pIsSave = params->pIsSave;
        self.pIsShare = params->pIsShare;
        
        //两个平台发消息的协议:1为文字 2为图片 3为语音 其他待扩展
        if (params->pContent.empty() == false) {
            NSString* content = [[NSString alloc] initWithUTF8String:params->pContent.c_str()];
            NSDictionary *contentDic = [content myObjectFromJSONString];
            NSString* msgType = [contentDic objectForKey:@"type"];
            switch ([msgType intValue]) {
                case 1:
                {
                    NSString *baseContent = [contentDic objectForKey:@"text"];
                    self.ptext = [AppUtils base64StringDecode:baseContent];
                    CGFloat width = [[contentDic objectForKey:@"width"] floatValue];
                    CGFloat height = [[contentDic objectForKey:@"height"] floatValue];
                    self.pImageSize = CGSizeMake(width, height);
                    self.pImgKey = [contentDic objectForKey:@"img"];
                    self.longitude = [[contentDic objectForKey:@"longitude"] doubleValue];
                    self.latitude = [[contentDic objectForKey:@"latitude"] doubleValue];
                }
                    break;
                case 2:
                {
                    NSString *baseContent = [contentDic objectForKey:@"text"];
                    self.ptext = [AppUtils base64StringDecode:baseContent];
                    self.ptitle = [contentDic objectForKey:@"title"];
                    self.pImgKey = [contentDic objectForKey:@"img"];
                    self.pVideoKey = [contentDic objectForKey:@"video"];
                    self.pVideoLength = [[contentDic objectForKey:@"length"] intValue];
                    self.longitude = [[contentDic objectForKey:@"longitude"] doubleValue];
                    self.latitude = [[contentDic objectForKey:@"latitude"] doubleValue];
                    
                }
                    break;
                
                default:
                {

                }
                    break;
            }
        }
        self.ePlay = (IM_PlayedStatus)params->ePlayed;
        self.eType = (IM_MESSAGE_TYPE)params->eType;
        self.eKind = (IM_KIND_TYPE)params->eKind;
        self.eCategory = (IM_CATEGORY_TYPE)params->eCategory;
        self.eDirection = (IM_DIRECTION_TYPE)params->eDirection;
        self.eUnread = (IM_UNREADSTATUS_TYPE)params->eUnread;
        self.eSend = (IM_SENDSTATUS_TYPE)params->eSend;
        self.eonLine = (IM_MESSAGE_ONLINE)params->eOnline;
    }
    return self;
}

-(MessageParams)getMessageParams{
    MessageParams params = {};
    
    if (self.pMsgId == nil) {
        self.pMsgId = @"";
    }
    
    params.pMsgId = [self.pMsgId UTF8String];
    
    if (self.pSessionId == nil) {
        self.pSessionId = @"";
    }
    params.pSessionId = [self.pSessionId UTF8String];
    
    if (self.pSender == nil) {
        self.pSender = @"";
    }
    params.pSender = [self.pSender UTF8String];
    
    if (self.pSenderName == nil) {
        self.pSenderName = @"";
    }
    params.pSenderName = [self.pSenderName UTF8String];
    
    if (self.pSenderKey == nil) {
        self.pSenderKey = @"";
    }
    params.pSenderKey = [self.pSenderKey UTF8String];
    
    if (self.pReceiver == nil) {
        self.pReceiver = @"";
    }
    params.pReceiver = [self.pReceiver UTF8String];
    
    params.pContent = [self.ptext UTF8String];
    
    if (self.pTime == nil) {
        self.pTime = @"";
    }
    params.pTime = [self.pTime UTF8String];

    params.eType = (MessageType)self.eType;
    params.eKind = (KindType)self.eKind;
    params.eCategory = (CategoryType)self.eCategory;
    params.eDirection = (DirectionType)self.eDirection;
    params.eUnread = (UnreadStatus)self.eUnread;
    params.eSend = (SendStatus)self.eSend;

    params.pIsSave = self.pIsSave;
    params.pIsShare = self.pIsShare;
    
    return params;
}

@end

@implementation OfflineMessageData

-(id)initWithOfflineMessageParams:(OfflineMessageParams *)params{
    if (self = [super init]) {
        self.pMsgListDic = [[NSMutableDictionary alloc] init];
        
        NSMutableArray *sessionIdArray = [[NSMutableArray alloc] init];
        for (int i = 0; i<params->ppSessionId.size(); i++) {
            NSString *pSessionId = [[NSString alloc] initWithUTF8String:params->ppSessionId[i].c_str()];
            [sessionIdArray addObject:pSessionId];
        }
        self.psessionIDs = sessionIdArray;
        
        self.pOfflineMsgCount = params->nOfflineTotal;
        self.pSessionCount = params->nSessionCount;
    }
    return self;
}

@end

@implementation IMPackageGroupMemberData

-(id)initWithGroupMemberParams:(MemberParams*)params{
    if (self = [super init]) {
        self.memberUserName = [[NSString alloc] initWithUTF8String:params->MemberID.c_str()];
        self.memberNickName = [[NSString alloc] initWithUTF8String:params->MemberName.c_str()];
        
        NSString *key = [[NSString alloc] initWithUTF8String:params->PortraitKey.c_str()];
        NSArray *keyArray = [key componentsSeparatedByString:@"###"];
        //正常有两个对象 一个为头像key另一个为视频key
        if ([keyArray count] == 2) {
            self.portraitKey = [keyArray objectAtIndex:0];
            self.videoKey = [keyArray objectAtIndex:1];
        }else if ([keyArray count] == 1){
            self.portraitKey = [keyArray objectAtIndex:0];
        }
        self.EmotionMood = [[NSString alloc] initWithUTF8String:params->EmotionMood.c_str()];
        self.memberCMSID = [[NSString alloc] initWithUTF8String:params->CmsID.c_str()];
        self.GroupCardName = [[NSString alloc]initWithUTF8String:params->MemberCardName.c_str()];
    }
    return self;
}

-(MemberParams)getGroupMemberParams{
    MemberParams params = {};//(MemberParams*)malloc(sizeof(MemberParams));
//    memset(params, 0, sizeof(MemberParams));
    
    params.CmsID = [self.memberCMSID? :@"" UTF8String];
    params.MemberID = [self.memberUserName UTF8String];
    params.MemberName = [self.memberNickName UTF8String];
    params.PortraitKey = [self.portraitKey? :@"" UTF8String];
    if (![self.EmotionMood isEmpty]) {
        params.EmotionMood = [self.EmotionMood? :@"" UTF8String];
    }else{
        params.EmotionMood = [@"" UTF8String];
    }
    
    return params;
}

@end

@implementation IMPackageGroupData

-(id)initWithGroupParams:(GroupParams*)params{
    if (self = [super init]) {
        
        MemberParams  MemberInfo;          // 群主ID,邀请者ID,修改者ID,退出者ID
        std::vector<MemberParams> Userlist;// 邀请者列表,被邀请者列表,群成员列表
        
        if (params->GroupID.empty() == true) {
            return nil;
        }
        self.groupID = [[NSString alloc] initWithUTF8String:params->GroupID.c_str()];
        self.groupName = [[NSString alloc] initWithUTF8String:params->GroupName.c_str()];
        self.groupPortraitKey = [[NSString alloc] initWithUTF8String:params->GroupPortraitKey.c_str()];
        self.ownerID = [[NSString alloc] initWithUTF8String:params->OwnerID.c_str()];
        self.groupMemberCount = [[NSString alloc] initWithUTF8String:params->GroupMemberCount.c_str()];
        self.groupMemberMax = [[NSString alloc] initWithUTF8String:params->GroupMaxMember.c_str()];
        self.groupCardName = [[NSString alloc]initWithUTF8String:params->GroupCardName.c_str()];
        self.groupMemberData = [[IMPackageGroupMemberData alloc] initWithGroupMemberParams:&params->MemberInfo];
        
        NSMutableArray* userList = [[NSMutableArray alloc] init];
        NSInteger inviteesize = params->Userlist.size();
        for(int i = 0; i < inviteesize; i++)
        {
            MemberParams member = params->Userlist[i];
            IMPackageGroupMemberData* data = [[IMPackageGroupMemberData alloc] initWithGroupMemberParams:&member];
            [userList addObject:data];
        }
        self.groupMemberList = userList;
    }
    return self;
}

@end

@implementation IMPackageBuddyData

-(id)initWithBuddyParams:(BuddyParams *)params{
    if (self = [super init]) {
        if (params == NULL) {
            return self;
        }
        self.buddyCMSID = [[NSString alloc] initWithUTF8String:params->pCmsId.c_str()];
        self.buddyUserName = [[NSString alloc] initWithUTF8String:params->pUserId.c_str()];
        self.buddyNickName = [[NSString alloc] initWithUTF8String:params->pUserName.c_str()];
        if (params->pEmail.empty() == false) {
            self.buddyEmail = [[NSString alloc] initWithUTF8String:params->pEmail.c_str()];
        }else{
            self.buddyEmail = @"";
        }
        if (params->pMobile.empty() == false) {
            self.buddyMobile = [[NSString alloc] initWithUTF8String:params->pMobile.c_str()];
        }else{
            self.buddyMobile = @"";
        }
        if (params->pSubname.empty() == false) {
            self.buddySubname = [[NSString alloc] initWithUTF8String:params->pSubname.c_str()];
        }
        else{
            self.buddySubname = @"";
        }
        if (params->pIsSave.empty() == false) {
            self.buddyIsSave = [[NSString alloc] initWithUTF8String:params->pIsSave.c_str()];
        }
        if (params->pIsShare.empty() == false) {
            self.buddyIsShare = [[NSString alloc] initWithUTF8String:params->pIsShare.c_str()];
        }
        
        NSString *key = [[NSString alloc] initWithUTF8String:params->pPortraitKey.c_str()];
        NSArray *keyArray = [key componentsSeparatedByString:@"###"];
        //正常有两个对象 一个为头像key另一个为视频key
        if ([keyArray count] == 2) {
            self.buddyPortraitKey = [keyArray objectAtIndex:0];
            self.buddyVideoKey = [keyArray objectAtIndex:1];
        }else if ([keyArray count] == 1){
            self.buddyPortraitKey = [keyArray objectAtIndex:0];
        }
        self.buddyEmotionMood = [[NSString alloc]initWithUTF8String:params->pEmotionMood.c_str()];
        self.buddyQrerUrl = [[NSString alloc] initWithUTF8String:params->pQrerUrl.c_str()];
        self.buddyCommunityUrl = [[NSString alloc] initWithUTF8String:params->pCommunityUrl.c_str()];
        
        params->eIsBuddy ? self.buddyIsBuddy = IM_BUDDY_BUDDY : self.buddyIsBuddy = IM_BUDDY_NOTBUDDY ;
    }
    return self;
}

+(NSDictionary*)firstLetterContactDictionaryWithBuddyList:(NSArray*)buddyList{
    NSMutableDictionary* tempfirstLetterToContactDataDic = [[NSMutableDictionary alloc] init];
    
    HanyuPinyinOutputFormat *outputFormat=[[HanyuPinyinOutputFormat alloc] init];
    [outputFormat setToneType:ToneTypeWithoutTone];
    [outputFormat setVCharType:VCharTypeWithV];
    [outputFormat setCaseType:CaseTypeUppercase];
    
    for (int i = 0;i<[buddyList count];i++) {
        
        IMPackageBuddyData *buddyData = [buddyList objectAtIndex:i];
        
        //抽取首字母,ChineseToPinyin不支持繁体字，废弃。
        //NSString* pinyin = [ChineseToPinyin pinyinFromChiniseString:data.name];
        NSString* pinyin = [PinyinHelper toHanyuPinyinStringWithNSString:buddyData.buddyNickName withHanyuPinyinOutputFormat:outputFormat withNSString:@""];
        NSString* pinyinNoSpace = [pinyin stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];//去掉空格
        //去除错误数据
        if ([pinyinNoSpace isEmpty]) {
            continue;
        }
        NSString *firstLetter = [[pinyinNoSpace substringToIndex:1] uppercaseString];//并转成大写
        firstLetter = [IMPackageBuddyData SpecialCharacterConversion:firstLetter];
        
        //把相同首字母的对象收到一个数组里，用首字母当key保存到字典里
        NSMutableArray* firstLetterDataArray = [tempfirstLetterToContactDataDic objectForKey:firstLetter];
        if (firstLetterDataArray == nil) {
            firstLetterDataArray = [[NSMutableArray alloc] init];
            [tempfirstLetterToContactDataDic setObject:firstLetterDataArray forKey:firstLetter];
        }
        [firstLetterDataArray addObject:buddyData];
    }
    
    NSArray* firstLetterArray = [IMPackageBuddyData firstLetterArray:tempfirstLetterToContactDataDic];
    //获取结束把数组按升序重新排列
    for (NSString *eachNameIndex in firstLetterArray) {
        NSSortDescriptor *nameDescriptor = [[NSSortDescriptor alloc] initWithKey:@"buddyNickName" ascending:NO];
        NSArray *descriptors = [NSArray arrayWithObject:nameDescriptor];
        NSMutableArray* someFirstLetterArray = [tempfirstLetterToContactDataDic objectForKey:eachNameIndex];
        [someFirstLetterArray sortUsingDescriptors:descriptors];
    }
    
    return tempfirstLetterToContactDataDic;
}


+(NSArray*)firstLetterArray:(NSDictionary*)contactBuddyDic{
    return [[contactBuddyDic allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
}

#pragma mark private method
+(NSString*)SpecialCharacterConversion:(NSString*)firstLetter{
    NSString* pinyinInitials = @"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    NSRange range = [pinyinInitials rangeOfString:firstLetter];
    if (range.length == 0) {
        firstLetter = @"#";
    }
    return firstLetter;
}

@end


@implementation IMPackageIqData

-(id)initWithIqParams:(IqParams *)iQParams{
    if (self = [super init]) {
        if (iQParams == NULL) {
            return self;
        }
        
        if (iQParams->pSmallNum.empty() == false) {
            self.pSmallNum = [[NSString alloc] initWithUTF8String:iQParams->pSmallNum.c_str()];
        }else{
            self.pSmallNum = @"";
        }
        
        if (iQParams->pTalkId.empty() == false) {
            self.pTalkId = [[NSString alloc] initWithUTF8String:iQParams->pTalkId.c_str()];
        }else{
            self.pTalkId = @"";
        }
        
        if (iQParams->pBody.empty() == false) {
            self.pBody = [[NSString alloc] initWithUTF8String:iQParams->pBody.c_str()];
        }else{
            self.pBody = @"";
        }
        
        self.etype = (IM_IqType)iQParams->eType;
    }
    return self;
}

@end
