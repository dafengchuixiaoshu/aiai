//
//  NewFriendViewController.m
//  aiai
//
//  Created by zhangyang on 15/2/10.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AddNewFriendViewController.h"
#import "AddNewFriendTableViewCell.h"

@interface AddNewFriendViewController ()<UITableViewDataSource,UITableViewDelegate>

@property(nonatomic,strong)NSMutableArray* addNewFriendArray;
@property(nonatomic,strong)UITableView* addNewFriendTableView;

@end

@implementation AddNewFriendViewController

-(id)init{
    if (self = [super init]) {
        self.addNewFriendArray = [[NSMutableArray alloc] init];
    }
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    self.centerBtnImageName = @"新的朋友";
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    self.addNewFriendTableView = [[UITableView alloc] initWithFrame:rect];
    self.addNewFriendTableView.backgroundColor = EveryView_BackGroundColor;
    [self.addNewFriendTableView setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    self.addNewFriendTableView.backgroundColor = [UIColor clearColor];
    self.addNewFriendTableView.delegate = self;
    self.addNewFriendTableView.dataSource = self;
    [self.addNewFriendTableView registerClass:[AddNewFriendTableViewCell class] forCellReuseIdentifier:@"AddNewFriendTableViewCell"];
    [self.view addSubview:self.addNewFriendTableView];
    
    //取fans列表的前几个 好友 为新朋友 , 没有直接的接口可以取到
    int fansCount = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetNewFansCount];
    NSArray *fansList = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetFansList];
    for (int i = 0; i<fansCount && i<[fansList count]; ++i) {
        id obj = [fansList objectAtIndex:i];
        [self.addNewFriendArray addObject:obj];
    }
    //进入后就清空新朋友数
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_ClearFansCount];
}

#pragma mark TableViewDelegate
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [self.addNewFriendArray count];
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return AddNewFriendTableViewCell_H;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString* cellIdentifier = @"AddNewFriendTableViewCell";
    AddNewFriendTableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    
    IMPackageBuddyData *data = [self.addNewFriendArray objectAtIndex:indexPath.row];
    cell.addNewFriendEntity = data;
    cell.indexPath = indexPath;
    
    __weak AddNewFriendTableViewCell *weakCell = cell;
    cell.addBlock = ^(NSString *userName){
        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_AddBuddyWithUserID:userName];
        data.buddyIsBuddy = IM_BUDDY_BUDDY;
        weakCell.addNewFriendEntity = data;
        //临时解决
//        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_ClearFansCount];
    };
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    
}

@end
