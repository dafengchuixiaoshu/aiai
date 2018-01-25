//
//  AppraisalServer.m
//  Test
//
//  Created by Circle Liu on 15/5/28.
//  Copyright (c) 2015年 Circle Liu. All rights reserved.
//

#import "AppraisalServer.h"

//#import <StoreKit/StoreKit.h>

#import "SaveManager.h"

#define TIME1 180  //多少秒后显示评价框
#define TIME2 2  //第几次打开App后可以弹出评价提示框
#define TIME3 (30 * 24 * 60 * 60) // 拒绝多少天后可以再次弹出评价框，第一个30是天数，修改的话改这个值就行

static AppraisalServer *instance;
static NSTimer *timer;
static NSInteger countTime = 0;

@implementation AppraisalServer

+ (void)start {
    if ([AppraisalServer control]) {
        instance = [[AppraisalServer alloc] init];
        
        timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:instance selector:@selector(countTime) userInfo:nil repeats:YES];
    }
}

- (id)init {
    if (self = [super init]) {
        
    }
    return self;
}

- (void)countTime {
    if (TIME1 <= countTime++) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"给ii评价" message:@"感谢你使用ii，如果喜欢我们的产品欢迎在App Store上给我们评价" delegate:instance cancelButtonTitle:nil otherButtonTitles:@"去评价", @"稍后提醒", @"不再提示", nil];
        [alert show];
        
        [timer invalidate];
    }
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    if (0 == buttonIndex) {
        NSMutableDictionary *data = [SaveManager valueWithKey:@"AppraisalServer"];
        
        NSString *version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
        
        data = [[NSMutableDictionary alloc] init];
        [data setObject:@"2" forKey:@"time2"];
        [data setObject:version forKey:@"version"];
        [data setObject:@"1" forKey:@"ed"];
        [data setObject:[NSDate date] forKey:@"date"];
        [SaveManager saveValue:data forKey:@"AppraisalServer"];

        NSString *string = [NSString stringWithFormat:@"https://itunes.apple.com/cn/app/id943354237?mt=8"];
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:string]];
    }
    else if (1 == buttonIndex) {
        countTime = 0;
        
        timer = [NSTimer scheduledTimerWithTimeInterval:1 target:instance selector:@selector(countTime) userInfo:nil repeats:YES];
    }
    else if (2 == buttonIndex) {
        NSMutableDictionary *data = [SaveManager valueWithKey:@"AppraisalServer"];
        
        NSString *version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
        
        data = [[NSMutableDictionary alloc] init];
        [data setObject:@"2" forKey:@"time2"];
        [data setObject:version forKey:@"version"];
        [data setObject:@"0" forKey:@"ed"];
        [data setObject:[NSDate date] forKey:@"date"];
        
        [SaveManager saveValue:data forKey:@"AppraisalServer"];
    }
}

//- (void)productViewControllerDidFinish:(SKStoreProductViewController *)viewController {
//    [[AppDelegate appRootController] dismissViewControllerAnimated:YES completion:nil];
//}

+ (BOOL)control {
    NSMutableDictionary *data = [SaveManager valueWithKey:@"AppraisalServer"];
    
    NSString *version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    
    if (nil == data) {
        data = [[NSMutableDictionary alloc] init];
        [data setObject:@"1" forKey:@"time2"];          // 第几次打开App，key对应上面的宏名
        [data setObject:version forKey:@"version"];     // 记录上次打开App的版本
        [data setObject:@"0" forKey:@"ed"];             // ed是英语动词过去式的结尾，在这里用于表示是否评价过。喷子勿喷！
//        [data setObject:[NSDate date] forKey:@"date"];// 记录上次用户拒绝评价的日期，用于判断是否计时弹出评价框，所以此处暂不存值，用户拒绝时再存值
        
        [SaveManager saveValue:data forKey:@"AppraisalServer"];
        
        return NO; 
    }
    
    if (![version isEqualToString:[data objectForKey:@"version"]]) {
        [data setObject:@"2" forKey:@"time2"];
        [data setObject:version forKey:@"version"];
        [data setObject:@"1" forKey:@"ed"];
//        [data setObject:@"" forKey:@"date"];
        
        [SaveManager saveValue:data forKey:@"AppraisalServer"];
    }
    
    // 如果上次用户拒绝过
    if (nil != [data objectForKey:@"date"]) {
        NSDate *date = [data objectForKey:@"date"];
        NSDate *today = [NSDate date];
        
        // 判断一下上次用户拒绝过的时间是否已经超过30天，如果超过了30天，就可以再次弹出评价框
        if ([today timeIntervalSince1970] - [date timeIntervalSince1970] < TIME3) {
            
            return NO;
        }
    }
    
    return YES;
}

@end
