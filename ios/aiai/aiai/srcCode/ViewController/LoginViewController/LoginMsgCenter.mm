//
//  LoginMsgCenter.m
//  aiai
//
//  Created by zhangyang on 15/4/2.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "LoginMsgCenter.h"
#import "DeviceSender.h"
#import "IMPackageEngine.h"

#import "LoginViewController.h"

static id<LoginMsgCenterDelegate> g_loginDelegate = nil;
static BOOL isLogin = NO;

@implementation LoginMsgCenter

+(void)notificationLoginResults:(LoginStatus)results loginUser:(GlobalUserInfo*)loginUser{
    
    //记录登录结果
    if (results == LoginStatus_LoginSuccess ||
        results == LoginStatus_OpenPlatformLoginSuccess) {
        isLogin = YES;
    }else {
        isLogin = NO;
    }
    
    //判断登录结果
    switch (results) {
        case LoginStatus_LoginSuccess:
        case LoginStatus_OpenPlatformLoginSuccess:
        {
            //登录成功做的事
            [GlobalUserInfo setMyselfInfo:loginUser];
            
            //登录im
            [[IMPackageEngine sharedInstanceIMPackageEngine] setUserName:[GlobalUserInfo myselfInfo].userName];
            [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_Login];
            
            //获取用户定位状态成功之后再通知 否则定位状态没有赋值
            [self getLocationStatusFromSeverComlate:^{
                //通知登录成功
                [[NSNotificationCenter defaultCenter] postNotificationName:USER_LOGIN_NOTIFICATION object:nil];
            }];
            
            //把推送的token告诉服务器
            [DeviceSender sendDeviceToPushServer:[DeviceSender Token] andUID:[NSString stringWithFormat: @"%zi", loginUser.userID]];
            
        }
            break;
        case LoginStatus_Loginfalse:
        {
            //登录失败
        }
            break;
        case LoginStatus_Logout:
        {
            //token和服务器断开
            [DeviceSender ReleseTokenFromServer];
            [GlobalUserInfo setMyselfInfo:nil];
            
            //注销IM
            [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_Logout];
            [[IMPackageEngine sharedInstanceIMPackageEngine] setUserName:@""];
            
            //清理appicon上的推送数量为0
            [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
            
            //通知menuView退出登录
            [[NSNotificationCenter defaultCenter] postNotificationName:USER_LOGOUT_NOTIFICATION object:nil];
        }
            
        default:
            break;
    }
    
    //处理之后通知代理结果
    if (g_loginDelegate && [g_loginDelegate respondsToSelector:@selector(loginResults:)]) {
        [g_loginDelegate loginResults:results];
    }
    g_loginDelegate = nil;
}

+(BOOL)isLogin{
    return isLogin;
}

+(void)setDelegate:(id<LoginMsgCenterDelegate>)delegate{
    g_loginDelegate = delegate;
}

+(void)presentLoginViewControllerDelegate:(id<LoginMsgCenterDelegate>)delegate Completion:(void (^)(void))completion{
    g_loginDelegate = delegate;
    
    LoginViewController* loginVC = [[LoginViewController alloc] init];
    loginVC.title = @"登录";
    loginVC.leftItemBtnBlock = ^{
        [LoginMsgCenter dismissLoginViewControllerCompletion:nil];
    };
    UINavigationController* navi = [[UINavigationController alloc] initWithRootViewController:loginVC];
    navi.navigationBar.hidden = YES;
    [[AppDelegate rootViewController] presentViewController:navi animated:YES completion:completion];
}

+(void)dismissLoginViewControllerCompletion:(void (^)(void))completion{
    [[AppDelegate rootViewController] dismissViewControllerAnimated:YES completion:completion];
}


#pragma mark 注册登录相关接口
//注册
+(void)registerRequestWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(BOOL results, GlobalUserInfo* loginUser))comlate{
    com::mm::pb::po::InputParamsPOJO inputObj;
    //所有数据都必传，如果没有则用电话代替
    if (userInfo.userName) {
        inputObj.set_username([userInfo.userName UTF8String]);
    }else{
        inputObj.set_username([userInfo.userTelphone UTF8String]);
    }
    if (userInfo.userPassWord) {
        inputObj.set_password([userInfo.userPassWord UTF8String]);
    }
    if (userInfo.userTelphone) {
        inputObj.set_mobile([userInfo.userTelphone UTF8String]);
    }
    if (userInfo.userFaceUrl) {
        inputObj.set_faceurl([userInfo.userFaceUrl UTF8String]);
    }
    if (userInfo.userFaceVedio) {
        inputObj.set_facevedio([userInfo.userFaceVedio UTF8String]);
    }
    if (userInfo.userNickName) {
        inputObj.set_nickname([userInfo.userNickName UTF8String]);
    }
    if (userInfo.openPlatformId && [userInfo.openPlatformId length] > 0) {
        inputObj.set_thirdpid([userInfo.openPlatformId UTF8String]);
        inputObj.set_thirdwid(userInfo.openPlatformcode.integerValue);
    }
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"register.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code < 0) {
            AIAI_LOG(@"resultcode error");
            comlate(NO,nil);
            return;
        }
        int listCount = outputOjb.list_size();
        if (listCount<=0) {
            AIAI_LOG(@"list_size error");
            comlate(NO,nil);
            return;
        }
        ::com::mm::pb::po::ReturnUser user = outputOjb.list(0);
        
        GlobalUserInfo* oneUser = [[GlobalUserInfo alloc] init];
        oneUser.userID = user.id();
        oneUser.userName = [NSString stringWithUTF8String:user.username().c_str()];
        oneUser.userPassWord = [userInfo userPassWord];
        oneUser.userTelphone = oneUser.userName;
        oneUser.userNickName = [NSString stringWithUTF8String:user.nickname().c_str()];
        oneUser.userFaceUrl = [NSString stringWithUTF8String:user.faceurl().c_str()];
        oneUser.userFaceVedio = [NSString stringWithUTF8String:user.facevedio().c_str()];
        oneUser.userLongitude = user.longitude();
        oneUser.userLatitude = user.latitude();
        oneUser.userGender = user.gender();
        
        comlate(YES,oneUser);
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"%@",error);
        comlate(NO,nil);
    }];
}

