//
//  BaseView.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/2.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseView.h"

@implementation BaseView

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        self.backgroundColor = [UIColor colorWithHexString:STS_GRAY_BACKGOUND_COLOR];
    }
    return self;
}

@end
