//
//  SearchResultCell.m
//  aiai
//
//  Created by wangnan on 15/3/27.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "SearchResultCell.h"
#import "IMPackageData.h"

@interface SearchResultCell ()

@property(nonatomic, strong)AsyImageView *headerView;       //头像
@property(nonatomic, strong)UILabel *userNickLabel;         //昵称
@property(nonatomic, strong)UILabel *phoneLabel;            //电话号码

@property(nonatomic, strong)UIButton *addBtn;               //添加按钮
@property(nonatomic, strong)IMPackageBuddyData *cellModel;       //cell的数据源

@end

@implementation SearchResultCell
@synthesize addBlock;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    if (self  = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        CGFloat padding = 13;
        CGFloat HeadImage_Y  = padding;
        CGFloat HeadImageW_H  = 35;
        
        //头像
        self.headerView = [[AsyImageView alloc] init];
        self.headerView.frame = CGRectMake(padding*2, HeadImage_Y, HeadImageW_H, HeadImageW_H);
        self.headerView.layer.cornerRadius = self.headerView.frame.size.width/2;
        self.headerView.layer.masksToBounds = YES;
        [self addSubview:self.headerView];
        
        //用户名
        CGFloat titleLabel_X = self.headerView.frame.origin.x+self.headerView.frame.size.width+padding;
        CGFloat titleLabel_Y = 6;
        CGFloat titleLabel_H = 25;
        
        self.userNickLabel = [[UILabel alloc] initWithFrame:CGRectMake(titleLabel_X, titleLabel_Y, 200, titleLabel_H)];
        self.userNickLabel.textAlignment = NSTextAlignmentLeft;
        self.userNickLabel.backgroundColor = EveryView_BackGroundColor;
        self.userNickLabel.textColor = [UIColor whiteColor];
        self.userNickLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:self.userNickLabel];
        
        //手机号
        CGFloat subTitleLabel_X = self.userNickLabel.frame.origin.x;
        CGFloat subTitleLabel_Y = self.userNickLabel.frame.origin.y+self.userNickLabel.frame.size.height;
        CGFloat subTitleLabel_H = 20;
        
        self.phoneLabel = [[UILabel alloc] initWithFrame:CGRectMake(subTitleLabel_X, subTitleLabel_Y, 200, subTitleLabel_H)];
        self.phoneLabel.textAlignment = NSTextAlignmentLeft;
        self.phoneLabel.backgroundColor = EveryView_BackGroundColor;
        self.phoneLabel.font = [UIFont systemFontOfSize:14];
        self.phoneLabel.alpha = 0.4;
        [self addSubview:self.phoneLabel];
        
        //添加按钮
        CGFloat addButton_W = 18;
        CGFloat addButton_H = 18;
        CGFloat addButton_X = [UIScreen mainScreen].bounds.size.width - addButton_W - padding*2;
        CGFloat addButton_Y = self.headerView.center.y - addButton_H/2;
        self.addBtn = [[UIButton alloc] init];
        self.addBtn.frame = CGRectMake(addButton_X, addButton_Y, addButton_W, addButton_H);
        self.addBtn.backgroundColor = [UIColor clearColor];
        self.addBtn.titleLabel.font = [UIFont systemFontOfSize:20];
        [self.addBtn addTarget:self action:@selector(addACtion) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:self.addBtn];
        
        UIImageView *cutLine = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        cutLine.frame = CGRectMake(padding, CELLHEIGHT - 1, CGRectGetWidth([UIScreen mainScreen].bounds) - padding *2, 1);
        cutLine.alpha = 0.5;
        [self addSubview:cutLine];
        
        self.backgroundColor = EveryView_BackGroundColor;
        self.selectionStyle = UITableViewCellSelectionStyleNone;
    }
    return self;
}

-(void)setCellViewWithModel:(IMPackageBuddyData*)entity andSearchStr:(NSString*)searchStr{
    
    self.cellModel = entity;
    searchStr = [searchStr lowercaseString];
    //加载头像
    NSString* imageurl = [AppUtils getImageNewUrlWithSrcImageUrl:entity.buddyPortraitKey width:self.headerView.frame.size.width height:self.headerView.frame.size.height];
    [self.headerView loadImageWithPath:imageurl andPlaceHolderName:DefalutHeadImage];
    
//    NSRange nameRange = [entity.buddyUserName rangeOfString:searchStr];
//    if (nameRange.length>0) {
//        
//        NSDictionary* style = @{@"body":self.userNickLabel.font,
//                                 @"bold":self.userNickLabel.font,
//                                 @"color":[UIColor yellowColor]};
//        self.userNickLabel.attributedText = [[NSString stringWithFormat:@"%@<color>%@</color>%@",[entity.buddyUserName substringToIndex:nameRange.location],searchStr,[entity.buddyUserName substringFromIndex:nameRange.location+nameRange.length]] attributedStringWithStyleBook:style];
//        self.phoneLabel.hidden = YES;
//        
//    }else{

    NSDictionary* style = @{@"body":self.phoneLabel.font,
                            @"bold":self.phoneLabel.font,
                            @"color":[UIColor yellowColor]};
//    self.userNickLabel.attributedText = [[NSString stringWithFormat:@"%@",entity.buddyUserName] attributedStringWithStyleBook:style];
//    self.phoneLabel.hidden = NO;
    
    self.userNickLabel.text = entity.buddyNickName;
    
    NSRange phoneRange = [entity.buddyMobile rangeOfString:searchStr];
    if (phoneRange.length == 0) {
        self.phoneLabel.hidden = YES;
    }else{
        self.phoneLabel.hidden = NO;
        
        self.phoneLabel.attributedText = [[NSString stringWithFormat:@"%@<color>%@</color>%@",[entity.buddyMobile substringToIndex:phoneRange.location],searchStr,[entity.buddyMobile substringFromIndex:phoneRange.location+phoneRange.length]] attributedStringWithStyleBook:style];
    }
//    }
    
    if (entity.buddyIsBuddy==IM_BUDDY_BUDDY || entity.buddyIsBuddy==IM_BUDDY_BUDDYFANS || [entity.buddyUserName isEqualToString:[GlobalUserInfo myselfInfo].userName]) {
        [self.addBtn setBackgroundImage:[UIImage imageNamed:@"addressBook_added.png"] forState:UIControlStateNormal];
        [self.addBtn setBackgroundImage:[UIImage imageNamed:@"addressBook_added.png"] forState:UIControlStateHighlighted];
        self.addBtn.userInteractionEnabled = NO;
    }else{
        [self.addBtn setBackgroundImage:[UIImage imageNamed:@"addressBook_add.png"] forState:UIControlStateNormal];
        [self.addBtn setBackgroundImage:[UIImage imageNamed:@"addressBook_add.png"] forState:UIControlStateHighlighted];
        self.addBtn.userInteractionEnabled = YES;
    }
    
}

-(void)addACtion{
    if (addBlock) {
        addBlock(self.cellModel.buddyUserName);
    }
}

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
