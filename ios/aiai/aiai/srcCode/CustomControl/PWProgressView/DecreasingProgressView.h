//
//  DecreasingProgressView.h
//  aiai
//
//  Created by zhangyang on 15/1/16.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DecreasingProgressView : UIView

@property(nonatomic,assign)CGFloat progress;

-(void)resetProgress;

@end
