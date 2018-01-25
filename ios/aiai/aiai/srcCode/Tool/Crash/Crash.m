//
//  Crash.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/1.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "Crash.h"
#include "sys/utsname.h"

#import <ASIHTTPRequest/ASIFormDataRequest.h>

#define CrashFileName @"crash.log"

@implementation Crash

#pragma mark 崩溃日志
void uncaughtExceptionHandler(NSException *exception)
{
    // 异常的堆栈信息
    NSArray *stackArray = [exception callStackSymbols];
    // 出现异常的原因
    NSString *reason = [exception reason];
    // 异常名称
    NSString *name = [exception name];
    NSString *exceptionInfo = [NSString stringWithFormat:@"Exception reason：%@\nException name：%@\nException stack：%@",name, reason, stackArray];
    NSLog(@"%@", exceptionInfo);
    
    NSMutableArray *tmpArr = [NSMutableArray arrayWithArray:stackArray];
    [tmpArr insertObject:reason atIndex:0];
    
    //保存到本地  --  下次启动的时候，上传这个log
    [SaveManager saveToFileWithString:exceptionInfo filePath:[SaveManager libraryPath:CrashFileName]];
}

+ (void)startCrashExceptionCatch
{
    //设置异常Log信息的处理
    NSSetUncaughtExceptionHandler(&uncaughtExceptionHandler);
    
    //下次启动上传log文件，上传成功后删除
    NSString* crashString = [SaveManager readToStringWithFilePath:[SaveManager libraryPath:CrashFileName]];
    if (crashString) {
        NSTimeInterval now = [[NSDate date] timeIntervalSince1970]*1000;
        NSString* nowString = [NSString stringWithFormat:@"%lld",(uint64_t)now];
        
        struct utsname systemInfo;
        uname(&systemInfo);
        NSString *phoneModel = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
        NSString *name = [[UIDevice currentDevice] name];
        NSString *vertion = [[UIDevice currentDevice] systemVersion];
        phoneModel = [NSString stringWithFormat:@"%@_%@-V%@",name,phoneModel,vertion];
        NSString *uuid = [AppUtils uuid];
        
        
        NSURL* requestUrl = [NSURL URLWithString:UPLOAD_CRASHLOG_URL];
        ASIFormDataRequest* postingRequest = [ASIFormDataRequest requestWithURL:requestUrl];
        [postingRequest setCompletionBlock:^{
            NSString* path = [SaveManager libraryPath:CrashFileName];
            [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
        }];
        [postingRequest setDelegate:self];
        [postingRequest setRequestMethod:@"POST"];
        [postingRequest setPostValue:crashString forKey:@"crashfile"];
        [postingRequest setPostValue:@"2" forKey:@"os"];
        [postingRequest setPostValue:nowString forKey:@"time"];
        [postingRequest setPostValue:phoneModel forKey:@"phoneBrand"];
        [postingRequest setPostValue:uuid forKey:@"uid"];
        [postingRequest startSynchronous];
    }
}

@end
