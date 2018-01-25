//
//  RootViewController+InteractionLogic.m
//  aiai
//
//  Created by zhangyang on 15/4/10.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "RootViewController+InteractionLogic.h"


@implementation RootViewController (InteractionLogic)


-(void)createLoginLogic{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(loginSucessNotification:) name:USER_LOGIN_NOTIFICATION object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(logoutSucessNotification:) name:USER_LOGOUT_NOTIFICATION object:nil];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] addDelegate:self];
}

-(void)loginSucessNotification:(NSNotification*)notification{
    //刷新历史记录联系人和分享等一些固定数据
    [self reloadSessionContactView];
    //登录成功就设置IM的经纬度
    [NSTimer scheduledTimerWithTimeInterval:10.0 target:self selector:@selector(setLocationWithUserLocation) userInfo:nil repeats:YES];
}

-(void)logoutSucessNotification:(NSNotification*)notification{
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self reloadSessionContactView];
    });
    
}

#pragma mark 设置用户当前位置
-(void)setLocationWithUserLocation{
    NSString* lonString = [[NSNumber numberWithDouble:[GlobalUserInfo myselfInfo].userLongitude] stringValue];
    NSString* latString = [[NSNumber numberWithDouble:[GlobalUserInfo myselfInfo].userLatitude] stringValue];
    [[IMPackageEngine sharedInstanceIMPackageEngine] setMapPointWithLongitude:lonString Latitude:latString];
}

#pragma mark ContactViewControllerDelegate
-(void)presentContactViewController{
    if ([LoginMsgCenter isLogin]) {
        ContactViewController* vc = [[ContactViewController alloc] init];
        vc.leftBlock = ^{
            [self dismissViewControllerAnimated:YES completion:nil];
        };
        vc.delegate = self;
        vc.groupDel = self;
        UINavigationController* navc = [[UINavigationController alloc] initWithRootViewController:vc];
        navc.navigationBarHidden = YES;
        [self.navigationController presentViewController:navc animated:YES completion:nil];
    }else{
        [LoginMsgCenter presentLoginViewControllerDelegate:self Completion:nil];
    }
}

-(void)insertNewBuddySessionNotify:(NSNotification*)notify{
    IMPackageBuddyData* entity = [notify object];
    [self insertNewBuddySession:entity];
}

-(void)selectBuddyEntity:(IMPackageBuddyData*)entity{
    [self dismissViewControllerAnimated:YES completion:^{
        [self insertNewBuddySession:entity];
    }];
}

-(void)insertNewBuddySession:(IMPackageBuddyData*)entity{
    //需要把buddydata转换成sessionData
    IMPackageSessionData* sessionData = [[IMPackageSessionData alloc] initWithIMPackageBuddyData:entity];
    NSMutableArray* tmpArray = [NSMutableArray arrayWithArray:self.contactView.contactArray];
    BOOL isExist = NO;
    for (int i = 0;i<[tmpArray count];i++) {
        IMPackageSessionData* oneData = [tmpArray objectAtIndex:i];
        if ([oneData.pSessionId isEqualToString:sessionData.pSessionId]) {
            [tmpArray exchangeObjectAtIndex:i withObjectAtIndex:0];
            isExist = YES;
            break;
        }
    }
    if (isExist == NO) {
        [tmpArray insertObject:sessionData atIndex:0];
    }
    self.contactView.contactArray = tmpArray;
    [self.contactView.myCollectionView reloadData];
}

-(void)selectGroupEntity:(IMPackageGroupData*)groupEntity{
    [self dismissViewControllerAnimated:YES completion:^{
        //由于新建群组只有群ID 所以需要重新获取一下群组信息
        IMPackageGroupData *groupData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_getGroupInfoWithGroupId:groupEntity.groupID];
        //需要把buddydata转换成sessionData
        IMPackageSessionData* sessionData = [[IMPackageSessionData alloc] initWithIMPackageGroupData:groupData];
        NSMutableArray* tmpArray = [NSMutableArray arrayWithArray:self.contactView.contactArray];
        
        BOOL isExist = NO;
        for (int i = 0;i<[tmpArray count];i++) {
            IMPackageSessionData* oneData = [tmpArray objectAtIndex:i];
            if ([oneData.pSessionId isEqualToString:sessionData.pSessionId]) {
                [tmpArray exchangeObjectAtIndex:i withObjectAtIndex:0];
                isExist = YES;
                break;
            }
        }
        if (isExist == NO) {
            [tmpArray insertObject:sessionData atIndex:0];
        }
        self.contactView.contactArray = tmpArray;
        [self.contactView.myCollectionView reloadData];
    }];
}

#pragma LoginDelegate
-(void)loginResults:(LoginStatus)results{
    if (results == LoginStatus_LoginSuccess) {
        [LoginMsgCenter dismissLoginViewControllerCompletion:^{
            ContactViewController* vc = [[ContactViewController alloc] init];
            vc.delegate = self;
            vc.groupDel = self;
            UINavigationController* navc = [[UINavigationController alloc] initWithRootViewController:vc];
            navc.navigationBarHidden = YES;
            [self.navigationController presentViewController:navc animated:YES completion:nil];
        }];
    }
}

#pragma ShowMsgViewControllerDelegate
-(void)willDisappear:(ShowMsgViewController *)vc{
    //移除退后一个添加按钮(因为在set里重新会添加,不敢改)
//    if ([self.contactView.contactArray count] > 0) {
//        [self.contactView.contactArray removeLastObject];
//    }
    //只是简单的刷新个数等 不需要把整个list重刷 因为期间会改变session的位置 , 如果重刷位置就重置了 , 体验不好
    self.contactView.contactArray = self.contactView.contactArray;
}

