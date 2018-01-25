//
//  IMPackageGlobal.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/2.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface IMPackageGlobal : NSObject

//获取IM发送内容的json
+ (NSString*)getIMJsonStr:(NSString*)content;

//提示铃音
+ (void)ringtone;
//震动提示
+ (void)vibration;

// 根据图片原尺寸进行等比例缩放
+ (CGSize)proportionZoom:(CGSize)srcSize;

@end
