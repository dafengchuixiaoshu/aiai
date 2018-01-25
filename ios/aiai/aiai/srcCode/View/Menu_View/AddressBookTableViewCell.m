//
//  AddressBookTableViewCell.m
//  aiai
//
//  Created by wangnan on 15/4/10.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AddressBookTableViewCell.h"

#define PADDING         13
#define HeadImageW_H    40
#define Label_H         20

@interface AddressBookTableViewCell ()

@property(nonatomic, strong)AsyImageView *headImageView;
@property(nonatomic, strong)UILabel *nickNameLabel;
@property(nonatomic, strong)UILabel *phoneNameLabel;
@property(nonatomic, strong)UIButton *actionBtn;

@end

@implementation AddressBookTableViewCell
@synthesize headImageView = _headImageView;
@synthesize nickNameLabel = _nickNameLabel;
@synthesize phoneNameLabel = _phoneNameLabel;
@synthesize actionBtn = _actionBtn;
@synthesize addBuddyBlock,invitationBlock;

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        _headImageView = [[AsyImageView alloc] init];
        CGFloat HeadImage_Y  = PADDING;
        _headImageView.frame = CGRectMake(PADDING, HeadImage_Y, HeadImageW_H, HeadImageW_H);
        _headImageView.layer.cornerRadius = _headImageView.frame.size.width/2;
        _headImageView.layer.masksToBounds = YES;
        [self addSubview:_headImageView];
        
        //昵称
        CGFloat nickName_X = _headImageView.frame.origin.x+_headImageView.frame.size.width+PADDING;
        CGFloat nickName_Y = PADDING;
        _nickNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(nickName_X, nickName_Y, 200, Label_H)];
        _nickNameLabel.textColor = [UIColor whiteColor];
        _nickNameLabel.textAlignment = NSTextAlignmentLeft;
        _nickNameLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_nickNameLabel];
        
        //手机联系人昵称
        CGFloat phoneName_X = _headImageView.frame.origin.x+_headImageView.frame.size.width+PADDING;
        CGFloat phoneName_Y = _nickNameLabel.frame.origin.y+_nickNameLabel.frame.size.height;
        CGFloat phoneName_H = 20;
        _phoneNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(phoneName_X, phoneName_Y, 200, phoneName_H)];
        _phoneNameLabel.textColor = [UIColor whiteColor];
        _phoneNameLabel.textAlignment = NSTextAlignmentLeft;
        _phoneNameLabel.font = [UIFont systemFontOfSize:16];
        _phoneNameLabel.alpha = 0.4;
        [self addSubview:_phoneNameLabel];
        
        UIImage *btnImg = [UIImage imageNamed:@"addressBook_add@2x"];
        _actionBtn = [[UIButton alloc] initWithFrame:CGRectMake([UIScreen mainScreen].bounds.size.width - PADDING*2 - btnImg.size.width, 0, PADDING*2 + btnImg.size.width, ADDRESSBOOKCELLHEIGHT)];
        _actionBtn.backgroundColor = [UIColor clearColor];
        [_actionBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [_actionBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
        _actionBtn.titleLabel.font = [UIFont systemFontOfSize:16];
        [self addSubview:_actionBtn];
        
        UIImageView *lineView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        lineView.frame = CGRectMake(PADDING, ADDRESSBOOKCELLHEIGHT - 1, [UIScreen mainScreen].bounds.size.width - PADDING*2, 1);
        lineView.alpha = 0.5;
        [self addSubview:lineView];
    }
    self.selectionStyle = UITableViewCellSelectionStyleNone;
    self.backgroundColor = EveryView_BackGroundColor;
    return self;
}

- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated{
    if (highlighted) {
        self.backgroundColor = Every_YellowColor;
    }else{
        self.backgroundColor = EveryView_BackGroundColor;
    }
}

-(void)setCellWithFaceString:(NSString*)faceUrl buddyNickName:(NSString*)buddyNickName phoneBookNick:(NSString*)phoneNickName contaceType:(Buddystatus)type{
    
    [_headImageView loadImageWithPath:faceUrl andPlaceHolderName:DefalutHeadImage];
    
    _nickNameLabel.text = buddyNickName? :@"";
    
    [self reSetActionButton];
    switch (type) {
        case notFriend:
        {
            [_actionBtn setImage:[UIImage imageNamed:@"addressBook_add.png"] forState:UIControlStateNormal];
            [_actionBtn setImage:[UIImage imageNamed:@"addressBook_add.png"] forState:UIControlStateHighlighted];
            [_actionBtn addTarget:self action:@selector(addContact) forControlEvents:UIControlEventTouchUpInside];
            //不想再建一个成员变量 , 就在这里setframe了
            _phoneNameLabel.text = [NSString stringWithFormat:@"手机联系人:%@",phoneNickName];
            _phoneNameLabel.frame = CGRectMake(_headImageView.frame.origin.x+_headImageView.frame.size.width+PADDING, _nickNameLabel.frame.origin.y+_nickNameLabel.frame.size.height, 200, Label_H);
        }
            break;
        case noRegistered:
        {
            [_actionBtn setTitle:@"邀请" forState:UIControlStateNormal];
            [_actionBtn setTitle:@"邀请" forState:UIControlStateHighlighted];
            [_actionBtn addTarget:self action:@selector(invitation) forControlEvents:UIControlEventTouchUpInside];
            
            _phoneNameLabel.text = [NSString stringWithFormat:@"%@",phoneNickName];
            _phoneNameLabel.frame = CGRectMake(_headImageView.frame.origin.x+_headImageView.frame.size.width+PADDING, PADDING, 200, Label_H*2);
        }
            break;
        case isFriend:
        {
            [_actionBtn setImage:[UIImage imageNamed:@"addressBook_added.png"] forState:UIControlStateNormal];
            [_actionBtn setImage:[UIImage imageNamed:@"addressBook_added.png"] forState:UIControlStateHighlighted];
            
            _phoneNameLabel.text = [NSString stringWithFormat:@"手机联系人:%@",phoneNickName];
            _phoneNameLabel.frame = CGRectMake(_headImageView.frame.origin.x+_headImageView.frame.size.width+PADDING, _nickNameLabel.frame.origin.y+_nickNameLabel.frame.size.height, 200, Label_H);
        }
            break;
        default:
            break;
    }
}

#pragma mark buttonAction
-(void)addContact{
    
    [self reSetActionButton];
    [_actionBtn setImage:[UIImage imageNamed:@"addressBook_added.png"] forState:UIControlStateNormal];
    [_actionBtn setImage:[UIImage imageNamed:@"addressBook_added.png"] forState:UIControlStateHighlighted];
    if (addBuddyBlock) {
        addBuddyBlock();
    }
}

-(void)invitation{
    
    if (invitationBlock) {
        invitationBlock();
    }
}

-(void)reSetActionButton{
    
    [_actionBtn setImage:nil forState:UIControlStateNormal];
    [_actionBtn setImage:nil forState:UIControlStateHighlighted];
    [_actionBtn setTitle:@"" forState:UIControlStateNormal];
    [_actionBtn setTitle:@"" forState:UIControlStateHighlighted];
    [_actionBtn removeTarget:self action:NULL forControlEvents:UIControlEventTouchUpInside];
    
}

@end
