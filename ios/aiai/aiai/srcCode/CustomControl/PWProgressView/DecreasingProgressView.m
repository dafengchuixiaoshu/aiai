//
//  DecreasingProgressView.m
//  aiai
//
//  Created by zhangyang on 15/1/16.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "DecreasingProgressView.h"

@interface DecreasingProgressView ()

@property(nonatomic,strong)UILabel* progressLabel;
@property(nonatomic,assign)CGFloat decreasingValue;

@end

@implementation DecreasingProgressView
@synthesize progressLabel = _progressLabel;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        _progressLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, frame.size.width, frame.size.height)];
        _progressLabel.backgroundColor = [UIColor orangeColor];
        _progressLabel.layer.cornerRadius = _progressLabel.frame.size.height/2;
        _progressLabel.layer.masksToBounds = YES;
        [self addSubview:_progressLabel];
    }
    return self;
}

-(void)resetProgress{
    self.hidden = YES;
    _decreasingValue = 0.0f;
    _progressLabel.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.height);
}

-(void)setProgress:(CGFloat)progress{
    if (-0.000001 < _decreasingValue && _decreasingValue < 0.000001 ) {
        _decreasingValue = self.frame.size.width* (1.0-progress);
    }
    
    CGRect rect = _progressLabel.frame;
    if (-0.000001 < progress && progress < 0.000001 ) {
        _decreasingValue = 0.0f;
        rect.size.width = 0.0f;
    }else{
        rect.size.width -= _decreasingValue;
    }
    _progressLabel.frame = rect;

    _progressLabel.center = CGPointMake(self.center.x, _progressLabel.center.y);
}


@end
