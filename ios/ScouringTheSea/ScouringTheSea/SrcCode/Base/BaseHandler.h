//
//  BaseHandler.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/7.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BaseHandler : NSObject

/**
 *  Handler准备开始处理Block
 */
typedef void (^GetReadyBlock)();

/**
 *  Handler处理成功时调用的Block
 */
typedef void (^SuccessBlock)(id responseObject);

/**
 *  Handler处理失败时调用的Block
 */
typedef void (^FailedBlock)(NSError* error);


+(void)requestJsonDataWithGETUrl:(NSString*)urlString ready:(GetReadyBlock)readyBlock success:(SuccessBlock)successBlock failed:(FailedBlock)failedBlock;

+(void)requestJsonDataWithPostUrl:(NSString *)urlString parame:(id)parameters ready:(GetReadyBlock)readyBlock success:(SuccessBlock)successBlock failed:(FailedBlock)failedBlock;

@end
