//
//  AddressBookTableViewCell.h
//  aiai
//
//  Created by wangnan on 15/4/10.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ContactHandler.h"

#define ADDRESSBOOKCELLHEIGHT       66

@interface AddressBookTableViewCell : UITableViewCell

-(void)setCellWithFaceString:(NSString*)faceUrl buddyNickName:(NSString*)buddyNickName phoneBookNick:(NSString*)phoneNickName contaceType:(Buddystatus)type;

@property(nonatomic, strong)dispatch_block_t addBuddyBlock;
@property(nonatomic, strong)dispatch_block_t invitationBlock;
@end
