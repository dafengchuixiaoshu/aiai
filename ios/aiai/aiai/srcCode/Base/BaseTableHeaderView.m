//
//  BaseTableHeaderView.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/11.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "BaseTableHeaderView.h"

@interface BaseTableHeaderView ()

@property(nonatomic,strong)UIImageView* nextImageView;

@property(nonatomic,strong)UIImageView* topCutLine;

@property(nonatomic,strong)UIImageView* bottomCutLine;

@end

@implementation BaseTableHeaderView
@synthesize Left_Right_Padding = _Left_Right_Padding;
@synthesize nextImageFront_Padding = _nextImageFront_Padding;
@synthesize nextImageView = _nextImageView;
@synthesize topCutLine = _topCutLine;
@synthesize bottomCutLine = _bottomCutLine;

-(id)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame]) {
        self.backgroundColor = [UIColor whiteColor];
        _Left_Right_Padding = 15;
        _nextImageFront_Padding = 10;
        
        _topCutLine = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        _topCutLine.frame = CGRectMake(0, 0, CGRectGetWidth([UIScreen mainScreen].bounds), 1);
        [self addSubview:_topCutLine];
        
        _nextImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"系统导航右箭头.png"]];
        CGRect rect = _nextImageView.frame;
        rect.origin.x = CGRectGetWidth(self.frame) - _Left_Right_Padding - _nextImageView.frame.size.width;
        rect.origin.y = (self.frame.size.height - _nextImageView.frame.size.height)/2;
        _nextImageView.frame = rect;
        [self addSubview:_nextImageView];
        
        _bottomCutLine = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        _bottomCutLine.frame = CGRectMake(0, self.frame.size.height-1, CGRectGetWidth([UIScreen mainScreen].bounds), 1);
        [self addSubview:_bottomCutLine];
        
        UITapGestureRecognizer* viewRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewTapAction)];
        viewRecognizer.delegate = (id)self;
        [self addGestureRecognizer:viewRecognizer];
        
    }
    return self;
}

-(void)setLeft_Right_Padding:(CGFloat)Left_Right_Padding{
    _Left_Right_Padding = Left_Right_Padding;
    
    CGRect rect = _nextImageView.frame;
    rect.origin.x = CGRectGetWidth(self.frame) - _Left_Right_Padding - _nextImageView.frame.size.width;
    _nextImageView.frame = rect;
}

-(CGFloat)nextImageWidth{
    return CGRectGetWidth(_nextImageView.frame);
}

-(void)setHiddenNextImage:(BOOL)hiddenNextImage{
    _nextImageView.hidden = hiddenNextImage;
}

-(void)viewTapAction{
    if (self.tapAction) {
        self.tapAction();
    }
}

@end
