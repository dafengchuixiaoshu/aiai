//
//  CreateGroupChatCell.h
//  F2CPlatform
//
//  Created by Circle Liu on 14-7-20.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

#define INITIATE_CELLHEIGHT     60
#define INITIATE_HEADERHEIGHT   20

@class IMPackageBuddyData;

@interface CreateGroupChatCell : UITableViewCell

- (void)fillData:(IMPackageBuddyData*)model andIsShowTheCutLine:(BOOL)isShow;

- (void)changeSelectStatus:(BOOL)selected;

- (void)existingMembers:(BOOL)existed;

- (BOOL)isExisted;

- (IMPackageBuddyData*)userData;

@end