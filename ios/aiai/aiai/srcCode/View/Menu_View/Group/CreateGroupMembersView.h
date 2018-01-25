//
//  CreateGroupMembersView.h
//  F2CPlatform
//
//  Created by Circle Liu on 14-7-20.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^deleteBlock)(NSString *memberName);

#define CREATVIEWHEIGHT 85

@interface CreateGroupMembersView : UIView

- (id)initWithFrame:(CGRect)frame comfirm:(dispatch_block_t)block deleteAction:(deleteBlock)delBlock;

- (void)insertPerson:(NSString*)memberName userURL:(NSString*)userURL;

- (void)deletePerson:(NSString*)memberName;

- (void)createActionAble:(BOOL)able;

@end
