//
//  NetWorkMonitors.m
//  F2CPlatform
//
//  Created by Circle Liu on 14-11-27.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "NetWorkMonitors.h"

#import <ASIHTTPRequest/Reachability.h>

#import "AppDelegate.h"


@interface NetWorkMonitors () {
    Reachability* hostReach;
}

@end

@implementation NetWorkMonitors

+ (NetWorkMonitors*)share {
    static NetWorkMonitors *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (id)init {
    if (self = [super init]) {
        hostReach = [Reachability reachabilityForInternetConnection];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(reachabilityChanged:)
                                                     name:kReachabilityChangedNotification
                                                   object:nil];
    }
    return self;
}

- (void)start {
    [hostReach startNotifier];
}

- (void)reachabilityChanged:(NSNotification *)note {
    Reachability* curReach = [note object];
    NSParameterAssert([curReach isKindOfClass: [Reachability class]]);
    NetworkStatus status = [curReach currentReachabilityStatus];
    
    if (status == NotReachable) {
//        [viewController showGlobalMessage:@"当前网络不可用，点此检查网络设置" andStyle:TxtCenter andHideTime:0 andLookMsg:^{
        
//        }];
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            
        });
    }else if (status == ReachableViaWiFi){
//        [viewController showGlobalMessage:@"当前网络: WIFI" andStyle:TxtCenter andHideTime:2 andLookMsg:nil];
        
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{

        });
    }else if (status == ReachableViaWWAN){
//        [viewController showGlobalMessage:@"当前网络: 2G/3G" andStyle:TxtCenter andHideTime:2 andLookMsg:nil];
        
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
           
        });
    }
}


@end
