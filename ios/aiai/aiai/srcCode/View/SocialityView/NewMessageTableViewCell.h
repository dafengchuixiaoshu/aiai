//
//  NewMessageTableViewCell.h
//  aiai
//
//  Created by wangnan on 15/11/23.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NewMessageData.h"

@interface NewMessageTableViewCell : UITableViewCell

-(void)setCellWithModel:(NewMessageData *)model;

@end
