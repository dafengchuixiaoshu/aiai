//
//  UserInfo.h
//  F2CPlatform
//
//  Created by zhangyang on 14-12-4.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BaseModel.h"

@interface GlobalUserInfo : BaseModel

@property(nonatomic,copy)NSNumber* id;
@property(nonatomic,copy)NSString* identity;
@property(nonatomic,copy)NSNumber* insertTime;
@property(nonatomic,copy)NSNumber* lastloginTime;
@property(nonatomic,copy)NSString* password;
@property(nonatomic,copy)NSString* realname;
@property(nonatomic,copy)NSString* sex;
@property(nonatomic,copy)NSNumber* status;
@property(nonatomic,copy)NSString* tel;
@property(nonatomic,copy)NSString* username;


+(GlobalUserInfo*)myselfInfo;
+(void)setMyselfInfo:(GlobalUserInfo*)userinfo;

@end