//登录
+(void)loginRequestWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(BOOL results,GlobalUserInfo* loginUser))comlate{
    com::mm::pb::po::InputParamsPOJO inputObj;
    inputObj.set_username([userInfo.userName UTF8String]);
    inputObj.set_password([userInfo.userPassWord UTF8String]);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"frontLogin.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code < 0) {
            AIAI_LOG(@"resultcode error");
            comlate(NO,nil);
            return;
        }
        int listCount = outputOjb.list_size();
        if (listCount<=0) {
            AIAI_LOG(@"list_size error");
            comlate(NO,nil);
            return;
        }
        ::com::mm::pb::po::ReturnUser user = outputOjb.list(0);
        
        GlobalUserInfo* oneUser = [[GlobalUserInfo alloc] init];
        oneUser.userID = user.id();
        oneUser.userName = [NSString stringWithUTF8String:user.username().c_str()];
        oneUser.userPassWord = [userInfo userPassWord];
        oneUser.userTelphone = oneUser.userName;
        oneUser.userNickName = [NSString stringWithUTF8String:user.nickname().c_str()];
        oneUser.userFaceUrl = [NSString stringWithUTF8String:user.faceurl().c_str()];
        oneUser.userFaceVedio = [NSString stringWithUTF8String:user.facevedio().c_str()];
        oneUser.userGender = user.gender();
        
        comlate(YES,oneUser);
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"%@",error);
        comlate(NO,nil);
    }];
}

