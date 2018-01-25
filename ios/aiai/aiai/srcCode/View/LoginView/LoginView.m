//
//  LoginView.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/26.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "LoginView.h"
#import "PlayMovieView.h"

@interface LoginView ()
@property(nonatomic,strong)UIButton    *registerBtn;
@property(nonatomic,strong)UIButton    *loginBtn;

@property(nonatomic,strong)PlayMovieView* playMovieView;
@end

@implementation LoginView

@synthesize usernameField;
@synthesize passwordField;
@synthesize registerBtn;
@synthesize loginBtn;

-(id)initWithFrame:(CGRect)frame{
    if(self = [super initWithFrame:frame]){
        
        CGFloat Control_X = 0;
        CGFloat Control_Y = 0;
        CGFloat Control_W = 0;
        CGFloat Control_H = 0;
        CGFloat btnPadding = 18;
        
        //背景图
//        UIImage *bgImage = [UIImage imageNamed:@"loginBg.png"];
//        UIImageView *bg = [[UIImageView alloc] initWithImage:bgImage];
//        bg.frame = CGRectMake(0, 0, frame.size.width, frame.size.height);
//        [self addSubview:bg];
        
        //拍照按钮
//        UIImage *cameraIcon_image = [UIImage imageNamed:@"cameraIcon.png"];
//        Control_X = (frame.size.width - cameraIcon_image.size.width)/2;
//        Control_Y = 85;
//        UIButton* camerBtn = [UIButton buttonWithType:UIButtonTypeCustom];
//        camerBtn.frame = CGRectMake(Control_X, Control_Y, cameraIcon_image.size.width, cameraIcon_image.size.height);
//        [camerBtn setImage:cameraIcon_image forState:UIControlStateNormal];
//        [camerBtn addTarget:self action:@selector(camerBtnBtnAction:) forControlEvents:UIControlEventTouchUpInside];
//        [self addSubview:camerBtn];
        
        CGFloat openPlatform_btn_W = 33;
        CGFloat openPlatform_btn_H = 33;
        CGFloat openPlatform_btn_Y = 70;
        
        //微博登录
        CGFloat wbLoginBtn_X = frame.size.width - openPlatform_btn_W - btnPadding;
        UIButton *wbLoginBtn = [[UIButton alloc] initWithFrame:CGRectMake(wbLoginBtn_X, openPlatform_btn_Y, openPlatform_btn_W, openPlatform_btn_H)];
        [wbLoginBtn setBackgroundImage:[UIImage imageNamed:@"login_wb.png"] forState:UIControlStateNormal];
        [wbLoginBtn setBackgroundImage:[UIImage imageNamed:@"login_wb.png"] forState:UIControlStateHighlighted];
        [wbLoginBtn addTarget:self action:@selector(wbLoginBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:wbLoginBtn];
        
        //qq登录
        CGFloat qqLoginBtn_X = wbLoginBtn.frame.origin.x - openPlatform_btn_W - btnPadding;
        if ([OpenPlatformServers qqAppIsInstalled] ) {
            UIButton *qqLoginBtn = [[UIButton alloc] initWithFrame:CGRectMake(qqLoginBtn_X, openPlatform_btn_Y, openPlatform_btn_W, openPlatform_btn_H)];
            [qqLoginBtn setBackgroundImage:[UIImage imageNamed:@"login_qq.png"] forState:UIControlStateNormal];
            [qqLoginBtn setBackgroundImage:[UIImage imageNamed:@"login_qq.png"] forState:UIControlStateHighlighted];
            [qqLoginBtn addTarget:self action:@selector(qqLoginBtnAction:) forControlEvents:UIControlEventTouchUpInside];
            [self addSubview:qqLoginBtn];
        }
        
        //微信登录按钮
        if([OpenPlatformServers weiXinAppIsInstalled]){
            //根据qq登录的按钮布局
            CGFloat wxLoginBtn_X = wbLoginBtn_X - openPlatform_btn_W - btnPadding;
            if ([OpenPlatformServers qqAppIsInstalled]) {
                wxLoginBtn_X = qqLoginBtn_X - openPlatform_btn_W - btnPadding;
            }
            
            UIButton *wxLoginBtn = [[UIButton alloc] initWithFrame:CGRectMake(wxLoginBtn_X, openPlatform_btn_Y, openPlatform_btn_W, openPlatform_btn_H)];
            [wxLoginBtn setBackgroundImage:[UIImage imageNamed:@"login_wx.png"] forState:UIControlStateNormal];
            [wxLoginBtn setBackgroundImage:[UIImage imageNamed:@"login_wx.png"] forState:UIControlStateHighlighted];
            [wxLoginBtn addTarget:self action:@selector(wxLoginBtnAction:) forControlEvents:UIControlEventTouchUpInside];
            [self addSubview:wxLoginBtn];
        }
        
        //拍照按钮
        UIImage *cameraIcon_image = [UIImage imageNamed:@"cameraIcon.png"];
        Control_X = (self.frame.size.width - cameraIcon_image.size.width)/2;
        Control_Y = wbLoginBtn.frame.origin.y + wbLoginBtn.frame.size.height + btnPadding;
        UIImageView* imageview = [[UIImageView alloc] initWithImage:cameraIcon_image];
        imageview.frame = CGRectMake(Control_X, Control_Y, cameraIcon_image.size.width, cameraIcon_image.size.height);
        [self addSubview:imageview];
        
        //入户名输入框
        Control_X = 18;
        Control_Y = imageview.frame.origin.y + imageview.frame.size.height + btnPadding*2;
        Control_W = frame.size.width - Control_X*2;
        Control_H = 35;
        usernameField = [[LoginTextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H) leftIconName:@"userNameIcon.png" placeholderText: @"请输入用户名/手机号"];
        [self addSubview:usernameField];

        
        //密码输入框
        Control_X = 18;
        Control_Y = usernameField.frame.origin.y+usernameField.frame.size.height + 10;
        Control_W = frame.size.width - Control_X*2;
        Control_H = 35;
        passwordField = [[LoginTextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H) leftIconName:@"passwordIcon.png" placeholderText: @"请输入密码"];
        passwordField.secureTextEntry = YES;
        [self addSubview:passwordField];
    
        //注册按钮
        Control_X = btnPadding;
        Control_Y = passwordField.frame.origin.y+passwordField.frame.size.height + 50;
        Control_W = (frame.size.width - btnPadding*3)/2;
        Control_H = 40;
        registerBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W, Control_H)];
        [registerBtn setBackgroundColor:[UIColor colorWithRed:1.0 green:156/255.0 blue:0 alpha:1]];
        [registerBtn.titleLabel setFont:[UIFont systemFontWithAdapter:16]];
        [registerBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [registerBtn setTitle:@"注册" forState:UIControlStateNormal];
        registerBtn.layer.cornerRadius = Control_H/2;
        registerBtn.layer.masksToBounds = YES;
        [self addSubview:registerBtn];
        [registerBtn addTarget:self action:@selector(registerAction:) forControlEvents:UIControlEventTouchUpInside];
        
        //登录按钮
        Control_X = registerBtn.frame.origin.x+registerBtn.frame.size.width+btnPadding;
        Control_Y = passwordField.frame.origin.y+passwordField.frame.size.height + 50;
        Control_W = (frame.size.width - btnPadding*3)/2;
        Control_H = 40;
        loginBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W, Control_H)];
        [loginBtn setBackgroundColor:[UIColor colorWithRed:1.0 green:156/255.0 blue:0 alpha:1]];
        [loginBtn.titleLabel setFont:[UIFont systemFontWithAdapter:16]];
        [loginBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [loginBtn setTitle:@"登录" forState:UIControlStateNormal];
        loginBtn.layer.cornerRadius = Control_H/2;
        loginBtn.layer.masksToBounds = YES;
        [self addSubview:loginBtn];
        [loginBtn addTarget:self action:@selector(loginBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        
        
    }
    return self;
}

-(void)camerBtnBtnAction:(id)sender{
    if (self.camerBtnBlock) {
        self.camerBtnBlock();
    }
}

-(void)loginBtnAction:(id)sender{
    [AppUtils closeKeyBoard];
    
    if (self.loginBtnBlock) {
        self.loginBtnBlock();
    }
}

-(void)registerAction:(id)sender{
    [AppUtils closeKeyBoard];
    
    if (self.registerBtnBlock) {
        self.registerBtnBlock();
    }
}

-(void)wxLoginBtnAction:(id)sender{
    if (_wxloginBtnBlock) {
        _wxloginBtnBlock();
    }
}

-(void)wbLoginBtnAction:(id)sender{
    if (_wbloginBtnBlock) {
        _wbloginBtnBlock();
    }
}

-(void)qqLoginBtnAction:(id)sender{
    if (_qqloginBtnBlock) {
        _qqloginBtnBlock();
    }
}

-(void)setPlayMovieViewWithMoviePath:(NSString*)path{
    if (path) {
        
        UIImage *cameraIcon_image = [UIImage imageNamed:@"cameraIcon.png"];
        CGFloat Control_X = (self.frame.size.width - cameraIcon_image.size.width)/2;
        CGFloat Control_Y = 85;
        UIImageView* imageview = [[UIImageView alloc] initWithImage:cameraIcon_image];
        
        _playMovieView = [[PlayMovieView alloc] initWithFrame:CGRectMake(Control_X, Control_Y, cameraIcon_image.size.width, cameraIcon_image.size.height)];
        _playMovieView.moviePath = path;
        [_playMovieView addSubview:imageview];
        [self addSubview:_playMovieView];
    }
}

@end
