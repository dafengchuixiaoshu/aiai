//
//  RegisterNextViewController.m
//  aiai
//
//  Created by zhangyang on 15/4/2.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "RegisterNextViewController.h"
#import "RegisterNextView.h"
#import "RECHeaderViewController.h"
#import "LoginMsgCenter.h"

@interface RegisterNextViewController ()<RECHeaderViewControllerDelegate>
@property(nonatomic,strong)RegisterNextView* registerNextView;
@property(nonatomic,strong)GlobalUserInfo* registerUserinfo;

@property(nonatomic,copy)NSURL* uploadFaceVedio;
@end

@implementation RegisterNextViewController

-(id)initWithUserinfo:(GlobalUserInfo *)userinfo{
    if (self = [super init]) {
        self.registerUserinfo = userinfo;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.centerBtnImageName = @"注册";
    
    //添加视图
    CGRect rect = self.view.frame;
    rect.origin.y = self.navigationController.navigationBar.frame.origin.y + self.navigationController.navigationBar.frame.size.height;
    rect.size.height -= rect.origin.y;
    
    __weak RegisterNextViewController *weakSelf = self;
    self.registerNextView = [[RegisterNextView alloc] initWithFrame:rect];
    self.registerNextView.finishBtnBlock = ^{
        if ([weakSelf.registerNextView.nickNameField.text isEmpty]) {
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"请输入昵称" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
            [alert show];
            return;
        }
        if ([weakSelf.registerNextView.passwordField.text isEmpty]) {
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"请输入密码" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
            [alert show];
            return;
        }
        if (weakSelf.uploadFaceVedio == nil) {
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"请录制头像" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
            [alert show];
            return;
        }
        
        UIImage *faceImage = [AppUtils getImage:weakSelf.uploadFaceVedio];
        [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[weakSelf class] data:UIImagePNGRepresentation(faceImage)  prepare:^{
            [WaitingAlertView showWaitingViewAtView:weakSelf.view];
        } successed:^(NSDictionary *dic) {
            weakSelf.registerUserinfo.userFaceUrl = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[dic objectForKey:@"fileName"]];;
            //先上传头像获取key
            NSData* uploadData = [NSData dataWithContentsOfURL:weakSelf.uploadFaceVedio];
            [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[weakSelf class] data:uploadData prepare:^{
                
            } successed:^(NSDictionary *dic) {
                //成功之后进行注册
                weakSelf.registerUserinfo.userNickName = weakSelf.registerNextView.nickNameField.text;
                weakSelf.registerUserinfo.userPassWord = weakSelf.registerNextView.passwordField.text;
                weakSelf.registerUserinfo.userFaceVedio = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[dic objectForKey:@"fileName"]];
                [LoginMsgCenter registerRequestWithUserInfo:weakSelf.registerUserinfo comlate:^(BOOL results, GlobalUserInfo* loginUser) {
                    [WaitingAlertView hideWaitingView];
                    if (NO == results) {
                        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"注册失败" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles: nil];
                        [alert show];
                    }else{
                        //注册之后走登录流程
                        [LoginMsgCenter notificationLoginResults:LoginStatus_LoginSuccess loginUser:loginUser];
                    }
                }];
            } failed:^(NSError *error) {
                [WaitingAlertView hideWaitingView];
            }];
        } failed:^(NSError *error) {
            [WaitingAlertView hideWaitingView];
        }];
    };
    self.registerNextView.camerBtnBtnBlock = ^{
        RECHeaderViewController* vc = [[RECHeaderViewController alloc] init];
        vc.delegate = weakSelf;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    };
    [self.view addSubview:self.registerNextView];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

MPMoviePlayerController* movieplayer = nil;
#pragma mark RECHeaderViewControllerDelegate
-(void)REC_VideoPath:(NSURL*)path videoTime:(CGFloat)time{
    self.uploadFaceVedio = path;
    
    [self.registerNextView replaceMovieViewWithMoviePath:path.resourceSpecifier];
}

@end
