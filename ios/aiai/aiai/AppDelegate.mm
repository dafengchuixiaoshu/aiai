//
//  AppDelegate.m
//  aiai
//
//  Created by zhangyang on 14/11/30.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "AppDelegate.h"
#import "AppDelegate+OtherFuntion.h"

#import "Notification.h"
#import "BackgroundUpAddressBookHander.h"
#import "IMPackageEngine.h"
#import "AppraisalServer.h"

@interface AppDelegate ()<WeiboSDKDelegate, WXApiDelegate>

@end


@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    //设置不云备份属性
    [AppDelegate addSkipBackupAttribute];
    //启动百度地图控制器
    [AppDelegate install_BMKMapManager];
    
    // Override point for customization after application launch.
    RootViewController* con = [[RootViewController alloc] init];
    UINavigationController* naviCon = [[UINavigationController alloc] initWithRootViewController:con];
    naviCon.navigationBarHidden = YES;
    
    self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    self.window.rootViewController = naviCon;
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    
    //初始化IM
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_Init];
    //忽略sigpipe信号
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_IgnoreSIGPIPE];
    
    // 第三方应用注册
    [[OpenPlatformServers share] registerServers];

    //注册推送
    [Notification registerNotifi];
    [Notification startAppFromeNotificationWithOptions:launchOptions];
    
    //百度统计
    [BaiduStatistics start];
    
    //后台上传通讯录
    [BackgroundUpAddressBookHander upAddressBook];
    
    //App评价模块
    [AppraisalServer start];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.

//    self.isForeground = YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
//    self.isForeground = NO;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    
    // 进入后台后，断开与IM的链接5
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_DisConnect];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    
    //唤醒时重新连接(登录)
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_Login];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    
}

#pragma mark - 实现远程推送需要实现的监听接口
- (void)application:(UIApplication *)app didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    
    [Notification didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
    
}
- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
    NSLog(@"apns -> 注册推送功能时发生错误， 错误信息:\n %@", error);
}
//处理远程推送
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo{
    [Notification didReceiveRemoteNotification:userInfo];
}

//可以禁止横屏
- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
    return UIInterfaceOrientationMaskPortrait;
}


- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    BOOL temp = NO;
    
    if ([[url absoluteString] hasPrefix:@"wb"]) {
        temp = [WeiboSDK handleOpenURL:url delegate:self];
    }
    else if ([[url absoluteString] hasPrefix:@"wx"]) {
        temp = [WXApi handleOpenURL:url delegate:self];
    }
    else if ([[url absoluteString] hasPrefix:@"tencent"]) {
        temp = [[OpenPlatformServers share] handleOpenURL:url];
    }
    else if ([url.host isEqualToString:@"safepay"]) {
//        [[AlipaySDK defaultService] processOderWithPaymentResult:url standbyCallback:^(NSDictionary *resultDic) {
//            switch ([[resultDic objectForKey:@"resultStatus"] intValue]) {
//                case AliPayFailed:
//                    ALERT(@"支付失败");
//                    break;
//                case AliPayCancel:
//                    ALERT(@"用户取消支付");
//                    break;
//                case AliPayNetWorkError:
//                    ALERT(@"网络连接失败");
//                    break;
//                case AliPaying:
//                    ALERT(@"支付处理中");
//                    break;
//                case AliPaySuccessed:
//                    [AppDelegate switchingToMyOrderView];
//                    break;
//                default:
//                    break;
//            };
//        }];
    }
    
    return temp;
}

#pragma mark 新浪微博回调接口

- (void)didReceiveWeiboRequest:(WBBaseRequest *)request
{
    if ([request isKindOfClass:WBProvideMessageForWeiboRequest.class])
    {
    }
}

- (void)didReceiveWeiboResponse:(WBBaseResponse *)response
{
    [OpenPlatformServers didReceiveWeiboResponse:response];
}

#pragma mark 微信 回调接口

- (void)onReq:(BaseReq*)req
{
    [OpenPlatformServers onReq:req];
}

- (void)onResp:(BaseResp*)resp
{
    [OpenPlatformServers onResp:resp];
}
@end
