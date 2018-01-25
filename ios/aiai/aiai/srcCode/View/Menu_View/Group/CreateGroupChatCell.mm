//
//  CreateGroupChatCell.m
//  F2CPlatform
//
//  Created by Circle Liu on 14-7-20.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "CreateGroupChatCell.h"

#import "AsyImageView.h"

#import "IMPackageData.h"

@interface CreateGroupChatCell ()

@property(nonatomic, strong)UIImageView     *selectImageView;
@property(nonatomic, strong)UIImageView     *lineImageView;
@property(nonatomic, strong)UILabel         *nameLabel;
@property(nonatomic, strong)AsyImageView    *avatarImageView;


@property(nonatomic, strong)IMPackageBuddyData   *userData;

@property(nonatomic, assign)BOOL            existingMember;

@end

@implementation CreateGroupChatCell
@synthesize selectImageView,avatarImageView,nameLabel,lineImageView;
@synthesize existingMember,userData;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
        
        CGFloat space = 15;
        CGFloat faceHeight = 40;
        CGFloat imgHeight = 20;
        
        UIImage *selectImg = [UIImage imageNamed:@"contact_unselected@2x.png"];
        selectImageView = [[UIImageView alloc] initWithFrame:CGRectMake(15, (INITIATE_CELLHEIGHT-imgHeight)/2, imgHeight, imgHeight)];
        [self addSubview:selectImageView];
        selectImageView.image = selectImg;
        
        avatarImageView = [[AsyImageView alloc] init];
        avatarImageView.frame = CGRectMake(selectImageView.frame.origin.x+selectImageView.frame.size.width+space, (INITIATE_CELLHEIGHT-faceHeight)/2, faceHeight, faceHeight);
        avatarImageView.layer.cornerRadius = avatarImageView.frame.size.width/2;
        avatarImageView.layer.masksToBounds = YES;
        [self addSubview:avatarImageView];
        
        nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(avatarImageView.frame.origin.x+avatarImageView.frame.size.width + space, 0, 200, INITIATE_CELLHEIGHT)];
        nameLabel.backgroundColor = [UIColor clearColor];
        nameLabel.textColor = [UIColor whiteColor];
        nameLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:nameLabel];
        
        lineImageView = [[UIImageView alloc] initWithFrame:CGRectMake(13, INITIATE_CELLHEIGHT-1, [UIScreen mainScreen].bounds.size.width - 13*2, 1)];
        lineImageView.image = [UIImage imageNamed:COMMON_CUTLINE];
        lineImageView.alpha = 0.5;
        [self addSubview:lineImageView];
        
        self.backgroundColor = EveryView_BackGroundColor;
    }
    return self;
}

- (void)awakeFromNib
{
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
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

- (void)prepareForReuse {
    [super prepareForReuse];
}

#pragma mark Public Method

- (void)fillData:(IMPackageBuddyData*)model andIsShowTheCutLine:(BOOL)isShow{
    
    userData = model;

    [avatarImageView loadImageWithPath:[AppUtils getImageNewUrlWithSrcImageUrl:model.buddyPortraitKey width:0 height:0] andPlaceHolderName:DefalutHeadImage];
    lineImageView.hidden = !isShow;
    
    nameLabel.text = model.buddyNickName;
    
}

- (void)changeSelectStatus:(BOOL)selected {
    selectImageView.image = selected ? [UIImage imageNamed:@"contact_selected@2x.png"] : [UIImage imageNamed:@"contact_unselected@2x.png"];
}

- (void)existingMembers:(BOOL)existed {
    selectImageView.image = existed ? [UIImage imageNamed:@"contact_selected@2x.png"] : [UIImage imageNamed:@"contact_unselected@2x.png"];
    existingMember = existed;
}

- (IMPackageBuddyData*)userData {
    return userData;
}

- (BOOL)isExisted {
    return existingMember;
}

@end
