//
//  GraffitiBtnView.m
//  aiai
//
//  Created by wangnan on 15/9/25.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "GraffitiBtnView.h"
#import "RKColorSlider.h"

@interface GraffitiBtnView ()
@property(nonatomic, strong)UIButton *strokeBtn;
@end

@implementation GraffitiBtnView

-(id)initWithFrame:(CGRect)frame{
    
    if (self = [super initWithFrame:frame]) {
        
        RKColorSlider *colorSlider = [[RKColorSlider alloc] initWithFrame:CGRectMake(0, frame.size.height - 15, 0, 0)];
        colorSlider.getColorBlock = ^(UIColor *currentColor){
            
            if (_colorBlock) {
                _colorBlock(currentColor);
            }
        };
        [self addSubview:colorSlider];
        
        //取消按钮
        UIImage *calcelImg = [UIImage imageNamed:@"取消按钮.png"];
        UIButton * cancelBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [cancelBtn setImage:calcelImg forState:UIControlStateNormal];
        cancelBtn.frame = CGRectMake(LEFTPADDING, 0, calcelImg.size.width, calcelImg.size.height);
        [cancelBtn addTarget:self action:@selector(cancelAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:cancelBtn];
        
        //笔画粗细按钮
        UIImage *strokeImg = [UIImage imageNamed:@"中画笔@2x.png"];
        _strokeBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [_strokeBtn setImage:strokeImg forState:UIControlStateNormal];
        _strokeBtn.frame = CGRectMake(self.center.x - LEFTPADDING - strokeImg.size.width, cancelBtn.frame.origin.y, strokeImg.size.width, strokeImg.size.height);
        [_strokeBtn addTarget:self action:@selector(strokeAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_strokeBtn];
        
        //撤销按钮
        UIImage *revokeImg = [UIImage imageNamed:@"撤销.png"];
        UIButton * revokeBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [revokeBtn setImage:revokeImg forState:UIControlStateNormal];
        revokeBtn.frame = CGRectMake(self.center.x + LEFTPADDING, cancelBtn.frame.origin.y, revokeImg.size.width, revokeImg.size.height);
        [revokeBtn addTarget:self action:@selector(revokeAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:revokeBtn];
        
        //完成按钮
        UIImage *confirmImg = [UIImage imageNamed:@"完成按钮.png"];
        UIButton * confirmBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [confirmBtn setImage:confirmImg forState:UIControlStateNormal];
        confirmBtn.frame = CGRectMake(self.frame.size.width - confirmImg.size.width - LEFTPADDING, cancelBtn.frame.origin.y, confirmImg.size.width, confirmImg.size.height);
        [confirmBtn addTarget:self action:@selector(confirmAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:confirmBtn];
        
        self.backgroundColor = [UIColor clearColor];
    }
    
    return self;
}

-(void)setImgWithWidth:(FontWidth)width{
    
    switch (width) {
        case font_low:
        {
            [_strokeBtn setImage:[UIImage imageNamed:@"细画笔@2x.png"] forState:UIControlStateNormal];
            [_strokeBtn setImage:[UIImage imageNamed:@"细画笔@2x.png"] forState:UIControlStateHighlighted];
        }
            break;
        case font_normal:
        {
            [_strokeBtn setImage:[UIImage imageNamed:@"中画笔@2x.png"] forState:UIControlStateNormal];
            [_strokeBtn setImage:[UIImage imageNamed:@"中画笔@2x.png"] forState:UIControlStateHighlighted];
        }
            break;
        case font_high:
        {
            [_strokeBtn setImage:[UIImage imageNamed:@"粗画笔@2x.png"] forState:UIControlStateNormal];
            [_strokeBtn setImage:[UIImage imageNamed:@"粗画笔@2x.png"] forState:UIControlStateHighlighted];
        }
            break;
        default:
            break;
    }
}

-(void)confirmAction:(id)sender{
    if (_confirmBlock) {
        _confirmBlock();
    }
}

-(void)cancelAction:(id)sender{
    if (_cancelBlock) {
        _cancelBlock();
    }
}

-(void)revokeAction:(id)sender{
    if (_revokeBlock) {
        _revokeBlock();
    }
}

-(void)strokeAction:(id)sender{
    if (_strokeBlock) {
        _strokeBlock();
    }
}

@end
