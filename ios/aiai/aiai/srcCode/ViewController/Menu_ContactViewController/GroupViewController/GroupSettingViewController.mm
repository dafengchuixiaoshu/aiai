//
//  GroupSettingViewController.m
//  aiai
//
//  Created by wangnan on 15/3/31.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "GroupSettingViewController.h"
#import "InitiateGroupChatViewController.h"

#import "AddMemberView.h"
#import "GroupSettingCell.h"
#import "GroupHeadView.h"

#import "IMPackageEngine.h"
#import "IMPackageData.h"

@interface GroupSettingViewController ()<UITableViewDataSource, UITableViewDelegate, InitiateGroupChatDelegate>

@property(nonatomic, strong)UITableView *groupTable;

@property(nonatomic, strong)NSString *groupID;
@property(nonatomic, strong)IMPackageGroupData *groupData;
@property(nonatomic, strong)IMPackageSessionData *sessionData;

@end

@implementation GroupSettingViewController
@synthesize groupID;
@synthesize quitBlock,groupBlock;
@synthesize groupData, sessionData;

-(id)initWithGroupID:(NSString*)groupIDentifi{
    if (self = [super init]) {
        groupID = groupIDentifi;
    }
    return self;
}

-(void)viewWillAppear:(BOOL)animated{
    
    [super viewWillAppear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] addDelegate:(id)self];
}

-(void)viewWillDisappear:(BOOL)animated{
    
    [super viewWillDisappear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] removeDelegate:(id)self];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.centerBtnImageName = @"群组信息";
    
    groupData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_getGroupInfoWithGroupId:groupID];
    sessionData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSession:groupID];
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height = [UIScreen mainScreen].bounds.size.height - rect.origin.y;
    self.groupTable = [[UITableView alloc] initWithFrame:rect];
    self.groupTable.backgroundColor = EveryView_BackGroundColor;
    self.groupTable.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.groupTable.dataSource = self;
    self.groupTable.delegate = self;
    [self.groupTable registerClass:[GroupSettingCell class] forCellReuseIdentifier:@"GroupSettingCell"];
    
    self.groupTable.tableHeaderView = [self creatHeaderView];
    self.groupTable.tableFooterView = [self creatFooterView];
    
    [self.view addSubview:self.groupTable];
}

#pragma mark-------- 创建footer 和 header 视图
-(UIView*)creatFooterView{
    
    UIView *footerView = [[UIView alloc] init];
    footerView.frame = CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, 150);
    
    UIButton *exitButton = [[UIButton alloc] initWithFrame:CGRectMake(0, footerView.frame.size.height - 44, [UIScreen mainScreen].bounds.size.width, 44)];
    exitButton.backgroundColor = [UIColor clearColor];
    exitButton.titleLabel.font = [UIFont systemFontOfSize:16];
    [exitButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [exitButton setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
    [exitButton setTitle:@"删除并退出群聊" forState:UIControlStateNormal];
    [exitButton setTitle:@"删除并退出群聊" forState:UIControlStateHighlighted];
    [exitButton addTarget:self action:@selector(quitGroup) forControlEvents:UIControlEventTouchUpInside];
    [footerView addSubview:exitButton];
    
    return footerView;
}

-(UIView*)creatHeaderView{
    
    __block NSMutableArray *membersArray = [[NSMutableArray alloc] init];
    [membersArray addObjectsFromArray:[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetGroupMemberListWithGroupID:groupID]];
    
    __weak GroupSettingViewController *weakSelf = self;
    AddMemberView *addMemberView = [[AddMemberView alloc] initWithOwenerID:groupData.ownerID members:membersArray addAction:^{
        
        if ([groupData.groupMemberCount intValue] >= [groupData.groupMemberMax intValue]) {

            return ;
        }
        
        membersArray = [[NSMutableArray alloc] initWithArray:[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetGroupMemberListWithGroupID:groupID]];
        
        InitiateGroupChatViewController *con = [[InitiateGroupChatViewController alloc] initWithGroupID:weakSelf.groupID existingMembers:membersArray];
        con.delegate = self;
        [weakSelf.navigationController pushViewController:con animated:YES];
        
    } avatarAction:^(NSDictionary *userInfo) {
        //点击头像
    } cutDownAction:^{
        //删除好友
    }];
    
    return addMemberView;
}

-(void)quitGroup{
    
    NSArray *memberList = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetGroupMemberListWithGroupID:groupID];
    
    NSMutableArray *faceKeyArray = [[NSMutableArray alloc] init];
    for (int i = 0; i<[memberList count]; ++i) {
        IMPackageGroupMemberData *data = [memberList objectAtIndex:i];
        if (![data.memberUserName isEqualToString:[GlobalUserInfo myselfInfo].userName]) {
            [faceKeyArray addObject:data.portraitKey? :@""];
        }
    }
    
    //如果没人了 那么直接退出就可以了
    if ([faceKeyArray count] == 0) {
        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_leavingGroupWithUserID:[IMPackageEngine sharedInstanceIMPackageEngine].userName andGroupID:self.groupID groupPortraitKey:nil];
        return;
    }
    
    __block GroupHeadView *headerView = [GroupHeadView groupHeadViewWithFrame:CGRectMake(100, 100, 200, 200) images:faceKeyArray prepare:^{
        
        [WaitingAlertView showWaitingViewAtView:_groupTable];
        _groupTable.scrollEnabled = NO;
        
    } complete:^(NSString *imgKey) {
        
        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_leavingGroupWithUserID:[IMPackageEngine sharedInstanceIMPackageEngine].userName andGroupID:self.groupID groupPortraitKey:imgKey];
        
        headerView = nil;
    }];
}

