//
//  BaseTableViewCell.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/10.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "BaseTableViewCell.h"

@interface BaseTableViewCell ()

@property(nonatomic,strong)UIImageView* nextImageView;

@property(nonatomic,strong)UIImageView* topCutLine;

@property(nonatomic,strong)UIImageView* bottomCutLine;

@end

@implementation BaseTableViewCell
@synthesize Left_Right_Padding = _Left_Right_Padding;
@synthesize nextImageFront_Padding = _nextImageFront_Padding;
@synthesize nextImageView = _nextImageView;
@synthesize topCutLine = _topCutLine;
@synthesize bottomCutLine = _bottomCutLine;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        self.backgroundColor = [UIColor whiteColor];
        self.selectionStyle = UITableViewCellSelectionStyleNone;
        _Left_Right_Padding = 15;
        _nextImageFront_Padding = 10;
        _cutLin_Padding = 0;
        _cutLinAlpha = 1.0;
        
        _topCutLine = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        _topCutLine.frame = CGRectMake(0, 0, CGRectGetWidth([UIScreen mainScreen].bounds), 1);
        [self.contentView addSubview:_topCutLine];
        
        _nextImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"系统导航右箭头.png"]];
        CGRect rect = _nextImageView.frame;
        rect.origin.x = CGRectGetWidth(self.frame) - _Left_Right_Padding - _nextImageView.frame.size.width;
        rect.origin.y = (self.frame.size.height - _nextImageView.frame.size.height)/2;
        _nextImageView.frame = rect;
        [self.contentView addSubview:_nextImageView];
        
        _bottomCutLine = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        _bottomCutLine.frame = CGRectMake(0, self.frame.size.height-1, CGRectGetWidth([UIScreen mainScreen].bounds), 1);
        [self.contentView addSubview:_bottomCutLine];
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

-(void)setLeft_Right_Padding:(CGFloat)Left_Right_Padding{
    _Left_Right_Padding = Left_Right_Padding;
    
    CGRect rect = _nextImageView.frame;
    rect.origin.x = CGRectGetWidth(self.frame) - _Left_Right_Padding - _nextImageView.frame.size.width;
    _nextImageView.frame = rect;
}

-(CGFloat)nextImageWidth{
    return CGRectGetWidth(_nextImageView.frame);
}

-(void)setHiddenNextImage:(BOOL)hiddenNextImage{
    _nextImageView.hidden = hiddenNextImage;
}

-(void)hiddenCutLineWithDataArray:(NSArray *)dataArray indexPath:(NSIndexPath*)indexPath{
    if ([dataArray count] == 1)
        return;
    if (indexPath.row == 0) {
        return;
    }else{
        self.topCutLine.hidden = YES;
    }
}

- (void)setFrame:(CGRect)frame {
    frame.size.width = CGRectGetWidth([UIScreen mainScreen].bounds);
    
    [super setFrame:frame];
}

-(void)setCutLin_Padding:(CGFloat)cutLin_Padding{
    _cutLin_Padding = cutLin_Padding;
}

-(void)setCutLinAlpha:(CGFloat)cutLinAlpha{
    _cutLinAlpha = cutLinAlpha;
}

-(void)layoutSubviews{
    [super layoutSubviews];
    
    CGRect rect = _nextImageView.frame;
    rect.origin.x = CGRectGetWidth(self.frame) - _Left_Right_Padding - _nextImageView.frame.size.width;
    rect.origin.y = (self.frame.size.height - _nextImageView.frame.size.height)/2;
    _nextImageView.frame = rect;
    
    _topCutLine.frame = CGRectMake(_cutLin_Padding, 0, CGRectGetWidth([UIScreen mainScreen].bounds) - _cutLin_Padding*2, 1);
    _topCutLine.alpha = _cutLinAlpha;
    
    _bottomCutLine.frame = CGRectMake(_cutLin_Padding, self.frame.size.height-1, CGRectGetWidth([UIScreen mainScreen].bounds) - _cutLin_Padding*2, 1);
    _bottomCutLine.alpha = _cutLinAlpha;
}


@end
