//
//  RootTableViewCell.m
//  aiai
//
//  Created by zhangyang on 14/12/13.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "MenuTableViewCell.h"

@interface MenuTableViewCell ()
@property(nonatomic,strong)UILabel* titleLable;
@property(nonatomic,strong)UILabel* topLineLabel;
@end

@implementation MenuTableViewCell
@synthesize titleLable = _titleLable;
@synthesize topLineLabel = _topLineLabel;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        self.backgroundColor = [UIColor clearColor];
        self.selectionStyle = UITableViewCellSelectionStyleNone;
        
        _topLineLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 76, 122,1)];
        _topLineLabel.backgroundColor = [UIColor whiteColor];
        [self.contentView addSubview:_topLineLabel];
        
        _titleLable = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 150, 44)];
        _titleLable.textColor = [UIColor whiteColor];
        _titleLable.textAlignment = NSTextAlignmentCenter;
        _titleLable.font = [UIFont boldSystemFontOfSize:20];
        
        [self.contentView addSubview:_titleLable];
    }
    return self;
}

-(void)setTitleText:(NSString *)titleText{
    _titleLable.text = titleText;
}

-(void)layoutSubviews{
    _topLineLabel.alpha = 0.2;
    _topLineLabel.center = self.contentView.center;
    _topLineLabel.frame = CGRectMake(_topLineLabel.frame.origin.x, 0, 125, _topLineLabel.frame.size.height);
    
    _titleLable.center = self.contentView.center;
}

@end