//第三方登录
+(void)OpenPlatformLoginRequestWithPlatformID:(NSString*)openPlatformID OpenPlatformCode:(NSString*)sourceCode prepare:(void (^)(void))prepare comlate:(void (^)(LoginStatus results,GlobalUserInfo* loginUser))comlate{
    
    com::mm::pb::po::InputParamsPOJO inputObj;
    inputObj.set_thirdpid(openPlatformID.UTF8String);
    inputObj.set_thirdwid(sourceCode.integerValue);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"frontLogin.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        prepare();
    } successed:^(NSDictionary *dic) {
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        //未进行注册
        if (code == LoginStatus_Unregister) {
            comlate(LoginStatus_Unregister,nil);
            return;
        }
        int listCount = outputOjb.list_size();
        if (listCount<=0) {
            AIAI_LOG(@"list_size error");
            comlate(LoginStatus_Loginfalse,nil);
            return;
        }
        ::com::mm::pb::po::ReturnUser user = outputOjb.list(0);
        
        GlobalUserInfo* oneUser = [[GlobalUserInfo alloc] init];
        oneUser.userID = user.id();
        oneUser.userName = [NSString stringWithUTF8String:user.username().c_str()];
//        oneUser.userPassWord = [userInfo userPassWord];
        oneUser.userTelphone = oneUser.userName;
        oneUser.userNickName = [NSString stringWithUTF8String:user.nickname().c_str()];
        oneUser.userFaceUrl = [NSString stringWithUTF8String:user.faceurl().c_str()];
        oneUser.userFaceVedio = [NSString stringWithUTF8String:user.facevedio().c_str()];
        oneUser.userGender = user.gender();
        
        comlate(LoginStatus_OpenPlatformLoginSuccess,oneUser);
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"%@",error);
        comlate(LoginStatus_Loginfalse,nil);
    }];
    
}

//注销
+(void)logoutRequestWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(void))comlate{
    com::mm::pb::po::InputParamsPOJO inputObj;
    inputObj.set_uid((uint32_t)userInfo.userID);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"logout.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code < 0) {
            AIAI_LOG(@"resultcode error");
            return;
        }
        comlate();
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"%@",error);
    }];
}

//修改用户信息
+(void)changeUserInfoRequestWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(void))comlate fail:(void (^)(void))fail{
    com::mm::pb::po::InputParamsPOJO inputObj;
    inputObj.set_uid((uint32_t)userInfo.userID);
    if (userInfo.userNickName != nil && [userInfo.userNickName length] > 0) {
        inputObj.set_nickname([userInfo.userNickName UTF8String]);
    }
//    if (userInfo.userGender == 0 || userInfo.userGender == 1) {
//        inputObj.set_gender((uint32_t)userInfo.userGender);
//    }
    if (userInfo.userFaceUrl != nil && [userInfo.userFaceUrl length] != 0) {
        inputObj.set_faceurl([userInfo.userFaceUrl UTF8String]);
    }
    if (userInfo.userFaceVedio != nil && [userInfo.userFaceVedio length] != 0) {
        inputObj.set_facevedio([userInfo.userFaceVedio UTF8String]);
    }
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"updateUserMsg.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {

        NSData* data = [dic objectForKey:@"data"];
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code < 0) {
            AIAI_LOG(@"resultcode error");
            fail();
            return;
        }
        else{
            comlate();
        }
        
    } failed:^(NSError *error) {
        fail();
        AIAI_LOG(@"%@",error);
    }];
}

//检测手机号
+(void)checkPhoneNumRequestWithPhoneNum:(NSString*)phoneNum comlate:(void (^)(void))comlate{
    com::mm::pb::po::InputParamsPOJO inputObj;
    inputObj.set_mobile([phoneNum UTF8String]);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"checkMobile.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code < 0) {
            AIAI_LOG(@"检测手机号 error");
            return;
        }
        comlate();
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"%@",error);
    }];
}

//获取验证码
+(void)identifyingCodeRequestWithPhoneNum:(NSString*)phoneNum comlate:(void (^)(void))comlate{
    com::mm::pb::po::InputParamsPOJO inputObj;
    inputObj.set_mobile([phoneNum UTF8String]);
    inputObj.set_messagetype(3);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"sendValidateMessage.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code < 0) {
            AIAI_LOG(@"sendValidateMessage.pb resultcode error");
            return;
        }
        comlate();
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"sendValidateMessage.pb = %@",error);
    }];
}

