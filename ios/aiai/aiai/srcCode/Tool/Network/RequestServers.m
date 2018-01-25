//
//  RequestServers.m
//  F2CPlatform
//
//  Created by Circle Liu on 14-4-9.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "RequestServers.h"
#import "JSONFormat.h"
#import <ASIHTTPRequest/ASINetworkQueue.h>

#define TimeOut_Front           30
#define TimeOut_Background      60
#define Protobuf_Tag            100

typedef void (^NewRequestSuccessed)(NSDictionary *dic);
typedef void (^NewRequestFailed)(NSError *error);

@interface RequestServers ()
@property (nonatomic,strong)NSMutableDictionary* headerDic;
@property (nonatomic,strong)NSMutableDictionary* requestDic;

//@property(nonatomic,strong)ASINetworkQueue* downloadQueue;
@property(nonatomic,strong)NSMutableArray* downloadRequestArray;
@end

@implementation RequestServers
@synthesize headerDic;
@synthesize requestDic;

+ (RequestServers*)share {
    static RequestServers *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (id)init {
    if (self = [super init]) {
        requestDic = [[NSMutableDictionary alloc] init];
        headerDic = [[NSMutableDictionary alloc] init];
    }
    return self;
}

#pragma mark Public Method
- (void)addSessionHeader:(NSDictionary*)headerDictionary{
    [headerDic setValuesForKeysWithDictionary:headerDictionary];
}

- (void)requestWithURL:(NSString*)url objclass:(Class)classname prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed {
    
    //根据类名生成该类专有的http字典，因为每个请求的url都不同，所以需要字典
    NSMutableDictionary* oneClassRequestDic = [requestDic objectForKey:NSStringFromClass(classname)];
    if (oneClassRequestDic == nil) {
        NSMutableDictionary* tempDic = [[NSMutableDictionary alloc] init];
        
        //请求之前预加载
        if(prepare)
            prepare();
        
        //进行对http的设置
        ASIHTTPRequest *request = [[ASIHTTPRequest alloc] initWithURL:[NSURL URLWithString:url]];
        [request setShouldAttemptPersistentConnection:NO];
        [request setTimeOutSeconds:TimeOut_Front];
        [request setDelegate:self];
        
        //设置header
        NSArray *headerAllkeys = [headerDic allKeys];
        for (NSString *key in headerAllkeys) {
            [request addRequestHeader:key value:[headerDic objectForKey:key]];
        }//设置完成之后需要移除所有header中的数据
        [headerDic removeAllObjects];

        //设置block
        NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
        [dic setValue:successed forKey:@"successed"];
        [dic setValue:failed forKey:@"failed"];
        [request setUserInfo:dic];
        
        //进行请求
        [request startAsynchronous];
        
        [tempDic setObject:request forKey:url];
        [requestDic setObject:tempDic forKey:NSStringFromClass(classname)];
    }else{
        ASIHTTPRequest *request = [oneClassRequestDic objectForKey:url];
        if (request == nil) {
            
            //请求之前预加载
            if(prepare)
                prepare();
            
            //进行对http的设置
            ASIHTTPRequest *request = [[ASIHTTPRequest alloc] initWithURL:[NSURL URLWithString:url]];
            [request setShouldAttemptPersistentConnection:NO];
            [request setTimeOutSeconds:TimeOut_Front];
            [request setDelegate:self];
            
            //设置header
            NSArray *headerAllkeys = [headerDic allKeys];
            for (NSString *key in headerAllkeys) {
                [request addRequestHeader:key value:[headerDic objectForKey:key]];
            }//设置完成之后需要移除所有header中的数据
            [headerDic removeAllObjects];
            
            //设置block
            NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
            [dic setValue:successed forKey:@"successed"];
            [dic setValue:failed forKey:@"failed"];
            [request setUserInfo:dic];
            
            //进行请求
            [request startAsynchronous];
            
            [oneClassRequestDic setObject:request forKey:url];
        }else{
            [request clearDelegatesAndCancel];
            [oneClassRequestDic removeObjectForKey:url];
            [self requestWithURL:url objclass:classname prepare:prepare successed:successed failed:failed];
        }
    }
}

- (void)requestWithURL:(NSString*)url objclass:(Class)classname params:(NSDictionary*)params prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed{
    
    //再进入正常逻辑
    if (nil == params) {
        params = [[NSMutableDictionary alloc] init];
    }
    
    NSMutableDictionary* tempDic = [[NSMutableDictionary alloc] init];
    
    //请求之前预加载
    prepare();
    
    //对http进行设置
    ASIFormDataRequest *request = [[ASIFormDataRequest alloc] initWithURL:[NSURL URLWithString:url]];
    [request setShouldAttemptPersistentConnection:NO];
    [request setRequestMethod:@"POST"];
    [request setTimeOutSeconds:TimeOut_Front];
    [request setDelegate:self];
    
    //设置header
    NSArray *headerAllkeys = [headerDic allKeys];
    for (NSString *key in headerAllkeys) {
        [request addRequestHeader:key value:[headerDic objectForKey:key]];
    }//设置完成之后需要移除所有header中的数据
    [headerDic removeAllObjects];
    
    //设置需要发送的值
    NSArray *allkeys = [params allKeys];
    for (NSString *key in allkeys) {
        [request setPostValue:[params objectForKey:key] forKey:key];
    }
    
    //设置block
    NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
    [dic setValue:successed forKey:@"successed"];
    [dic setValue:failed forKey:@"failed"];
    [request setUserInfo:dic];
    
    //进行请求
    [request startAsynchronous];
    
    [tempDic setObject:request forKey:url];
    [requestDic setObject:tempDic forKey:NSStringFromClass(classname)];
}

#pragma mark PB协议的post的请求
- (void)requestProtobufWithURL:(NSString*)url objclass:(Class)classname data:(NSData*)srouceData prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed{
    
    
    //由于post请求的url头 是一样的 所以不能判断url(如果判断会直接cancel上一个请求) 直接请求即可
//    //再进入正常逻辑
//    NSMutableDictionary* oneClassRequestDic = [requestDic objectForKey:NSStringFromClass(classname)];
//    if (oneClassRequestDic == nil) {
        NSMutableDictionary* tempDic = [[NSMutableDictionary alloc] init];
        
        //请求之前预加载
        prepare();
        
        //设置http
        ASIFormDataRequest *request = [[ASIFormDataRequest alloc] initWithURL:[NSURL URLWithString:url]];
        [request setShouldAttemptPersistentConnection:NO];
        [request setDelegate:self];
        [request setTag:Protobuf_Tag];
        [request setRequestMethod:@"POST"];
        [request setTimeOutSeconds:TimeOut_Front];
        
        //设置外部header
        NSArray *headerAllkeys = [headerDic allKeys];
        for (NSString *key in headerAllkeys) {
            [request addRequestHeader:key value:[headerDic objectForKey:key]];
        }//设置完成之后需要移除所有header中的数据
        [headerDic removeAllObjects];
        
        //设置HTTPHeader中Content-Type的值
        [request addRequestHeader:@"Content-Type" value:@"application/x-protobuf"];
        //设置HTTPHeader中Content-Length的值
        [request addRequestHeader:@"Content-Length" value:[NSString stringWithFormat:@"%d",[srouceData length]]];
        //设置body
        NSMutableData* sendData = [[NSMutableData alloc] initWithData:srouceData];
        [request setPostBody:sendData];
        
        //设置block
        NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
        [dic setValue:successed forKey:@"successed"];
        [dic setValue:failed forKey:@"failed"];
        [request setUserInfo:dic];
        
        //进行请求
        [request startAsynchronous];
        
        [tempDic setObject:request forKey:url];
        [requestDic setObject:tempDic forKey:NSStringFromClass(classname)];
//    }else{
//        ASIFormDataRequest *request = [oneClassRequestDic objectForKey:url];
//        if (request == nil) {
//            
//            //请求之前预加载
//            prepare();
//            
//            //设置http
//            ASIFormDataRequest *request = [[ASIFormDataRequest alloc] initWithURL:[NSURL URLWithString:url]];
//            [request setDelegate:self];
//            [request setTag:Protobuf_Tag];
//            [request setRequestMethod:@"POST"];
//            [request setTimeOutSeconds:TimeOut_Front];
//            
//            //设置外部header
//            NSArray *headerAllkeys = [headerDic allKeys];
//            for (NSString *key in headerAllkeys) {
//                [request addRequestHeader:key value:[headerDic objectForKey:key]];
//            }//设置完成之后需要移除所有header中的数据
//            [headerDic removeAllObjects];
//            
//            //设置HTTPHeader中Content-Type的值
//            [request addRequestHeader:@"Content-Type" value:@"application/x-protobuf"];
//            //设置HTTPHeader中Content-Length的值
//            [request addRequestHeader:@"Content-Length" value:[NSString stringWithFormat:@"%d",[srouceData length]]];
//            //设置body
//            NSMutableData* sendData = [[NSMutableData alloc] initWithData:srouceData];
//            [request setPostBody:sendData];
//            
//            //设置block
//            NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
//            [dic setValue:successed forKey:@"successed"];
//            [dic setValue:failed forKey:@"failed"];
//            [request setUserInfo:dic];
//            
//            //进行请求
//            [request startAsynchronous];
//            
//            [oneClassRequestDic setObject:request forKey:url];
//        }else{
//            [request clearDelegatesAndCancel];
//            [oneClassRequestDic removeObjectForKey:url];
//            [self requestProtobufWithURL:url objclass:classname data:srouceData prepare:prepare successed:successed failed:failed];
//        }
//    }
}

#pragma mark 数据下载

-(void)requestDownLoadWithURL:(NSString*)url savePath:(NSString*)path prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed{
    
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:[NSURL URLWithString:url]];
    __weak ASIHTTPRequest *weakRequest = request;
    [request setDownloadDestinationPath:path];
    [request setCompletionBlock:^( void ){
        successed(nil);
    }];
    // 使用 failed 块，在下载失败时做一些事情
    [request setFailedBlock :^( void ){
        failed([weakRequest error]);
    }];
    prepare();
    [request startAsynchronous];
    
    if (_downloadRequestArray == nil) {
        _downloadRequestArray = [[NSMutableArray alloc] init];
    }
    [_downloadRequestArray addObject:request];
    
//    if (! _downloadQueue ) {
//        _downloadQueue = [[ASINetworkQueue alloc] init];
//    }
//
//    [_downloadQueue reset]; // 队列清零
////    [ _downloadQueue setDownloadProgressDelegate:view]; // 设置 queue 进度条
////    [_downloadQueue setShowAccurateProgress:YES]; // 进度精确显示
//    [_downloadQueue setDelegate:self]; // 设置队列的代理对象
//    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:[NSURL URLWithString:url]];
//    [request setCompletionBlock:^( void ){
//        
//    }];
//    // 使用 failed 块，在下载失败时做一些事情
//    [request setFailedBlock :^( void ){
//        
//    }];
//    [_downloadQueue addOperation:request];
//    [_downloadQueue go];
}

