//
//  AppDelegate+OtherFuntion.h
//  aiai
//
//  Created by zhangyang on 15/7/22.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate (OtherFuntion)

@property(nonatomic,assign) BOOL isForeground;//是否自后台切入应用

//获取根视图控制器
+(UINavigationController*)rootViewController;
//获取当前app对象
+(AppDelegate*)currentApp;

//添加云备份不上传属性
+(void)addSkipBackupAttribute;
//启动百度地图控制器
+(void)install_BMKMapManager;

@end
