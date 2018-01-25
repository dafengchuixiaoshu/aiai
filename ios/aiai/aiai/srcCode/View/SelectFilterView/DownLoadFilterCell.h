//
//  DownLoadFilterCell.h
//  aiai
//
//  Created by wangnan on 15/5/28.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

#define DOWNLOADFILTER_CELLHEIGHT       80

@class LoadFilterModel;
@interface DownLoadFilterCell : UITableViewCell

@property(nonatomic, strong)dispatch_block_t downLoadBlock;

-(void)setFilterCellWithModel:(LoadFilterModel*)model;

//加载中改变进度条
-(void)changeProgress;
-(void)downLoadFinish;
@end
