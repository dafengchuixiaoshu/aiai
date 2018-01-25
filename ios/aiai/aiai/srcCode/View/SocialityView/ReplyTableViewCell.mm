//
//  ReplyTableViewCell.m
//  aiai
//
//  Created by wangnan on 15/11/17.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "ReplyTableViewCell.h"

@interface ReplyTableViewCell ()


@property(nonatomic, strong)AsyImageView *headerView;           //头像
@property(nonatomic, strong)UILabel *timeLabel;                 //时间
@property(nonatomic, strong)UILabel *contentLabel;                   //评论
@property(nonatomic, strong)UILabel *nickNameLabel;                  //昵称

@end

@implementation ReplyTableViewCell

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        //头像
        _headerView = [[AsyImageView alloc] init];
        _headerView.frame = CGRectMake(13, 13, 40, 40);
        [self addSubview:_headerView];
        
        //昵称
        CGFloat nameWide = 100;
        _nickNameLabel = [[UILabel alloc] init];
        _nickNameLabel.frame = CGRectMake(_headerView.frame.origin.x + _headerView.frame.size.width + 15, _headerView.frame.origin.y, nameWide, 20);
        _nickNameLabel.textColor = [UIColor blackColor];
        _nickNameLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_nickNameLabel];
        
        //评论
        CGFloat contentWide = 200;
        _contentLabel = [[UILabel alloc] init];
        _contentLabel.frame = CGRectMake(_nickNameLabel.frame.origin.x, _nickNameLabel.frame.origin.y + _nickNameLabel.frame.size.height + 10, contentWide, 20);
        _contentLabel.textColor = [UIColor blackColor];
        _contentLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_contentLabel];
        
        //发布时间
        CGFloat timeWide = 100;
        _timeLabel = [[UILabel alloc] init];
        _timeLabel.frame = CGRectMake([UIScreen mainScreen].bounds.size.width - timeWide - 10, _headerView.frame.origin.y, timeWide, 20);
        _timeLabel.textColor = [UIColor blackColor];
        _timeLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_timeLabel];
        
    }
    return self;
}

-(void)setCellWithModel:(ReplyData*)model{
    
    [_headerView loadImageWithPath:model.faceStr andPlaceHolderName:DefalutHeadImage];
    _nickNameLabel.text = model.nickName;
    _contentLabel.text = model.content;
    _timeLabel.text = model.sendtime;
}

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
