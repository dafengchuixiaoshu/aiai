//
//  Notification.m
//  aiai
//
//  Created by wangnan on 15/5/5.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "Notification.h"
#import "DeviceSender.h"

@implementation Notification

+(void)registerNotifi{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0) {
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];
        [[UIApplication sharedApplication] registerForRemoteNotifications];
    }else {
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert)];
    }
}

+(void)startAppFromeNotificationWithOptions:(NSDictionary *)launchOptions{
    //远程推送
    if (launchOptions) {
        NSDictionary* pushNotificationKey = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
        if (pushNotificationKey) {
//            NSNumber *type = [pushNotificationKey objectForKey:@"type"];
//            AIAI_LOG(@"type = %@",type);
        }
    }
}

//生成token和将token发送至服务器
+ (void)didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken{
    
    NSString* token = [NSString stringWithFormat:@"%@",deviceToken];
    AIAI_LOG(@"token = %@",deviceToken);
    [DeviceSender SetToken:token];
    [DeviceSender sendDeviceToPushServer:token andUID:[NSString stringWithFormat:@"%zi",[GlobalUserInfo myselfInfo].userID]];
    
}

//处理远程推送
+ (void)didReceiveRemoteNotification:(NSDictionary *)userInfo
{
    NSInteger receiverId = [[userInfo objectForKey:@"receiverId"] integerValue];
    NSInteger type = [[userInfo objectForKey:@"type"] integerValue];
    
    if (type == PUSH_MESSAGE_NOTIFICATION) {
        if (receiverId == [GlobalUserInfo myselfInfo].userID) {
            //之后在后台才会处理
            if ([[AppDelegate currentApp] isForeground]) {
                
            }
        }
    }
}
@end
