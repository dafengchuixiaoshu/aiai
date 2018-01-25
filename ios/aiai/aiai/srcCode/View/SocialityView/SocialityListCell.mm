//
//  SocialityListCell.m
//  aiai
//
//  Created by wangnan on 15/11/2.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "SocialityListCell.h"
#import "UserHeaderView.h"

@interface SocialityListCell ()

@property(nonatomic, strong)PostCellElement *myModel;

@property(nonatomic, strong)AsyImageView *thumbnailView;        //缩略图
@property(nonatomic, strong)UserHeaderView *headerView;         //头像
@property(nonatomic, strong)UILabel *timeLabel;                 //时间
@property(nonatomic, strong)UILabel *praiseCount;               //赞的数量
@property(nonatomic, strong)UIButton *logoBtn;                  //标签

@end
@implementation SocialityListCell


-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        //缩略图
        _thumbnailView = [[AsyImageView alloc] init];
        _thumbnailView.frame = CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, 100);
        [self addSubview:_thumbnailView];
        
        //头像
        _headerView = [[UserHeaderView alloc] init];
        _headerView.frame = CGRectMake(13, 90, 40, 40);
        [self addSubview:_headerView];
        
        //发布时间
        _timeLabel = [[UILabel alloc] init];
        _timeLabel.frame = CGRectMake(_headerView.frame.origin.x + _headerView.frame.size.width + 5, _thumbnailView.frame.origin.y+_thumbnailView.frame.size.height, 100, 20);
        _timeLabel.textColor = [UIColor blackColor];
        _timeLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_timeLabel];
        
        //赞的数量
        CGFloat praiseWide = 100;
        _praiseCount = [[UILabel alloc] init];
        _praiseCount.frame = CGRectMake(self.frame.size.width - praiseWide - 15, _thumbnailView.frame.origin.y+_thumbnailView.frame.size.height, praiseWide, 20);
        _praiseCount.textColor = [UIColor blackColor];
        _praiseCount.font = [UIFont systemFontOfSize:16];
        [self addSubview:_praiseCount];
        
        //标签
        CGFloat btnWidth = 150;
        CGFloat btnheight = 40;
        _logoBtn = [[UIButton alloc] init];
        _logoBtn.frame = CGRectMake(_thumbnailView.frame.size.width - 13 - btnWidth, _thumbnailView.frame.size.height - 13 - btnheight, btnWidth, btnheight);
        _logoBtn.backgroundColor = [UIColor redColor];
        _logoBtn.titleLabel.font = [UIFont systemFontOfSize:14];
        [_logoBtn addTarget:self action:@selector(logoAction:) forControlEvents:UIControlEventTouchUpInside];
        [_thumbnailView addSubview:_logoBtn];
    }
    
    self.selectionStyle = UITableViewCellSelectionStyleNone;
    return self;
}

-(void)logoAction:(id)sender{
    
    if (_labelBlock) {
        _labelBlock(_myModel.label);
    }
    
}

-(void)setCellWithModel:(PostCellElement*)model{
    
    _myModel = model;
    
    [_thumbnailView loadImageWithPath:model.firstkey andPlaceHolderName:DefalutHeadImage];
    [_headerView loadImageWithPath:model.face andPlaceHolderName:DefalutHeadImage];
    _timeLabel.text = model.sendtime;
    _praiseCount.text = [NSString stringWithFormat:@"有%@人赞",model.praisecount];
    
    [_logoBtn setTitle:model.label forState:UIControlStateNormal];
    [_logoBtn setTitle:model.label forState:UIControlStateHighlighted];
}


@end
