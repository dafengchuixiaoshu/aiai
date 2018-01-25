//
//  NewMessageTableViewCell.m
//  aiai
//
//  Created by wangnan on 15/11/23.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "NewMessageTableViewCell.h"

@interface NewMessageTableViewCell ()

@property(nonatomic, strong)AsyImageView *headerView;           //头像
@property(nonatomic, strong)AsyImageView *firstKeyView;           //第一帧
@property(nonatomic, strong)UILabel *timeLabel;                 //时间
@property(nonatomic, strong)UILabel *contentLabel;                   //评论
@property(nonatomic, strong)UILabel *nickNameLabel;                  //昵称

@end

@implementation NewMessageTableViewCell

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        //头像
        CGFloat headerWide = 40;
        _headerView = [[AsyImageView alloc] init];
        _headerView.frame = CGRectMake(13, 13, headerWide, headerWide);
        [self addSubview:_headerView];
        
        //第一帧视频
        CGFloat keyWide = 40;
        _firstKeyView = [[AsyImageView alloc] init];
        _firstKeyView.frame = CGRectMake(self.frame.size.width - 15 - keyWide, 13, keyWide, keyWide);
        [self addSubview:_firstKeyView];
        
        //昵称
        CGFloat nameWide = 100;
        _nickNameLabel = [[UILabel alloc] init];
        _nickNameLabel.frame = CGRectMake(_headerView.frame.origin.x + _headerView.frame.size.width + 15, _headerView.frame.origin.y, nameWide, 20);
        _nickNameLabel.textColor = [UIColor blackColor];
        _nickNameLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_nickNameLabel];
        
        //回复内容
        CGFloat contentWide = 200;
        _contentLabel = [[UILabel alloc] init];
        _contentLabel.frame = CGRectMake(_nickNameLabel.frame.origin.x, _nickNameLabel.frame.origin.y + _nickNameLabel.frame.size.height + 10, contentWide, 20);
        _contentLabel.textColor = [UIColor blackColor];
        _contentLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_contentLabel];
        
        //发布时间
        CGFloat timeWide = 100;
        _timeLabel = [[UILabel alloc] init];
        _timeLabel.frame = CGRectMake(_nickNameLabel.frame.origin.x, _contentLabel.frame.origin.y + _contentLabel.frame.size.height + 10, timeWide, 20);
        _timeLabel.textColor = [UIColor blackColor];
        _timeLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_timeLabel];
        
    }
    return self;
}

-(void)setCellWithModel:(NewMessageData *)model{
    
    [_headerView loadImageWithPath:model.faceStr andPlaceHolderName:DefalutHeadImage];
    [_firstKeyView loadImageWithPath:model.firstKey andPlaceHolderName:DefalutHeadImage];
    _nickNameLabel.text = model.nickName;
    _contentLabel.text = model.content;
    _timeLabel.text = model.timeStr;
}

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
