//
//  Crash.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/1.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "Crash.h"
#include "sys/utsname.h"

#define CrashFileName @"crash.log"
#define UPLOAD_CRASHLOG_URL @"www.baidiu.com"

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

        //开始上传
        AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
        manager.requestSerializer = [AFJSONRequestSerializer serializer];
        //这个决定了下面responseObject返回的类型
        //    manager.responseSerializer = [AFHTTPResponseSerializer serializer];
        manager.responseSerializer = [AFJSONResponseSerializer serializer];
        manager.responseSerializer.acceptableContentTypes = [NSSet setWithObject:@"text/plain"];

        NSDictionary *dict = @{ @"crashfile":crashString, @"os":@"2",@"time":nowString, @"phoneBrand":phoneModel,@"uid":uuid};
        [manager POST:UPLOAD_CRASHLOG_URL parameters:dict success: ^(AFHTTPRequestOperation *operation, id responseObject) {
            //	    NSLog(@"postjson--> %@", responseObject);  //这样显示JSON的话需要设置text/plain
            NSString *result = [[NSString alloc] initWithData:responseObject encoding:NSUTF8StringEncoding];
            NSLog(@"%@",result);
        } failure: ^(AFHTTPRequestOperation *operation, NSError *error) {
            NSLog(@"%@", error);
        }];
    }
}

#pragma mark - get/post登录
- (void)getLogin {
    //1.管理器
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    
    //2.设置登录参数
    NSDictionary *dict = @{ @"username":@"xn", @"password":@"123" };
    
    //3.请求
    [manager GET:@"http://localhost/login.php" parameters:dict success: ^(AFHTTPRequestOperation *operation, id responseObject) {
        NSLog(@"GET --> %@, %@", responseObject, [NSThread currentThread]); //自动返回主线程
    } failure: ^(AFHTTPRequestOperation *operation, NSError *error) {
        NSLog(@"%@", error);
    }];
}

/**
 *  和上面的GET用法完全一样, 只有一个POST参数不一样
 */
- (void)postLogin {
    //1.管理器
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    
    //2.设置登录参数
    NSDictionary *dict = @{ @"username":@"xn", @"password":@"123" };
    
    //3.请求
    [manager POST:@"http://localhost/login.php" parameters:dict success: ^(AFHTTPRequestOperation *operation, id responseObject) {
        NSLog(@"POST --> %@, %@", responseObject, [NSThread currentThread]); //自动返回主线程
    } failure: ^(AFHTTPRequestOperation *operation, NSError *error) {
        NSLog(@"%@", error);
    }];
}

#pragma mark - post json数据与上传文件等
- (void)postJSON {
    //1.管理器
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    
    //2.设定类型. (这里要设置request-response的类型)
    manager.requestSerializer = [AFJSONRequestSerializer serializer];
    //这个决定了下面responseObject返回的类型
    //    manager.responseSerializer = [AFHTTPResponseSerializer serializer];
    manager.responseSerializer = [AFJSONResponseSerializer serializer];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObject:@"text/plain"];
    
    //2.设置登录参数
    NSDictionary *dict = @{ @"username":@"xn", @"password":@"123" };
    
    //3.发送请求
    [manager POST:@"http://localhost/postjson.php" parameters:dict success: ^(AFHTTPRequestOperation *operation, id responseObject) {
        //	    NSLog(@"postjson--> %@", responseObject);  //这样显示JSON的话需要设置text/plain
        NSString *result = [[NSString alloc] initWithData:responseObject encoding:NSUTF8StringEncoding];
        NSLog(@"%@",result);
    } failure: ^(AFHTTPRequestOperation *operation, NSError *error) {
        NSLog(@"%@", error);
    }];
}

@end