-(void)cancelDownloadRequest{
    for (ASIHTTPRequest* request in _downloadRequestArray) {
        [request clearDelegatesAndCancel];
    }
    [_downloadRequestArray removeAllObjects];
}

#pragma mark 上传数据方法

#define TWITTERFON_FORM_BOUNDARY  @"1234567890qazwsxedcAaB03x" //分界线的标识符

-(NSData*)createUploadDataBodyWithSrcData:(NSData*)srcData{

    //分界线 --AaB03x
    NSString *MPboundary = [[NSString alloc]initWithFormat:@"--%@", TWITTERFON_FORM_BOUNDARY];
    //结束符 AaB03x--
    NSString *endMPboundary = [[NSString alloc]initWithFormat:@"%@--", MPboundary];
    
    //http body的字符串
    NSMutableString *body = [[NSMutableString alloc]init];
    ////添加分界线，换行
    [body appendFormat:@"%@\r\n",MPboundary];
    //声明pic字段，文件名为boris.png
    [body appendFormat:@"Content-Disposition: form-data; name=\"userfiles\"; filename=\"xiehaiduo.jpg\"\r\n"];
    //声明上传文件的格式
    [body appendFormat:@"Content-Type: image/*\r\n\r\n"];
    
    //声明结束符：--AaB03x--
    NSString *end=[[NSString alloc]initWithFormat:@"\r\n%@\r\n\r\n", endMPboundary];
    //声明myRequestData，用来放入http body
    NSMutableData *myRequestData = [NSMutableData data];
    //将body字符串转化为UTF8格式的二进制
    [myRequestData appendData:[body dataUsingEncoding:NSUTF8StringEncoding]];
    //将image的data加入
    [myRequestData appendData:srcData];
    
    //加入结束符--AaB03x--
    [myRequestData appendData:[end dataUsingEncoding:NSUTF8StringEncoding]];
    
    return myRequestData;
}

