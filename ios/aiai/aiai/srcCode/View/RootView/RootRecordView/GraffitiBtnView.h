//
//  GraffitiBtnView.h
//  aiai
//
//  Created by wangnan on 15/9/25.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GraffitiView.h"

typedef void (^GetColor)(UIColor *currentColor);

@interface GraffitiBtnView : UIView

@property(nonatomic, copy)dispatch_block_t cancelBlock;
@property(nonatomic, copy)dispatch_block_t confirmBlock;
@property(nonatomic, copy)dispatch_block_t revokeBlock;
@property(nonatomic, copy)dispatch_block_t strokeBlock;
@property(nonatomic, copy)GetColor colorBlock;

-(void)setImgWithWidth:(FontWidth)width;
@end
