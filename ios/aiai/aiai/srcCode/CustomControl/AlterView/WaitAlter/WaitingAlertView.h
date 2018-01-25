//
//  WaitingView.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/3.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, WaitingAlertView_Mode){
    WaitingAlertView_Mode_Full = 0,//填充
    WaitingAlertView_Mode_Fit,//正常显示
};

@interface WaitingAlertView : UIView

//自定义显示
-(id)initWithFrame:(CGRect)frame mode:(WaitingAlertView_Mode)mode;
//指定区域内显示
+(void)showWaitingViewWithFrame:(CGRect)frame;
//整个屏幕
+(void)showWaitingViewAtWindow;
//指定View,不拉伸
+(void)showWaitingViewAtView:(UIView*)view;


//显示在view的center
+(void)showWaitingViewAtCenterView:(UIView*)view;

//指定View,动画填充满
+(void)showFullWaitingViewAtView:(UIView*)view;
//消失
+(void)hideWaitingView;

@end
