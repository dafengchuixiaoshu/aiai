//
//  ContactTableHeaderView.h
//  aiai
//
//  Created by zhangyang on 15/2/5.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BaseTableHeaderView.h"

typedef void (^ContactTableHeaderViewBtnAction)(void);

@interface ContactTableHeaderView : UIView

@property(nonatomic,strong)ContactTableHeaderViewBtnAction addContactBlock;
@property(nonatomic,strong)ContactTableHeaderViewBtnAction strangerBlock;
@property(nonatomic,strong)ContactTableHeaderViewBtnAction groupBlock;

//设置新朋友个数
-(void)setMsgCountLabelText:(NSString*)count;
@end
