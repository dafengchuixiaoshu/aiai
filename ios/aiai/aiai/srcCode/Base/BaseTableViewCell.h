//
//  BaseTableViewCell.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/10.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BaseTableViewCell : UITableViewCell

@property(nonatomic,assign)CGFloat Left_Right_Padding;

@property(nonatomic,assign)CGFloat cutLin_Padding;
@property(nonatomic,assign)CGFloat cutLinAlpha;//0-1

@property(nonatomic,readonly, assign)CGFloat nextImageFront_Padding;
@property(nonatomic,readonly, assign)CGFloat nextImageWidth;

@property(nonatomic,assign)BOOL hiddenNextImage;

-(void)hiddenCutLineWithDataArray:(NSArray *)dataArray indexPath:(NSIndexPath*)indexPath;

//需要增加cellsubview点击事件的block

@end
