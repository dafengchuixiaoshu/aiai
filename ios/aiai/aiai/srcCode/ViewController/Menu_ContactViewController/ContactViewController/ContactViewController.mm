//
//  ContactViewController.m
//  aiai
//
//  Created by zhangyang on 15/1/29.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ContactViewController.h"
#import "AddContactViewController.h"
#import "StrangerViewController.h"
#import "GroupViewController.h"
#import "ContactDetailViewController.h"

#import "IMPackageEngine.h"

#import "ContactTableViewCell.h"
#import "ContactTableHeaderView.h"
#import "GroupListCell.h"

@interface ContactViewController ()<UITableViewDataSource,UITableViewDelegate,IMPackageEngineDelegate>
@property(nonatomic,strong)NSMutableDictionary* contactDic;
@property(nonatomic,strong)UITableView* contactTabelView;

@property (nonatomic,strong) ContactTableViewCell *currentCell;
@end

@implementation ContactViewController

-(void)viewDidAppear:(BOOL)animated{
    
    [super viewDidAppear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] addDelegate:self];
    
    if (self.contactTabelView) {
        ContactTableHeaderView *headview = (ContactTableHeaderView*)self.contactTabelView.tableHeaderView;
        [headview setMsgCountLabelText:[NSString stringWithFormat:@"%d",[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetNewFansCount]]];
    }
}

-(void)viewDidDisappear:(BOOL)animated{
    
    [super viewDidDisappear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] removeDelegate:self];
}

-(void)IMPackageLoginStatus:(int)detaileType{
    if (detaileType == kIM_MsgType_Login_sucess_Key){
        NSArray* buddylist = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetBuddyList];
        self.contactDic = [[NSMutableDictionary alloc] initWithDictionary:[IMPackageBuddyData firstLetterContactDictionaryWithBuddyList:buddylist]];
        [self.contactTabelView reloadData];
    }else if(detaileType == kIM_MsgType_Login_faild_Key){
        
    }else if(detaileType == kIM_MsgType_Login_noExist_Key){
        
    }
}

-(void)leftItemClicked:(id)sender{
    if (self.leftBlock) {
        self.leftBlock();
    }else{
        [super leftItemClicked:sender];
    }
}

-(void)viewDidLoad{
    [super viewDidLoad];

    self.centerBtnImageName = @"联系人";
    
    __weak ContactViewController* weakself = self;
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    self.contactTabelView = [[UITableView alloc] initWithFrame:rect];
    [self.contactTabelView setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    self.contactTabelView.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    self.contactTabelView.sectionIndexBackgroundColor = [UIColor clearColor];
    self.contactTabelView.sectionIndexTrackingBackgroundColor = [UIColor clearColor];
    self.contactTabelView.sectionIndexColor = [UIColor whiteColor];
    self.contactTabelView.delegate = self;
    self.contactTabelView.dataSource = self;
    [self.contactTabelView registerClass:[ContactTableViewCell class] forCellReuseIdentifier:@"ContactTableViewCell"];
    ContactTableHeaderView* headview = [[ContactTableHeaderView alloc] initWithFrame:CGRectMake(0, 44, self.view.frame.size.width, 136)];
    headview.addContactBlock = ^{
        AddContactViewController* con = [[AddContactViewController alloc] init];
        [weakself.navigationController pushViewController:con animated:YES];
    };
    headview.strangerBlock = ^{

//        StrangerViewController* con = [[StrangerViewController alloc] init];
//        [weakself.navigationController pushViewController:con animated:YES];
    };
    headview.groupBlock = ^{
        GroupViewController* con = [[GroupViewController alloc] init];
        con.groupDel = self.groupDel;
        [weakself.navigationController pushViewController:con animated:YES];
    };
    self.contactTabelView.tableHeaderView = headview;

    [self.view addSubview:self.contactTabelView];

    //起线程请求联系人
    [WaitingAlertView showWaitingViewAtView:self.contactTabelView];
    dispatch_queue_t queue = dispatch_queue_create("loadBuddy", NULL);
    dispatch_async(queue, ^{
        NSArray* buddylist = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetBuddyList];
        self.contactDic = [[NSMutableDictionary alloc] initWithDictionary:[IMPackageBuddyData firstLetterContactDictionaryWithBuddyList:buddylist]];
        dispatch_async(dispatch_get_main_queue(), ^{
            [WaitingAlertView hideWaitingView];
            [self.contactTabelView reloadData];
        });
    });
}


