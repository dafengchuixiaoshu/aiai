//
//  ContactTableViewCell.h
//  aiai
//
//  Created by zhangyang on 15/2/5.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseTableViewCell.h"
#import "IMPackageData.h"

#define ContactTableViewCell_Height 60


@interface ContactTableViewCell : UITableViewCell

-(void)setCellWithData:(IMPackageBuddyData*)buddyEntity AndIsShowCutline:(BOOL)isShow;

//隐藏删除按钮
-(void)resetCell;

//重新设置用户昵称
-(void)setNameLabelText:(NSString*)nameStr;

@property(nonatomic, strong)dispatch_block_t deleteBlock;
@property(nonatomic, strong)dispatch_block_t swipBlock;
@end
