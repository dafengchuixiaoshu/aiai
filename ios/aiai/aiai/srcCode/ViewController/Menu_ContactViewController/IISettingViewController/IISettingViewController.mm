//
//  IISettingViewController.m
//  aiai
//
//  Created by wangnan on 15/4/19.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "IISettingViewController.h"
#import "LoginViewController.h"
#import "MenuViewController.h"
#import "ChangePasswordViewController.h"
#import "ChangeFaceImgViewController.h"

#import "IISettingTableViewCell.h"

#import "LoginMsgCenter.h"

#import <ASIHTTPRequest/Reachability.h>

#define SETTINGCELLSAVE         @"发送前保存至相册"
#define SETTINGCELLLOAD         @"非WIFI下自动下载"
#define SETTINGCELLPLAY         @"下载完成后自动播放"
#define SETTINGCELLPASSWORD     @"修改密码"
#define SETTINGCELLCHECK        @"版本检查"
#define SETTINGCELLCHANGEVIDEO  @"修改头像"

@interface IISettingViewController ()<UITableViewDataSource,UITableViewDelegate,LoginMsgCenterDelegate>

@property(nonatomic, strong)NSArray *titleArray;

@property(nonatomic, strong)UITableView *settingTable;
@end

@implementation IISettingViewController

-(id)init{
    if (self = [super init]) {
        
        self.titleArray = [NSArray arrayWithObjects:[NSArray arrayWithObjects:SETTINGCELLSAVE, nil],[NSArray arrayWithObjects:SETTINGCELLLOAD,SETTINGCELLPLAY, nil],[NSArray arrayWithObjects:SETTINGCELLCHANGEVIDEO,SETTINGCELLPASSWORD, nil], nil];
        
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.centerBtnImageName = @"设置";
    
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    self.settingTable = [[UITableView alloc] initWithFrame:rect];
    [self.settingTable setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    self.settingTable.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    self.settingTable.sectionIndexBackgroundColor = [UIColor clearColor];
    self.settingTable.sectionIndexTrackingBackgroundColor = [UIColor clearColor];
    self.settingTable.delegate = self;
    self.settingTable.dataSource = self;
    [self.settingTable registerClass:[IISettingTableViewCell class] forCellReuseIdentifier:@"IISettingTableViewCell"];
    self.settingTable.tableFooterView = [self creatFooterView];
    [self.view addSubview:self.settingTable];
}

#pragma mark-------- 创建footer 和 header 视图
-(UIView*)creatFooterView{
    
    UIView *footerView = [[UIView alloc] init];
    footerView.frame = CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, 150);
    
    UIButton *exitButton = [[UIButton alloc] initWithFrame:CGRectMake(0, footerView.frame.size.height - 44, [UIScreen mainScreen].bounds.size.width, 44)];
    exitButton.backgroundColor = [UIColor clearColor];
    exitButton.titleLabel.font = [UIFont systemFontOfSize:16];
    [exitButton setTitleColor:Every_YellowColor forState:UIControlStateNormal];
    [exitButton setTitleColor:Every_YellowColor forState:UIControlStateHighlighted];
    [exitButton setTitle:@"退出当前账号" forState:UIControlStateNormal];
    [exitButton setTitle:@"退出当前账号" forState:UIControlStateHighlighted];
    [exitButton addTarget:self action:@selector(logout) forControlEvents:UIControlEventTouchUpInside];
    [footerView addSubview:exitButton];
    
    return footerView;
}

-(void)logout{
    
    [LoginMsgCenter presentLoginViewControllerDelegate:self Completion:^{
        
        [[AppDelegate rootViewController] popToRootViewControllerAnimated:NO];
        [LoginMsgCenter notificationLoginResults:LoginStatus_Logout loginUser:nil];
        
    }];
}

-(void)loginResults:(LoginStatus)results{
    if (results == LoginStatus_LoginSuccess) {
        [LoginMsgCenter dismissLoginViewControllerCompletion:nil];
    }
}


#pragma mark -- tableDelegate

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    NSArray *rowArray = [self.titleArray objectAtIndex:section];
    return [rowArray count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    //读取该用户的设置数据 进行展示和修改
    NSString *path = [SaveManager libraryPath:IISETTING];
    NSData* cacheData = [SaveManager readToDataWithFilePath:path];
    NSMutableDictionary *settingDic = [NSKeyedUnarchiver unarchiveObjectWithData:cacheData];
    NSNumber *userId = [NSNumber numberWithInteger:[GlobalUserInfo myselfInfo].userID];
    NSMutableDictionary *userDic = [settingDic objectForKey:userId];
    
    IISettingTableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"IISettingTableViewCell" forIndexPath:indexPath];
    
    NSArray *rowTitle = [self.titleArray objectAtIndex:indexPath.section];
    NSString *title = [rowTitle objectAtIndex:indexPath.row];
    
    if (indexPath.section <=1) {
        BOOL switchisOn;
        if ([title isEqualToString:SETTINGCELLSAVE]) {
            switchisOn = [[userDic objectForKey:SAVE] intValue];
        }
        else if([title isEqualToString:SETTINGCELLLOAD]){
            switchisOn = [[userDic objectForKey:AUTOLOAD] intValue];
        }
        else if ([title isEqualToString:SETTINGCELLPLAY]){
            switchisOn = [[userDic objectForKey:AUTOPLAY] intValue];
        }
        [cell setTitleText:title subText:nil needSwitch:YES andSwitchOn:switchisOn];
    }else{
        [cell setTitleText:title subText:nil needSwitch:NO andSwitchOn:NO];
    }
    
    cell.actionBlock = ^(BOOL on){
        
        if ([title isEqualToString:SETTINGCELLSAVE]) {
            [userDic setObject:[NSNumber numberWithBool:on] forKey:SAVE];
        }
        else if([title isEqualToString:SETTINGCELLLOAD]){
            [userDic setObject:[NSNumber numberWithBool:on] forKey:AUTOLOAD];
        }
        else if ([title isEqualToString:SETTINGCELLPLAY]){
            [userDic setObject:[NSNumber numberWithBool:on] forKey:AUTOPLAY];
        }
        
        NSData* settingData = [NSKeyedArchiver archivedDataWithRootObject:settingDic];
        [SaveManager saveToFileWithData:settingData filePath:path];
    };
    
    return cell;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return IISETTINGCELLHEIGHT;
}

