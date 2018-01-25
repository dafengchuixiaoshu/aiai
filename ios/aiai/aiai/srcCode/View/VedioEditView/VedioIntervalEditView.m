//
//  VedioIntervalEditView.m
//  UIWheelSlider
//
//  Created by xiehaiduo on 15/1/24.
//  Copyright (c) 2015年 yuanfeng. All rights reserved.
//

#import "VedioIntervalEditView.h"
#import "IntervalSlider.h"

@interface VedioIntervalEditView ()
@property (nonatomic, strong) UILabel * valueLabel;
@property (nonatomic, strong) IntervalSlider * intervalSlider;

@end

@implementation VedioIntervalEditView

- (id)initWithFrame:(CGRect)frame andPictuerNumber:(NSInteger)picNumber
{
    self = [super initWithFrame:frame];
    if (self) {
        
        self.backgroundColor = EveryView_BackGroundColor;
        
        CGFloat padding = 15;
        self.valueLabel = [[UILabel alloc] initWithFrame:CGRectMake((frame.size.width-100)/2, 5, 100, 20)];
        [self.valueLabel setText:[NSString stringWithFormat:@"间隔 %0.2f 秒",self.intervalSlider.value]];
        [self.valueLabel setBackgroundColor:[UIColor clearColor]];
        [self.valueLabel setTextColor:[UIColor whiteColor]];
        [self.valueLabel setFont:[UIFont boldSystemFontOfSize:13]];
        [self.valueLabel setTextAlignment:NSTextAlignmentCenter];
        [self addSubview:self.valueLabel];
        
        
        
        UIButton* btnOk = [UIButton buttonWithType:UIButtonTypeCustom];
        btnOk.frame = CGRectMake(15, 0, 40, 20);
        [btnOk addTarget:self action:@selector(editOK) forControlEvents:UIControlEventTouchUpInside];
        [btnOk setTitle:@"确定" forState:UIControlStateNormal];
        [self addSubview:btnOk];
        
        UIButton* btnCancel = [UIButton buttonWithType:UIButtonTypeCustom];
        btnCancel.frame = CGRectMake(frame.size.width-45, 0, 40, 20);
        [btnCancel addTarget:self action:@selector(editCancel) forControlEvents:UIControlEventTouchUpInside];
        [btnCancel setTitle:@"取消" forState:UIControlStateNormal];
        [self addSubview:btnCancel];
        
        
        
        CGFloat intervalSlider_Y = self.valueLabel.frame.size.height+self.valueLabel.frame.origin.y + padding;
        self.intervalSlider = [[IntervalSlider alloc] initWithFrame:CGRectMake(padding, intervalSlider_Y, self.frame.size.width-20, 110)andPictuerNumber:picNumber];
        [self.intervalSlider addTarget:self action:@selector(sliderValueDidChange:) forControlEvents:UIControlEventValueChanged];
        [self addSubview:self.intervalSlider];
        [self.valueLabel setText:[NSString stringWithFormat:@"间隔 %0.2f 秒",self.intervalSlider.value]];
        self.value = self.intervalSlider.value;
    }
    return self;
}

-(void)sliderValueDidChange:(id)sender{
    [self.valueLabel setText:[NSString stringWithFormat:@"间隔 %0.2f 秒",self.intervalSlider.value]];
}

-(void)editOK {
    self.value = self.intervalSlider.value;
    if (self.completeBlock) {
        self.completeBlock(self.value);
    }
}

-(void)editCancel {
    if (self.cancelBlock) {
        self.cancelBlock();
    }
}

@end
