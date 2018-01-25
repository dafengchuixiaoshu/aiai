//
//  UserPageHeaderView.m
//  aiai
//
//  Created by wangnan on 15/11/19.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "UserPageHeaderView.h"
#import "UserHeaderView.h"

@interface UserPageHeaderView ()
@property(nonatomic, strong)UserHeaderView *faceView;
@property(nonatomic, strong)UILabel *nickNameLabel;
@property(nonatomic, strong)UILabel *signatureLabel;            //签名
@property(nonatomic, strong)UIButton *attention;                //关注按钮
@end

@implementation UserPageHeaderView

-(id)initWithFrame:(CGRect)frame{

    if (self = [super initWithFrame:frame]) {
        
        //用户头像
        CGFloat perWide = 40;
        _faceView = [[UserHeaderView alloc] init];
        _faceView.backgroundColor = [UIColor clearColor];
        _faceView.frame = CGRectMake(0, self.frame.size.height - perWide - 20, perWide, perWide);
        [self addSubview:_faceView];
        
        //昵称
        CGFloat nickeWide = 140;
        CGFloat nickeHeight = 20;
        _nickNameLabel = [[UILabel alloc] init];
        _nickNameLabel.frame = CGRectMake(_faceView.frame.origin.x + _faceView.frame.size.width, _faceView.frame.origin.y, nickeWide, nickeHeight);
        _nickNameLabel.font = [UIFont systemFontOfSize:14];
        _nickNameLabel.textColor = [UIColor whiteColor];
        [self addSubview:_nickNameLabel];
        
        //签名
        CGFloat signatureWide = 140;
        CGFloat signatureHeight = 20;
        _signatureLabel = [[UILabel alloc] init];
        _signatureLabel.frame = CGRectMake(_nickNameLabel.frame.origin.x , _nickNameLabel.frame.origin.y + _nickNameLabel.frame.size.height, signatureWide, signatureHeight);
        _signatureLabel.font = [UIFont systemFontOfSize:14];
        _signatureLabel.textColor = [UIColor whiteColor];
        [self addSubview:_signatureLabel];
        
        //关注
        CGFloat attentionBtnWide = 40;
        CGFloat attentionBtnHeight = 40;
        _attention = [UIButton buttonWithType:UIButtonTypeCustom];
        _attention.frame = CGRectMake(self.frame.size.width - attentionBtnWide - 10, _faceView.frame.origin.y, attentionBtnWide, attentionBtnHeight);
        [_attention setTitle:@"关注" forState:UIControlStateNormal];
        [_attention setTitle:@"关注" forState:UIControlStateHighlighted];
        _attention.titleLabel.font = [UIFont systemFontOfSize:14];
        [_attention addTarget:self action:@selector(attentionAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_attention];
    }
    return self;
}

-(void)attentionAction:(id)sender{
    
}

#pragma mark -- set Method
-(void)setNickNameStr:(NSString *)nickNameStr{
    if (nickNameStr) {
        _nickNameLabel.text = nickNameStr;
    }
}

-(void)setFaceUrl:(NSString *)faceUrl{
    if (faceUrl) {
        [_faceView loadImageWithPath:faceUrl andPlaceHolderName:DefalutHeadImage];
    }
}

-(void)setSignatureStr:(NSString *)signatureStr{
    if (signatureStr) {
        _signatureLabel.text = signatureStr;
    }
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
