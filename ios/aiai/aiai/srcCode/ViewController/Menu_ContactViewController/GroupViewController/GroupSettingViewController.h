//
//  GroupSettingViewController.h
//  aiai
//
//  Created by wangnan on 15/3/31.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"


typedef void (^changeGroupAction)(NSString *groupName);

@interface GroupSettingViewController : BaseViewController

-(id)initWithGroupID:(NSString*)groupIDentifi;

@property(nonatomic, strong)dispatch_block_t quitBlock;
@property(nonatomic, strong)changeGroupAction groupBlock;

@property(nonatomic, strong)dispatch_block_t joinBlock;

@end