#pragma mark -- 邀请人成功回调
-(void)InitiateGroupChat_NewGroupID:(NSString*)groupID isCreatGroup:(BOOL)isCreat{
    
    
    NSMutableArray *membersArray = [[NSMutableArray alloc] init];
    [membersArray addObjectsFromArray:[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetGroupMemberListWithGroupID:self.groupID]];
    
    AddMemberView *addMemberView = (AddMemberView *)_groupTable.tableHeaderView;
    [addMemberView refreshElement:membersArray];
    
    _groupTable.tableHeaderView = addMemberView;
    
    [_groupTable reloadData];
    
    if (_joinBlock) {
        _joinBlock();
    }
}

#pragma mark tableView 的 代理

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return 3;
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    static NSString *indentifier = @"GroupSettingCell";
    GroupSettingCell *cell = (GroupSettingCell*)[tableView dequeueReusableCellWithIdentifier:indentifier forIndexPath:indexPath];
    
    switch (indexPath.row) {
        case 0:
        {
            [cell setTitleText:@"群组名称" subText:groupData.groupName needSwitch:NO andSwitchOn:NO];
            cell.changeNameBlock = ^(NSString *newName){
                
                [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_ModifyGroupNameWithGroupID:groupID WithModifyerID:[IMPackageEngine sharedInstanceIMPackageEngine].userName AndGroupName:newName];
                
                if (groupBlock) {
                    groupBlock(newName);
                }
                
            };
        }
            break;
        case 1:
        {
            NSString *cardName = groupData.groupCardName;
            if (!cardName || [cardName length] == 0) {
                
                cardName = [GlobalUserInfo myselfInfo].userNickName;
            }
            cell.userInteractionEnabled = NO;
            [cell setTitleText:@"我的群昵称" subText:cardName needSwitch:NO andSwitchOn:NO];
            cell.changeNameBlock = ^(NSString *newName){
                
                [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_ModifyMemberCardNameWithGroupID:groupID WithModifyerID:[IMPackageEngine sharedInstanceIMPackageEngine].userName AndCardName:newName];
                
            };
        }
            break;
        case 2:
        {
            [cell setTitleText:@"新消息提示" subText:nil needSwitch:YES andSwitchOn:sessionData.eRemind];
            cell.msgActionBlock = ^(BOOL on){
                
                [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_SetSessionNewMsgRemindWithSessionID:groupID AndSessionOptionType:(IM_SessionReceiveMessage)on];
                sessionData.eRemind = on;
                
            };
            
        }
            break;
        default:
            break;
    }
    return cell;
}

#pragma mark InitiateGroupChat Delegate

- (void)IMPackageGroupDetaileType:(int)detaileType groupData:(IMPackageGroupData*)groupData {
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
        [WaitingAlertView hideWaitingView];
        _groupTable.scrollEnabled = YES;
        
        if (detaileType == kIM_MsgType_Group_notifyQuitGroup_Key) {
    //        [self.navigationController popViewControllerAnimated:YES];
            
        }
        if (kIM_MsgType_Group_quitSuccess_Key == detaileType) {
            
                if (quitBlock) {
                    quitBlock();
                    [self.navigationController popViewControllerAnimated:YES];
                }
            
        }
    });
}

-(void)dealloc{
    NSLog(@"controller dealloc");
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
