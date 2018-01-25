//
//  RootTableHeaderView.m
//  aiai
//
//  Created by zhangyang on 14/12/13.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "MenuTableHeaderView.h"
#import "UserHeaderView.h"

@interface MenuTableHeaderView ()

@property(nonatomic,strong)UserHeaderView* headImageView;
@property(nonatomic,strong)UIButton* nickNameButton;
@property(nonatomic,strong)UISwitch *statusSwith;
@property(nonatomic,strong)UILabel *locationLabel;

@end

@implementation MenuTableHeaderView
@synthesize headImageView = _headImageView;
@synthesize nickNameButton = _nickNameButton;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        _headImageView = [[UserHeaderView alloc] init];
        CGFloat HeadImageW_H  = 80;
        _headImageView.frame = CGRectMake(0, 130, HeadImageW_H, HeadImageW_H);
        
        __weak UserHeaderView *weakHeader = _headImageView;
        _headImageView.doubleTapBlock = ^(UIGestureRecognizer* recognizer){
            [weakHeader playUserHeaderVideo];
        };
        _headImageView.startPlayBlock = ^(){
            [weakHeader beginAnimating];
        };
        _headImageView.stopPlayBlock = ^(){
            [weakHeader resetAnimating];
        };
        [self addSubview:_headImageView];
        
        _nickNameButton = [[UIButton alloc] initWithFrame:CGRectMake(0, _headImageView.frame.origin.y + _headImageView.frame.size.height + 10, 150, 44)];
        [_nickNameButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        _nickNameButton.titleLabel.font = [UIFont boldSystemFontOfSize:20];
        [_nickNameButton addTarget:self action:@selector(nickNameBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_nickNameButton];
        
        _locationLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, 40)];
        _locationLabel.font = [UIFont systemFontOfSize:20];
        _locationLabel.text = @"位置:";
        _locationLabel.textColor = [UIColor whiteColor];
        [self addSubview:_locationLabel];
        
        _statusSwith = [[UISwitch alloc] initWithFrame:CGRectMake(_nickNameButton.frame.origin.x, _nickNameButton.frame.origin.y + _nickNameButton.frame.size.height + 10, 40, 40)];
        _statusSwith.onTintColor = Every_YellowColor;
        [_statusSwith addTarget:self action:@selector(switchAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_statusSwith];
        
    }
    return self;
}

-(void)nickNameBtnAction:(id)sender{
    if (self.nickNameBtnBlock) {
        self.nickNameBtnBlock();
    }
}

-(void)switchAction:(id)sender{
    if (_setStatusBlock) {
        _setStatusBlock(_statusSwith.on);
    }
}

-(void)layoutSubviews{
    _headImageView.center = self.center;
    CGRect rect = _headImageView.frame;
    rect.origin.y = 69;
    _headImageView.frame = rect;

    _nickNameButton.frame = CGRectMake(0, _headImageView.frame.origin.y + _headImageView.frame.size.height + 10, self.frame.size.width, 44);
    
    _locationLabel.frame = CGRectMake(_headImageView.frame.origin.x, _nickNameButton.frame.origin.y + _nickNameButton.frame.size.height + 10, 50, _locationLabel.frame.size.height);
    
    _statusSwith.center = self.center;
    rect = _statusSwith.frame;
    rect.origin.x = _locationLabel.frame.origin.x + _locationLabel.frame.size.width + 2;
    rect.origin.y = _nickNameButton.frame.origin.y + _nickNameButton.frame.size.height + 10;
    _statusSwith.frame = rect;
}

-(void)setHeadImageUrl:(NSString *)headImageUrl{
    NSString* imageurl = [AppUtils getImageNewUrlWithSrcImageUrl:headImageUrl width:_headImageView.frame.size.width height:_headImageView.frame.size.height];
    [_headImageView loadImageWithPath:imageurl andPlaceHolderName:DefalutHeadImage];
}

-(void)setHeadVedioUrl:(NSString *)headVedioUrl{
    _headImageView.headerVideoPath = headVedioUrl;
}

-(void)setNickName:(NSString *)nickName{
    _nickName = [nickName copy];
    [_nickNameButton setTitle:nickName forState:UIControlStateNormal];
}

-(void)setSwitchStatus:(NSInteger)switchStatus{
    _statusSwith.on = switchStatus == 1 ? YES:NO;
}

@end
