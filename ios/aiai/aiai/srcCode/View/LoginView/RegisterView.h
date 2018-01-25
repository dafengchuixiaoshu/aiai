//
//  RegisterView.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/26.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LoginTextField.h"

typedef void (^registerBtnAction)(void);
typedef void (^protocolBtnAction)(void);

@interface RegisterView : UIView

@property (nonatomic, retain) LoginTextField* inputPhoneField;
@property (nonatomic, retain) LoginTextField* inputCaptchaField;

@property(nonatomic,strong)registerBtnAction getCaptchaBtnBlock;
@property(nonatomic,strong)registerBtnAction nextBtnBlock;
@property(nonatomic,strong)protocolBtnAction protocolBtnBlock;

//获取验证码之后开始倒计时
-(void)captchaCountdown;
-(void)stopCountdown;

@end