- (void)uploadSrouceDataRequestWithURL:(NSString*)url objclass:(Class)classname data:(NSData*)srouceData prepare:(void (^)(void))prepare successed:(void (^)(NSDictionary *dic))successed failed:(void (^)(NSError *error))failed {
    
    //再进入正常逻辑
    NSMutableDictionary* oneClassRequestDic = [requestDic objectForKey:NSStringFromClass(classname)];
    if (oneClassRequestDic == nil) {
        NSMutableDictionary* tempDic = [[NSMutableDictionary alloc] init];
        
        //请求之前预加载
        prepare();
        
        //设置http
        ASIFormDataRequest *request = [[ASIFormDataRequest alloc] initWithURL:[NSURL URLWithString:url]];
        [request setDelegate:self];
        [request setRequestMethod:@"POST"];
        
        //设置外部header
        NSArray *headerAllkeys = [headerDic allKeys];
        for (NSString *key in headerAllkeys) {
            [request addRequestHeader:key value:[headerDic objectForKey:key]];
        }//设置完成之后需要移除所有header中的数据
        [headerDic removeAllObjects];
        
        //设置HTTPHeader中Content-Type的值
        NSString *content = [[NSString alloc]initWithFormat:@"multipart/form-data; boundary=%@", TWITTERFON_FORM_BOUNDARY];
        [request addRequestHeader:@"Content-Type" value:content];
        //设置HTTPHeader中Content-Type的值
        NSMutableData* myRequestData = (NSMutableData*)[self createUploadDataBodyWithSrcData:srouceData];
        [request addRequestHeader:@"Content-Length" value:[NSString stringWithFormat:@"%lul", (unsigned long)[myRequestData length]]];
        //设置body
        [request setPostBody:myRequestData];
        
        //设置block
        NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
        [dic setValue:successed forKey:@"successed"];
        [dic setValue:failed forKey:@"failed"];
        [request setUserInfo:dic];
        
        //进行请求
        [request startAsynchronous];
        
        [tempDic setObject:request forKey:url];
        [requestDic setObject:tempDic forKey:NSStringFromClass(classname)];
    }else{
        ASIHTTPRequest *request = [oneClassRequestDic objectForKey:url];
        if (request == nil) {
            
            //请求之前预加载
            prepare();
            
            //设置http
            ASIFormDataRequest *request = [[ASIFormDataRequest alloc] initWithURL:[NSURL URLWithString:url]];
            [request setDelegate:self];
            [request setRequestMethod:@"POST"];
            
            //设置外部header
            NSArray *headerAllkeys = [headerDic allKeys];
            for (NSString *key in headerAllkeys) {
                [request addRequestHeader:key value:[headerDic objectForKey:key]];
            }//设置完成之后需要移除所有header中的数据
            [headerDic removeAllObjects];
            
            //设置HTTPHeader中Content-Type的值
            NSString *content = [[NSString alloc]initWithFormat:@"multipart/form-data; boundary=%@", TWITTERFON_FORM_BOUNDARY];
            [request addRequestHeader:@"Content-Type" value:content];
            //设置HTTPHeader中Content-Type的值
            NSMutableData* myRequestData = (NSMutableData*)[self createUploadDataBodyWithSrcData:srouceData];
            [request addRequestHeader:@"Content-Length" value:[NSString stringWithFormat:@"%lul", (unsigned long)[myRequestData length]]];
            //设置body
            [request setPostBody:myRequestData];
            
            //设置block
            NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
            [dic setValue:successed forKey:@"successed"];
            [dic setValue:failed forKey:@"failed"];
            [request setUserInfo:dic];
            
            //进行请求
            [request startAsynchronous];
            
            [oneClassRequestDic setObject:request forKey:url];
        }else{
            [request clearDelegatesAndCancel];
            [oneClassRequestDic removeObjectForKey:url];
            [self uploadSrouceDataRequestWithURL:url objclass:classname data:srouceData prepare:prepare successed:successed failed:failed];
        }
    }
}

