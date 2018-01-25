//
//  BaseHandler.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/7.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseHandler.h"
#import "SkinModel.h"

@implementation BaseHandler

+(void)requestJsonDataWithGETUrl:(NSString*)urlString ready:(GetReadyBlock)readyBlock success:(SuccessBlock)successBlock failed:(FailedBlock)failedBlock{
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    NSString * encodingString = [urlString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    [manager GET:encodingString parameters:nil success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        SkinModel* skinModel = [SkinModel objectWithKeyValues:responseObject];
        StatusModel* statusModel = [StatusModel objectWithKeyValues:skinModel.status];
        if ([statusModel.code integerValue] == 1 && ![skinModel.returndata isEqual:[NSNull null]]) {
            successBlock(skinModel.returndata);
        }else{
            failedBlock([NSError errorWithDomain:statusModel.desc code:[statusModel.code integerValue] userInfo:nil]);
        }
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        failedBlock(error);
    }];
}

+(void)requestJsonDataWithPostUrl:(NSString *)urlString parame:(id)parameters ready:(GetReadyBlock)readyBlock success:(SuccessBlock)successBlock failed:(FailedBlock)failedBlock{
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    [manager POST:urlString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        SkinModel* skinModel = [SkinModel objectWithKeyValues:responseObject];
        successBlock(skinModel.returndata);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        failedBlock(error);
    }];
}

+(void)uploadDataWithUrl:(NSString *)urlString parame:(id)parameters filePath:(NSURL*)filePath ready:(GetReadyBlock)readyBlock success:(SuccessBlock)successBlock failed:(FailedBlock)failedBlock{
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];

    [manager POST:urlString parameters:parameters constructingBodyWithBlock:^(id<AFMultipartFormData> formData) {
        [formData appendPartWithFileURL:filePath name:@"image" error:nil];
    } success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        NSLog(@"Success: %@", responseObject);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        failedBlock(error);
    }];
}

@end
