//
//  OpenPlatformServers.h
//  Test
//
//  Created by Circle Liu on 14-5-23.
//  Copyright (c) 2014年 Circle Liu. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "WeiboSDK.h"
#import "WXApi.h"
#import "WXApiObject.h"

#import "TencentOpenAPI/QQApiInterface.h"
#import <TencentOpenAPI/TencentOAuth.h>
#import <TencentOpenAPI/sdkdef.h>
#import <TencentOpenAPI/TencentOAuthObject.h>

#define kSinaWeiboAppKey         @"3886771298"
#define kSinaWeiboRedirectURI    @"www.xmatrix.com.cn"

#define kWeixinAppKey            @"wx7c261cfbad3c8800"
#define kWeixinAppSecret         @"75e37de291021507244713be594ab955"

#define kQQAppKey                @"1104631293"

#define SourceID                 @"SourceID"
#define SourceCode               @"SourceCode"

@interface OpenPlatformServers : NSObject <QQApiInterfaceDelegate>

/*********************************** 公共方法接口 ***********************************/

+ (OpenPlatformServers*)share;

- (void)registerServers;

- (BOOL)handleOpenURL:(NSURL *)url;

/*********************************** 新浪微博接口 ***********************************/

/* 设置从新浪微博获取来的token */
+ (void)setSinaWeiboToken:(NSString*)token;

/* 返回从新浪微博获取来的token。暂时没有用 */
+ (NSString*)sinaWeiboToken;

/* 设置WeiboSDK的调试模式
   当开启调试模式时，WeiboSDK会在控制台输出详细的日志信息，开发者可以据此调试自己的程序。默认为 NO */
+ (void)enableDebugMode:(BOOL)enabled;

/* 检查用户手机是否安装了微博客户端程序
   @return 已安装返回YES，未安装返回NO */
+ (BOOL)sinaWeiboAppIsInstalled;

/* 新浪微博登录 */
+ (void)sinaWeiboLogin;

/* 分享文字内容 */
+ (BOOL)shareToWeiBoWithMessage:(NSString*)message;

/* 分享文字内容和图片，图片和媒体内容只能存在一种 */
+ (BOOL)shareToWeiBoWithMessage:(NSString*)message image:(UIImage*)image mediaURL:(NSString*)url;

/* 分享文字内容和媒体内容，图片和媒体内容只能存在一种 */
//+ (BOOL)shareToWeiBoWithMessage:(NSString*)message mediaTitle:(NSString*)title description:(NSString*)description thumbnail:(UIImage*)thumbnail mediaURL:(NSString*)url;

/*********************************** 微信接口 ***********************************/

/* 检查用户手机是否安装了微信客户端程序
 @return 已安装返回YES，未安装返回NO */
+ (BOOL)weiXinAppIsInstalled;

/* 微信登录 */
+(void)WXLogin;

/* 向微信客户端分享链接内容
 content    为分享的文字内容
 title      为分享内容的标题
 thumbImage 为分享内容的缩略图,文档说大小不能超过32K，超过将不跳转至微信
 webpageUrl 为分享内容的链接地址
 scene      0为聊天界面   1为朋友圈   2为收藏 */
+ (void)shareToWeiXinLinkContent:(NSString*)content title:(NSString*)title thumbImage:(UIImage*)thumbImage webpageUrl:(NSString*)webpageUrl scene:(int)scene;

/*********************************** QQ接口 ***********************************/

/* 返回QQ的userID */
- (NSString*)qqUserID;

- (void)qqLogin:(id<TencentSessionDelegate>)delegate;

/* 检查用户手机是否安装了QQ客户端程序
 @return 已安装返回YES，未安装返回NO */
+ (BOOL)qqAppIsInstalled;

/* 向QQ客户端分享链接内容
 content    为分享的文字内容
 title      为分享内容的标题
 thumbImage 为分享内容的缩略图,文档说大小不能超过32K，超过将不跳转至微信
 webpageUrl 为分享内容的链接地址
 0为QQ空间   1为聊天界面       */
+ (void)shareToQQLinkContent:(NSString*)content title:(NSString*)title thumbImage:(UIImage*)thumbImage webpageUrl:(NSString*)webpageUrl type:(int)type;

/********************************* 回调方法 **************************************/

+ (void)didReceiveWeiboResponse:(WBBaseResponse *)response;

+ (void)onReq:(BaseReq*)req;

+ (void)onResp:(BaseResp*)resp;

@end