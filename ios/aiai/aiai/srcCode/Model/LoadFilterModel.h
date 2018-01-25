//
//  LoadFilterModel.h
//  aiai
//
//  Created by wangnan on 15/5/30.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ResultFilterOut.pb.h"

typedef NS_ENUM(NSInteger, DownLoadState) {
    
    NODownLoad      = 0,
    DownLoading     = 1,
    FinishDownLoad  = 2,
    FailDownLoad    = 3,
};

typedef NS_ENUM(NSInteger, DownLoadType) {
    
    downLoad_filter = 0,        //特效下载
    downLoad_font = 1,          //字体下载
};

@class FilterResult;
@interface LoadFilterModel : NSObject

@property(nonatomic,copy)NSString *loadName;                //下载文件的名称
@property(nonatomic,copy)NSString *loadimagePath;           //下载文件的效果图
@property(nonatomic,copy)NSString *upLoadAuth;          //上传人昵称


@property(nonatomic,copy)NSString *loadTotalSize;           //下载文件的总大小
@property(nonatomic,copy)NSString *loadReceivedSize;        //已下载的大小
@property(nonatomic,strong)NSMutableData *loadReceivedData; //接受的数据


@property(nonatomic,copy)NSString *loadURL;                   //下载地址
@property(nonatomic,copy)NSString *targetPath;                //本地存放的地址
@property(nonatomic,copy)NSString *tempPath;                  //临时地址
@property(nonatomic,assign)DownLoadState loadState;           //是否正在下载
@property(nonatomic,assign)DownLoadType loadType;             //下载类型
@property(nonatomic,assign)BOOL  isInQueue;                   //判断这个请求是否在队列中
@property(nonatomic,assign)BOOL isFirstReceived;              //是否是第一次接受数据，如果是则不累加第一次返回的数据长度，之后变累加


-(void)setFilterModel:(::com::mm::pb::po::FilterResult)result;      //下载特效
-(void)setFontModel:(::com::mm::pb::po::FilterResult)result;        //下载字体
@end
