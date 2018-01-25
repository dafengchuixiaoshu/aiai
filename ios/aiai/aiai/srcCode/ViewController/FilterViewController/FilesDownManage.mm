
//  FilesDownManage.m
//  Created by yu on 13-1-21.
//

#import "FilesDownManage.h"
#import "ZipArchive.h"
#import "FilterManager.h"

#define TEMPPATH [CommonHelper getTempFolderPathWithBasepath:_basepath]
#define OPENFINISHLISTVIEW
#define MAXCOUNT    5 

@interface FilesDownManage ()<ASIHTTPRequestDelegate,ASIProgressDelegate>

@property(nonatomic,copy)NSString *basepath;

@property(nonatomic,strong)NSMutableArray *downinglist;//正在下载的文件列表(ASIHttpRequest对象)
@property(nonatomic,strong)NSMutableArray *queuelist;   //等待下载的数据

@property(nonatomic,strong)NSMutableDictionary *requestDic;   //存放请求数据

@end

@implementation FilesDownManage

static   FilesDownManage *sharedFilesDownManage = nil;

#pragma mark -- init methods --
- (id)init
{
    self = [super init];
    if (self != nil) {
        
        _downinglist=[[NSMutableArray alloc] init];
        _queuelist = [[NSMutableArray alloc] init];
        _requestDic = [[NSMutableDictionary alloc] init];
    }
    return self;
}
-(void)clearManage{
    
    [self clearAllRquests];
    [_downinglist removeAllObjects];
    [_queuelist removeAllObjects];
    [_requestDic removeAllObjects];
}

+(FilesDownManage *) sharedFilesDownManage{
    @synchronized(self){
        if (sharedFilesDownManage == nil) {
            sharedFilesDownManage = [[self alloc] init];
            sharedFilesDownManage.basepath = @"DownLoad";
        }
    }
    return  sharedFilesDownManage;
}

#pragma mark -- 入口 --
#pragma mark -- httpAction
-(void)requestDownLoadWithfilterModel:(LoadFilterModel*)model receiveBlock:(RequestBlock)receiveBlock successed:(RequestBlock)successed failed:(RequestBlock)failed{
    
    NSString *path = @"zip";
    if (model.loadType == downLoad_filter) {
        path = @"zip";
    }else if (model.loadType == downLoad_font){
        path = @"fontzip";
    }
    path= [CommonHelper getTargetPathWithBasepath:_basepath subpath:path];
    path = [path stringByAppendingPathComponent:model.loadName];
    model.targetPath = path ;
    model.loadState=DownLoading;
    NSString *tempfilePath= [TEMPPATH stringByAppendingPathComponent:model.loadName];
    model.tempPath = tempfilePath;
    
    //每次最多下载5个特效,放进待下载数组
    if ([_downinglist count]>MAXCOUNT) {
        __block RequestBlock tsuccess = successed;
        __block RequestBlock treceive = receiveBlock;
        NSDictionary *dic = [NSDictionary dictionaryWithObjectsAndKeys:treceive,@"receive",tsuccess,@"success",failed,@"fail", nil];
        [_requestDic setObject:dic forKey:model.loadName];
        [_queuelist addObject:model];
    }else{
        [self beginRequest:model receiveBlock:^(LoadFilterModel *model) {
            receiveBlock(model);
        } successed:^(LoadFilterModel *model) {
            successed(model);
        } failed:^(LoadFilterModel *model) {
            failed(model);
        }];
    }
}

-(void)startLoadReceiveBlock:(RequestBlock)receiveBlock successed:(RequestBlock)successed failed:(RequestBlock)failed{
    
    //如果还有未下载的数据 那么继续进行下载
    if ([_queuelist count]>0) {
        
        LoadFilterModel *model ;
        for (int i = 0; i<[_queuelist count]; ++i) {
            LoadFilterModel *tmodel = [_queuelist objectAtIndex:i];
            if (tmodel.loadTotalSize == nil) {
                model = tmodel;
                break;
            }
        }
        
        model.loadState = DownLoading;
        [self beginRequest:model receiveBlock:^(LoadFilterModel *model) {
            
            receiveBlock(model);
            
        } successed:^(LoadFilterModel *model) {
            
            model.loadState = FinishDownLoad;
            successed(model);
            
        } failed:^(LoadFilterModel *model) {
            
            failed(model);
            
        }];
    }
}

