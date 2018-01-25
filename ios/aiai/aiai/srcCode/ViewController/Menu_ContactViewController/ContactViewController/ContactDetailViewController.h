//
//  ContactDetailViewController.h
//  aiai
//
//  Created by wangnan on 15/7/1.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"

typedef void (^DeleteBuddy)(NSString *buddyUserName);
typedef void (^ModifySubname)(NSString *modifyName);

@interface ContactDetailViewController : BaseViewController

-(id)initWithBuddyUserName:(NSString*)userName;

@property(nonatomic, strong)DeleteBuddy deleteBlock;
@property(nonatomic, strong)ModifySubname modifyBlock;

@end
