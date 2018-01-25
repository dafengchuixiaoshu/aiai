//
//  SocialityDetailHeaderView.m
//  aiai
//
//  Created by wangnan on 15/11/13.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "SocialityDetailHeaderView.h"

#import "PlayMovieView.h"

@interface SocialityDetailHeaderView ()

@property(nonatomic,strong)PlayMovieView *playView;     //播放视图
@property(nonatomic,strong)AsyImageView *firstImg;      //第一帧
@property(nonatomic,strong)UILabel *nickLabel;          //昵称
@property(nonatomic,strong)UIButton *praiseBtn;         //赞
@property(nonatomic,strong)UIButton *commentCountBtn;   //评论
@property(nonatomic,strong)UILabel *textLabel;          //文字内容
@property(nonatomic,strong)UILabel *timeLabel;          //时间

@end

@implementation SocialityDetailHeaderView

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        
        //播放视图
        _playView = [[PlayMovieView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, self.frame.size.width)];
        [self addSubview:_playView];
        
        _firstImg = [[AsyImageView alloc] init];
        _firstImg.frame = _playView.frame;
        [self addSubview:_firstImg];
        
        //用户头像
        CGFloat perWide = 40;
        _perHeader = [[UserHeaderView alloc] init];
        _perHeader.backgroundColor = [UIColor clearColor];
        _perHeader.frame = CGRectMake(0, _playView.frame.origin.y + _playView.frame.size.height - 20, perWide, perWide);
        [self addSubview:_perHeader];
        
        //赞
        CGFloat praiseWide = 40;
        CGFloat praiseHeight = 40;
        _praiseBtn = [[UIButton alloc] init];
        _praiseBtn.frame = CGRectMake(_perHeader.frame.origin.x + _perHeader.frame.size.width + 10, _perHeader.frame.origin.y, praiseWide, praiseHeight);
        _praiseBtn.titleLabel.font = [UIFont systemFontOfSize:14];
        _praiseBtn.titleLabel.textColor = [UIColor whiteColor];
        [_praiseBtn addTarget:self action:@selector(praiseAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_praiseBtn];
        
        //评论
        CGFloat commentWide = 40;
        CGFloat commentHeight = 40;
        _commentCountBtn = [[UIButton alloc] init];
        _commentCountBtn.frame = CGRectMake(_praiseBtn.frame.origin.x + _praiseBtn.frame.size.width + 10, _perHeader.frame.origin.y, commentWide, commentHeight);
        _commentCountBtn.titleLabel.font = [UIFont systemFontOfSize:14];
        _commentCountBtn.titleLabel.textColor = [UIColor whiteColor];
        [_commentCountBtn addTarget:self action:@selector(commentAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_commentCountBtn];
        
        //再创作
        CGFloat recreatBtnWidth = 40;
        CGFloat recreatBtnHeight = 40;
        UIButton *recreatBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        recreatBtn.frame = CGRectMake(_commentCountBtn.frame.origin.x + _commentCountBtn.frame.size.width + 10, _perHeader.frame.origin.y, recreatBtnWidth, recreatBtnHeight);
        [recreatBtn setTitle:@"再创作" forState:UIControlStateNormal];
        [recreatBtn setTitle:@"再创作" forState:UIControlStateHighlighted];
        recreatBtn.titleLabel.font = [UIFont systemFontOfSize:14];
        [recreatBtn addTarget:self action:@selector(recreatAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:recreatBtn];
        
        //分享
        CGFloat shareBtnWide = 40;
        CGFloat shareBtnHeight = 40;
        UIButton *shareBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        shareBtn.frame = CGRectMake(recreatBtn.frame.origin.x + recreatBtn.frame.size.width + 10, _perHeader.frame.origin.y, shareBtnWide, shareBtnHeight);
        [shareBtn setTitle:@"分享" forState:UIControlStateNormal];
        [shareBtn setTitle:@"分享" forState:UIControlStateHighlighted];
        shareBtn.titleLabel.font = [UIFont systemFontOfSize:14];
        [shareBtn addTarget:self action:@selector(shareAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:shareBtn];
        
        //昵称
        CGFloat nickeWide = 140;
        CGFloat nickeHeight = 40;
        _nickLabel = [[UILabel alloc] init];
        _nickLabel.frame = CGRectMake(0, _perHeader.frame.origin.y + _perHeader.frame.size.height, nickeWide, nickeHeight);
        _nickLabel.font = [UIFont systemFontOfSize:14];
        _nickLabel.textColor = [UIColor whiteColor];
        [self addSubview:_nickLabel];
        
        //时间
        CGFloat timeWide = 140;
        CGFloat timeHeight = 40;
        _timeLabel = [[UILabel alloc] init];
        _timeLabel.frame = CGRectMake(self.frame.size.width - timeWide - 10, _nickLabel.frame.origin.y, timeWide, timeHeight);
        _timeLabel.font = [UIFont systemFontOfSize:14];
        _timeLabel.textColor = [UIColor whiteColor];
        [self addSubview:_timeLabel];
        
        //文字内容
        CGFloat textWide = 240;
        CGFloat textHeight = 40;
        _textLabel = [[UILabel alloc] init];
        _textLabel.frame = CGRectMake(0, _nickLabel.frame.origin.y + _nickLabel.frame.size.height, textWide, textHeight);
        _textLabel.font = [UIFont systemFontOfSize:14];
        _textLabel.textColor = [UIColor whiteColor];
        [self addSubview:_textLabel];
        
    }
    
    self.backgroundColor = [UIColor clearColor];
    return self;
}

#pragma mark --- btnAction
-(void)recreatAction:(id)sender{
    
    NSLog(@"点到了再创作");
}

-(void)shareAction:(id)sender{
    NSLog(@"点到了分享");
}

-(void)commentAction:(id)sender{
    
}

-(void)praiseAction:(id)sender{
    if (_praiseBlock) {
        _praiseBlock();
    }
}

#pragma mark -- setMethod
-(void)setVideoUrl:(NSString *)videoUrl{
    _videoUrl = [videoUrl copy];
    
    if (_playView) {
        [_playView stopMovie];
        
        [AppUtils videoPathWitSourceUrl:videoUrl ready:^{
            [WaitingAlertView showWaitingViewAtCenterView:_playView];
        } complate:^(NSString *videoPath) {
            [WaitingAlertView hideWaitingView];
            //添加播放器
            _playView.moviePath = videoPath;
            //开始播放 去掉第一帧图片
            [_firstImg removeFromSuperview];
            [_playView playMovie];
            
        } failed:^(NSError *error) {
            [WaitingAlertView hideWaitingView];
        }];
    }
}

-(void)setNickName:(NSString *)nickName{
    if (nickName) {
        _nickLabel.text = nickName;
    }
}

-(void)setPraiseCount:(NSString *)praiseCount{
    if (praiseCount) {
        
        [_praiseBtn setTitle:praiseCount forState:UIControlStateNormal];
        [_praiseBtn setTitle:praiseCount forState:UIControlStateHighlighted];
    }
}

-(void)setCommentCount:(NSString *)commentCount{
    if (commentCount) {
        [_commentCountBtn setTitle:commentCount forState:UIControlStateNormal];
        [_commentCountBtn setTitle:commentCount forState:UIControlStateHighlighted];
    }
}

-(void)setPerText:(NSString *)perText{
    if (perText) {
        _textLabel.text = perText;
    }
}

-(void)setSendTime:(NSString *)sendTime{
    if (sendTime) {
        _timeLabel.text = sendTime;
    }
}

-(void)setFirstImgUrl:(NSString *)firstImgUrl{
    if (firstImgUrl) {
        [_firstImg loadImageWithPath:firstImgUrl andPlaceHolderName:DefalutHeadImage];
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