-(UIView*)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    
    UIView *headerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, 26)];
    headerView.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.05];
    
    UILabel* keyLabel = [[UILabel alloc] initWithFrame:CGRectMake(13, 0, self.view.frame.size.width, 26)];
    keyLabel.backgroundColor = [UIColor clearColor];
    keyLabel.textColor = [UIColor whiteColor];
    keyLabel.font = [UIFont systemFontOfSize:16];
    keyLabel.alpha = 0.5;
    if (section == 0) {
        keyLabel.text = [NSString stringWithFormat:@"发送设置"];
    }else if (section == 1){
        keyLabel.text = [NSString stringWithFormat:@"播放设置"];
    }else{
        keyLabel.text = [NSString stringWithFormat:@"基本设置"];
    }
    [headerView addSubview:keyLabel];
    return headerView;
}

-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    
    return 26;
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return [self.titleArray count];
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    
    NSArray *rowTitle = [self.titleArray objectAtIndex:indexPath.section];
    NSString *title = [rowTitle objectAtIndex:indexPath.row];

    if ([title isEqualToString:SETTINGCELLPASSWORD]) {
        ChangePasswordViewController *con = [[ChangePasswordViewController alloc] init];
        [self.navigationController pushViewController:con animated:YES];
    }
    else if ([title isEqualToString:SETTINGCELLCHANGEVIDEO]){
        ChangeFaceImgViewController *con = [[ChangeFaceImgViewController alloc] init];
        con.changeFaceImgBlock = ^(){
            
            if (_setting_changeFaceImgBlock) {
                _setting_changeFaceImgBlock();
            }
            
        };
        [self.navigationController pushViewController:con animated:YES];
    }
    
}



#pragma mark --  是否保存照片
+(BOOL)isSaveImg{
    
    NSMutableDictionary *userDic = [self readDic];
    return [[userDic objectForKey:SAVE] intValue];
}

#pragma mark -- 判断是否为wifi环境
+ (BOOL)isWIFI{
    
    NetworkStatus status = [[Reachability reachabilityForLocalWiFi] currentReachabilityStatus];
    return status == kReachableViaWiFi;
}

#pragma mark -- 是否在wifi下自动下载
+ (BOOL)isAutoLoadWithWIFI{
    
    if ([IISettingViewController isWIFI]) {
        
        NSMutableDictionary *userDic = [self readDic];
        return [[userDic objectForKey:AUTOLOAD] intValue];
    }
    
    return NO;
}

#pragma mark -- 下载后是否自动播放
+ (BOOL)isAutoPlayAfterLoad{
    
    NSMutableDictionary *userDic = [self readDic];
    return [[userDic objectForKey:AUTOPLAY] intValue];
}

//读取文件
+(NSMutableDictionary*)readDic{
    
    NSString *path = [SaveManager libraryPath:IISETTING];
    NSData* cacheData = [SaveManager readToDataWithFilePath:path];
    NSMutableDictionary *settingDic = [NSKeyedUnarchiver unarchiveObjectWithData:cacheData];
    
    NSNumber *userId = [NSNumber numberWithInteger:[GlobalUserInfo myselfInfo].userID];
    NSMutableDictionary *userDic = [settingDic objectForKey:userId];
    
    return userDic;
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
