//
//  AddNewFriendTableViewCell.h
//  aiai
//
//  Created by zhangyang on 15/2/10.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseTableViewCell.h"
#import "IMPackageData.h"

#define AddNewFriendTableViewCell_H 60

typedef void (^AddBtnAction)(NSString *userName);

@interface AddNewFriendTableViewCell : UITableViewCell

@property(nonatomic,strong)IMPackageBuddyData* addNewFriendEntity;
@property(nonatomic,strong)NSIndexPath* indexPath;

@property(nonatomic, strong)AddBtnAction addBlock;

@end
