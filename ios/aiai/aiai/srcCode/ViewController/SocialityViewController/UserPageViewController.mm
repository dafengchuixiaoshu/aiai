//
//  UserPageViewController.m
//  aiai
//
//  Created by wangnan on 15/11/18.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "UserPageViewController.h"
//UI
#import "UserPageHeaderView.h"
#import "SocialityListCell.h"

#import "requestParam.pb.h"

@interface UserPageViewController ()<UITableViewDataSource, UITableViewDelegate>

@property(nonatomic, copy)NSString *uid;
@property(nonatomic, strong)NSMutableArray *listArray;

@property(nonatomic, strong)UserPageHeaderView *headerView;
@property(nonatomic, strong)UITableView *userPageTable;
@end

@implementation UserPageViewController

-(id)initWithUid:(NSString*)uid{
    if (self = [super init]) {
        _uid = uid;
        _listArray = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.centerBtnImageName = _titleStr? :@"个人主页";
    
    CGRect headerRect = CGRectMake(0, 64, self.view.frame.size.width, 200);
    _headerView = [[UserPageHeaderView alloc] initWithFrame:headerRect];
    
    //TABLE
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height = rect.size.height - rect.origin.y;
    _userPageTable = [[UITableView alloc] init];
    _userPageTable.frame = rect;
    _userPageTable.backgroundColor = EveryView_BackGroundColor;
    [_userPageTable setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    _userPageTable.backgroundColor = [UIColor clearColor];
    _userPageTable.delegate = self;
    _userPageTable.dataSource = self;
    [_userPageTable registerClass:[SocialityListCell class] forCellReuseIdentifier:@"SocialityListCell"];
    _userPageTable.tableHeaderView = _headerView;
    [self.view addSubview:_userPageTable];
    
    //获取数据
    [self getPageData];
}

-(void)getPageData{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"userPage";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    tutorial::RequestParam_Param *param1 = builder.add_param();
    NSString *value1 = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    [self setParams:param1 AndKey:@"uid" Value:value1];
    
    tutorial::RequestParam_Param *param2 = builder.add_param();
    [self setParams:param2 AndKey:@"ouid" Value:_uid];
    
    tutorial::RequestParam_Param *param3 = builder.add_param();
    [self setParams:param3 AndKey:@"seq" Value:@"0"];
    
    tutorial::RequestParam_Param *param4 = builder.add_param();
    [self setParams:param4 AndKey:@"pageSize" Value:@"10"];
    
    __weak UserPageViewController *weakSelf = self;
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::UserPage outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        
        dispatch_async(dispatch_get_main_queue(), ^{
            weakSelf.headerView.nickNameStr = [NSString stringWithUTF8String:outPutObj.nickname().c_str()];
            weakSelf.headerView.faceUrl = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL, [NSString stringWithUTF8String:outPutObj.face().c_str()]];
            weakSelf.headerView.signatureStr = [NSString stringWithUTF8String:outPutObj.sign().c_str()];
            weakSelf.headerView.faceVideo = [NSString stringWithUTF8String:outPutObj.facevideo().c_str()];
            int count = outPutObj.work_size();
            for (int i = 0; i < count; ++i) {
                tutorial::UserPage_Work work = outPutObj.work(i);
                PostCellElement *data = [[PostCellElement alloc] init];
                [data setModelWithworkModel:work];
                [_listArray addObject:data];
            }
            [weakSelf.userPageTable reloadData];
        });
        
    } failed:^(NSError *error) {
        
    }];
}

#pragma mark -- tableview Delegate
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [_listArray count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    SocialityListCell* cell = [tableView dequeueReusableCellWithIdentifier:@"SocialityListCell" forIndexPath:indexPath];
    
    PostCellElement *model = [_listArray objectAtIndex:indexPath.row];
    [cell setCellWithModel:model];
    
    return cell;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 200;
}

#pragma mark ---set PB Params
-(void)setParams:(tutorial::RequestParam_Param *)params AndKey:(NSString*)key Value:(NSString*)value{
    
    params->set_paramkey([key UTF8String]);
    params->set_paramvalue([value UTF8String]);
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
