//
//  SocialityListViewController.m
//  aiai
//
//  Created by wangnan on 15/11/12.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "SocialityListViewController.h"
#import "RecordingViewController.h"
#import "SocialityDetailViewController.h"
#import "LabelListViewController.h"
#import "NewsListViewController.h"

//data
#import "requestParam.pb.h"
#import "PostCellElement.h"
#import "messageNum.pb.h"

//UI
#import "SocialityListCell.h"

@interface SocialityListViewController ()<UITableViewDataSource, UITableViewDelegate>

@property(nonatomic, strong)NSMutableArray *listArray;
@property(nonatomic, strong)UITableView *listTable;

@end

@implementation SocialityListViewController

-(id)init{
    if (self = [super init]) {
        _listArray = [[NSMutableArray alloc] init];
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    //title
    self.centerBtnImageName = @"发布";
    self.rightBtnImageName = @"摄像";
    
    //创建table
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height = rect.size.height - rect.origin.y;
    _listTable = [[UITableView alloc] init];
    _listTable.frame = rect;
    _listTable.backgroundColor = EveryView_BackGroundColor;
    [_listTable setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    _listTable.backgroundColor = [UIColor clearColor];
    _listTable.delegate = self;
    _listTable.dataSource = self;
    [_listTable registerClass:[SocialityListCell class] forCellReuseIdentifier:@"SocialityListCell"];
    [self.view addSubview:_listTable];
    //获取数据
    [self getSocialityList];
    [self getNewMessageCount];
}

-(void)rightItemClicked:(id)sender{
    
    RecordingViewController *con = [[RecordingViewController alloc] init];
    [self.navigationController pushViewController:con animated:YES];
    
}

#pragma mark -- 获取数据
-(void)getSocialityList{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"communityList";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    //set发帖人ID
    NSString *value1 = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    tutorial::RequestParam_Param *param1 = builder.add_param();
    [self setParams:param1 AndKey:@"uid" Value:value1];
    
    //set视频标签
    tutorial::RequestParam_Param *param2 = builder.add_param();
    [self setParams:param2 AndKey:@"seq" Value:@"0"];
    
    //set特效 没用就传空
    tutorial::RequestParam_Param *param3 = builder.add_param();
    [self setParams:param3 AndKey:@"pageSize" Value:@"10"];
    
    __weak SocialityListViewController *weakSelf = self;
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::CommunityList outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        int count = outPutObj.community_size();
        for (int i = 0; i < count; ++i) {
            tutorial::CommunityList_Community community = outPutObj.community(i);
            PostCellElement *data = [[PostCellElement alloc] init];
            [data setModelWithCModel:community];
            [_listArray addObject:data];
        }
        [weakSelf.listTable reloadData];
        
        
    } failed:^(NSError *error) {
        
    }];
}

-(void)getNewMessageCount{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"messageNum";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    //set发帖人ID
    NSString *value1 = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    tutorial::RequestParam_Param *param1 = builder.add_param();
    [self setParams:param1 AndKey:@"uid" Value:value1];
    
    __weak SocialityListViewController *weakSelf = self;
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::MessageNum outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        dispatch_async(dispatch_get_main_queue(), ^{
            int newCount = outPutObj.num();
            if (newCount != 0) {
                //如果有新回复 创建table头
                UIButton *headerView = [[UIButton alloc] init];
                headerView.frame = CGRectMake(0, 0, _listTable.frame.size.width, 30);
                headerView.titleLabel.textColor = [UIColor whiteColor];
                headerView.titleLabel.font = [UIFont systemFontOfSize:16];
                [headerView setTitle:[NSString stringWithFormat:@"%d个新回复",newCount] forState:UIControlStateNormal];
                [headerView setTitle:[NSString stringWithFormat:@"%d个新回复",newCount] forState:UIControlStateHighlighted];
                [headerView addTarget:self action:@selector(showNewList:) forControlEvents:UIControlEventTouchUpInside];
                _listTable.tableHeaderView = headerView;
            }else{
                weakSelf.listTable.tableHeaderView = nil;
            }
        });
        NSLog(@"aa");
        
    } failed:^(NSError *error) {
        
    }];
    
}

#pragma mark ---set PB Params
-(void)setParams:(tutorial::RequestParam_Param *)params AndKey:(NSString*)key Value:(NSString*)value{
    
    params->set_paramkey([key UTF8String]);
    params->set_paramvalue([value UTF8String]);
}

#pragma mark -- buttonAction
-(void)showNewList:(id)sender{
    
    NewsListViewController *con = [[NewsListViewController alloc] init];
    [self.navigationController pushViewController:con animated:YES];
    
}

#pragma mark -- tableview Delegate
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [_listArray count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    SocialityListCell* cell = [tableView dequeueReusableCellWithIdentifier:@"SocialityListCell" forIndexPath:indexPath];
    
    PostCellElement *model = [_listArray objectAtIndex:indexPath.row];
    [cell setCellWithModel:model];
    //点击标签按钮
    cell.labelBlock = ^(NSString *labelStr){
        LabelListViewController *con = [[LabelListViewController alloc] initWithLabel:labelStr];
        [self.navigationController pushViewController:con animated:YES];
    };
    return cell;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 200;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    
    //进入详情
    PostCellElement *model = [_listArray objectAtIndex:indexPath.row];
    SocialityDetailViewController *con = [[SocialityDetailViewController alloc]initWithSid:model.sid];
    [self.navigationController pushViewController:con animated:YES];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
    
}
@end
