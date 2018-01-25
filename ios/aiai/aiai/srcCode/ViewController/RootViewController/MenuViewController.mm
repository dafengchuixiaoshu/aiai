//
//  BlurSidebarViewController.m
//  aiai
//
//  Created by zhangyang on 14/12/13.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "MenuViewController.h"
#import "MenuTableHeaderView.h"
#import "MenuTableViewCell.h"
#import "IMPackageGlobal.h"
#import "IMPackageEngine.h"
#import "IMPackageData.h"
#import "AppDelegate.h"

#import "ContactViewController.h"
#import "LoginViewController.h"
#import "IISettingViewController.h"
#import "FeedBackViewController.h"

#import "LoginMsgCenter.h"
#import "PlayMovieView.h"//头像播放

#define CellView_H      45
#define HeaderView_H    ([UIScreen mainScreen].bounds.size.height - CellView_H*6)


#define CELLTITLE_CONTACT       @"联系人"
#define CELLTITLE_SHARE         @"分享给好友"
#define CELLTITLE_FEEDBACE      @"意见反馈"
#define CELLTITLE_SETTING       @"设置"

@interface MenuViewController ()<UITableViewDataSource,UITableViewDelegate,LoginMsgCenterDelegate>

@property(nonatomic,strong)UITableView* menuTableView;
@property(nonatomic,strong)NSArray* titleArray;

@property(nonatomic,assign)BOOL isAutoLogin;

@end

@implementation MenuViewController

-(id)init{
    if (self = [super init]) {
        self.titleArray = [NSArray arrayWithObjects:CELLTITLE_CONTACT,CELLTITLE_FEEDBACE,CELLTITLE_SETTING, nil];
    }
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    
    self.menuTableView = [[UITableView alloc] initWithFrame:self.contentView.bounds];
    [self.menuTableView setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    self.menuTableView.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    self.menuTableView.delegate = self;
    self.menuTableView.dataSource = self;
    [self.menuTableView registerClass:[MenuTableViewCell class] forCellReuseIdentifier:@"MenuTableViewCell"];
    
    MenuTableHeaderView* headview = [[MenuTableHeaderView alloc] initWithFrame:CGRectMake(0, 0, CGRectGetWidth(self.menuTableView.frame), HeaderView_H)];
    headview.headImageUrl = nil;
    headview.nickName = @"登录";
    headview.nickNameBtnBlock = ^{
        MenuTableHeaderView* tempHeadView = (MenuTableHeaderView*)self.menuTableView.tableHeaderView;
        if ([tempHeadView.nickName isEqualToString:@"登录"]) {
            [LoginMsgCenter presentLoginViewControllerDelegate:self Completion:nil];
        }
    };
    headview.setStatusBlock = ^(NSInteger status){
        
        [LoginMsgCenter setLocationStatusToSeverWithSwitchStatus:status];
        
    };
    
    self.menuTableView.tableHeaderView = headview;
    [self.contentView addSubview:self.menuTableView];
    
    //退出登录是的通知
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(logoutNotification:) name:USER_LOGOUT_NOTIFICATION object:nil];
    //登录成功通知
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(loginNotification:) name:USER_LOGIN_NOTIFICATION object:nil];
    
    //登录逻辑
    if ([SaveManager valueWithKey:USERNAME_KEY] && [[SaveManager valueWithKey:USERNAME_KEY] length]>0 &&
        [SaveManager valueWithKey:USERPASSWORD_KEY] && [[SaveManager valueWithKey:USERPASSWORD_KEY] length]>0) {
        self.isAutoLogin = YES;
        [LoginMsgCenter setDelegate:self];
        [LoginViewController loginWithCacheUserName:[SaveManager valueWithKey:USERNAME_KEY] password:[SaveManager valueWithKey:USERPASSWORD_KEY]];
        
    }
    //第三方登录
    else if ([SaveManager valueWithKey:USEROPENPLATFORMID] && [[SaveManager valueWithKey:USEROPENPLATFORMID] length]>0 &&
             [SaveManager valueWithKey:USEROPENPLATFORMCODE] && [[SaveManager valueWithKey:USEROPENPLATFORMCODE] length]>0){
        self.isAutoLogin = YES;
        [LoginMsgCenter setDelegate:self];
        [LoginViewController loginWithOpenPlatformID:[SaveManager valueWithKey:USEROPENPLATFORMID] OpenPlatformCode:[SaveManager valueWithKey:USEROPENPLATFORMCODE] finished:nil];
    }
}

