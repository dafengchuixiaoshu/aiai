//
//  RegisterView.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/26.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "RegisterView.h"

#define Register_redColor [UIColor redColor]

@interface RegisterView ()
@property (nonatomic, retain) UIButton* getCaptchaBtn;
@property (nonatomic, strong) UIButton *protocolBtn;

@property(nonatomic,strong)NSTimer* timer;
@property(nonatomic,assign)int second;

@end

@implementation RegisterView

@synthesize inputPhoneField;
@synthesize inputCaptchaField;
@synthesize getCaptchaBtn;

-(id)initWithFrame:(CGRect)frame{
    if(self = [super initWithFrame:frame]){
        
        UILabel* cutline = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, frame.size.width, 1)];
        cutline.backgroundColor = [UIColor grayColor];
        [self addSubview:cutline];
        
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(resignFirstAction)];
        [self addGestureRecognizer:tap];
        
        //获取验证码
        CGFloat Control_W = 150;
        CGFloat Control_H = 35;
        CGFloat Control_X = self.frame.size.width - Control_W - 18;
        CGFloat Control_Y = 35;
        getCaptchaBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X , Control_Y, Control_W, Control_H)];
        [getCaptchaBtn setTitle:@"获取验证码" forState:UIControlStateNormal];
        getCaptchaBtn.titleLabel.font = [UIFont systemFontWithAdapter:14];
        [getCaptchaBtn setTitleColor:Every_YellowColor forState:UIControlStateNormal];
        getCaptchaBtn.layer.borderColor = Every_YellowColor.CGColor;
        getCaptchaBtn.layer.borderWidth = 1.0;
        getCaptchaBtn.layer.cornerRadius = Control_H/2;
        getCaptchaBtn.layer.masksToBounds = YES;
        [getCaptchaBtn addTarget:self action:@selector(getCaptchaBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:getCaptchaBtn];
        
        //电话输入框
        Control_X = 18;
        Control_Y = Control_Y;
        Control_W = frame.size.width - Control_X*2 - getCaptchaBtn.frame.size.width;
        Control_H = 35;
        inputPhoneField = [[LoginTextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H) leftIconName:@"telphoneIcon.png" placeholderText:@"请输入手机号"];
        inputPhoneField.keyboardType = AIAI_KeyboardTypeNumberPad;
        [self addSubview:inputPhoneField];
    
        //验证码输入框
        Control_X = 18;
        Control_Y = inputPhoneField.frame.origin.y+inputPhoneField.frame.size.height + 10;
        Control_W = frame.size.width - Control_X*2;
        Control_H = 35;
        inputCaptchaField = [[LoginTextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H) leftIconName:@"messageIcon.png" placeholderText:@"请输入验证码"];
        inputCaptchaField.keyboardType = AIAI_KeyboardTypeNumberPad;
        [self addSubview:inputCaptchaField];
        
        //
        CGFloat agreeLabel_Y = inputCaptchaField.frame.origin.y+inputCaptchaField.frame.size.height + 20;
        CGFloat agreeLabel_w = self.frame.size.width;
        UILabel* agreeLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, agreeLabel_Y, agreeLabel_w, 20)];
        agreeLabel.text = @"当点击“下一步”按钮，即表示您已同意";
        agreeLabel.textColor = Every_ContentTextColor;
        agreeLabel.font = [UIFont systemFontOfSize:13];
        agreeLabel.textAlignment = NSTextAlignmentCenter;
        [self addSubview:agreeLabel];
        
        CGFloat protocolBtn_Y = agreeLabel.frame.origin.y + 20;
        self.protocolBtn = [[UIButton alloc] initWithFrame:CGRectMake(0 , protocolBtn_Y, agreeLabel_w, agreeLabel.frame.size.height)];
        self.protocolBtn.titleLabel.font = [UIFont systemFontOfSize:13];
        [self.protocolBtn setTitleColor:Every_YellowColor forState:UIControlStateNormal];
        [self.protocolBtn setTitle:@"《ii软件许可及服务协议》" forState:UIControlStateNormal];
        [self addSubview:self.protocolBtn];
        [self.protocolBtn addTarget:self action:@selector(protocolBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        //
        
        //下一步按钮
        Control_W = 160;
        Control_X = (frame.size.width - Control_W)/2;
        Control_Y = inputCaptchaField.frame.origin.y+inputCaptchaField.frame.size.height + 150;
        Control_H = 35;
        UIButton* nextBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W, Control_H)];
        [nextBtn setBackgroundImage:[UIImage imageNamed:@"register_btn_able.png"] forState:UIControlStateNormal];
        nextBtn.layer.borderColor = [UIColor whiteColor].CGColor;
        nextBtn.layer.borderWidth = 1.0;
        nextBtn.layer.cornerRadius = Control_H/2;
        nextBtn.layer.masksToBounds = YES;
        [nextBtn setTitle:@"下一步" forState:UIControlStateNormal];
        nextBtn.titleLabel.font = [UIFont systemFontWithAdapter:14];
        [nextBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [nextBtn addTarget:self action:@selector(nextBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:nextBtn];
        /*
        //注册协议
        Control_X = 0;
        Control_Y = 0;
        Control_W = 0;
        Control_H = 0;
        UIImage* notSelectImage = [UIImage imageNamed:@"register_agree_no.png"];
        UIImage* selectImage = [UIImage imageNamed:@"register_agree.png"];
        UIButton* selectBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X , Control_Y, Control_W, Control_H)];
        [selectBtn setImage:notSelectImage forState:UIControlStateNormal];
        [selectBtn setImage:selectImage forState:UIControlStateSelected];
        selectBtn.selected = YES;
        [selectBtn addTarget:self action:@selector(selectBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:selectBtn];
        
        CGFloat agreeLabel_x = selectBtn.frame.origin.x+selectBtn.frame.size.width + 20;
        CGFloat agreeLabel_w = 36;
        UILabel* agreeLabel = [[UILabel alloc] initWithFrame:CGRectMake(agreeLabel_x, selectBtn.frame.origin.y, agreeLabel_w, selectBtn.frame.size.height)];
        agreeLabel.text = @"同意";
        agreeLabel.textColor = Every_ContentTextColor;
        agreeLabel.font = Every_SystemFontSize_14;
        [self addSubview:agreeLabel];
        
        CGFloat protocolBtn_X = agreeLabel.frame.origin.x + agreeLabel.frame.size.width;
        CGFloat protocolBtn_Y = agreeLabel.frame.origin.y;
        UIButton* protocolBtn = [[UIButton alloc] initWithFrame:CGRectMake(protocolBtn_X , protocolBtn_Y, 150,agreeLabel.frame.size.height)];
        protocolBtn.titleLabel.font = Every_SystemFontSize_14;
        [protocolBtn setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
        [protocolBtn setTitle:@"《海选注册协议》" forState:UIControlStateNormal];
        [self addSubview:protocolBtn];
        [protocolBtn addTarget:self action:@selector(protocolBtnAction:) forControlEvents:UIControlEventTouchUpInside];
         */
    }
    return self;
}

