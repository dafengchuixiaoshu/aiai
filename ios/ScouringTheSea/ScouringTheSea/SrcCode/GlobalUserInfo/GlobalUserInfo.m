//
//  UserInfo.m
//  F2CPlatform
//
//  Created by zhangyang on 14-12-4.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "GlobalUserInfo.h"

@implementation GlobalUserInfo

GlobalUserInfo* g_myself = nil;

+(GlobalUserInfo*)myselfInfo{
    if (g_myself == nil) {
        g_myself = [SaveManager readDataUnArchiverWithFilePath:[SaveManager documentsPath:@"userinfo"]];
    }
    return g_myself;
}

+(void)setMyselfInfo:(GlobalUserInfo*)userinfo{
    if (userinfo) {
        g_myself = userinfo;
        [SaveManager saveDataArchiverWithModel:userinfo filePath:[SaveManager documentsPath:@"userinfo"]];
    }
}

@end