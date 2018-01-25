//
//  CheckSoft.m
//  F2CPlatform
//
//  Created by zhangyang on 13-11-28.
//  Copyright (c) 2013年 xiehaiduo. All rights reserved.
//

#import "CheckAppUpgrade.h"

@implementation CheckAppUpgrade

#pragma mark 版本升级检测
+(void)checkSoftResult:(void (^)(UpgradeStatus status,NSString* upgradeUrl,NSString* title,NSString* msg))complete{
    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];  //检测版本升级
    NSString *appCurVersion = [infoDictionary objectForKey:@"CFBundleShortVersionString"];   // 当前应用软件版本
    if ([appCurVersion floatValue] > 1.0) {
        complete(UpgradeStatus_None,@"www",@"需要更新",@"检测到最新版本需要更新，是否需要更新");
    }
}

+(void)openAppStoreWithUrlStr:(NSString*)urlStr{
    NSURL * url = [NSURL URLWithString:urlStr];
    if ([[UIApplication sharedApplication] canOpenURL:url])
    {
        [[UIApplication sharedApplication] openURL:url];
    }
}

@end
