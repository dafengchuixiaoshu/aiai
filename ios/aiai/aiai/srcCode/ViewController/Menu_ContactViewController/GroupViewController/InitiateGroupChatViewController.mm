//
//  CreateGroupChatViewController.m
//  F2CPlatform
//
//  Created by Circle Liu on 14-7-20.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "InitiateGroupChatViewController.h"
#import "GroupViewController.h"

#import "CreateGroupChatCell.h"
#import "CreateGroupMembersView.h"
#import "GroupHeadView.h"

#import "IMPackageEngine.h"
#import "IMPackageData.h"

typedef enum TableViewShowType {
	TableViewShowNormalResult = 0,
    TableViewShowSearchedResult = 1
} TableViewShowType;

@interface InitiateGroupChatViewController () <UITableViewDataSource, UITableViewDelegate, UISearchDisplayDelegate>


@property(nonatomic, strong)UITableView             *tableView;
@property(nonatomic, strong)UISearchBar             *searchBar;
@property(nonatomic, strong)CreateGroupMembersView  *membersView;
@property(nonatomic, strong)NSMutableArray          *selectedUserIDArray;
@property(nonatomic, strong)NSMutableArray          *searchedDataArray;
@property(nonatomic, strong)NSMutableDictionary     *contacterInfo;
@property(nonatomic, assign)TableViewShowType       tableViewShowType;
@property(nonatomic, copy)NSString                  *groupID;
@property(nonatomic, assign)IM_MESSAGE_TYPE         messageType;
@property(nonatomic, strong)NSArray                 *existingMembers;

@end

@implementation InitiateGroupChatViewController
@synthesize tableView = _tableView;
@synthesize searchBar = _searchBar;
@synthesize membersView = _membersView;
@synthesize groupID = _groupID;
@synthesize existingMembers = _existingMembers;
@synthesize selectedUserIDArray,searchedDataArray,contacterInfo,tableViewShowType;

@synthesize delegate;

