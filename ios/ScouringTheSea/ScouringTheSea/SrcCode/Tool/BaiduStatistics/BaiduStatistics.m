//
//  BaiduStatistics.m
//  F2CPlatform
//
//  Created by Circle Liu on 14-12-3.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "BaiduStatistics.h"
#import "BaiduMobStat.h"

@implementation BaiduStatistics

+ (void)start {
    BaiduMobStat* statTracker = [BaiduMobStat defaultStat];
    statTracker.enableExceptionLog = YES; // 是否允许截获并发送崩溃信息，请设置YES或者NO
    statTracker.logStrategy = BaiduMobStatLogStrategyAppLaunch;//根据开发者设定的时间间隔接口发送 也可以使用启动时发送策略
    statTracker.logSendWifiOnly = NO; //是否仅在WIfi情况下发送日志数据
    statTracker.enableDebugOn = NO; //打开调试模式，发布时请去除此行代码或者设置为False即可。
    //    statTracker.channelId = @"Cydia";//设置您的app的发布渠道
    //    statTracker.logSendInterval = 1; //为1时表示发送日志的时间间隔为1小时,只有 statTracker.logStrategy = BaiduMobStatLogStrategyAppLaunch这时才生效。
    //    statTracker.sessionResumeInterval = 1;//设置应用进入后台再回到前台为同一次session的间隔时间[0~600s],超过600s则设为600s，默认为30s
    [statTracker startWithAppId:@"00df352b0d"];//设置您在mtj网站上添加的app的appkey
}

+(void)baidu_pageviewStartWithName:(NSString*)name{
    
    [[BaiduMobStat defaultStat] pageviewStartWithName:name];
}

+(void)baidu_pageviewEndWithName:(NSString*)name{
    
    [[BaiduMobStat defaultStat] pageviewEndWithName:name];
}
@end
