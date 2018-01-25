//
//  RequestServers.h
//  F2CPlatform
//
//  Created by Circle Liu on 14-4-9.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <ASIHTTPRequest/ASIHTTPRequest.h>
#import <ASIHTTPRequest/ASIFormDataRequest.h>

@interface RequestServers : NSObject

+ (RequestServers*)share;

//只支持http基本返回值--GET
- (void)requestWithURL:(NSString*)url objclass:(Class)classname prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed;

//只支持http基本返回值--POST
- (void)requestWithURL:(NSString*)url objclass:(Class)classname params:(NSDictionary*)params prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed;

//上传资源的请求
- (void)uploadSrouceDataRequestWithURL:(NSString*)url objclass:(Class)classname data:(NSData*)srouceData prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed;

//Pb的post请求
- (void)requestProtobufWithURL:(NSString*)url objclass:(Class)classname data:(NSData*)srouceData prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed;

//根据类名释放所有http,一般在离开界面时使用
- (void)cancelRequestWithClassName:(Class)classname;

//释放所有http,一般在离开界面时使用
- (void)cancelAllRequest;


//添加http的header
- (void)addSessionHeader:(NSDictionary*)headerDic;

#pragma mrak Download
-(void)requestDownLoadWithURL:(NSString*)url savePath:(NSString*)path prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed;
-(void)cancelDownloadRequest;

@end