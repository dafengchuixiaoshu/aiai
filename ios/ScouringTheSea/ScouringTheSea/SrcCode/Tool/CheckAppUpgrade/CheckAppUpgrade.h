//
//  CheckSoft.h
//  F2CPlatform
//
//  Created by zhangyang on 13-11-28.
//  Copyright (c) 2013年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>

//检测功能类，需要初始化的功能都放到这里,如检测升级，插播广告等

typedef NS_ENUM(NSInteger, UpgradeStatus) {
    UpgradeStatus_None,//不需要升级
    UpgradeStatus_Suggestions,//建议升级
    UpgradeStatus_Compulsory//强制升级
};

@interface CheckAppUpgrade : NSObject

//检测升级
+(void)checkSoftResult:(void (^)(UpgradeStatus status,NSString* upgradeUrl,NSString* title,NSString* msg))complete;

+(void)openAppStoreWithUrlStr:(NSString*)urlStr;
@end