#pragma mark --- tableView代理

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    NSArray* allkeys = [[self.contactDic allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
    NSString* key = [allkeys objectAtIndex:section];
    
    return [[self.contactDic objectForKey:key] count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    ContactTableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"ContactTableViewCell" forIndexPath:indexPath];
    
    NSArray* allkeys = [[self.contactDic allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
    NSString* key = [allkeys objectAtIndex:indexPath.section];
    NSMutableArray* oneFirstb = (NSMutableArray*)[self.contactDic objectForKey:key];
    IMPackageBuddyData* entity = [oneFirstb objectAtIndex:indexPath.row];
    
    BOOL isShow = indexPath.row != [oneFirstb count]-1 ? YES : NO;
    [cell setCellWithData:entity AndIsShowCutline:isShow];
    cell.deleteBlock = ^(){
        
        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_DeleteBuddyWihUserID:entity.buddyUserName];
        [oneFirstb removeObject:entity];
        if ([oneFirstb count] == 0) {
            [self.contactDic removeObjectForKey:key];
            [tableView reloadData];
//            [tableView beginUpdates];
//            [tableView deleteSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationFade];
//            [tableView endUpdates];
        }else{
            [tableView reloadData];
//            [tableView beginUpdates];
//            [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
//            [tableView endUpdates];
        }
    };
    cell.swipBlock = ^(){
        NSArray *cellarray = [self.contactTabelView visibleCells];
        [cellarray makeObjectsPerformSelector:@selector(resetCell)];
    };
    return cell;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return ContactTableViewCell_Height;
}

-(UIView*)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    NSArray* allkeys = [[self.contactDic allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
    
    UIView *headerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, 26)];
    headerView.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.05];
    
    UILabel* keyLabel = [[UILabel alloc] initWithFrame:CGRectMake(13, 0, self.view.frame.size.width, 26)];
    keyLabel.backgroundColor = [UIColor clearColor];
    keyLabel.textColor = [UIColor whiteColor];
    keyLabel.font = [UIFont systemFontOfSize:16];
    keyLabel.alpha = 0.5;
    keyLabel.text = [NSString stringWithFormat:@"%@",[allkeys objectAtIndex:section]];
    
    [headerView addSubview:keyLabel];
    return headerView;
}

-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    
    return 26;
}

-(NSArray*)sectionIndexTitlesForTableView:(UITableView *)tableView{
    return [[self.contactDic allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return [[self.contactDic allKeys] count];
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    
    NSArray *cellarray = [self.contactTabelView visibleCells];
    [cellarray makeObjectsPerformSelector:@selector(resetCell)];
    
    NSArray* allkeys = [[self.contactDic allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
    NSString* key = [allkeys objectAtIndex:indexPath.section];
    NSMutableArray* oneFirstb = (NSMutableArray*)[self.contactDic objectForKey:key];
    IMPackageBuddyData* entity = [oneFirstb objectAtIndex:indexPath.row];

    if (self.delegate && [self.delegate respondsToSelector:@selector(selectBuddyEntity:)]) {
        [self.delegate selectBuddyEntity:entity];
    }else{
        ContactDetailViewController *con = [[ContactDetailViewController alloc] initWithBuddyUserName:entity.buddyUserName];
        con.deleteBlock = ^(NSString *buddyUserName){
            [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_DeleteBuddyWihUserID:entity.buddyUserName];
            [oneFirstb removeObject:entity];
            if ([oneFirstb count] == 0) {
                
                [self.contactDic removeObjectForKey:key];
                [tableView reloadData];
//                [tableView beginUpdates];
//                [tableView deleteSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationFade];
//                [tableView endUpdates];
            }else{
                [tableView reloadData];
//                [tableView beginUpdates];
//                [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
//                [tableView endUpdates];
            }
        };
        con.modifyBlock = ^(NSString *modifyName){
            
            ContactTableViewCell *cell = (ContactTableViewCell *)[tableView cellForRowAtIndexPath:indexPath];
            [cell setNameLabelText:modifyName];
        };
        [self.navigationController pushViewController:con animated:YES];
    }
}

#pragma mark scrollerViewDelegate
- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView{
    
    NSArray *cellarray = [self.contactTabelView visibleCells];
    [cellarray makeObjectsPerformSelector:@selector(resetCell)];
    
}
@end
