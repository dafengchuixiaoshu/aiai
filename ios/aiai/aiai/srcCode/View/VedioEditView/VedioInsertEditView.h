//
//  VedioIntervalEditView.h
//  UIWheelSlider
//
//  Created by xiehaiduo on 15/1/24.
//  Copyright (c) 2015年 yuanfeng. All rights reserved.
//

#import <UIKit/UIKit.h>

//点击确认
typedef void (^InsertEditCompleteBlock)(CGFloat value);
//点击取消
typedef void (^InsertEditCancelBlock)(void);

@interface VedioInsertEditView : UIView

//选中的时间值
@property(nonatomic)CGFloat value;

//插入特效的时间点，需要传入视频长度
- (id)initWithFrame:(CGRect)frame andVedioLength:(CGFloat)length;

@property(nonatomic,strong)InsertEditCompleteBlock completeBlock;
@property(nonatomic,strong)InsertEditCancelBlock cancelBlock;

@end
