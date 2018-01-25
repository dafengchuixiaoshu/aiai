//
//  WaitingView.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/3.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "WaitingAlertView.h"
#import "AppDelegate.h"

#define WaitingView_bgImage             @"WaitAlterBg.png"
#define WaitingView_annimationImage     @"WaitAlter.png"

static WaitingAlertView* g_WaitingView = nil;

@implementation WaitingAlertView

-(id)initWithFrame:(CGRect)frame mode:(WaitingAlertView_Mode)mode{
    if (self = [super initWithFrame:frame]) {
//        UIImageView* bgImageview = [[UIImageView alloc] initWithImage:[UIImage imageNamed:WaitingView_bgImage]];
//        [self addSubview:bgImageview];
        UIImage* annimationImage = [UIImage imageNamed:WaitingView_annimationImage];
        UIImageView* annimationImageview = [[UIImageView alloc] initWithImage:annimationImage];
        CGRect rect = annimationImageview.frame;
        if (mode == WaitingAlertView_Mode_Fit) {
            rect.size.width = frame.size.width<rect.size.width?frame.size.width:rect.size.width;
        }else if (mode == WaitingAlertView_Mode_Full){
            rect.size.width = frame.size.width;
        }
        rect.size.height = rect.size.width;
        rect.origin.x = fabs((frame.size.width - rect.size.width))/2;
        rect.origin.y = fabs((frame.size.height - rect.size.height))/2;
        
        annimationImageview.frame = rect;
        [self addSubview:annimationImageview];
        
        CABasicAnimation* rotationAnimation;
        rotationAnimation = [CABasicAnimation animationWithKeyPath:@"transform.rotation.z"];
        rotationAnimation.toValue = [NSNumber numberWithFloat: M_PI * 2.0 ];
        [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
        rotationAnimation.duration = 2;
        rotationAnimation.repeatCount = INT_MAX;//你可以设置到最大的整数值
        rotationAnimation.cumulative = NO;
        rotationAnimation.removedOnCompletion = NO;
        rotationAnimation.fillMode = kCAFillModeForwards;
        [annimationImageview.layer addAnimation:rotationAnimation forKey:@"Rotation"];
    }
    return self;
}

//指定frame
+(void)showWaitingViewWithFrame:(CGRect)frame{
    [WaitingAlertView hideWaitingView];
    
    g_WaitingView = [[WaitingAlertView alloc] initWithFrame:frame mode:WaitingAlertView_Mode_Fit];
    [[[UIApplication sharedApplication].delegate window] addSubview:g_WaitingView];
}

//整个屏幕
+(void)showWaitingViewAtWindow{
    CGRect rect = [UIScreen mainScreen].bounds;
    rect.origin.x = 0;
    rect.origin.y = 0;
    [self showWaitingViewWithFrame:rect];
}

//指定View
+(void)showWaitingViewAtView:(UIView*)view{
    [WaitingAlertView hideWaitingView];
    
    g_WaitingView = [[WaitingAlertView alloc] initWithFrame:CGRectMake(0, 0, view.frame.size.width, view.frame.size.height) mode:WaitingAlertView_Mode_Fit];
    [view addSubview:g_WaitingView];
}

//显示在view的center
+(void)showWaitingViewAtCenterView:(UIView*)view{
    
    [WaitingAlertView hideWaitingView];
    
    g_WaitingView = [[WaitingAlertView alloc] initWithFrame:CGRectMake(0, 0, view.frame.size.width, view.frame.size.height) mode:WaitingAlertView_Mode_Fit];
    g_WaitingView.center = view.center;
    [view addSubview:g_WaitingView];
    
}

//指定View,动画填充满
+(void)showFullWaitingViewAtView:(UIView*)view{
    
    [WaitingAlertView hideWaitingView];
    
    g_WaitingView = [[WaitingAlertView alloc] initWithFrame:CGRectMake(0, 0, view.frame.size.width, view.frame.size.height) mode:WaitingAlertView_Mode_Full];
    [view addSubview:g_WaitingView];
    
}

//消失
+(void)hideWaitingView{
    [g_WaitingView removeFromSuperview];
    g_WaitingView = nil;
}

@end
