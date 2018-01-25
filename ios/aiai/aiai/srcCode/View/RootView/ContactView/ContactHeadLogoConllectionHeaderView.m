//
//  ContactHeadLogoConllectionHeaderView.m
//  aiai
//
//  Created by zhangyang on 15/1/14.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ContactHeadLogoConllectionHeaderView.h"

#import "UIButton+FillColor.h"

@interface ContactHeadLogoConllectionHeaderView ()

@property(nonatomic,strong)UIButton* openCloseBtn;

@end

@implementation ContactHeadLogoConllectionHeaderView
@synthesize openCloseBtn;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        self.backgroundColor = [UIColor clearColor];//EveryView_BackGroundColor;
        
        //创建展开关闭按钮
        openCloseBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        UIImage* openCloseImage = [UIImage imageNamed:@"联系人展开按钮.png"];
        [openCloseBtn setBackgroundImage:openCloseImage forState:UIControlStateNormal];
        [openCloseBtn setBackgroundImage:openCloseImage forState:UIControlStateHighlighted];
        [openCloseBtn addTarget:self action:@selector(openCloseButtonPress:) forControlEvents:UIControlEventTouchUpInside];
        CGFloat openCloseBtn_X = (self.frame.size.width - openCloseImage.size.width)/2;
        CGFloat openCloseBtn_Y = 0;
        CGFloat openCloseBtn_H = openCloseImage.size.height;
        CGFloat openCloseBtn_W = openCloseImage.size.width;
        openCloseBtn.frame = CGRectMake(openCloseBtn_X, openCloseBtn_Y, openCloseBtn_W, openCloseBtn_H);
        [self addSubview:openCloseBtn];
    }
    return self;
}

#pragma mark 按钮事件
-(void)openCloseButtonPress:(id)sender{
    if (self.openCloseBtnBlock) {
        self.openCloseBtnBlock();
    }
}

-(void)changeButtonTransForm{
    self.openCloseBtn.selected = !self.openCloseBtn.selected;
    if (self.openCloseBtn.selected) {
        self.openCloseBtn.transform = CGAffineTransformMakeRotation((180.0f * M_PI) / 180.0f);
    }else{
        self.openCloseBtn.transform = CGAffineTransformMakeRotation(0);
    }
}

@end
