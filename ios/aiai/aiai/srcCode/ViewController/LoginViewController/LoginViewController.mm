//
//  LoginViewController.m
//  F2CPlatform
//
//  Created by BJMD131464 on 13-9-3.
//  Copyright (c) 2013年 xiehaiduo. All rights reserved.
//

#import "LoginViewController.h"
#import "RegisterViewController.h"
#import "LoginView.h"
#import "LoginMsgCenter.h"

#import "RECHeaderViewController.h"

@interface LoginViewController ()<RECHeaderViewControllerDelegate>

@property (nonatomic, strong)LoginView* loginView;

@end

@implementation LoginViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
//    self.rightBtnImageName = @"OK@2x.png";
    
    __weak LoginViewController *weakSelf = self;
    self.loginView = [[LoginView alloc] initWithFrame:self.view.frame];
    [self.loginView setPlayMovieViewWithMoviePath:nil];

    if ([self.loginView.usernameField.text length] == 0) {
        [self.loginView.usernameField becomeFirstResponder];
    }else if ([self.loginView.passwordField.text length] == 0){
        [self.loginView.passwordField becomeFirstResponder];
    }
    
    self.loginView.camerBtnBlock = ^{
//        RECHeaderViewController* vc = [[RECHeaderViewController alloc] init];
//        vc.delegate = weakSelf;
//        [weakSelf.navigationController pushViewController:vc animated:YES];
    };
    self.loginView.loginBtnBlock = ^{
        [LoginViewController loginWithUserName:weakSelf.loginView.usernameField.text password:weakSelf.loginView.passwordField.text];
    };
    self.loginView.registerBtnBlock = ^{
        RegisterViewController* con = [[RegisterViewController alloc] init];
        [weakSelf.navigationController pushViewController:con animated:YES];
    };
    //第三方登录
    self.loginView.wxloginBtnBlock = ^(){
        [weakSelf wxLogin];
    };
    self.loginView.wbloginBtnBlock = ^(){
        [weakSelf wbLogin];
    };
    self.loginView.qqloginBtnBlock = ^(){
        [weakSelf qqLogin];
    };

    [self.view insertSubview:self.loginView atIndex:0];
    
    //第三方授权成功的通知
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(opLogin:) name:USEROPENPLATFORMLOGINNOTIFICATION object:nil];
}

-(void)wxLogin{
    
    [OpenPlatformServers WXLogin];
}

-(void)wbLogin{
    
    [OpenPlatformServers sinaWeiboLogin];
    
}

-(void)qqLogin{
    
    [[OpenPlatformServers share] qqLogin:(id)self];
}

-(void)leftItemClicked:(id)sender{
    if (self.leftItemBtnBlock) {
        self.leftItemBtnBlock();
    }else{
        [super leftItemClicked:sender];
    }
}

-(void)rightItemClicked:(id)sender{
    
    RegisterViewController* con = [[RegisterViewController alloc] init];
    [self.navigationController pushViewController:con animated:YES];
}

#pragma mark RECHeaderViewControllerDelegate
-(void)REC_VideoPath:(NSURL*)path videoTime:(CGFloat)time{
    
}

#pragma mark 登录逻辑
+(void)loginWithCacheUserName:(NSString*)userName password:(NSString*)passWord{
    if ([SaveManager valueWithKey:USERFIRSTSTARTUP_KEY] || [[SaveManager valueWithKey:USER_ID_KEY] integerValue] == 0) {
        [LoginViewController loginWithUserName:userName password:passWord];
    }else{
        GlobalUserInfo* cacheUser = [[GlobalUserInfo alloc] init];
        cacheUser.userID = [[SaveManager valueWithKey:USER_ID_KEY] integerValue];
        cacheUser.userName = [SaveManager valueWithKey:USERNAME_KEY];
        cacheUser.userPassWord = [SaveManager valueWithKey:USERPASSWORD_KEY];
        cacheUser.userTelphone = [SaveManager valueWithKey:USERTELPHONE_KEY];
        cacheUser.userNickName = [SaveManager valueWithKey:USER_NICKNAME_KEY];
        cacheUser.userFaceUrl = [SaveManager valueWithKey:USER_FACEURL_KEY];
        cacheUser.userFaceVedio = [SaveManager valueWithKey:USER_FACEVIDEO_KEY];
        cacheUser.userGender = [[SaveManager valueWithKey:USER_GENDER_KEY] integerValue];
        [LoginMsgCenter notificationLoginResults:LoginStatus_LoginSuccess loginUser:cacheUser];
    }
}

