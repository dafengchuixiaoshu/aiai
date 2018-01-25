//
//  RegisterViewController.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/26.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "RegisterViewController.h"
#import "RegisterView.h"
#import "RegisterNextViewController.h"
#import "LoginMsgCenter.h"
#import "RegisterProtocolViewController.h"

@interface RegisterViewController ()

@property(nonatomic,strong)RegisterView* registerView;

@end

@implementation RegisterViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.centerBtnImageName = @"注册";
    
    //添加视图
    CGRect rect = self.view.frame;
    rect.origin.y = self.navigationController.navigationBar.frame.origin.y + self.navigationController.navigationBar.frame.size.height;
    rect.size.height -= rect.origin.y;
    
    __weak RegisterViewController *weakSelf = self;
    self.registerView = [[RegisterView alloc] initWithFrame:rect];
    self.registerView.alpha = 1.0;
    self.registerView.getCaptchaBtnBlock = ^{
        //检查手机号是否使用并获取验证码
        [LoginMsgCenter checkPhoneNumRequestWithPhoneNum:weakSelf.registerView.inputPhoneField.text comlate:^{
            [LoginMsgCenter identifyingCodeRequestWithPhoneNum:weakSelf.registerView.inputPhoneField.text comlate:^{
                [weakSelf.registerView captchaCountdown];
            }];
        }];
    };
    self.registerView.protocolBtnBlock = ^{
        RegisterProtocolViewController* newCon = [[RegisterProtocolViewController alloc] init];
        newCon.controllerTitle = @"ii软件许可及服务协议";
        [weakSelf.navigationController pushViewController:newCon animated:YES];
    };
    self.registerView.nextBtnBlock = ^{
        if ([weakSelf.registerView.inputPhoneField.text isEmpty]) {
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"请输入电话" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
            [alert show];
            return;
        }
        if ([weakSelf.registerView.inputCaptchaField.text isEmpty]) {
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"请输入验证码" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
            [alert show];
            return;
        }
        //校验验证码
        [LoginMsgCenter checkIdentifyingCodeRequestWithMobile:weakSelf.registerView.inputPhoneField.text identifyingCode:weakSelf.registerView.inputCaptchaField.text comlate:^{
            GlobalUserInfo* userinfo = [[GlobalUserInfo alloc] init];
            userinfo.userTelphone = weakSelf.registerView.inputPhoneField.text;
            userinfo.openPlatformcode = weakSelf.openPlatformcode;
            userinfo.openPlatformId = weakSelf.openPlatformId;
            RegisterNextViewController* con = [[RegisterNextViewController alloc] initWithUserinfo:userinfo];
            [weakSelf.navigationController pushViewController:con animated:YES];
        }];
    };
    self.registerView.protocolBtnBlock = ^{
        RegisterProtocolViewController* con = [[RegisterProtocolViewController alloc] init];
        [weakSelf.navigationController pushViewController:con animated:YES];
    };
    [self.view addSubview:self.registerView];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)viewWillAppear:(BOOL)animated{
    
    [super viewWillAppear:animated];
    
}

#pragma mark UIPanGestureRecognizer
/*
-(void)checkTelphoneIsVaild{
    [AppUtils closeKeyBoard];
    if ([self.registerView.inputPhoneField.text length]==0) {
//@"请输入手机号"
        return;
    }
    
    __weak RegisterViewController* weakself = self;
    
    __block NSString* phoneNum = self.registerView.inputPhoneField.text;
    BOOL isValidPhone = [AppUtils checkPhoneNumber:phoneNum];
    if (isValidPhone) {
        
        
    }else{
    //@"您输入的不是一个正确的手机号"
    }
}
*/
-(void)leftItemClicked:(id)sender{
    [self.registerView stopCountdown];
    
    if (self.leftItemBtnBlock) {
        self.leftItemBtnBlock();
    }else{
        [super leftItemClicked:sender];
    }
}

-(void)rightItemClicked:(id)sender{
    [AppUtils closeKeyBoard];
    
    if (![self.registerView.inputPhoneField.text length]){
        //@"请输入手机号"
        return;
    }
    
    if (![self.registerView.inputCaptchaField.text length]){
        //@"验证码不能为空"
        return;
    }
    
//    __weak RegisterViewController *weakself = self;
    
    //注册接口
 
}

@end
