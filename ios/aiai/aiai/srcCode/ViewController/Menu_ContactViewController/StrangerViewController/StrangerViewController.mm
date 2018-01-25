//
//  StrangerViewController.m
//  aiai
//
//  Created by zhangyang on 15/2/7.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "StrangerViewController.h"
#import "BaseTableViewCell.h"

@interface StrangerViewController ()<UITableViewDataSource,UITableViewDelegate,UISearchBarDelegate>
@property(nonatomic,strong)NSArray* strangerArray;
@property(nonatomic,strong)UITableView* strangerTableView;

@end

@implementation StrangerViewController

-(id)init{
    if (self = [super init]) {
        self.strangerArray = [NSArray arrayWithObjects:@"新的朋友",@"通讯录",nil];
    }
    return self;
}


-(void)viewDidLoad{
    [super viewDidLoad];
    self.centerBtnImageName = @"路人";
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    self.strangerTableView = [[UITableView alloc] initWithFrame:rect];
    self.strangerTableView.backgroundColor = EveryView_BackGroundColor;
    [self.strangerTableView setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    self.strangerTableView.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    self.strangerTableView.delegate = self;
    self.strangerTableView.dataSource = self;
    [self.strangerTableView registerClass:[BaseTableViewCell class] forCellReuseIdentifier:@"BaseTableViewCell"];
    [self.view addSubview:self.strangerTableView];
}

#pragma mark TableViewDelegate
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [self.strangerArray count];
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString* cellIdentifier = @"BaseTableViewCell";
    BaseTableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    cell.backgroundColor = [UIColor clearColor];
    cell.textLabel.text = [self.strangerArray objectAtIndex:indexPath.row];
    cell.textLabel.textColor = [UIColor whiteColor];
    cell.imageView.image = [UIImage imageNamed:DefalutHeadImage];
    
    [cell hiddenCutLineWithDataArray:self.strangerArray indexPath:indexPath];
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 0) {
        //新的好友
    }else if(indexPath.row == 1){
        //通讯录
    }
}

@end
