//
//  UserInfo.h
//  F2CPlatform
//
//  Created by zhangyang on 14-12-4.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface GlobalUserInfo : NSObject

@property(nonatomic,assign)NSInteger userID;
@property(nonatomic,copy)NSString* userName;
@property(nonatomic,copy)NSString* userPassWord;
@property(nonatomic,copy)NSString* userTelphone;
@property(nonatomic,copy)NSString* userFaceUrl;
@property(nonatomic,copy)NSString* userFaceVedio;
@property(nonatomic,copy)NSString* userNickName;
@property(nonatomic,assign)NSInteger userGender;
@property(nonatomic,assign)NSInteger Locstatus;       //判断用户是否已开启定位

@property(nonatomic,assign)double userLongitude;
@property(nonatomic,assign)double userLatitude;

//第三方登录使用
@property(nonatomic, copy)NSString *openPlatformId;
@property(nonatomic, copy)NSString *openPlatformcode;

+(GlobalUserInfo*)myselfInfo;
+(void)setMyselfInfo:(GlobalUserInfo*)userinfo;

@end
