//
//  BaseTableHeaderView.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/11.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^BaseTableHeaderTapAction)(void);

@interface BaseTableHeaderView : UIView

@property(nonatomic,assign)CGFloat Left_Right_Padding;

@property(nonatomic,readonly, assign)CGFloat nextImageFront_Padding;
@property(nonatomic,readonly, assign)CGFloat nextImageWidth;

@property(nonatomic,assign)BOOL hiddenNextImage;

@property(nonatomic,strong)BaseTableHeaderTapAction tapAction;

@end
