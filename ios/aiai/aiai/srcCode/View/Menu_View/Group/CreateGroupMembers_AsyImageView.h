//
//  CreateGroupMembers_AsyImageView.h
//  F2CPlatform
//
//  Created by zhangyang on 14-8-14.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "AsyImageView.h"

typedef void (^tapBlock)(NSString *memberID);

@interface CreateGroupMembers_AsyImageView : AsyImageView

- (id)initWithAction:(tapBlock)block;

@property(nonatomic, copy)NSString *memberID;

@end
