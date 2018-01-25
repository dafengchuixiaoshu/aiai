//
//  UserInfo.m
//  F2CPlatform
//
//  Created by Circle Liu on 14-12-4.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "GlobalUserInfo.h"

static GlobalUserInfo* g_myself = nil;

@implementation GlobalUserInfo

+(GlobalUserInfo*)myselfInfo{
    if (g_myself == nil) {
        g_myself = [[GlobalUserInfo alloc] init];
        
        g_myself.userName = [SaveManager valueWithKey:USERNAME_KEY];
        g_myself.userPassWord = [SaveManager valueWithKey:USERPASSWORD_KEY];
        g_myself.userTelphone = [SaveManager valueWithKey:USERTELPHONE_KEY];
        g_myself.userLatitude = [[SaveManager valueWithKey:USER_LATITUDE] doubleValue];
        g_myself.userLongitude = [[SaveManager valueWithKey:USER_LONGITUDE] doubleValue];
        
        g_myself.userID = 0;
        g_myself.userName = @"";
        g_myself.userPassWord = @"";
        g_myself.userTelphone = @"";
        g_myself.userFaceUrl = @"";
        g_myself.userNickName = @"";
        g_myself.userGender = 0;
        
        g_myself.openPlatformId = nil;
        g_myself.openPlatformcode = nil;
    }
    
    return g_myself;
}

+(void)setMyselfInfo:(GlobalUserInfo*)userinfo{
    
    if (userinfo) {
        //先存储数据
        [SaveManager saveValue:userinfo.userName forKey:USERNAME_KEY];
        [SaveManager saveValue:userinfo.userTelphone forKey:USERTELPHONE_KEY];
        [SaveManager saveValue:[NSNumber numberWithInteger:userinfo.userID] forKey:USER_ID_KEY];
        [SaveManager saveValue:userinfo.userNickName forKey:USER_NICKNAME_KEY];
        [SaveManager saveValue:userinfo.userFaceUrl forKey:USER_FACEURL_KEY];
        [SaveManager saveValue:userinfo.userFaceVedio forKey:USER_FACEVIDEO_KEY];
        [SaveManager saveValue:[NSNumber numberWithInteger:userinfo.userGender] forKey:USER_GENDER_KEY];
        if (userinfo.userPassWord) {
            [SaveManager saveValue:userinfo.userPassWord forKey:USERPASSWORD_KEY];
        }
        if (userinfo.openPlatformcode) {
            [SaveManager saveValue:userinfo.openPlatformcode forKey:USEROPENPLATFORMCODE];
        }
        if (userinfo.openPlatformId) {
            [SaveManager saveValue:userinfo.openPlatformId forKey:USEROPENPLATFORMID];
        }
        
        if (g_myself == nil) {
            g_myself = [[GlobalUserInfo alloc] init];
        }
        g_myself.userID = userinfo.userID;
        g_myself.userName = userinfo.userName;
        g_myself.userPassWord = userinfo.userPassWord;
        g_myself.userTelphone = userinfo.userTelphone;
        g_myself.userFaceUrl = userinfo.userFaceUrl;
        g_myself.userNickName = userinfo.userNickName;
        g_myself.userGender = userinfo.userGender;
        g_myself.userFaceVedio = userinfo.userFaceVedio;
        //定位只在地图上赋值
//        g_myself.userLongitude = userinfo.userLongitude;
//        g_myself.userLatitude = userinfo.userLatitude;
    }
    else{
        
        [SaveManager saveValue:@"" forKey:USERNAME_KEY];
        [SaveManager saveValue:@"" forKey:USERPASSWORD_KEY];
        [SaveManager saveValue:@"" forKey:USERTELPHONE_KEY];
        [SaveManager saveValue:@"" forKey:USEROPENPLATFORMCODE];
        [SaveManager saveValue:@"" forKey:USEROPENPLATFORMID];
        
        g_myself.userID = 0;
        g_myself.userName = @"";
        g_myself.userPassWord = @"";
        g_myself.userTelphone = @"";
        g_myself.userFaceUrl = @"";
        g_myself.userNickName = @"";
        g_myself.userGender = 0;
    }
}


@end