- (id)initWithGroupID:(NSString*)groupID existingMembers:(NSArray*)existingMembers
{
    self = [super init];
    if (self) {
        // Custom initialization
        selectedUserIDArray = [[NSMutableArray alloc] init];
        searchedDataArray = [[NSMutableArray alloc] init];
        contacterInfo = [[NSMutableDictionary alloc] init];
        
        tableViewShowType = TableViewShowNormalResult;
        
        _groupID = groupID;
        _existingMembers = existingMembers;
        
        //默认为普通的邀请成员模式
        _conType = NORMAL;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.centerBtnImageName = @"联系人";
    
    CGFloat boomViewHeight = 84;
    CGRect rect = [[UIScreen mainScreen] bounds];
    rect.origin.y = 64;
    rect.size.height = rect.size.height-(boomViewHeight + rect.origin.y);
    
    _tableView = [[UITableView alloc] initWithFrame:rect];
    _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    _tableView.dataSource = self;
    _tableView.delegate = self;
    _tableView.sectionIndexBackgroundColor = [UIColor clearColor];
    _tableView.sectionIndexTrackingBackgroundColor = [UIColor clearColor];
    _tableView.sectionIndexColor = [UIColor whiteColor];
    _tableView.backgroundColor = [UIColor clearColor];
    [self.view addSubview:_tableView];

    _membersView = [[CreateGroupMembersView alloc] initWithFrame:CGRectMake(0, _tableView.frame.origin.y + _tableView.frame.size.height, [UIScreen mainScreen].bounds.size.width, boomViewHeight) comfirm:^{
        
        if ([selectedUserIDArray  count] == 0) {
//            [self showMessage:@"请选择好友" andStyle:TxtCenter andHideTime:2 andLookMsg:nil];
            [_membersView createActionAble:YES];
            return ;
        }
        
        //如果是at 选择后直接退出
        if (AT == _conType) {
            if ([delegate respondsToSelector:@selector(ATNewPeople:)]) {
                [delegate ATNewPeople:selectedUserIDArray];
            }
            return;
        }
        
        if ([selectedUserIDArray count] > 0 ) {

            NSMutableArray* faceKeyArray = [[NSMutableArray alloc] init];
            if (_groupID == nil) {
                
                NSString *userName = [IMPackageEngine sharedInstanceIMPackageEngine].userName;
                IMPackageBuddyData *data = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetContactWithUserId:userName isFromServer:NO];

                if (data.buddyUserName != nil && ![data.buddyUserName isEqual:[NSNull null]] && [data.buddyUserName length] != 0) {
                    [selectedUserIDArray addObject:data];
                }
                
                for (int i = 0; i<MIN([selectedUserIDArray count], 8); i++) {
                    IMPackageBuddyData* model = [selectedUserIDArray objectAtIndex:i];
                    if (![model.buddyUserName isEmpty]) {
                        [faceKeyArray addObject:[AppUtils getImageNewUrlWithSrcImageUrl:model.buddyPortraitKey width:0 height:0]];
                    }
                }
                
                __block GroupHeadView *headerView = [GroupHeadView groupHeadViewWithFrame:CGRectMake(100, 100, 200, 200) images:faceKeyArray prepare:^{
                    
                    [WaitingAlertView showWaitingViewWithFrame:_tableView.frame];
                    _tableView.scrollEnabled = NO;
                    
                } complete:^(NSString *imgKey) {
                    
                    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_AddNewGroupWithAddUserID:userName groupPortraitKey:imgKey addContactList:selectedUserIDArray];
                    headerView = nil;
                }];
                
            }
            else {
                
                //如果原群组头像 大于最大数 就不用重新拼头像了 如果原头像不够最大数 还要重新拼头像
                if ([_existingMembers count] <= [GroupHeadView MaxCount]) {
                    
                    for (int i = 0; i<[_existingMembers count]; i++) {
                        IMPackageGroupMemberData* model = [_existingMembers objectAtIndex:i];
                        [faceKeyArray addObject:model.portraitKey];
                    }
                    for (int i = 0; i<MIN([GroupHeadView MaxCount]-[_existingMembers count], [selectedUserIDArray count]); i++) {
                        IMPackageBuddyData* model = [selectedUserIDArray objectAtIndex:i];
                        [faceKeyArray addObject:[AppUtils getImageNewUrlWithSrcImageUrl:model.buddyPortraitKey width:0 height:0]];
                    }
                    
                    __block GroupHeadView *headerView = [GroupHeadView groupHeadViewWithFrame:CGRectMake(100, 100, 200, 200) images:faceKeyArray prepare:^{
                        
                        [WaitingAlertView showWaitingViewWithFrame:_tableView.frame];
                        _tableView.scrollEnabled = NO;
                        
                    } complete:^(NSString *imgKey) {
                        
                        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_InviteGroupWithGroupID:_groupID WithInviterID:[IMPackageEngine sharedInstanceIMPackageEngine].userName groupPortraitKey:imgKey andAddContactList:selectedUserIDArray];
                        
                        headerView = nil;
                    }];
                    
                }else{
                    [WaitingAlertView showWaitingViewWithFrame:_tableView.frame];
                    _tableView.scrollEnabled = NO;
                    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_InviteGroupWithGroupID:_groupID WithInviterID:[IMPackageEngine sharedInstanceIMPackageEngine].userName groupPortraitKey:nil andAddContactList:selectedUserIDArray];
                }
            }
        }
    } deleteAction:^(NSString *memberName) {
        for (int i = 0;i<[selectedUserIDArray count];i++) {
            IMPackageBuddyData* data = [selectedUserIDArray objectAtIndex:i];
            if ([data.buddyUserName isEqualToString:memberName]) {
                [selectedUserIDArray removeObjectAtIndex:i];
                [_tableView reloadData];
                break;
            }
        }
    }];
    [self.view addSubview:_membersView];
    
    UIView *headerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, 10+44+20)];//上间距+搜索框+下间距
    headerView.backgroundColor = EveryView_BackGroundColor;
    
    CGFloat origin_x = 13;
    _searchBar = [[UISearchBar alloc] initWithFrame:CGRectMake(origin_x, 10.0f, [UIScreen mainScreen].bounds.size.width-origin_x*2, 44.0f)];
    UITextField *searchField = [_searchBar valueForKey:@"_searchField"];
    searchField.textColor = [UIColor whiteColor];
    searchField.backgroundColor = [UIColor clearColor];
    _searchBar.delegate = (id)self;
    _searchBar.placeholder = [NSString stringWithCString:"请输入需要查找的好友昵称" encoding: NSUTF8StringEncoding];
    _searchBar.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.05];
    _searchBar.backgroundImage = [AppUtils imageWithColor:[UIColor clearColor] rect:_searchBar.frame];
    [headerView addSubview:_searchBar];
    _tableView.tableHeaderView = headerView;
    
    [WaitingAlertView showWaitingViewWithFrame:_tableView.frame];
    _tableView.scrollEnabled = NO;
    dispatch_queue_t queue = dispatch_queue_create("getBuddy", NULL);
    dispatch_async(queue, ^{
        NSArray* buddylist = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetBuddyList];
        contacterInfo = [[NSMutableDictionary alloc] initWithDictionary:[IMPackageBuddyData firstLetterContactDictionaryWithBuddyList:buddylist]];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [WaitingAlertView hideWaitingView];
            _tableView.scrollEnabled = YES;
            [_tableView reloadData];
        });
    });
}


- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
    [[IMPackageEngine sharedInstanceIMPackageEngine] addDelegate:(id)self];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillHideNotification object:nil];
    [[IMPackageEngine sharedInstanceIMPackageEngine] removeDelegate:(id)self];
}

-(void)leftItemClicked:(id)sender{
    
    [WaitingAlertView hideWaitingView];
    //如果是at界面 需要dismis退出
    if (AT == _conType) {
        [self dismissViewControllerAnimated:YES completion:nil];
    }
    [super leftItemClicked:sender];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark Private method

- (void)searchByKeyWord:(NSString*)keyWord {
    [searchedDataArray removeAllObjects];

    NSEnumerator *enumerator = [contacterInfo objectEnumerator];
    
    for (NSArray *array in enumerator) {
        for (IMPackageBuddyData *model in array) {
            NSRange range = [model.buddyNickName rangeOfString:keyWord];
            if (NSNotFound != range.location) {
                [searchedDataArray addObject:model];
            }
        }
    }
    
    [_tableView reloadData];
}

#pragma mark NSNotificationCenter method

- (void)keyboardWillShow:(NSNotification *)notification {
    //获取键盘的高度
    NSDictionary *userInfo = [notification userInfo];
    NSValue *aValue = [userInfo objectForKey:UIKeyboardFrameEndUserInfoKey];
    CGRect keyboardRect = [aValue CGRectValue];
    
    CGFloat scrollSpeed = [[userInfo objectForKey:UIKeyboardAnimationDurationUserInfoKey] doubleValue];
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    rect.origin.y = 64;
    rect.size.height =rect.size.height - (_membersView.frame.size.height + rect.origin.y);

    [UIView animateWithDuration:scrollSpeed animations:^{
        _tableView.frame = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height - keyboardRect.size.height);
        
        CGRect rect1 = _membersView.frame;
        _membersView.frame = CGRectMake(rect1.origin.x, _tableView.frame.origin.y + _tableView.frame.size.height, rect1.size.width, rect1.size.height);
    }];
}

- (void)keyboardWillHide:(NSNotification *)notification {
    NSDictionary *userInfo = [notification userInfo];
    NSValue *aValue = [userInfo objectForKey:UIKeyboardFrameEndUserInfoKey];
    CGRect keyboardRect = [aValue CGRectValue];
    
    CGFloat scrollSpeed = [[userInfo objectForKey:UIKeyboardAnimationDurationUserInfoKey] doubleValue];
    
    [UIView animateWithDuration:scrollSpeed animations:^{
        CGRect rect = _tableView.frame;
        _tableView.frame = CGRectMake(rect.origin.x, rect.origin.y, self.view.frame.size.width, rect.size.height + keyboardRect.size.height);
        
        rect = _membersView.frame;
        _membersView.frame = CGRectMake(rect.origin.x, _tableView.frame.origin.y + _tableView.frame.size.height, rect.size.width, rect.size.height);
    }];
}

#pragma mark UISearchBar Delegate

- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
    [self searchByKeyWord:searchBar.text];
    
    [_searchBar resignFirstResponder];
}

- (void)searchBarTextDidBeginEditing:(UISearchBar *)searchBar {
    [searchedDataArray removeAllObjects];
    
    tableViewShowType = TableViewShowSearchedResult;
    [_tableView reloadData];
}

- (void)searchBarCancelButtonClicked:(UISearchBar *)searchBar {
    [searchBar resignFirstResponder];
    
    tableViewShowType = TableViewShowNormalResult;
    [_tableView reloadData];
    
    _searchBar.text = @"";
}

- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText {
    [self searchByKeyWord:searchText];
}

#pragma mark UITableView DataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return tableViewShowType == TableViewShowNormalResult ? [contacterInfo count] : 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    NSArray *keyWords = [[contacterInfo allKeys] sortedArrayUsingSelector:@selector(compare:)];
    NSArray *valueArray = [contacterInfo objectForKey:[keyWords objectAtIndex:section]];

    return tableViewShowType == TableViewShowNormalResult ? [valueArray count] : [searchedDataArray count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *identifier = @"CreateGroupChatCell";
    CreateGroupChatCell *cell = [tableView dequeueReusableCellWithIdentifier:identifier];
    if (!cell) {
        cell = [[CreateGroupChatCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifier];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
    }
    NSArray *keyWords = [[contacterInfo allKeys] sortedArrayUsingSelector:@selector(compare:)];
    NSString *key = [keyWords objectAtIndex:indexPath.section];
    NSArray *valueArray = [contacterInfo objectForKey:key];
    
    IMPackageBuddyData *model = tableViewShowType == TableViewShowNormalResult ? [valueArray objectAtIndex:indexPath.row] : [searchedDataArray objectAtIndex:indexPath.row];
    BOOL show = indexPath.row != ([valueArray count]-1) ? YES:NO;
    [cell fillData:model andIsShowTheCutLine:show];
    
    if ([self isExisted:[cell userData].buddyUserName]) {
        [cell existingMembers:YES];
    }
    else {
        [self isSelected:[cell userData].buddyUserName] ? [cell changeSelectStatus:YES] : [cell changeSelectStatus:NO];
    }
    
    return cell;
}

- (BOOL)isSelected:(NSString*)memberID {
    for (IMPackageBuddyData *data in selectedUserIDArray) {
        if ([data.buddyUserName isEqualToString:memberID]) {
            return YES;
        }
    }
    return NO;
}

- (BOOL)isExisted:(NSString*)memberID {
    for (IMPackageGroupMemberData *data in _existingMembers) {
        if ([data.memberUserName isEqualToString:memberID]) {
            return YES;
        }
    }
    return NO;
}

#pragma mark UITableView Delegate

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return INITIATE_CELLHEIGHT;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    return tableViewShowType == TableViewShowNormalResult ? INITIATE_HEADERHEIGHT : 0;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    if (tableViewShowType == TableViewShowSearchedResult) return nil;
    
    UIView *headerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, INITIATE_HEADERHEIGHT)];
    headerView.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.05];
    
    UILabel* titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0 , [UIScreen mainScreen].bounds.size.width, INITIATE_HEADERHEIGHT)];
    titleLabel.font = [UIFont boldSystemFontOfSize:16];
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.backgroundColor = [UIColor clearColor];
    titleLabel.alpha = 0.5;
    NSString* sectionText = [[[contacterInfo allKeys] sortedArrayUsingSelector:@selector(compare:)] objectAtIndex:section];
    titleLabel.text = [NSString stringWithFormat:@"  %@",sectionText];
    [headerView addSubview:titleLabel];
    
    return headerView;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    CreateGroupChatCell *cell = (CreateGroupChatCell*)[tableView cellForRowAtIndexPath:indexPath];
    
    IMPackageBuddyData *data = [cell userData];
    
    if ([self isExisted:data.buddyUserName]) {
        return ;
    }
 
    if ([self isSelected:data.buddyUserName]) {
        [self removeSelectedMember:data.buddyUserName];
        
        [_membersView deletePerson:data.buddyUserName];
        
        [cell changeSelectStatus:NO];
    }
    else {
        [selectedUserIDArray addObject:data];
        
        [_membersView insertPerson:data.buddyUserName userURL:[AppUtils getImageNewUrlWithSrcImageUrl:data.buddyPortraitKey width:0 height:0]];
        
        [cell changeSelectStatus:YES];
    }
}

