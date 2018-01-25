//
//  HandelDataQueue.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/1.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "HandelDataQueue.h"

@implementation HandelDataQueue

+ (HandelDataQueue*)share {
    static HandelDataQueue *_instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [[HandelDataQueue alloc] init];
    });
    return _instance;
}

-(id)init
{
    self = [super init];
    if(self){
        _queue = dispatch_queue_create([@"HandelDataQueue" UTF8String], DISPATCH_QUEUE_SERIAL);
    }
    return self;
}

-(void)exe:(void(^)())block
{
    dispatch_async(_queue, block);
}

@end
