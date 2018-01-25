//
//  NewsListViewController.m
//  aiai
//
//  Created by wangnan on 15/11/20.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "NewsListViewController.h"
#import "NewMessageTableViewCell.h"

//PB
#import "NewMessageData.h"
#import "requestParam.pb.h"

@interface NewsListViewController ()<UITableViewDataSource, UITableViewDelegate>

@property(nonatomic, strong)NSMutableArray *listArray;
@property(nonatomic, strong)UITableView *listTable;

@end

@implementation NewsListViewController

-(id)init{
    if (self = [super init]) {
        _listArray = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.centerBtnImageName = @"新消息";
    
    //创建table
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    _listTable = [[UITableView alloc] init];
    _listTable.frame = rect;
    _listTable.backgroundColor = EveryView_BackGroundColor;
    [_listTable setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    _listTable.backgroundColor = [UIColor clearColor];
    _listTable.delegate = self;
    _listTable.dataSource = self;
    [_listTable registerClass:[NewMessageTableViewCell class] forCellReuseIdentifier:@"NewMessageTableViewCell"];
    [self.view addSubview:_listTable];
    //获取数据
    [self getMessageList];
}

-(void)getMessageList{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"popNewMessage";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    tutorial::RequestParam_Param *param1 = builder.add_param();
    param1->set_paramkey([@"uid" UTF8String]);
    NSString *value1 = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    param1->set_paramvalue([value1 UTF8String]);
    
    __weak NewsListViewController *weakSelf = self;
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::NewMessageList outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        int count = outPutObj.newmessage_size();
        for (int i = 0; i < count; ++i) {
            tutorial::NewMessageList_NewMessage message = outPutObj.newmessage(i);
            NewMessageData *data = [[NewMessageData alloc] init];
            [data setWithNewMessageModel:message];
            [weakSelf.listArray addObject:data];
        }
        [weakSelf.listTable reloadData];
        
        
    } failed:^(NSError *error) {
        
    }];
    
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [_listArray count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    NewMessageTableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"NewMessageTableViewCell" forIndexPath:indexPath];
    
    NewMessageData *model = [_listArray objectAtIndex:indexPath.row];
    [cell setCellWithModel:model];
    
    return cell;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 100;
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
