//
//  SearchResultCell.h
//  aiai
//
//  Created by wangnan on 15/3/27.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>


#define CELLHEIGHT  60

typedef void (^AddBtnAction)(NSString *userName);

@class IMPackageBuddyData;
@interface SearchResultCell : UITableViewCell

@property(nonatomic, strong)AddBtnAction addBlock;

-(void)setCellViewWithModel:(IMPackageBuddyData*)entity andSearchStr:(NSString*)searchStr;
@end
