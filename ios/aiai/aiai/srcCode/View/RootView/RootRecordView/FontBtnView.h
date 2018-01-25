//
//  FontBtnView.h
//  aiai
//
//  Created by wangnan on 15/10/19.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>


typedef void (^GetColor)(UIColor *currentColor);

@interface FontBtnView : UIView

@property(nonatomic, copy)dispatch_block_t cancelBlock;
@property(nonatomic, copy)dispatch_block_t changeBlock;
@property(nonatomic, copy)dispatch_block_t downLoadFontBlock;
@property(nonatomic, copy)dispatch_block_t confirmBlock;
@property(nonatomic, copy)GetColor colorBlock;

@end
