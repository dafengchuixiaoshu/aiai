//
//  SearchResultViewController.m
//  aiai
//
//  Created by zhangyang on 15/2/7.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "SearchResultViewController.h"
#import "IMPackageEngine.h"
#import "IMPackageData.h"
#import "SearchResultCell.h"

@interface SearchResultViewController ()<UITableViewDataSource,UITableViewDelegate,IMPackageEngineDelegate>
@property(nonatomic,copy)NSString* keyWord;
@property(nonatomic,strong)NSMutableArray* searchResultArray;
@property(nonatomic,strong)UITableView* searchResultTableView;
@end

@implementation SearchResultViewController

-(id)initWithSearchText:(NSString*)text{
    if (self = [super init]) {
        self.keyWord = text;
    }
    return self;
}

-(void)viewWillAppear:(BOOL)animated{
    
    [super viewWillAppear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] addDelegate:self];
}
-(void)viewDidDisappear:(BOOL)animated{
    
    [super viewDidDisappear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] removeDelegate:self];
}

-(void)viewDidLoad{
    [super viewDidLoad];
    self.centerBtnImageName = @"添加朋友";
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    self.searchResultTableView = [[UITableView alloc] initWithFrame:rect];
    [self.searchResultTableView setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    self.searchResultTableView.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    self.searchResultTableView.delegate = self;
    self.searchResultTableView.dataSource = self;
    [self.searchResultTableView registerClass:[SearchResultCell class] forCellReuseIdentifier:@"SearchResultCell"];
    
    [self.view addSubview:self.searchResultTableView];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_SearchContactWithuserID:self.keyWord];
}

#pragma mark TableViewDelegate
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [self.searchResultArray count];
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return CELLHEIGHT;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    IMPackageBuddyData* entity = [self.searchResultArray objectAtIndex:indexPath.row];
    
    static NSString* cellIdentifier = @"SearchResultCell";
    SearchResultCell* cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    [cell setCellViewWithModel:entity andSearchStr:self.keyWord];
    cell.addBlock = ^(NSString *userName){
        if([[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_AddBuddyWithUserID:userName]==S_OK){
            
            entity.buddyIsBuddy = IM_BUDDY_BUDDY;
            [tableView reloadRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationNone];
            
        }else{
            AIAI_LOG(@"add error");
        }
    };
    
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 0) {
        //新的好友
    }else if(indexPath.row == 1){
        //通讯录
    }
}

#pragma mark IMEngineNotify
-(void)IMPackage_GetSearchList:(NSArray*)searchList{
    if (self.searchResultArray) {
        [self.searchResultArray removeAllObjects];
    }else{
        self.searchResultArray = [NSMutableArray array];
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        NSArray* searchArray = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSearchList];
        for(int i = 0;i<[searchArray count];i++){
            IMPackageBuddyData* data = [searchArray objectAtIndex:i];
            [self.searchResultArray addObject:data];
        }
        [self.searchResultTableView reloadData];
    });
}

@end
