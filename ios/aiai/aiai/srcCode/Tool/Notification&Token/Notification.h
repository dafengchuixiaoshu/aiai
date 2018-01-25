//
//  Notification.h
//  aiai
//
//  Created by wangnan on 15/5/5.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Notification : NSObject

//注册消息通道
+(void)registerNotifi;

//点击推送启动App
+(void)startAppFromeNotificationWithOptions:(NSDictionary *)launchOptions;

//生成token和将token发送至服务器
+ (void)didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken;

//处理远程推送
+ (void)didReceiveRemoteNotification:(NSDictionary *)userInfo;

@end
