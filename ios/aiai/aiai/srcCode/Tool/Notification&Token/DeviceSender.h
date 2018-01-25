//
//  DeviceSender.h
//  aiai
//
//  Created by wangnan on 15/4/30.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DeviceSender : NSObject

//get方法
+(NSString *)Token;
//set方法
+(void)SetToken:(NSString *)theToken;



//发送注册token
+(void)sendDeviceToPushServer:(NSString*)deviceToken andUID:(NSString *)uID;

//退出时release token
+(void)ReleseTokenFromServer;

@end