-(void)beginRequest:(LoadFilterModel *)model receiveBlock:(RequestBlock)receive successed:(RequestBlock)successed failed:(RequestBlock)failed{
    
    ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:[NSURL URLWithString:model.loadURL]];
    __weak ASIHTTPRequest *weakRequest = request;
    [request setDownloadDestinationPath:[model targetPath]];
    [request setTemporaryFileDownloadPath:model.tempPath];
    [request setNumberOfTimesToRetryOnTimeout:2];
    [request setTimeOutSeconds:60.0f];
    [request setShouldContinueWhenAppEntersBackground:YES];
    [request setCompletionBlock:^( void ){
        
        model.loadState = FinishDownLoad;
        NSString *configPath=[model.tempPath stringByAppendingString:@".plist"];
        NSFileManager *fileManager=[NSFileManager defaultManager];
        NSError *error;
        if([fileManager fileExistsAtPath:configPath])//如果存在临时文件的配置文件
        {
            [fileManager removeItemAtPath:configPath error:&error];
            if(!error)
            {
                AIAI_LOG(@"%@",[error description]);
            }
        }
        [_queuelist removeObject:model];
        [_downinglist removeObject:weakRequest];
        if (model.loadType == downLoad_filter) {
            //特效下载后需要的存储和解压缩操作
            [self saveFinishedFileWithModel:(LoadFilterModel*)model sourceName:FILTERFINISH];
            [self unzipFileWithFilterName:model.loadName zipPath:FILTERZIPCACHE sourchPath:FILTERCACHE];
        }else if (model.loadType == downLoad_font){
            [self saveFinishedFileWithModel:(LoadFilterModel*)model sourceName:FONTFINISH];
            [self unzipFileWithFilterName:model.loadName zipPath:FONTZIPCACHE sourchPath:FONTCACHE];
        }
        //开始新的请求
        [self startLoadReceiveBlock:^(LoadFilterModel *model) {
            
            NSDictionary *dic = [_requestDic objectForKey:model.loadName];
            if (dic) {
                RequestBlock receiveBlock = [dic objectForKey:@"receive"];
                receiveBlock(model);
            }
        } successed:^(LoadFilterModel *model) {
            NSDictionary *dic = [_requestDic objectForKey:model.loadName];
            if (dic) {
                RequestBlock successedBlock = [dic objectForKey:@"success"];
                successedBlock(model);
                //开始下载后把此数据从待下载数组中删除
                [_queuelist removeObject:model];
                [_requestDic removeObjectForKey:model.loadName];
            }
        } failed:^(LoadFilterModel *model) {
            //            failed(model);
        }];
        
        successed(model);
    }];
    __block long long receiveSize = 0;
    [request setBytesReceivedBlock:^(unsigned long long size, unsigned long long total) {
        
        model.loadState = DownLoading;
        model.loadTotalSize = [NSString stringWithFormat:@"%lld", total];
        model.loadReceivedSize=[NSString stringWithFormat:@"%lld",receiveSize+=size];
        receive(model);
        
    }];
    
    // 使用 failed 块，在下载失败时做一些事情
    [request setFailedBlock :^( void ){
        
        if ([weakRequest isExecuting]) {
            [weakRequest cancel];
        }
        model.loadState = FailDownLoad;
        failed(model);
        
    }];
    
    [request startAsynchronous];
    [_downinglist addObject:request];
    
}

-(void)clearAllRquests{
    NSFileManager *fileManager=[NSFileManager defaultManager];
    NSError *error;
    for (ASIHTTPRequest *request in _downinglist) {
        
        [request clearDelegatesAndCancel];
        LoadFilterModel *fileInfo=(LoadFilterModel*)[request.userInfo objectForKey:@"File"];
        NSString *path=fileInfo.tempPath;;
        NSString *configPath=[NSString stringWithFormat:@"%@.plist",path];
        [fileManager removeItemAtPath:path error:&error];
        [fileManager removeItemAtPath:configPath error:&error];
        if(!error){
            AIAI_LOG(@"%@",[error description]);
        }
        
    }
    [_downinglist removeAllObjects];
}

#pragma mark -- saveFile
-(void)saveFinishedFileWithModel:(LoadFilterModel*)model sourceName:(NSString*)sourceName{
    
    NSString *document = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    NSString *plistPath = [[document stringByAppendingPathComponent:self.basepath]stringByAppendingPathComponent:sourceName];
    NSMutableDictionary *finishedinfo = [[NSMutableDictionary alloc] initWithContentsOfFile:plistPath];
    if (!finishedinfo) {
        finishedinfo = [[NSMutableDictionary alloc] init];
    }
    
    NSDictionary *filedic = [NSDictionary dictionaryWithObjectsAndKeys:model.loadName,@"filename",model.loadTotalSize,@"filesize",model.targetPath,@"filepath",model.loadimagePath? :@"",@"fileimage", nil];
    [finishedinfo setObject:filedic forKey:model.loadName? :@""];
    
    if (![finishedinfo writeToFile:plistPath atomically:YES]) {
        AIAI_LOG(@"write plist fail");
    }
}

-(void)unzipFileWithFilterName:(NSString*)filterName zipPath:(NSString*)zipPath sourchPath:(NSString*)sourcePath{
    ZipArchive* zip = [[ZipArchive alloc] init];
    NSString* unzipto = [NSString stringWithFormat:@"%@/%@",zipPath,filterName];
    if( [zip UnzipOpenFile:unzipto]){
        
        NSString *path = [NSString stringWithFormat:@"%@/%@",sourcePath,filterName];
        if ([[NSFileManager defaultManager] fileExistsAtPath:path] == NO) {
            [[NSFileManager defaultManager] createDirectoryAtPath:path withIntermediateDirectories:NO attributes:nil error:nil];
        }
        
        BOOL ret = [zip UnzipFileTo:path overWrite:YES];
        if( NO==ret ){}
        
        [zip UnzipCloseFile];
    }
}

@end
