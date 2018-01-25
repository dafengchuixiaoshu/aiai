//
//  LoginView.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/26.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LoginTextField.h"

typedef void (^LoginBtnAction)(void);
typedef void (^WXLoginBtnAction)(void);
typedef void (^WBLoginBtnAction)(void);
typedef void (^QQLoginBtnAction)(void);
typedef void (^RegisterBtnAction)(void);

@interface LoginView : UIView

-(void)setPlayMovieViewWithMoviePath:(NSString*)path;

@property(nonatomic,strong)LoginTextField *usernameField;
@property(nonatomic,strong)LoginTextField *passwordField;

@property(nonatomic,strong)LoginBtnAction camerBtnBlock;
@property(nonatomic,strong)LoginBtnAction loginBtnBlock;
@property(nonatomic,strong)WXLoginBtnAction wxloginBtnBlock;
@property(nonatomic,strong)WBLoginBtnAction wbloginBtnBlock;
@property(nonatomic,strong)QQLoginBtnAction qqloginBtnBlock;
@property(nonatomic,strong)RegisterBtnAction registerBtnBlock;

@end
