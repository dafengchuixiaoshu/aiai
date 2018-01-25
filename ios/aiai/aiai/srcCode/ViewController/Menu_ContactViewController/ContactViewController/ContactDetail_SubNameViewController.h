//
//  ContactDetail_SubNameViewController.h
//  aiai
//
//  Created by wangnan on 15/8/4.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"


typedef void (^ModifySubname)(NSString *modifyName);

@interface ContactDetail_SubNameViewController : BaseViewController

-(id)initWithBuddyUserName:(NSString*)buddyName andSubName:(NSString *)subName;

@property(nonatomic, strong)ModifySubname modifyBlock;

@end
