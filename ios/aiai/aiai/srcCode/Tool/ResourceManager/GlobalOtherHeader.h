//
//  GlobalOtherHeader.h
//  aiai
//
//  Created by zhangyang on 14/12/13.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#ifndef aiai_GlobalOtherHeader_h
#define aiai_GlobalOtherHeader_h

#define IOS7_OR_LATER   ( [[[UIDevice currentDevice] systemVersion] compare:@"7.0"] != NSOrderedAscending )
#define IOS8_OR_LATER   ( [[[UIDevice currentDevice] systemVersion] compare:@"8.0"] != NSOrderedAscending )

#define iPhone5 ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(640, 1136), [[UIScreen mainScreen] currentMode].size) : NO)

#define iPhone6 ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(750, 1334), [[UIScreen mainScreen] currentMode].size) : NO)
#define iPhone6_W_Scale (750.0/640)
#define iPhone6_H_Scale (1334.0/1136)

#define iPhone6_Plus ([UIScreen instancesRespondToSelector:@selector(currentMode)] ? CGSizeEqualToSize(CGSizeMake(1242, 2208), [[UIScreen mainScreen] currentMode].size) : NO)
#define iPhone6P_W_Scale (1242.0/640)
#define iPhone6P_H_Scale (2208.0/1136)

#endif

//通知key
#define USER_LOGIN_NOTIFICATION         @"userLoginNotification"
#define USER_LOGOUT_NOTIFICATION        @"userLogoutNotification"

//推送key
#define PUSH_MESSAGE_NOTIFICATION   3

//地图默认比例
#define MAPZOOMLEVEL                15

//第三方登录通知
#define USEROPENPLATFORMLOGINNOTIFICATION           @"userOpenPlatformLoginNotification" // 第三方账号授权登录
#define USERS_LOGIN_LOGOUT_UPDATE_NOTIFICATION      @"userLoginLogoutUpdateNotification" // 用户登录和注销的时候需要发送通知跟新页面

//联系人详情发起聊天通知
#define SENDMSG_NOTIFY          @"sendMsgToBuddy"

//日志定义
#ifdef AIAI_DEBUG
#define AIAI_LOG(format, ...) NSLog(format, ## __VA_ARGS__)
#else
#define AIAI_LOG(format, ...)
#endif

//下载管理
#define FILTERCACHE                         [SaveManager documentsPath:@"/DownLoad/mp4/"]   //下载特效的存放地址
#define FILTERZIPCACHE                      [SaveManager documentsPath:@"/DownLoad/zip/"]   //特效下载的源文件(压缩文件),只用上边的路径
#define FONTCACHE                           [SaveManager documentsPath:@"/DownLoad/font/"]   //下载特效的存放地址
#define FONTZIPCACHE                        [SaveManager documentsPath:@"/DownLoad/fontzip/"]   //特效下载的源文件(压缩文件),只用上边的路径
#define FILTERFINISH                        @"filter_finishPlist.plist"
#define FONTFINISH                          @"font_finishPlist.plist"

//设置的参数
#define IISETTING   @"IISetting.json"

//此三个选项 1是真0是否
#define SAVE        @"save"
#define AUTOLOAD    @"autoLoad"
#define AUTOPLAY    @"autoPlay"
