//
//  IMPackageGlobal.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/2.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "IMPackageGlobal.h"
#import <AudioToolbox/AudioToolbox.h>

@implementation IMPackageGlobal

//获取IM发送内容的json
+ (NSString*)getIMJsonStr:(NSString*)content{
    NSDictionary *dic = [NSDictionary dictionaryWithObject:content forKey:@"contentText"];
    NSString *str = [[NSString alloc]initWithData:[dic myJSONData] encoding:NSUTF8StringEncoding];
    NSRange range = NSMakeRange(21, [str length]-21-3);
    NSString *subStr = [str substringWithRange:range];
    return subStr;
}

+ (void)ringtone {
    static SystemSoundID okk = 0;
    NSString *pathe=[[NSBundle mainBundle] pathForResource:@"in" ofType:@"caf"];
    CFURLRef url =(__bridge CFURLRef)[NSURL fileURLWithPath:pathe];
    AudioServicesCreateSystemSoundID(url,&okk);
    AudioServicesPlaySystemSound(okk);
}

+ (void)vibration {
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}


+ (CGSize)proportionZoom:(CGSize)srcSize {
    CGSize resultSize = srcSize;
    
    if (srcSize.height > 150) {
        CGFloat temp = 150 / srcSize.height;
        
        resultSize.width *= temp;
        resultSize.height *= temp;
    }
    
    if (resultSize.width > 100) {
        CGFloat temp = 100 / resultSize.height;
        
        resultSize.width *= temp;
        resultSize.height *= temp;
    }
    
    return resultSize;
}

@end
