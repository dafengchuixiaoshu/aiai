//
//  ContactViewController.h
//  aiai
//
//  Created by zhangyang on 15/1/29.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"

typedef void (^dismissContactViewControllerBlock)(void);

@class IMPackageBuddyData;
@protocol ContactViewControllerDelegate <NSObject>

-(void)selectBuddyEntity:(IMPackageBuddyData*)entity;

@end

@protocol GroupViewControllerDelegate;
@interface ContactViewController : BaseViewController

@property(nonatomic, weak)id<ContactViewControllerDelegate> delegate;
@property(nonatomic, weak)id<GroupViewControllerDelegate>groupDel;

@property(nonatomic,strong)dismissContactViewControllerBlock leftBlock;

@end
