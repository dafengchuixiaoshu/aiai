//
//  SocialityListCell.h
//  aiai
//
//  Created by wangnan on 15/11/2.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PostCellElement.h"

typedef void (^LabelBtnAction)(NSString *labelStr);

@interface SocialityListCell : UITableViewCell

@property(nonatomic, copy)LabelBtnAction labelBlock;

-(void)setCellWithModel:(PostCellElement*)model;

@end
