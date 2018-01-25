//
//  IISettingViewController.h
//  aiai
//
//  Created by wangnan on 15/4/19.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"

@interface IISettingViewController : BaseViewController

//是否保存照片
+(BOOL)isSaveImg;

//是否在wifi下自动下载
+ (BOOL)isAutoLoadWithWIFI;

//下载后是否自动播放
+ (BOOL)isAutoPlayAfterLoad;

@property(nonatomic, strong)dispatch_block_t setting_changeFaceImgBlock;
@end
