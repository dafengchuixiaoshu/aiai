//
//  AddressMapHeadView.m
//  aiai
//
//  Created by zhangyang on 15/1/19.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AddressMapTopView.h"

@interface AddressMapTopView ()

@property(nonatomic, strong)UIButton *openCloseBtn;

@end

@implementation AddressMapTopView
@synthesize openCloseBtn;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        self.backgroundColor = [UIColor blackColor];
        //创建展开关闭按钮
        openCloseBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        UIImage* openCloseImage = [UIImage imageNamed:@"联系人展开按钮.png"];
        [openCloseBtn setBackgroundImage:openCloseImage forState:UIControlStateNormal];
        [openCloseBtn addTarget:self action:@selector(openCloseButtonPress) forControlEvents:UIControlEventTouchUpInside];
        CGFloat openCloseBtn_X = (self.frame.size.width - openCloseImage.size.width)/2;
        CGFloat openCloseBtn_Y = 5;
        openCloseBtn.frame = CGRectMake(openCloseBtn_X, openCloseBtn_Y, openCloseImage.size.width*2, openCloseImage.size.height*2);
        [self addSubview:openCloseBtn];
        
    }
    return self;
}

#pragma mark 按钮事件
-(void)openCloseButtonPress{
    [self changeBtnTransform];
    
    if (self.openCloseBtnBlock) {
        self.openCloseBtnBlock();
    }
}

-(void)changeBtnTransform{
    
    openCloseBtn.selected = !openCloseBtn.selected;
    if (openCloseBtn.selected) {
        openCloseBtn.transform = CGAffineTransformMakeRotation((180.0f * M_PI) / 180.0f);
    }else{
        openCloseBtn.transform = CGAffineTransformMakeRotation(0);
    }
}


@end
