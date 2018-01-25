//
//  ChangePasswordViewController.m
//  aiai
//
//  Created by wangnan on 15/5/8.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ChangePasswordViewController.h"
#import "LoginTextField.h"

#import "InputParamsPOJO.pb.h"
#import "OutputResultData.pb.h"

@interface ChangePasswordViewController ()

@property(nonatomic, strong)LoginTextField *oldPasswordField;
@property(nonatomic, strong)LoginTextField *currentPasswordField;

@property(nonatomic, strong)UIButton *passwordShowBtn;
@end

@implementation ChangePasswordViewController
@synthesize oldPasswordField,currentPasswordField;
@synthesize passwordShowBtn;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.centerBtnImageName = @"修改密码";
    
    //老密码输入框
    CGFloat Control_X = 18;
    CGFloat Control_Y = self.navigationController.navigationBar.frame.size.height + 35;
    CGFloat Control_W = self.view.frame.size.width - Control_X*2 ;
    CGFloat Control_H = 35;
    oldPasswordField = [[LoginTextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H) leftIconName:@"passwordIcon.png" placeholderText:@"请输入登录密码"];
    [self.view addSubview:oldPasswordField];
    
    //新密码输入框
    Control_X = 18;
    Control_Y = oldPasswordField.frame.origin.y+oldPasswordField.frame.size.height + 20;
    Control_W = (self.view.frame.size.width - Control_X*2)*3/4;
    Control_H = 35;
    currentPasswordField = [[LoginTextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H) leftIconName:@"passwordIcon.png" placeholderText:@"请输入新密码"];
    currentPasswordField.secureTextEntry = YES;
    [self.view addSubview:currentPasswordField];
    
    //密码可见
    Control_W = self.view.frame.size.width - Control_X*2 - currentPasswordField.frame.size.width;
    Control_H = 35;
    Control_X = self.view.frame.size.width - Control_W - 18;
    Control_Y = currentPasswordField.frame.origin.y;
    passwordShowBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X , Control_Y, Control_W, Control_H)];
    [passwordShowBtn setTitle:@"密码可见" forState:UIControlStateNormal];
    passwordShowBtn.titleLabel.font = [UIFont systemFontWithAdapter:14];
    [passwordShowBtn setTitleColor:Every_YellowColor forState:UIControlStateNormal];
    passwordShowBtn.layer.borderColor = Every_YellowColor.CGColor;
    passwordShowBtn.layer.borderWidth = 1.0;
    passwordShowBtn.layer.cornerRadius = Control_H/2;
    passwordShowBtn.layer.masksToBounds = YES;
    [passwordShowBtn addTarget:self action:@selector(passwordShowBtnAction:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:passwordShowBtn];
    
    //完成按钮
    Control_W = 160;
    Control_X = (self.view.frame.size.width - Control_W)/2;
    Control_Y = currentPasswordField.frame.origin.y+currentPasswordField.frame.size.height + 50;
    Control_H = 35;
    UIButton *finishBtn = [[UIButton alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W, Control_H)];
    [finishBtn setBackgroundImage:[UIImage imageNamed:@"register_btn_able.png"] forState:UIControlStateNormal];
    finishBtn.layer.borderColor = [UIColor whiteColor].CGColor;
    finishBtn.layer.borderWidth = 1.0;
    finishBtn.layer.cornerRadius = Control_H/2;
    finishBtn.layer.masksToBounds = YES;
    [finishBtn setTitle:@"完成" forState:UIControlStateNormal];
    finishBtn.titleLabel.font = [UIFont systemFontWithAdapter:14];
    [finishBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [finishBtn addTarget:self action:@selector(finishBtnAction:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:finishBtn];
    
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(resignFirstAction:)];
    [self.view addGestureRecognizer:tap];
}


-(void)passwordShowBtnAction:(id)sender{
    currentPasswordField.secureTextEntry = !currentPasswordField.secureTextEntry;
}

-(void)finishBtnAction:(id)sender{
    
    NSString *password = [SaveManager valueWithKey:USERPASSWORD_KEY];
    if ([password isEqualToString:oldPasswordField.text]) {
        
        com::mm::pb::po::InputParamsPOJO inputObj;
        inputObj.set_uid((int32_t)[GlobalUserInfo myselfInfo].userID);
        inputObj.set_oldpwd([oldPasswordField.text UTF8String]);
        inputObj.set_newpwd([currentPasswordField.text UTF8String]);
        
        //申请内存对数据进行序列化
        ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[inputObj.ByteSize()]();
        inputObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
        
        //序列化之后进行网络请求
        NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"updatePwd.pb"];
        
        __weak ChangePasswordViewController *weakSelf = self;
        [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:inputObj.ByteSize()] prepare:^{
            
        } successed:^(NSDictionary *dic) {
            NSData* data = [dic objectForKey:@"data"];
            
            com::mm::pb::po::OutputResultData outputOjb;
            outputOjb.ParseFromArray([data bytes], (int)[data length]);
            ::google::protobuf::int32 code = outputOjb.resultcode();
            if (code >= 0) {
                //如果密码修改成功,更新本地存储的密码,并返回到上一界面
                [SaveManager saveValue:currentPasswordField.text forKey:USERNAME_KEY];
                [weakSelf.navigationController popViewControllerAnimated:YES];
            }
            
        } failed:^(NSError *error) {
            AIAI_LOG(@"%@",error);
        }];
        
    }
    
}

-(void)resignFirstAction:(id)sender{
    [oldPasswordField resignFirstResponder];
    [currentPasswordField resignFirstResponder];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