#pragma mark IMEngine_Delegate
-(void)IMPackageLoginStatus:(int)detaileType{
    if (detaileType == kIM_MsgType_Login_sucess_Key){
        [self reloadSessionContactView];
    }else if(detaileType == kIM_MsgType_Login_faild_Key){
        
    }else if(detaileType == kIM_MsgType_Login_noExist_Key){
        
    }
}

//发送视频和图片
+(void)sendVedioWithUserinfo:(IMPackageSessionData*)sessionData vedioUrl:(NSURL*)url vedioTime:(CGFloat)time videoTitle:(NSString*)title text:(NSString*)text msgType:(IM_MESSAGE_TYPE)type completion:(void (^)(void))completion{
    
    //根据视频提取第一贞图片
    UIImage *firstFrameImage = [AppUtils getImage:url];
    NSString* savePath = [RootViewController saveImgAndGetPathWithSrcImg:firstFrameImage];
    
    //发送视频
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_SendVideoMsgWithToUser:sessionData.pSessionId videoPath:url.resourceSpecifier faceImagePath:savePath textBuffer:nil videoTitle:title videoLength:(int)time msgType:type];
    if (completion) {
        completion();
    }
}

+(void)sendImageWithUserinfo:(IMPackageSessionData*)sessionData firstFrameImage:(UIImage*)firstFrameImage text:(NSString*)text msgType:(IM_MESSAGE_TYPE)type completion:(void (^)(void))completion{
    //根据视频提取第一贞图片
    NSString* savePath = [RootViewController saveImgAndGetPathWithSrcImg:firstFrameImage];
    //发送图片的例子
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_SendImgMsgWithToUser:sessionData.pSessionId imgPath:savePath textBuffer:nil imgWidth:480 imgHeight:480 msgType:type];
    completion();
}

-(void)reloadSessionContactView{
    dispatch_async(dispatch_get_main_queue(), ^{
        NSArray* sessionlist = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSessionList];
        self.contactView.contactArray = (NSMutableArray*)sessionlist;
        [self.contactView addSpecialBuddyData];
        [self.contactView.myCollectionView reloadData];
    });
}

-(void)deleteSessionWithSessionData:(IMPackageSessionData*)sessionData{
    if (sessionData.pSessionId) {
        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_RemoveSession:sessionData.pSessionId];
    }
}

- (void)IMPackageRecvMessageType:(int)detaileType msgData:(IMPackageMessageData*)msgModel{
    
    AIAI_LOG(@"收到消息");
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.contactView receiveMsgUpdateCollectionCellWithIMPackageMessageData:msgModel];
    });
}

- (void)IMPackageSendMsgProcessWithMsgModel:(IMPackageMessageData*)msgModel{
    AIAI_LOG(@"发送消息中");
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.contactView willSendingMsgUpdateCollectionCellWithIMPackageMessageData:msgModel];
    });
}

- (void)IMPackageSendMsgSuccessWithMsgModel:(IMPackageMessageData*)msgModel{
    AIAI_LOG(@"发送消息成功");
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.contactView didSendMsgSuccessUpdateCollectionCellWithIMPackageMessageData:msgModel];
    });
}
- (void)IMPackageSendMsgFailWithMsgModel:(IMPackageMessageData*)msgModel{
    AIAI_LOG(@"发送消息失败");
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.contactView didSendMsgFaildUpdateCollectionCellWithIMPackageMessageData:msgModel];
    });
}

#pragma mark 保存图片获取路径
+(NSString*)saveImgAndGetPathWithSrcImg:(UIImage*)img{
    
    NSData *data = nil;
    if (UIImagePNGRepresentation(img) == nil) {
        data = UIImageJPEGRepresentation(img, 1);
    } else {
        data = UIImagePNGRepresentation(img);
    }
    
    UInt64 recordTime = [[NSDate date] timeIntervalSince1970]*1000;
    NSString *path = [NSString stringWithFormat:@"%lli.png", recordTime];
    NSString *imgPath = [SaveManager tmpPath:path];
    [SaveManager saveToFileWithData:data filePath:imgPath];
    
    return imgPath;
}

-(void)setImgSetting{
    NSString *path = [SaveManager libraryPath:IISETTING];
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) {
        [[NSFileManager defaultManager] createFileAtPath:path contents:nil attributes:nil];
    }
    NSData *settingData = [SaveManager readToDataWithFilePath:path];
    NSMutableDictionary *settingDic = [NSKeyedUnarchiver unarchiveObjectWithData:settingData];
    
    if (!settingDic) {
        settingDic = [[NSMutableDictionary alloc] init];
    }
    
    NSNumber *userId = [NSNumber numberWithInteger:[GlobalUserInfo myselfInfo].userID];
    NSMutableDictionary *userDic = [settingDic objectForKey:userId];
    if (!userDic) {
        
        userDic = [[NSMutableDictionary alloc] init];
        [userDic setObject:[NSNumber numberWithInt:1] forKey:SAVE];
        [userDic setObject:[NSNumber numberWithInt:1] forKey:AUTOLOAD];
        [userDic setObject:[NSNumber numberWithInt:1] forKey:AUTOPLAY];
        
        [settingDic setObject:userDic forKey:userId];
        
        NSData* settingData = [NSKeyedArchiver archivedDataWithRootObject:settingDic];
        [SaveManager saveToFileWithData:settingData filePath:path];
    }
}

@end
