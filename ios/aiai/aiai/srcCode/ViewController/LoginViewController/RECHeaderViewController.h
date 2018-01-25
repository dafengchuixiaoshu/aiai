//
//  RECHeaderViewController.h
//  aiai
//
//  Created by zhangyang on 15/4/3.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"

@protocol RECHeaderViewControllerDelegate <NSObject>

-(void)REC_VideoPath:(NSURL*)path videoTime:(CGFloat)time;

@end

@interface RECHeaderViewController : BaseViewController

@property(nonatomic,weak)id<RECHeaderViewControllerDelegate> delegate;

@end