- (void)cancelRequest:(ASIHTTPRequest*)request{
    [request clearDelegatesAndCancel];
    
    //移除block
    [(NSMutableDictionary*)request.userInfo removeAllObjects];
    request.userInfo = nil;
    
    NSArray* allClassRequestDicArray = [requestDic allValues];
    if (allClassRequestDicArray && [allClassRequestDicArray count]>0) {
        for (NSMutableDictionary* oneClassRequestDic in allClassRequestDicArray) {
            if (oneClassRequestDic) {
                //移除request
                [oneClassRequestDic removeObjectForKey:request.url.absoluteString];
            }
        }
    }
}

- (void)cancelRequestWithClassName:(Class)classname {
    NSMutableDictionary* oneClassRequestDic = [requestDic objectForKey:NSStringFromClass(classname)];
    if (oneClassRequestDic) {
        NSArray* allRequest = [oneClassRequestDic allValues];
        for (ASIHTTPRequest* request in allRequest) {
            [request clearDelegatesAndCancel];
            //移除block
            [(NSMutableDictionary*)request.userInfo removeAllObjects];
            request.userInfo = nil;
        }
        [oneClassRequestDic removeAllObjects];
    }
}

- (void)cancelAllRequest {
    NSArray *keys = [requestDic allKeys];
    for (NSString * key in keys) {
        NSMutableDictionary* oneClassRequestDic = [requestDic objectForKey:key];
        if (oneClassRequestDic) {
            NSArray* allRequest = [oneClassRequestDic allValues];
            for (ASIHTTPRequest* request in allRequest) {
                [request clearDelegatesAndCancel];
                //移除block
                [(NSMutableDictionary*)request.userInfo removeAllObjects];
                request.userInfo = nil;
            }
            [oneClassRequestDic removeAllObjects];
        }
    }
}

- (void)requestFinished:(ASIHTTPRequest *)request {
    //先把block取出来
    NewRequestSuccessed successed = [request.userInfo objectForKey:@"successed"];
    
    [self cancelRequest:request];
    
    id json = nil;
    if (successed) {
        if (request.tag == Protobuf_Tag) {
            if ([request responseStatusCode] == 200) {
                successed([NSDictionary dictionaryWithObject:[request responseData] forKey:@"data"]);
            }else{
                successed(nil);
            }
        }else{
            json = [[[request responseString] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding] myObjectFromJSONString];
            if (json == nil) {
                json = [[request responseData] myObjectFromJSONData];
            }
            successed(json);
        }
    }
}

- (void)requestFailed:(ASIHTTPRequest *)request {
    NewRequestFailed failed = [request.userInfo objectForKey:@"failed"];
    
    [self cancelRequest:request];
    
    if (failed) {
        failed([request error]);
    }
}


@end