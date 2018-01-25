//
//  PushNotification.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/10/28.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "PushNotification.h"

@implementation PushNotification

+(void)registerPushNotification{
#if __IPHONE_8_0 < __IPHONE_OS_VERSION_MAX_ALLOWED
        UIUserNotificationSettings *notiSettings = [UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeBadge | UIUserNotificationTypeAlert | UIUserNotificationTypeSound) categories:nil];
        [[UIApplication sharedApplication] registerUserNotificationSettings:notiSettings];
#else
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:
         UIRemoteNotificationTypeBadge |
         UIRemoteNotificationTypeSound |
         UIRemoteNotificationTypeAlert];
#endif
}

+(void)addDeviceTokenToPushSevers:(NSData*)pToken{
    //需要把这个token添加到服务器上，否则不能接受推送
}

+(NSString*)pushMsgWithDic:(NSDictionary *)userInfo{
    NSLog(@"userInfo == %@",userInfo);
    NSString *message = [[userInfo objectForKey:@"aps"]objectForKey:@"alert"];
    return message;
}

+(void)registerFaild:(NSError *)error{
    //失败处理
}

@end
