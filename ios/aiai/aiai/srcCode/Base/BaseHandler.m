//
//  BaseHandler.m
//  zlydoc-iphone
//
//  Created by Ryan on 14-6-25.
//  Copyright (c) 2014年 zlycare. All rights reserved.
//

#import "BaseHandler.h"

@implementation BaseHandler

+ (NSString *)requestUrlWithServerHost:(NSString*)serverHost ServerQueueName:(NSString*)queueName paramerString:(NSString *)paramer
{
    NSString* baseInfoString = [AppUtils base64StringEncode:paramer];
    return [[serverHost stringByAppendingString:queueName] stringByAppendingString:baseInfoString];
}

+ (NSString *)requestUrlWithCompletedHead:(NSString*)headString paramerString:(NSString *)paramer
{
    NSString* baseInfoString = [AppUtils base64StringEncode:paramer];
    return [headString stringByAppendingString:baseInfoString];
}

@end
