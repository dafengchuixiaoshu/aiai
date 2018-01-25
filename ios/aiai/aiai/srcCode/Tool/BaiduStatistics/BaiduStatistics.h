//
//  BaiduStatistics.h
//  F2CPlatform
//
//  Created by Circle Liu on 14-12-3.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BaiduStatistics : NSObject

/**
 *  设置应用的appkey (在[百度移动统计](http://mtj.baidu.com)获取)，在其他api调用以前必须先调用该api.
 *  此处AppId即为应用的appKey
 */
+ (void)start;

/**
 *  标识某个页面访问的开始，请参见Example程序，在合适的位置调用。
 */
+(void)baidu_pageviewStartWithName:(NSString*)name;

/**
 *  标识某个页面访问的结束，与pageviewStartWithName配对使用，请参见Example程序，在合适的位置调用。
 */
+(void)baidu_pageviewEndWithName:(NSString*)name;
@end
