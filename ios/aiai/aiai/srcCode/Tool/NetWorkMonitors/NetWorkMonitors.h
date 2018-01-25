//
//  NetWorkMonitors.h
//  F2CPlatform
//
//  Created by zhangyang on 14-11-27.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NetWorkMonitors : NSObject

+ (NetWorkMonitors*)share;

// 开启网络监测服务
- (void)start;

@end
