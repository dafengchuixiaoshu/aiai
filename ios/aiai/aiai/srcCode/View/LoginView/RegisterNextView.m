//
//  RegisterNextView.m
//  aiai
//
//  Created by zhangyang on 15/4/2.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "RegisterNextView.h"
#import "PlayMovieView.h"

@implementation RegisterNextView

-(id)initWithFrame:(CGRect)frame{
    if(self = [super initWithFrame:frame]){
        
        UILabel* cutline = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, frame.size.width, 1)];
        cutline.backgroundColor = [UIColor grayColor];
        [self addSubview:cutline];
        
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(resignFirstAction)];
        [self addGestureRecognizer:tap];
        
        CGFloat Control_X = 0;
        CGFloat Control_Y = 0;
        CGFloat Control_W = 0;
        CGFloat Control_H = 0;
        //拍照按钮
        UIImage *cameraIcon_image = [UIImage imageNamed:@"cameraIcon.png"];
        Control_X = (frame.size.width - cameraIcon_image.size.width)/2;
        Control_Y = 35;
        _camerBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        _camerBtn.frame = CGRectMake(Control_X, Control_Y, cameraIcon_image.size.width, cameraIcon_image.size.height);
        [_camerBtn setImage:cameraIcon_image forState:UIControlStateNormal];
        [_camerBtn addTarget:self action:@selector(camerBtnBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_camerBtn];
        
        //昵称输入框
        Control_X = 18;
        Control_Y = _camerBtn.frame.origin.y+_camerBtn.frame.size.height+30;
        Control_W = frame.size.width - Control_X*2 ;
        Control_H = 35;
        _nickNameField = [[LoginTextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H) leftIconName:@"userNameIcon.png" placeholderText:@"请输入昵称"];
        [self addSubview:_nickNameField];
        
        //密码可见
        Control_W = 150;
        Control_H = 35;
        Control_X = self.frame.size.width - Control_W - 18;
        Control_Y = _nickNameField.frame.origin.y+_nickNameField.frame.size.height + 20;
        _passwordShowBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X , Control_Y, Control_W, Control_H)];
        [_passwordShowBtn setTitle:@"密码可见" forState:UIControlStateNormal];
        _passwordShowBtn.titleLabel.font = [UIFont systemFontWithAdapter:14];
        [_passwordShowBtn setTitleColor:Every_YellowColor forState:UIControlStateNormal];
        _passwordShowBtn.layer.borderColor = Every_YellowColor.CGColor;
        _passwordShowBtn.layer.borderWidth = 1.0;
        _passwordShowBtn.layer.cornerRadius = Control_H/2;
        _passwordShowBtn.layer.masksToBounds = YES;
        [_passwordShowBtn addTarget:self action:@selector(passwordShowBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_passwordShowBtn];
        
        //密码输入框
        Control_X = 18;
        Control_Y = _passwordShowBtn.frame.origin.y;
        Control_W = frame.size.width - Control_X*2 - _passwordShowBtn.frame.size.width;
        Control_H = 35;
        _passwordField = [[LoginTextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H) leftIconName:@"passwordIcon.png" placeholderText:@"请输入密码"];
        _passwordField.secureTextEntry = YES;
        [self addSubview:_passwordField];
        
        
        //完成按钮
        Control_W = 160;
        Control_X = (frame.size.width - Control_W)/2;
        Control_Y = _passwordField.frame.origin.y+_passwordField.frame.size.height + 50;
        Control_H = 35;
        _finishBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W, Control_H)];
        [_finishBtn setBackgroundImage:[UIImage imageNamed:@"register_btn_able.png"] forState:UIControlStateNormal];
        _finishBtn.layer.borderColor = [UIColor whiteColor].CGColor;
        _finishBtn.layer.borderWidth = 1.0;
        _finishBtn.layer.cornerRadius = Control_H/2;
        _finishBtn.layer.masksToBounds = YES;
        [_finishBtn setTitle:@"完成" forState:UIControlStateNormal];
        _finishBtn.titleLabel.font = [UIFont systemFontWithAdapter:14];
        [_finishBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [_finishBtn addTarget:self action:@selector(finishBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_finishBtn];
    }
    return self;
}

-(void)passwordShowBtnAction:(id)sender{
    _passwordField.secureTextEntry = !_passwordField.secureTextEntry;
}

-(void)finishBtnAction:(id)sender{
    if (self.finishBtnBlock) {
        self.finishBtnBlock();
    }
}

-(void)camerBtnBtnAction:(id)sender{
    if (self.camerBtnBtnBlock) {
        self.camerBtnBtnBlock();
    }
}

-(void)resignFirstAction{
    [_passwordField resignFirstResponder];
    [_nickNameField resignFirstResponder];
}

-(void)replaceMovieViewWithMoviePath:(NSString*)path{
    
    self.moveView = [[PlayMovieView alloc] initWithFrame:_camerBtn.frame];
    self.moveView.moviePath = path;
    self.moveView.layer.cornerRadius = self.moveView.frame.size.width / 2;
    self.moveView.layer.masksToBounds = YES;
    [self addSubview:self.moveView];
}

@end
