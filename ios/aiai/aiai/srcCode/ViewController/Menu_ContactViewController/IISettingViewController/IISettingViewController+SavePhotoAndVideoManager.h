//
//  IISettingViewController+SavePhotoAndVideoManager.h
//  aiai
//
//  Created by zhangyang on 15/5/4.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "IISettingViewController.h"

@interface IISettingViewController (SavePhotoAndVideoManager)

//根据配置文件进行保存
+(void)saveImageWithSetting:(UIImage*)saveImage;
+(void)saveVideoWithSetting:(NSURL*)videoUrl;

//保存到相册
+(void)saveImageToAlbum:(UIImage *)saveImage;
+(void)saveVedioToAlbum:(NSURL*)videoUrl;

@end
