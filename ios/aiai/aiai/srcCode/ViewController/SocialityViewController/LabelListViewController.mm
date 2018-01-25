//
//  LabelListViewController.m
//  aiai
//
//  Created by wangnan on 15/11/18.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "LabelListViewController.h"
//data
#import "requestParam.pb.h"
#import "PostCellElement.h"

//UI
#import "SocialityListCell.h"

@interface LabelListViewController ()<UITableViewDelegate,UITableViewDataSource>

@property(nonatomic, strong)NSString *labelText;
@property(nonatomic, strong)NSMutableArray *listArray;

@property(nonatomic, strong)UITableView *listTable;

@end

@implementation LabelListViewController

-(id)initWithLabel:(NSString*)labelText{
    if (self = [super init]) {
        
        _labelText = labelText;
        _listArray = [[NSMutableArray alloc] init];
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    //title
    self.centerBtnImageName = _labelText;
    
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
    [_listTable registerClass:[SocialityListCell class] forCellReuseIdentifier:@"SocialityListCell"];
    [self.view addSubview:_listTable];
    
    [self getlabelList];
}

-(void)getlabelList{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"labelList";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    tutorial::RequestParam_Param *param1 = builder.add_param();
    [self setParams:param1 AndKey:@"label" Value:_labelText];
    
    tutorial::RequestParam_Param *param2 = builder.add_param();
    [self setParams:param2 AndKey:@"seq" Value:@"0"];
    
    tutorial::RequestParam_Param *param3 = builder.add_param();
    [self setParams:param3 AndKey:@"pageSize" Value:@"10"];
    
    __weak LabelListViewController *weakSelf = self;
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::LabelList outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        int count = outPutObj.label_size();
        for (int i = 0; i < count; ++i) {
            tutorial::LabelList_Label label = outPutObj.label(i);
            PostCellElement *data = [[PostCellElement alloc] init];
            [data setModelWithLabelModel:label];
            [_listArray addObject:data];
        }
        [weakSelf.listTable reloadData];
        
        
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
