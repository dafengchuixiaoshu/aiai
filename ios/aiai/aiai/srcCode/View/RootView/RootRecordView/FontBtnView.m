//
//  FontBtnView.m
//  aiai
//
//  Created by wangnan on 15/10/19.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "FontBtnView.h"
#import "GraffitiView.h"
#import "RKColorSlider.h"

@implementation FontBtnView


-(id)initWithFrame:(CGRect)frame{
    
    if (self = [super initWithFrame:frame]) {
        
        RKColorSlider *colorSlider = [[RKColorSlider alloc] initWithFrame:CGRectMake(0, frame.size.height - 15, 0, 0)];
        colorSlider.getColorBlock = ^(UIColor *currentColor){
            
            if (_colorBlock) {
                _colorBlock(currentColor);
            }
        };
        [self addSubview:colorSlider];
        
        UIImage* cancelImage = [UIImage imageNamed:@"取消按钮.png"];
        UIButton *cancelBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [cancelBtn setBackgroundImage:cancelImage forState:UIControlStateNormal];
        cancelBtn.frame = CGRectMake(LEFTPADDING, 0, cancelImage.size.width, cancelImage.size.height);
        [cancelBtn addTarget:self action:@selector(cancelBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:cancelBtn];
        
        UIImage* changeImage = [UIImage imageNamed:@"切换字体.png"];
        UIButton *changeBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [changeBtn setBackgroundImage:changeImage forState:UIControlStateNormal];
        changeBtn.frame = CGRectMake(self.center.x - LEFTPADDING - changeImage.size.width, cancelBtn.frame.origin.y, changeImage.size.width, changeImage.size.height);
        [changeBtn addTarget:self action:@selector(changeBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:changeBtn];
        
        UIImage* downLoadImage = [UIImage imageNamed:@"字体下载.png"];
        UIButton *downLoadBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [downLoadBtn setBackgroundImage:downLoadImage forState:UIControlStateNormal];
        downLoadBtn.frame = CGRectMake(self.center.x + LEFTPADDING, cancelBtn.frame.origin.y, downLoadImage.size.width, downLoadImage.size.height);
        [downLoadBtn addTarget:self action:@selector(downLoadFont:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:downLoadBtn];
        
        UIImage* confirmImage = [UIImage imageNamed:@"完成按钮.png"];
        UIButton *confirmBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [confirmBtn setBackgroundImage:confirmImage forState:UIControlStateNormal];
        confirmBtn.frame = CGRectMake(self.frame.size.width - confirmImage.size.width - LEFTPADDING, cancelBtn.frame.origin.y, confirmImage.size.width, confirmImage.size.height);
        [confirmBtn addTarget:self action:@selector(confirmAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:confirmBtn];
        
    }
    return self;
    
}

#pragma mark -- buttonAction
-(void)cancelBtnAction:(id)sender{
    if (_cancelBlock) {
        _cancelBlock();
    }
}

-(void)changeBtnAction:(id)sender{
    if (_changeBlock) {
        _changeBlock();
    }
}

-(void)downLoadFont:(id)sender{
    if (_downLoadFontBlock) {
        _downLoadFontBlock();
    }
}

-(void)confirmAction:(id)sender{
    if (_confirmBlock) {
        _confirmBlock();
    }
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
