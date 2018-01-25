//
//  DownLoadFontViewController.m
//  aiai
//
//  Created by wangnan on 15/9/28.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "DownLoadFontViewController.h"

#import "DownLoadFilterCell.h"

#import "ResultFilterOut.pb.h"
#import "InputParamsPOJO.pb.h"

#import "LoadFilterModel.h"
#import "FilesDownManage.h"
#import "FilterManager.h"

@interface DownLoadFontViewController ()<UITableViewDataSource, UITableViewDelegate>

@property(nonatomic, strong)UITableView *fontTable;
@property(nonatomic, strong)NSMutableArray *fontArray;

@end

@implementation DownLoadFontViewController

-(id)init{
    if (self = [super init]) {
        
        _fontArray = [[NSMutableArray alloc] init];
    }
    
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    
    self.centerBtnImageName = @"更多字体";
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    _fontTable = [[UITableView alloc] initWithFrame:rect];
    [_fontTable setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    _fontTable.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    _fontTable.sectionIndexBackgroundColor = [UIColor clearColor];
    _fontTable.sectionIndexTrackingBackgroundColor = [UIColor clearColor];
    _fontTable.delegate = self;
    _fontTable.dataSource = self;
    [_fontTable registerClass:[DownLoadFilterCell class] forCellReuseIdentifier:@"DownLoadFilterCell"];
    [self.view addSubview:_fontTable];
    //获取字体列表
    [self downLoadFont];
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

-(void)downLoadFont{
    
//    读取本地已下载的特效数据
    NSString *document = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    NSString *downLoadStr = [NSString stringWithFormat:@"DownLoad/%@",FONTFINISH];
    NSString *plistPath = [document stringByAppendingPathComponent:downLoadStr];
    NSMutableDictionary *finishDic = [[NSMutableDictionary alloc] init];
    if ([[NSFileManager defaultManager]fileExistsAtPath:plistPath]) {
        finishDic = [[NSMutableDictionary alloc]initWithContentsOfFile:plistPath];
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
    
    __weak DownLoadFontViewController *weakSelf = self;
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"getFontResourceList.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        com::mm::pb::po::ResultFilterOut outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outPutObj.resultcode();
        if (code == 0) {
            for (int i = 0; i<outPutObj.resoucelist_size(); ++i) {
                ::com::mm::pb::po::FilterResult result = outPutObj.resoucelist(i);
                LoadFilterModel *fontModel = [[LoadFilterModel alloc] init];
                [fontModel setFontModel:result];
                //如果找到了就说明是已下载的
                if ([finishDic objectForKey:fontModel.loadName]) {
                    fontModel.loadState = FinishDownLoad;
                }
                [weakSelf.fontArray addObject:fontModel];
            }
        }
        [weakSelf.fontTable reloadData];
        
    } failed:^(NSError *error) {
        
    }];
    
}

#pragma mark --- tabledelegate
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [self.fontArray count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    DownLoadFilterCell* cell = [tableView dequeueReusableCellWithIdentifier:@"DownLoadFilterCell" forIndexPath:indexPath];
    
    LoadFilterModel *model = [self.fontArray objectAtIndex:indexPath.row];
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