#pragma mark LoginMsgCenterNotifi
-(void)loginNotification:(NSNotification*)notification{
    
    if (self.isAutoLogin) {
        //登录成功替换用户名和头像
        MenuTableHeaderView* headview = (MenuTableHeaderView*)self.menuTableView.tableHeaderView;
        headview.headImageUrl = [GlobalUserInfo myselfInfo].userFaceUrl;
        headview.headVedioUrl = [GlobalUserInfo myselfInfo].userFaceVedio;
        headview.nickName = [GlobalUserInfo myselfInfo].userNickName;
        headview.switchStatus = [GlobalUserInfo myselfInfo].Locstatus;
    }else{
        [LoginMsgCenter dismissLoginViewControllerCompletion:^{
            //登录成功替换用户名和头像
            MenuTableHeaderView* headview = (MenuTableHeaderView*)self.menuTableView.tableHeaderView;
            headview.headImageUrl = [GlobalUserInfo myselfInfo].userFaceUrl;
            headview.headVedioUrl = [GlobalUserInfo myselfInfo].userFaceVedio;
            headview.nickName = [GlobalUserInfo myselfInfo].userNickName;
            headview.switchStatus = [GlobalUserInfo myselfInfo].Locstatus;
        }];
    }
    
}

-(void)logoutNotification:(NSNotification*)notification{
    
    self.isAutoLogin = NO;
    
    MenuTableHeaderView* headview = (MenuTableHeaderView*)self.menuTableView.tableHeaderView;
    headview.headImageUrl = nil;
    headview.nickName = @"登录";
    headview.nickNameBtnBlock = ^{
        MenuTableHeaderView* tempHeadView = (MenuTableHeaderView*)self.menuTableView.tableHeaderView;
        if ([tempHeadView.nickName isEqualToString:@"登录"]) {
            [LoginMsgCenter presentLoginViewControllerDelegate:self Completion:nil];
        }
    };
    
}

#pragma mark TableViewDelegate
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [self.titleArray count];
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return CellView_H;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString* cellIdentifier = @"MenuTableViewCell";
    MenuTableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    cell.titleText = [self.titleArray objectAtIndex:indexPath.row];
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    
    NSString *title = [self.titleArray objectAtIndex:indexPath.row];
    [self showHideSidebar];
    
    if ([LoginMsgCenter isLogin]) {
        if ([title isEqualToString:CELLTITLE_CONTACT]){
            ContactViewController* con = [[ContactViewController alloc] init];
            [[AppDelegate rootViewController] pushViewController:con animated:YES];
        }
        else if ([title isEqualToString:CELLTITLE_SETTING]){
            IISettingViewController *con = [[IISettingViewController alloc] init];
            con.setting_changeFaceImgBlock = ^(){
                MenuTableHeaderView* headview = (MenuTableHeaderView*)self.menuTableView.tableHeaderView;
                headview.headImageUrl = [GlobalUserInfo myselfInfo].userFaceUrl;
                headview.headVedioUrl = [GlobalUserInfo myselfInfo].userFaceVedio;
                headview.nickName = [GlobalUserInfo myselfInfo].userNickName;
            };
            [[AppDelegate rootViewController] pushViewController:con animated:YES];
        }
        else if([title isEqualToString:CELLTITLE_FEEDBACE]){
            FeedBackViewController *con = [[FeedBackViewController alloc] init];
            [[AppDelegate rootViewController] pushViewController:con animated:YES];
        }
    }else{
        [LoginMsgCenter presentLoginViewControllerDelegate:self Completion:nil];
    }
}

@end
