//
//  RootViewController+InteractionLogic.h
//  aiai
//
//  Created by zhangyang on 15/4/10.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "RootViewController.h"
#import "IMPackageData.h"
#import "IMPackageEngine.h"
#import "LoginMsgCenter.h"
#import "ContactViewController.h"
#import "GroupViewController.h"
#import "ShowMsgViewController.h"

@interface RootViewController (InteractionLogic)<IMPackageEngineDelegate,LoginMsgCenterDelegate,ContactViewControllerDelegate,GroupViewControllerDelegate,ShowMsgViewControllerDelegate>

//创建登录逻辑，包括用户登录和IM登录
-(void)createLoginLogic;

//激活联系人界面，需要判断是否登录
-(void)presentContactViewController;

//发送消息
+(void)sendVedioWithUserinfo:(IMPackageSessionData*)sessionData vedioUrl:(NSURL*)url vedioTime:(CGFloat)time videoTitle:(NSString*)title text:(NSString*)text msgType:(IM_MESSAGE_TYPE)type completion:(void (^)(void))completion;
+(void)sendImageWithUserinfo:(IMPackageSessionData*)sessionData firstFrameImage:(UIImage*)firstFrameImage text:(NSString*)text msgType:(IM_MESSAGE_TYPE)type completion:(void (^)(void))completion;

//更新sessionList
-(void)reloadSessionContactView;
-(void)deleteSessionWithSessionData:(IMPackageSessionData*)sessionData;

//联系人详情通知回调
-(void)insertNewBuddySessionNotify:(NSNotification*)notify;

//设置保存路径
-(void)setImgSetting;

@end