- (void)removeSelectedMember:(NSString*)memberID {
    for (IMPackageBuddyData *data in selectedUserIDArray) {
        if ([data.buddyUserName isEqualToString:memberID]) {
            [selectedUserIDArray removeObject:data];
            break ;
        }
    }
}

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView {
    if (TableViewShowNormalResult == tableViewShowType) {
        return [[contacterInfo allKeys] sortedArrayUsingSelector:@selector(compare:)];
        
    }
    else {
        return nil;
    }
}

#pragma mark UIScrollView Delegate

- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView {
    if ([_searchBar isFirstResponder]) {
        [_searchBar resignFirstResponder];
        if (0 == _searchBar.text.length) {
            tableViewShowType = TableViewShowNormalResult;
            [_tableView reloadData];
        }
    }
}

#pragma mark IMPackageEngine Delegate

//创建群组失败
- (void)IMPackageNotifyGroupFaildWithtype:(IM_GroupErrCode)errType AndGroupMemCount:(int)memCount{
    dispatch_async(dispatch_get_main_queue(), ^{
        
        [WaitingAlertView hideWaitingView];
        _tableView.scrollEnabled = YES;
        
        [_membersView createActionAble:YES];
        if (errType == IM_GROUP_CREAT_ERRCODE) {
            NSString *errStr = [NSString stringWithFormat:@"创建群聊失败，邀请人数超过了%d人的群成员上限",memCount];
            NSLog(@"%@",errStr);
//            [self showMessage:errStr andStyle:TxtCenter andHideTime:2 andLookMsg:nil];
        }else if (errType != IM_GROUP_INVITE_ERRCODE){
//            [self showMessage: @"创建群组失败" andStyle:TxtCenter andHideTime:2 andLookMsg:nil];
        }else if (errType == IM_GROUP_INVITE_ERRCODE){
            NSInteger count = MAX(0, memCount-[_existingMembers count]);
            NSString *errStr = [NSString stringWithFormat:@"邀请好友超过了成员上限，还能邀请%zi位好友",count];
            NSLog(@"%@",errStr);
//            [self showMessage:errStr andStyle:TxtCenter andHideTime:2 andLookMsg:nil];
        }
    });
    NSLog(@"创建群组失败");
}

//创建群组成功回调
- (void)IMPackageGroupDetaileType:(int)detaileType groupData:(IMPackageGroupData*)groupData {
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
        [WaitingAlertView hideWaitingView];
        _tableView.scrollEnabled = YES;
        [_membersView createActionAble:YES];
        
        if ([self.groupDel respondsToSelector:@selector(selectGroupEntity:)]) {
            [self.groupDel selectGroupEntity:groupData];
            return ;
        }
        
        //邀请好友
        if ([groupData.groupID isEqualToString:_groupID]) {
            if (self.delegate && [self.delegate respondsToSelector:@selector(InitiateGroupChat_NewGroupID:isCreatGroup:)]) {
                [self.delegate InitiateGroupChat_NewGroupID:groupData.groupID isCreatGroup:NO];
            }
            [self.navigationController popViewControllerAnimated:YES];
        }
        //创建群组
        else {
            //在此界面收到建群回调 如果是自己建的群就跳转 如果别人建的群不作任何操作
            IMPackageGroupData *groupInfoData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_getGroupInfoWithGroupId:groupData.groupID];
            if ([groupInfoData.ownerID isEqualToString:[IMPackageEngine sharedInstanceIMPackageEngine].userName]) {
                    
                if (self.delegate && [self.delegate respondsToSelector:@selector(InitiateGroupChat_NewGroupID:isCreatGroup:)]) {
                    [self.delegate InitiateGroupChat_NewGroupID:groupData.groupID isCreatGroup:YES];
                }
                [self.navigationController popViewControllerAnimated:YES];
            }
        }
    });
    
}

@end