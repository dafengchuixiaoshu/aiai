//
//  DownLoadFilterViewController.m
//  aiai
//
//  Created by wangnan on 15/5/28.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "DownLoadFilterViewController.h"
#import "DownLoadFilterCell.h"

#import "ResultFilterOut.pb.h"
#import "InputParamsPOJO.pb.h"

#import "LoadFilterModel.h"
#import "FilesDownManage.h"
#import "FilterManager.h"

@interface DownLoadFilterViewController ()<UITableViewDataSource, UITableViewDelegate>

@property(nonatomic, strong)UITableView *filterTable;
@property(nonatomic, strong)NSMutableArray *filterArray;

@end

@implementation DownLoadFilterViewController

-(id)init{
    if (self = [super init]) {
        
        self.filterArray = [[NSMutableArray alloc] init];
        
    }
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    
    self.centerBtnImageName = @"更多特效";
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    self.filterTable = [[UITableView alloc] initWithFrame:rect];
    [self.filterTable setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    self.filterTable.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    self.filterTable.sectionIndexBackgroundColor = [UIColor clearColor];
    self.filterTable.sectionIndexTrackingBackgroundColor = [UIColor clearColor];
    self.filterTable.delegate = self;
    self.filterTable.dataSource = self;
    [self.filterTable registerClass:[DownLoadFilterCell class] forCellReuseIdentifier:@"DownLoadFilterCell"];
    [self.view addSubview:self.filterTable];
    
    [self downLoadFilter];
}

-(void)viewWillDisappear:(BOOL)animated{
    
    [super viewWillDisappear:animated];
    
    [[FilesDownManage sharedFilesDownManage] clearManage];
    
    dispatch_queue_t queue = dispatch_queue_create("removeFile", NULL);
    dispatch_async(queue, ^{
        //删除下载的源文件 (zip文件)
        NSString* unzipto = [NSString stringWithFormat:@"%@/",FILTERZIPCACHE];
        if([[NSFileManager defaultManager] fileExistsAtPath:unzipto])//如果存在临时文件的配置文件
        {
            [[NSFileManager defaultManager] removeItemAtPath:unzipto error:nil];
        }
    });
}

-(void)leftItemClicked:(id)sender{
    if (self.delegate && [self.delegate respondsToSelector:@selector(notifyDelegateReloadData)]) {
        [self.delegate notifyDelegateReloadData];
    }
    [super leftItemClicked:sender];
}

-(void)downLoadFilter{
    
    //读取本地已下载的特效数据
    NSString *document = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    NSString *downLoadStr = [NSString stringWithFormat:@"DownLoad/%@",FILTERFINISH];
    NSString *plistPath = [document stringByAppendingPathComponent:downLoadStr];
    NSMutableDictionary *finishDic = [[NSMutableDictionary alloc] init];
    if ([[NSFileManager defaultManager]fileExistsAtPath:plistPath]) {
        finishDic = [[NSMutableDictionary alloc]initWithContentsOfFile:plistPath];
    }
    //预制信息默认为已下载
    for (int i = 0; i < [[FilterManager share].preinstallEffectTitleArray count]; ++i) {
        
        NSString *titleStr = [[FilterManager share].preinstallEffectTitleArray objectAtIndex:i];
        [finishDic setObject:@"finish" forKey:titleStr];
    }
    
    ::google::protobuf::int32 value = 0;
#ifdef NO_APP_STORE_VERTION
    value = 0;
#else
    value = 1;
#endif
    com::mm::pb::po::InputParamsPOJO builder;
    builder.set_asup(value);
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    __weak DownLoadFilterViewController *weakSelf = self;
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"getFilterResourceList.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        com::mm::pb::po::ResultFilterOut outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outPutObj.resultcode();
        if (code == 0) {
            for (int i = 0; i<outPutObj.resoucelist_size(); ++i) {
                ::com::mm::pb::po::FilterResult result = outPutObj.resoucelist(i);
                LoadFilterModel *filterModel = [[LoadFilterModel alloc] init];
                [filterModel setFilterModel:result];
                //如果找到了就说明是已下载的
                if ([finishDic objectForKey:filterModel.loadName]) {
                    filterModel.loadState = FinishDownLoad;
                }
                [weakSelf.filterArray addObject:filterModel];
            }
            [weakSelf.filterTable reloadData];
        }
        
    } failed:^(NSError *error) {
        
    }];
}

//-(void)setModelState:(LoadFilterModel*)model{
//    
//    
//}

-(void)dealloc{
    
    AIAI_LOG(@"DownLoadFilterViewController dealloc");
}

#pragma mark --- tabledelegate
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [self.filterArray count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    DownLoadFilterCell* cell = [tableView dequeueReusableCellWithIdentifier:@"DownLoadFilterCell" forIndexPath:indexPath];
    
    LoadFilterModel *model = [self.filterArray objectAtIndex:indexPath.row];
    [cell setFilterCellWithModel:model];
    __weak DownLoadFilterCell *weakCell = cell;
    cell.downLoadBlock = ^(){
        
        [[FilesDownManage sharedFilesDownManage] requestDownLoadWithfilterModel:model receiveBlock:^(LoadFilterModel *model) {
            
            [weakCell changeProgress];
        } successed:^(LoadFilterModel *model) {
            
            [weakCell downLoadFinish];
        } failed:^(LoadFilterModel *model) {
            AIAI_LOG(@"load fail");
        }];
    };
    return cell;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return DOWNLOADFILTER_CELLHEIGHT;
}

@end
