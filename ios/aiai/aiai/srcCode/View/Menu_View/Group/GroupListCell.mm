//
//  GroupListCell.m
//  aiai
//
//  Created by wangnan on 15/3/31.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "GroupListCell.h"

@interface GroupListCell ()

@property(nonatomic, strong)AsyImageView   *avatarImageView;        //群头像
@property(nonatomic, strong)UILabel        *badgeLabel;             //群名称
@property(nonatomic, strong)UILabel        *memberCountLabel;            //成员数量label

@end

@implementation GroupListCell
@synthesize avatarImageView;
@synthesize badgeLabel;
@synthesize memberCountLabel;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        CGFloat space = 13;
        
        //头像
        CGFloat padding_TB = 13;
        CGFloat padding_LR = 13;
        
        CGFloat headImageView_H = 40;
        avatarImageView = [[AsyImageView alloc] init];
        avatarImageView.frame = CGRectMake(padding_LR, padding_TB, headImageView_H, headImageView_H);
        avatarImageView.backgroundColor = [UIColor clearColor];
        avatarImageView.layer.cornerRadius = avatarImageView.frame.size.width/2;
        avatarImageView.layer.masksToBounds = YES;
        [self addSubview:avatarImageView];
        
        //群组名
        CGFloat nameLabel_X = avatarImageView.frame.origin.x+avatarImageView.frame.size.width+space;
        CGFloat nameLabel_Y = 0;
        CGFloat nameLabel_W = 200;
        CGFloat nameLabel_H = GROUPLISTCELLHEIGHT;
        badgeLabel = [[UILabel alloc] initWithFrame:CGRectMake(nameLabel_X, nameLabel_Y, nameLabel_W, nameLabel_H)];
        badgeLabel.textAlignment = NSTextAlignmentLeft;
        badgeLabel.backgroundColor = [UIColor clearColor];
        badgeLabel.font = [UIFont systemFontOfSize:16];
        badgeLabel.textColor = [UIColor whiteColor];
        [self addSubview:badgeLabel];
        
        //群组数量
        CGFloat number_W = 100;
        CGFloat number_H = GROUPLISTCELLHEIGHT;
        CGFloat number_x = [UIScreen mainScreen].bounds.size.width - number_W - padding_LR;
        CGFloat number_Y = 0;
        memberCountLabel = [[UILabel alloc] init];
        memberCountLabel.frame = CGRectMake(number_x, number_Y, number_W, number_H);
        memberCountLabel.backgroundColor = [UIColor clearColor];
        memberCountLabel.textColor = [UIColor whiteColor];
        memberCountLabel.textAlignment = NSTextAlignmentRight;
        memberCountLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:memberCountLabel];
        
        UIImageView *lineView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        lineView.frame = CGRectMake(padding_LR, GROUPLISTCELLHEIGHT-1, CGRectGetWidth([UIScreen mainScreen].bounds) - padding_LR*2, 1);
        lineView.alpha = 0.5;
        [self.contentView addSubview:lineView];
        
        self.backgroundColor = EveryView_BackGroundColor;
        self.selectionStyle = UITableViewCellSelectionStyleNone;
    }
    
    return self;
}

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated{
    if (highlighted) {
        self.backgroundColor = Every_YellowColor;
    }else{
        self.backgroundColor = EveryView_BackGroundColor;
    }
}


-(void)setGroupCellWithGroupData:(IMPackageGroupData*)groupData{
    [avatarImageView loadImageWithPath:[AppUtils getImageNewUrlWithSrcImageUrl:groupData.groupPortraitKey width:40 height:40] andPlaceHolderName:DefalutHeadImage];
    
    badgeLabel.text = groupData.groupName;
    
    memberCountLabel.text = [NSString stringWithFormat:@"%@人",groupData.groupMemberCount];
}

@end
