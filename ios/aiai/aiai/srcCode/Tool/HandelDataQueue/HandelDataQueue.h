//
//  HandelDataQueue.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/1.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HandelDataQueue : NSObject
{
    dispatch_queue_t    _queue;
}

+ (HandelDataQueue*)share;

-(void)exe:(void(^)())block;

@end
