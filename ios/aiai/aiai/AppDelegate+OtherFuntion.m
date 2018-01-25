//
//  AppDelegate+OtherFuntion.m
//  aiai
//
//  Created by zhangyang on 15/7/22.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AppDelegate+OtherFuntion.h"

//系统路径
#define DEFAULT_HOMEPATH [NSHomeDirectory() stringByAppendingPathComponent:@"Library"]
//Documents目录
#define DEFAULT_DOCPATH [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"]

@implementation AppDelegate (OtherFuntion)

-(BOOL)isForeground{
    return [[SaveManager valueWithKey:ISFOREGROUND_KEY] boolValue];
}

-(void)setIsForeground:(BOOL)isForeground{
    [SaveManager saveValue:[NSNumber numberWithBool:isForeground] forKey:ISFOREGROUND_KEY];
}

+ (BOOL)addSkipBackupAttributeToItemAtURL:(NSURL *)URL
{
    assert([[NSFileManager defaultManager] fileExistsAtPath: [URL path]]);
    
    NSError *error = nil;
    BOOL success = [URL setResourceValue: [NSNumber numberWithBool: YES]forKey: NSURLIsExcludedFromBackupKey error: &error];
    
    if(!success){
        NSLog(@"Error excluding %@ from backup %@", [URL lastPathComponent], error);
    }
    
    return success;
}
+(void)addSkipBackupAttribute{
    //设置不云备份属性
    [AppDelegate addSkipBackupAttributeToItemAtURL:[NSURL fileURLWithPath:DEFAULT_DOCPATH]];
    [AppDelegate addSkipBackupAttributeToItemAtURL:[NSURL fileURLWithPath:DEFAULT_HOMEPATH]];
}

+(UINavigationController*)rootViewController{
    AppDelegate* app = (AppDelegate*)[UIApplication sharedApplication].delegate;
    return (UINavigationController*)app.window.rootViewController;
}

+(AppDelegate*)currentApp{
    return (AppDelegate*)[UIApplication sharedApplication].delegate;
}

static BMKMapManager* g_BMKmapManager = nil;//百度地图控制器
+(void)install_BMKMapManager{
    // 要使用百度地图，请先启动BaiduMapManager
    g_BMKmapManager = [[BMKMapManager alloc]init];
    BOOL ret = [g_BMKmapManager start:@"tMtKmVDuArcVWG4AahBpRxjw" generalDelegate:[AppDelegate currentApp]];
    if (!ret) {
        AIAI_LOG(@"manager start failed!");
    }
}

#pragma mark BMKGeneralDelegate
- (void)onGetNetworkState:(int)iError
{
    if (0 == iError) {
        AIAI_LOG(@"联网成功");
    }
    else{
        AIAI_LOG(@"onGetNetworkState %d",iError);
    }
    
}

- (void)onGetPermissionState:(int)iError
{
    if (0 == iError) {
        AIAI_LOG(@"授权成功");
    }
    else {
        AIAI_LOG(@"onGetPermissionState %d",iError);
    }
}

@end