#pragma mark Action
-(void)bgTapGestureRecognizer:(id)sender{
    [AppUtils closeKeyBoard];
}

//注册事件
-(void)nextBtnAction:(id)sender{
    if (self.nextBtnBlock) {
        self.nextBtnBlock();
    }
}

//获取验证码
-(void)getCaptchaBtnAction:(id)sender{
    if (self.getCaptchaBtnBlock) {
        self.getCaptchaBtnBlock();
    }
}
-(void)protocolBtnAction:(id)sender{
    
    if (self.protocolBtnBlock) {
        self.protocolBtnBlock();
    }
}

-(void)resignFirstAction{
    [inputPhoneField resignFirstResponder];
    [inputCaptchaField resignFirstResponder];
}

#pragma mark textField 协议
-(BOOL) textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string{
    
    return YES;
}

#pragma mark 验证码倒计时
-(void)captchaCountdown{
    self.second = 60;
    self.timer = [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(countdown:) userInfo:nil repeats:YES];
}

-(void)countdown:(NSTimer*)theTimer{
    if (self.second == 0) {
        [theTimer invalidate];
        [getCaptchaBtn setEnabled:YES];
        [getCaptchaBtn setTitle:@"重新获取" forState:UIControlStateNormal];
        [getCaptchaBtn setBackgroundImage:[UIImage imageNamed:@"register_btn_able.png"] forState:UIControlStateNormal];
        [getCaptchaBtn setTitleColor:Register_redColor forState:UIControlStateNormal];
    }else{
        self.second--;
        //性能小优化，只渲染一两次
        if (self.second>58) {
            [getCaptchaBtn setBackgroundImage:[UIImage imageNamed:@"register_btn_enable.png"] forState:UIControlStateNormal];
            [getCaptchaBtn setTitleColor:Every_ContentTextColor forState:UIControlStateNormal];
            [getCaptchaBtn setEnabled:NO];
        }
        NSString *string = [NSString stringWithFormat:@" %dS重新发送",self.second];
        [getCaptchaBtn setTitle:string forState:UIControlStateNormal];
    }
}

-(void)stopCountdown{
    if ([self.timer isValid]) {
        [self.timer invalidate];
        self.timer = nil;
    }
}

@end
