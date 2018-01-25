//
//  DownLoadFilterViewController.h
//  aiai
//
//  Created by wangnan on 15/5/28.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"

@protocol DownLoadFilterViewControllerDelegate <NSObject>

-(void)notifyDelegateReloadData;

@end

@interface DownLoadFilterViewController : BaseViewController

@property(nonatomic,weak)id<DownLoadFilterViewControllerDelegate> delegate;

@end
