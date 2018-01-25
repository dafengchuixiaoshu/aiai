//
//  LoginMsgCenter.h
//  aiai
//
//  Created by zhangyang on 15/4/2.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "InputParamsPOJO.pb.h"
#import "OutputResultData.pb.h"
#import "requestParam.pb.h"
#import "sendStatus.pb.h"

typedef NS_ENUM(NSInteger, LoginStatus) {
    
    LoginStatus_LoginSuccess,
    LoginStatus_Loginfalse,
    LoginStatus_Logout,
    LoginStatus_ChangePassword,
    
    LoginStatus_OpenPlatformLoginSuccess,
    //第三方登录未注册
    LoginStatus_Unregister = -6,
};

@protocol LoginMsgCenterDelegate <NSObject>

@optional
-(void)loginResults:(LoginStatus)results;

@end

@interface LoginMsgCenter : NSObject

+(void)notificationLoginResults:(LoginStatus)results loginUser:(GlobalUserInfo*)loginUser;

+(BOOL)isLogin;
+(void)setDelegate:(id<LoginMsgCenterDelegate>)delegate;
+(void)presentLoginViewControllerDelegate:(id<LoginMsgCenterDelegate>)delegate Completion:(void (^)(void))completion;
+(void)dismissLoginViewControllerCompletion:(void (^)(void))completion;


//注册
+(void)registerRequestWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(BOOL results,GlobalUserInfo* loginUser))comlate;
//登录
+(void)loginRequestWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(BOOL results,GlobalUserInfo* loginUser))comlate;
//第三方登录
+(void)OpenPlatformLoginRequestWithPlatformID:(NSString*)openPlatformID OpenPlatformCode:(NSString*)sourceCode prepare:(void (^)(void))prepare comlate:(void (^)(LoginStatus results,GlobalUserInfo* loginUser))comlate;
//注销
+(void)logoutRequestWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(void))comlate;
//修改用户信息
+(void)changeUserInfoRequestWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(void))comlate fail:(void (^)(void))fail;
//检测手机号
+(void)checkPhoneNumRequestWithPhoneNum:(NSString*)phoneNum comlate:(void (^)(void))comlate;
//获取验证码
+(void)identifyingCodeRequestWithPhoneNum:(NSString*)phoneNum comlate:(void (^)(void))comlate;
//校验验证码
+(void)checkIdentifyingCodeRequestWithMobile:(NSString*)phoneNum identifyingCode:(NSString*)identifyingCode comlate:(void (^)(void))comlate;

//获取附近的人信息
+(void)getNearFriendsWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(NSArray* friendsList))comlate;

//登陆成功后获取用户定位状态
+(void)getLocationStatusFromSeverComlate:(void (^)(void))comlate;

//设置用户定位状态(开:关)
+(void)setLocationStatusToSeverWithSwitchStatus:(NSInteger)status;
@end
