//
//  AddressMapHeadView.h
//  aiai
//
//  Created by zhangyang on 15/1/19.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^AddressMapTopViewBtnAction)(void);

@interface AddressMapTopView : UIView

@property(nonatomic,copy)AddressMapTopViewBtnAction openCloseBtnBlock;

@end
