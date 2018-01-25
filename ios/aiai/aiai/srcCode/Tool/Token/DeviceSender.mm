//
//  DeviceSender.m
//  aiai
//
//  Created by wangnan on 15/4/30.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "DeviceSender.h"
#import "InputParamsPOJO.pb.h"
#import "OutputResultData.pb.h"

@implementation DeviceSender

static NSString *token;

+(NSString *)Token{
    return token;
}

+(void)SetToken:(NSString *)theToken{
    token = theToken;
}

+(void)registerNotifi{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0) {
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];
        [[UIApplication sharedApplication] registerForRemoteNotifications];
    }else {
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert)];
    }
}

+(void)sendDeviceToPushServer:(NSString*)deviceToken andUID:(NSString *)uID
{
    if (deviceToken&&deviceToken.length>0) {
        token = deviceToken;
        
        NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
        NSString *app_Version = [infoDictionary objectForKey:@"CFBundleShortVersionString"];
        
        NSString *strUID=@"";
        if (uID&&uID.length>0) {
            strUID = [NSString stringWithFormat:@",'uid':'%@'",uID];
        }else{
            return;
        }
        int formal;//是否使用正式证书:0.不使用；1.使用
#ifdef NO_APP_STORE_VERTION
        formal = 0;
#else
        formal = 1;
#endif
        
        com::mm::pb::po::InputParamsPOJO inputObj;
        
        inputObj.set_iphonetoken([token UTF8String]);
        inputObj.set_version([app_Version UTF8String]);
        inputObj.set_source([@"ios" UTF8String]);
        inputObj.set_uid(88);
        inputObj.set_formal(formal);
        
        //申请内存对数据进行序列化
        ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
        inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
        
        //序列化之后进行网络请求
        NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"machineLoginToken.pb"];
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
            
            
        } failed:^(NSError *error) {
            
        }];
    }
}

+(void)ReleseTokenFromServer{
    
    com::mm::pb::po::InputParamsPOJO inputObj;
    
    inputObj.set_iphonetoken([token UTF8String]);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
    inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"machineLogoutToken.pb"];
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
        
        
    } failed:^(NSError *error) {
        
    }];
    
}

@end
