//
//  RootTableHeaderView.h
//  aiai
//
//  Created by zhangyang on 14/12/13.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^MenuTableHeaderViewBtnAction)(void);
typedef void (^SetLocationSwitchStatus)(NSInteger status);

@interface MenuTableHeaderView : UIView

@property(nonatomic,copy)NSString* headImageUrl;
@property(nonatomic,copy)NSString* headVedioUrl;
@property(nonatomic,copy)NSString* nickName;
@property(nonatomic,assign)NSInteger switchStatus;

@property(nonatomic,copy)MenuTableHeaderViewBtnAction nickNameBtnBlock;
@property(nonatomic,copy)SetLocationSwitchStatus setStatusBlock;

@end
