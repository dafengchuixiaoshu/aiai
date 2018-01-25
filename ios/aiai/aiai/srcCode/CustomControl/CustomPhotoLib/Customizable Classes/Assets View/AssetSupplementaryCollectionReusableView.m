//
//  AssetSupplementaryCollectionReusableView.m
//  ThirteenSeconds
//
//  Created by zhangyang on 14-8-15.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "AssetSupplementaryCollectionReusableView.h"

@implementation AssetSupplementaryCollectionReusableView{
    UILabel* _titleLable;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        self.backgroundColor = [UIColor redColor];
        
        _titleLable = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 320, 20)];
        _titleLable.backgroundColor = [UIColor clearColor];
        _titleLable.font = [UIFont boldSystemFontOfSize:14];
        _titleLable.textColor = [UIColor whiteColor];
        _titleLable.text = @"";
        [self addSubview:_titleLable];
    }
    return self;
}

-(void)setTitleText:(NSString *)titleText{
    _titleLable.text = titleText;
}

-(NSString*)titleText{
    return _titleLable.text;
}

//- (void)setNeedsDisplay {
//    [super setNeedsDisplay];
//    
//    [self setBackgroundColor:[UIColor redColor]];
//}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
