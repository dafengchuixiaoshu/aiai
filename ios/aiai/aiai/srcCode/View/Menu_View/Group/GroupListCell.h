//
//  GroupListCell.h
//  aiai
//
//  Created by wangnan on 15/3/31.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "IMPackageData.h"

#define GROUPLISTCELLHEIGHT     60

@interface GroupListCell : UITableViewCell

-(void)setGroupCellWithGroupData:(IMPackageGroupData*)groupData;
@end
