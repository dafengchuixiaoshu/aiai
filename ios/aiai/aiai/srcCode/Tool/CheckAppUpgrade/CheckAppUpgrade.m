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
//    NSString *appCurName = [infoDictionary objectForKey:@"CFBundleDisplayName"];    //    当前应用名称
    NSString* urlString = [NSString stringWithFormat:@"%@channelId=C010000&os=2&version=%@",CHECKSOFT_URL,appCurVersion];
    
    [[RequestServers share] requestWithURL:urlString objclass:[self class] prepare:^{
        
    }successed:^(NSDictionary *dic) {
        if (dic == nil || [dic isEqual:[NSNull null]]) {
            return;
        }
        NSString* status = [NSString stringWithFormat:@"%@",[dic objectForKey:@"status"]];
        if ([status isEqualToString:@"1"]) {
            NSDictionary* softDic = [dic objectForKey:@"software"];
            //获取需要外传的数据，包括title，msg，下载新版本的url
            NSString* message = [NSString stringWithFormat:@"%@",[softDic objectForKey:@"info"]];
            NSString* title = [NSString stringWithFormat:@"%@%@",@"检测到最新版本:",[softDic objectForKey:@"version"]];
            NSString* urlString = [NSString stringWithFormat:@"%@",[softDic objectForKey:@"url"]];
   
            //判断是强制升级还是建议升级
            NSString* flage = [NSString stringWithFormat:@"%@",[softDic objectForKey:@"flag"]];   //--1表示 建议升级2 表示 强制升级
            if ([flage isEqualToString:@"1"]) {
                complete(UpgradeStatus_Suggestions,urlString,title,message);
            }else if([flage isEqualToString:@"2"]){
                complete(UpgradeStatus_Compulsory,urlString,title,message);
            }
        }else if ([status isEqualToString:@"4"]){
            NSString* msg = [NSString stringWithFormat:@"%@",[dic objectForKey:@"msg"]];
            complete(UpgradeStatus_None,nil,nil,msg);
        }else if ([status isEqualToString:@"-1"]){
            complete(UpgradeStatus_None,nil,nil,@"当前已是最高版本");
        }
    } failed:^(NSError *error) {
        
    }];
}

+(void)openAppStoreWithUrlStr:(NSString*)urlStr{
    NSURL * url = [NSURL URLWithString:urlStr];
    if ([[UIApplication sharedApplication] canOpenURL:url])
    {
        [[UIApplication sharedApplication] openURL:url];
    }
}

@end
