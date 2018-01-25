//
//  NSString+isEmpty.h
//  F2CPlatform
//
//  Created by zhangyang on 14-6-13.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (isEmpty)

-(BOOL)isEmpty;

//判断字符串是否超出上下限
-(BOOL)offNormalUpper:(NSInteger)upper AndLower:(NSInteger)lower;

//判断字符串是否包含特殊字符，是否为纯数字,是否超出上下限,
-(BOOL)checkFormatWithUpper:(NSInteger)upper AndLower:(NSInteger)lower;

//判断是否正在语音识别
-(BOOL)voiceRecognitioning;

//判断字符串结尾处是否为emoji
-(int)isEmoji;

+ (NSInteger)calcStringLength:(NSString*)srcStr;

@end
