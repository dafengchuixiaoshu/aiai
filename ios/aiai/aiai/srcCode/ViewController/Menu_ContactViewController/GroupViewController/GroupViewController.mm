//
//  GroupViewController.m
//  aiai
//
//  Created by zhangyang on 15/2/7.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "GroupViewController.h"
#import "GroupSettingViewController.h"
#import "InitiateGroupChatViewController.h"

#import "GroupListCell.h"

#import "UIButton+FillColor.h"

@interface GroupViewController ()<UITableViewDataSource,UITableViewDelegate,InitiateGroupChatDelegate>
@property(nonatomic,strong)NSMutableArray* groupArray;
@property(nonatomic,strong)UITableView* groupTableView;

@end

@implementation GroupViewController

-(id)init{
    if (self = [super init]) {
        
    }
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    self.centerBtnImageName = @"群聊";
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    self.groupTableView = [[UITableView alloc] initWithFrame:rect];
    self.groupTableView.backgroundColor = EveryView_BackGroundColor;
    [self.groupTableView setSeparatorStyle:UITableViewCellSeparatorStyleNone];
//    self.groupTableView.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    self.groupTableView.delegate = self;
    self.groupTableView.dataSource = self;
    [self.groupTableView registerClass:[GroupListCell class] forCellReuseIdentifier:@"GroupListCell"];
    
    UIView *headerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, GROUPLISTCELLHEIGHT)];
    UIImageView *iconView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:DefalutHeadImage]];
    iconView.frame = CGRectMake(13, 13, 40, 40);
    [headerView addSubview:iconView];
    
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(iconView.frame.origin.x+iconView.frame.size.width+13, 0, 150, GROUPLISTCELLHEIGHT)];
    titleLabel.text = @"新建群聊";
    titleLabel.backgroundColor = [UIColor clearColor];
    titleLabel.font = [UIFont systemFontOfSize:16];
    titleLabel.textColor = [UIColor whiteColor];
    [headerView addSubview:titleLabel];
    
    UIImageView *lineView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
    lineView.frame = CGRectMake(13, headerView.frame.size.height - 1, CGRectGetWidth([UIScreen mainScreen].bounds) - 13*2, 1);
    lineView.alpha = 0.5;
    [headerView addSubview:lineView];
    
    //添加button
    UIButton* btn = [UIButton buttonWithType:UIButtonTypeCustom];
    btn.frame = headerView.frame;
    [btn setBackgroundColor:Every_YellowColor forState:UIControlStateHighlighted];
    [btn addTarget:self action:@selector(creatGroup) forControlEvents:UIControlEventTouchUpInside];
    [headerView addSubview:btn];
    [headerView sendSubviewToBack:btn];
    
    self.groupTableView.tableHeaderView = headerView;
    
    [self.view addSubview:self.groupTableView];
    
    //起线程请求联系人
    [WaitingAlertView showWaitingViewAtView:self.groupTableView];
    dispatch_queue_t queue = dispatch_queue_create("loadGroup", NULL);
    dispatch_async(queue, ^{
        self.groupArray = [[NSMutableArray alloc] initWithArray:[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_getGroupList]];
        dispatch_async(dispatch_get_main_queue(), ^{
            [WaitingAlertView hideWaitingView];
            [self.groupTableView reloadData];
        });
    });
}

-(void)creatGroup{
    InitiateGroupChatViewController *con = [[InitiateGroupChatViewController alloc] initWithGroupID:nil existingMembers:nil];
    con.delegate = self;
    con.groupDel = self.groupDel;
    [self.navigationController pushViewController:con animated:YES];
}

#pragma mark TableViewDelegate
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [self.groupArray count];
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return GROUPLISTCELLHEIGHT;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString* cellIdentifier = @"GroupListCell";
    GroupListCell* cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier forIndexPath:indexPath];
    
    IMPackageGroupData *data = [self.groupArray objectAtIndex:indexPath.row];
    [cell setGroupCellWithGroupData:data];
    
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    
    IMPackageGroupData *groupData = [self.groupArray objectAtIndex:indexPath.row];
    if ([self.groupDel respondsToSelector:@selector(selectGroupEntity:)]) {
        [self.groupDel selectGroupEntity:groupData];
    }else{
        GroupSettingViewController *con = [[GroupSettingViewController alloc] initWithGroupID:groupData.groupID];
        con.quitBlock = ^(){
            [self.groupArray removeObjectAtIndex:indexPath.row];
            [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationNone];
        };
        
        con.groupBlock = ^(NSString *groupName){
            groupData.groupName = groupName;
            [tableView reloadRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationNone];
        };
        
        __weak GroupViewController *weakSelf = self;
        con.joinBlock = ^(){
            [weakSelf.groupArray removeAllObjects];
            weakSelf.groupArray = (NSMutableArray*)[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_getGroupList];
            [weakSelf.groupTableView reloadData];
        };
        [self.navigationController pushViewController:con animated:YES];
    }
}

#pragma mark creatGroupDelegate
-(void)InitiateGroupChat_NewGroupID:(NSString*)groupID isCreatGroup:(BOOL)isCreat{
    
    [self.groupArray removeAllObjects];
    self.groupArray = (NSMutableArray*)[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_getGroupList];
    [self.groupTableView reloadData];
}

@end
