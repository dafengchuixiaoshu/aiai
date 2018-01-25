//
//  IISettingViewController+SavePhotoAndVideoManager.m
//  aiai
//
//  Created by zhangyang on 15/5/4.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "IISettingViewController+SavePhotoAndVideoManager.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import "ALAssetsLibrary+CustomPhotoAlbum.h"

@implementation IISettingViewController (SavePhotoAndVideoManager)

static ALAssetsLibrary* assetLibrary = nil;

+(void)saveImageWithSetting:(UIImage*)saveImage{
    
    if ([IISettingViewController isSaveImg]) {
        [IISettingViewController saveImageToAlbum:saveImage];
    }
}

+(void)saveVideoWithSetting:(NSURL*)videoUrl{
    
    if ([IISettingViewController isSaveImg]) {
        [IISettingViewController saveVedioToAlbum:videoUrl];
    }
}

+(void)saveImageToAlbum:(UIImage *)saveImage{
    
    if (assetLibrary == nil) {
        assetLibrary = [[ALAssetsLibrary alloc] init];
    }
    //根据设置保存图片
    NSDictionary* infoDict =[[NSBundle mainBundle] infoDictionary];
    NSString*appName = [infoDict objectForKey:@"CFBundleDisplayName"];
    appName = [NSString stringWithFormat:@"%@ %@",appName,@"photo"];
    [assetLibrary saveImage:saveImage toAlbum:appName withCompletionBlock:^(NSError *error) {
        if (error) {
            AIAI_LOG(@"照片存储失败,请检查磁盘空间");
        }else{
            AIAI_LOG(@"照片存储成功");
        }
    }];
}

+(void)saveVedioToAlbum:(NSURL*)videoUrl{
    
    if (assetLibrary == nil) {
        assetLibrary = [[ALAssetsLibrary alloc] init];
    }
    //根据设置保存视频
    NSDictionary* infoDict =[[NSBundle mainBundle] infoDictionary];
    NSString*appName =[infoDict objectForKey:@"CFBundleDisplayName"];
    appName = [NSString stringWithFormat:@"%@ %@",appName,@"photo"];
    [assetLibrary saveVideo:videoUrl toAlbum:appName withCompletionBlock:^(NSError *error) {
        if (error) {
            AIAI_LOG(@"视频存储失败,请检查磁盘空间");
        }else{
            AIAI_LOG(@"视频存储成功");
        }
    }];
}

@end
