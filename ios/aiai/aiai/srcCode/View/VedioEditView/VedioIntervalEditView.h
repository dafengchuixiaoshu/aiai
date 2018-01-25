//
//  VedioIntervalEditView.h
//  UIWheelSlider
//
//  Created by xiehaiduo on 15/1/24.
//  Copyright (c) 2015年 yuanfeng. All rights reserved.
//

#import <UIKit/UIKit.h>

//点击确认
typedef void (^IntervalEditCompleteBlock)(CGFloat value);
//点击取消
typedef void (^IntervalEditCancelBlock)(void);

@interface VedioIntervalEditView : UIView

//选中的时间值
@property(nonatomic)CGFloat value;

//动画定格：需要传入帧数
- (id)initWithFrame:(CGRect)frame andPictuerNumber:(NSInteger)picNumber;

@property(nonatomic,strong)IntervalEditCompleteBlock completeBlock;
@property(nonatomic,strong)IntervalEditCancelBlock cancelBlock;

@end
