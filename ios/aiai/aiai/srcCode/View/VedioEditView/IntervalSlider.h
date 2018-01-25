//
//  IntervalSlider.h
//
//
//  Created by xiehaiduo on 15/1/24.
//  Copyright (c) 2015年 yuanfeng. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface IntervalSlider : UIControl

//播放帧的时间间隔
@property(nonatomic)CGFloat value;

//传入帧数
- (id)initWithFrame:(CGRect)frame andPictuerNumber:(NSInteger)picNumber;

@end
