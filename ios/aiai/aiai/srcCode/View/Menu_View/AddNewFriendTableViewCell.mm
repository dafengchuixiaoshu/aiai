//
//  AddNewFriendTableViewCell.m
//  aiai
//
//  Created by zhangyang on 15/2/10.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AddNewFriendTableViewCell.h"


@interface AddNewFriendTableViewCell ()

@property(nonatomic,strong)AsyImageView *headImageView;
@property(nonatomic,strong)UILabel *titleLabel;
@property(nonatomic,strong)UILabel *subTitleLabel;

@property(nonatomic,strong)UIButton *inviteBtn;//邀请label

@end

@implementation AddNewFriendTableViewCell
@synthesize headImageView = _headImageView;
@synthesize titleLabel = _titleLabel;
@synthesize subTitleLabel = _subTitleLabel;
@synthesize inviteBtn = _inviteBtn;
@synthesize addBlock;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        self.backgroundColor = EveryView_BackGroundColor;
        
        CGFloat padding = 15;
        
        _headImageView = [[AsyImageView alloc] init];
        CGFloat HeadImage_Y  = padding;
        CGFloat HeadImageW_H  = 35;
        _headImageView.frame = CGRectMake(padding*2, HeadImage_Y, HeadImageW_H, HeadImageW_H);
        _headImageView.layer.cornerRadius = _headImageView.frame.size.width/2;
        _headImageView.layer.masksToBounds = YES;
        [self addSubview:_headImageView];
        
        CGFloat titleLabel_X = _headImageView.frame.origin.x+_headImageView.frame.size.width+padding;
        CGFloat titleLabel_Y = 0;
        CGFloat titleLabel_H = AddNewFriendTableViewCell_H;
        _titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(titleLabel_X, titleLabel_Y, 150, titleLabel_H)];
        _titleLabel.textColor = [UIColor whiteColor];
        _titleLabel.textAlignment = NSTextAlignmentLeft;
        _titleLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_titleLabel];
        
        //邀请label
        CGFloat inviteLabel_W = 18;
        CGFloat inviteLabel_H = 18;
        CGFloat inviteLabel_X = [UIScreen mainScreen].bounds.size.width - inviteLabel_W - padding*2;
        CGFloat inviteLabel_Y = (AddNewFriendTableViewCell_H - inviteLabel_H)/2;
        _inviteBtn = [[UIButton alloc] initWithFrame:CGRectMake(inviteLabel_X, inviteLabel_Y, inviteLabel_W, inviteLabel_H)];
        [_inviteBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [_inviteBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
        [self addSubview:_inviteBtn];
        
        UIImageView *lineView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        lineView.alpha = 0.5;
        lineView.frame = CGRectMake(13, AddNewFriendTableViewCell_H - 1, CGRectGetWidth([UIScreen mainScreen].bounds) - 13*2, 1);
        [self addSubview:lineView];
    }
    self.selectionStyle = UITableViewCellSelectionStyleNone;
    return self;
}

-(void)add{
    
    if (addBlock) {
        addBlock(_addNewFriendEntity.buddyUserName);
    }
}

-(void)layoutSubviews{
    
    [super layoutSubviews];
}

-(void)setAddNewFriendEntity:(IMPackageBuddyData *)addNewFriendEntity{
    _addNewFriendEntity = addNewFriendEntity;
    
    _titleLabel.text = addNewFriendEntity.buddyNickName;
//    _subTitleLabel.text = @"你好约嘛";//addNewFriendEntity.buddyData.buddyEmotionMood;
    //设置头像
    UIImage* logo = [UIImage imageNamed:addNewFriendEntity.buddyPortraitKey];
    if (logo) {
        self.headImageView.image = logo;
    }else{
        NSString* newUrl = [AppUtils getImageNewUrlWithSrcImageUrl:addNewFriendEntity.buddyPortraitKey width:self.headImageView.frame.size.width height:self.headImageView.frame.size.height];
        [self.headImageView loadImageWithPath:newUrl andPlaceHolderName:DefalutHeadImage];
    }
    
    if (addNewFriendEntity.buddyIsBuddy == IM_BUDDY_NOTBUDDY) {
        
        [_inviteBtn addTarget:self action:@selector(add) forControlEvents:UIControlEventTouchUpInside];
        [_inviteBtn setImage:[UIImage imageNamed:@"addressBook_add@2x.png"] forState:UIControlStateNormal];
        [_inviteBtn setImage:[UIImage imageNamed:@"addressBook_add@2x.png"] forState:UIControlStateHighlighted];

    }else if (addNewFriendEntity.buddyIsBuddy == IM_BUDDY_BUDDY){
        
        [_inviteBtn removeTarget:self action:NULL forControlEvents:UIControlEventTouchUpInside];
        [_inviteBtn setImage:[UIImage imageNamed:@"addressBook_added@2x.png"] forState:UIControlStateNormal];
        [_inviteBtn setImage:[UIImage imageNamed:@"addressBook_added@2x.png"] forState:UIControlStateHighlighted];
    }else{
        
        [_inviteBtn removeTarget:self action:NULL forControlEvents:UIControlEventTouchUpInside];
        [_inviteBtn setImage:[UIImage imageNamed:@"addressBook_add@2x.png"] forState:UIControlStateNormal];
        [_inviteBtn setImage:[UIImage imageNamed:@"addressBook_add@2x.png"] forState:UIControlStateHighlighted];
    }
}

@end