+ (void)loginWithUserName:(NSString*)userName password:(NSString*)passWord{

    NSCharacterSet *set = [NSCharacterSet whitespaceAndNewlineCharacterSet];
    
    NSString *trimedString = [userName stringByTrimmingCharactersInSet:set];
    
    if ( ![trimedString length] || 0 == [userName length]) {
    
        AIAI_LOG(@"用户名不能为空");

        return;
    }
    else if ( !passWord || 0 == [passWord length]){
        AIAI_LOG(@"密码不能为空");

        return;
    }
    
    //登录
    GlobalUserInfo* info = [[GlobalUserInfo alloc] init];
    info.userName = userName;
    info.userPassWord = passWord;
    [LoginMsgCenter loginRequestWithUserInfo:info comlate:^(BOOL results,GlobalUserInfo* loginUser){
#ifdef LOGINSUCCESS
        if (NO == results) {
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"登录失败" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
            [alert show];
        }else{
            [LoginMsgCenter notificationLoginResults:results?LoginStatus_LoginSuccess:LoginStatus_Loginfalse loginUser:loginUser];
        }
#else
        GlobalUserInfo* defaultUser = [[GlobalUserInfo alloc] init];
        defaultUser.userID = 1;
        defaultUser.userName = @"18600326932";
        defaultUser.userPassWord = @"123";
        defaultUser.userTelphone = @"18600326932";
        defaultUser.userNickName = @"张阳";
        defaultUser.userFaceUrl = nil;
        defaultUser.userFaceVedio = nil;
        defaultUser.userGender = 1;
        [LoginMsgCenter notificationLoginResults:LoginStatus_LoginSuccess loginUser:defaultUser];
#endif
    }];
}

#pragma mark 微博微信登录回调方法
//第三方登录消息通知回调函数
- (void)opLogin:(NSNotification*)notification {
    
    NSString* openPlatformCode = [[notification userInfo] objectForKey:SourceCode];
    NSString* openPlatformID = [[notification userInfo] objectForKey:SourceID];
    
    [LoginViewController loginWithOpenPlatformID:openPlatformID OpenPlatformCode:openPlatformCode finished:^{

        //第三方授权之后继续走注册逻辑
        RegisterViewController* con = [[RegisterViewController alloc] init];
        con.openPlatformcode = openPlatformCode;
        con.openPlatformId = openPlatformID;
        [self.navigationController pushViewController:con animated:YES];
        
    }];
    
}

//qq登录回调
#pragma mark QQ 开放平台回调方法
- (void)tencentDidLogin {
    
    NSString* openPlatformCode = @"1";
    NSString* openPlatformID = [[OpenPlatformServers share] qqUserID];
    
    [LoginViewController loginWithOpenPlatformID:openPlatformID OpenPlatformCode:openPlatformCode finished:^{
        
        //第三方授权之后继续走注册逻辑
        RegisterViewController* con = [[RegisterViewController alloc] init];
        con.openPlatformcode = openPlatformCode;
        con.openPlatformId = openPlatformID;
        [self.navigationController pushViewController:con animated:YES];
        
    }];
    
}

//第三方登录
+ (void)loginWithOpenPlatformID:(NSString*)openPlatformID OpenPlatformCode:(NSString*)sourceCode finished:(void (^)(void))finished{
    
    [LoginMsgCenter OpenPlatformLoginRequestWithPlatformID:openPlatformID OpenPlatformCode:sourceCode prepare:^{
        
    } comlate:^(LoginStatus results, GlobalUserInfo *loginUser) {
        
        if (LoginStatus_Loginfalse == results) {
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"登录失败" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
            [alert show];
        }
        else if (LoginStatus_Unregister == results){
            if (finished) {
                finished();
            }
        }
        else if (LoginStatus_OpenPlatformLoginSuccess == results){
            loginUser.openPlatformId = openPlatformID;
            loginUser.openPlatformcode = sourceCode;
            [LoginMsgCenter notificationLoginResults:LoginStatus_OpenPlatformLoginSuccess loginUser:loginUser];
        }
        
    }];
}

#pragma mark UIView Method
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    [AppUtils closeKeyBoard];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}



@end