//校验验证码
+(void)checkIdentifyingCodeRequestWithMobile:(NSString*)phoneNum identifyingCode:(NSString*)identifyingCode comlate:(void (^)(void))comlate{
    com::mm::pb::po::InputParamsPOJO inputObj;
    inputObj.set_mobile([phoneNum UTF8String]);
    inputObj.set_message([identifyingCode UTF8String]);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"checkMobileMessage.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code != 0) {
            AIAI_LOG(@"sendValidateMessage.pb resultcode error");
            return;
        }
        comlate();
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"sendValidateMessage.pb = %@",error);
    }];
}

//获取3km范围内附近人的信息
+(void)getNearFriendsWithUserInfo:(GlobalUserInfo*)userInfo comlate:(void (^)(NSArray* friendsList))comlate {
    com::mm::pb::po::InputParamsPOJO inputObj;
    inputObj.set_uid((uint32_t)userInfo.userID);
    inputObj.set_latitude(userInfo.userLatitude);
    inputObj.set_longitude(userInfo.userLongitude);
    inputObj.set_radius(3000);//暂时默认3km
    
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"nearFri.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code < 0) {
            AIAI_LOG(@"resultcode error");
            return;
        }
        int listCount = outputOjb.list_size();
        if (listCount<=0) {
            AIAI_LOG(@"list_size error");
            return;
        }
        
        NSMutableArray* friendList = [[NSMutableArray alloc]init];
        for (int i = 0; i<outputOjb.list_size(); i++) {
            ::com::mm::pb::po::ReturnUser user = outputOjb.list(i);
            GlobalUserInfo* oneFriend = [[GlobalUserInfo alloc] init];
            oneFriend.userID = user.id();
            oneFriend.userName = [NSString stringWithUTF8String:user.username().c_str()];
            oneFriend.userTelphone = oneFriend.userName;
            oneFriend.userNickName = [NSString stringWithUTF8String:user.nickname().c_str()];
            oneFriend.userFaceUrl = [NSString stringWithUTF8String:user.faceurl().c_str()];
            oneFriend.userFaceVedio = [AppUtils compatibleImageKeyWithKey:[NSString stringWithUTF8String:user.facevedio().c_str()]] ;
            oneFriend.userGender = user.gender();
            oneFriend.userLatitude = user.latitude();
            oneFriend.userLongitude = user.longitude();
            
            [friendList addObject:oneFriend];
        }
        
        comlate(friendList);
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"%@",error);
    }];
}

//登陆成功后获取用户定位状态
+(void)getLocationStatusFromSeverComlate:(void (^)(void))comlate{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"getSwitchStatus";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    tutorial::RequestParam_Param *param1 = builder.add_param();
    param1->set_paramkey([@"uid" UTF8String]);
    NSString *value1 = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    param1->set_paramvalue([value1 UTF8String]);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    NSString* requestUrlString = [NSString stringWithFormat:@"http://182.92.189.13:9980/httpProxy"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::SendStatus outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        //获取状态后存入userinfo 可以在userinfo中取值
        [GlobalUserInfo myselfInfo].Locstatus = outPutObj.status();
        comlate();
        
    } failed:^(NSError *error) {
        comlate();
    }];
}

//设置用户定位状态(开:关)
+(void)setLocationStatusToSeverWithSwitchStatus:(NSInteger)status{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"nearSwitch";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    tutorial::RequestParam_Param *param1 = builder.add_param();
    param1->set_paramkey([@"uid" UTF8String]);
    NSString *value1 = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    param1->set_paramvalue([value1 UTF8String]);
    
    tutorial::RequestParam_Param *param2 = builder.add_param();
    param2->set_paramkey([@"status" UTF8String]);
    param2->set_paramvalue([[NSString stringWithFormat:@"%ld",status] UTF8String]);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    NSString* requestUrlString = [NSString stringWithFormat:@"http://182.92.189.13:9980/httpProxy"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::SendStatus outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        //如果修改成功再保存
        if (outPutObj.status() == 1) {
            [GlobalUserInfo myselfInfo].Locstatus = outPutObj.status();
        }
        
    } failed:^(NSError *error) {
        
    }];
}

@end
