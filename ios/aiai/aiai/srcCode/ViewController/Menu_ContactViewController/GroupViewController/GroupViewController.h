//
//  GroupViewController.h
//  aiai
//
//  Created by zhangyang on 15/2/7.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"

@class IMPackageGroupData;
@protocol GroupViewControllerDelegate <NSObject>

-(void)selectGroupEntity:(IMPackageGroupData*)groupEntity;

@end


@interface GroupViewController : BaseViewController

@property(nonatomic, assign)id<GroupViewControllerDelegate>groupDel;

@end
