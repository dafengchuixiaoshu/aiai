//
//  UserHeaderView.m
//  aiai
//
//  Created by zhangyang on 15/4/9.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "UserHeaderView.h"
#import "IMPackageData.h"

#define ANIMATECOUNT        5
#define ANIMATEDURATION     3

@interface UserHeaderView ()
@property(nonatomic,strong)MPMoviePlayerController* movieplayer;
@property(nonatomic,strong)UIImageView *animateImgView;

@end

@implementation UserHeaderView

-(id)init{
    if (self = [super init]) {
        
        //增加单击和双击手势
        UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(singleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(doubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        //这行很关键，意思是只有当没有检测到doubleTapGestureRecognizer 或者 检测doubleTapGestureRecognizer失败，singleTapGestureRecognizer才有效
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    //圆角
    CAShapeLayer *layer = [CAShapeLayer layer];
    UIBezierPath *aPath = [UIBezierPath bezierPathWithOvalInRect:self.bounds];
    layer.path = aPath.CGPath;
    self.layer.mask = layer;
}

-(void)setHeaderVideoPath:(NSString *)headerVideoPath{
    _headerVideoPath = headerVideoPath;
}

- (void)singleTap:(UIGestureRecognizer*)gestureRecognizer
{
    if (self.singleTapBlock) {
        self.singleTapBlock(gestureRecognizer);
    }
}

- (void)doubleTap:(UIGestureRecognizer*)gestureRecognizer
{
    if (self.doubleTapBlock) {
        self.doubleTapBlock(gestureRecognizer);
    }
}


-(void)playUserHeaderVideo{
    if (_headerVideoPath == nil) {
        if (_groupMemberArray && ![self.animateImgView isAnimating]) {
            [self playGroupHeaderWithArray:_groupMemberArray];
        }else{
            AIAI_LOG(@"头像视频路径为空");
        }
        return;
    }
    
    [AppUtils videoPathWitSourceUrl:_headerVideoPath ready:^{
        [WaitingAlertView showFullWaitingViewAtView:self];
    } complate:^(NSString *videoPath) {
        [WaitingAlertView hideWaitingView];
        
        if (!_movieplayer) {
            //添加播放器
            _movieplayer = [[MPMoviePlayerController alloc] init];
            _movieplayer.movieSourceType = MPMovieSourceTypeFile;//必须设置，否则不能播放
            _movieplayer.controlStyle = MPMovieControlStyleNone;//隐藏系统控件
            _movieplayer.shouldAutoplay = NO;
            _movieplayer.repeatMode = MPMovieRepeatModeNone;//这个属性不知道为啥不好用，所以只好设置成none，用消息来完成循环
            _movieplayer.view.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.width);
        }
        
        _movieplayer.contentURL = [NSURL fileURLWithPath:videoPath];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(moviePlayerReaderToPlaying:) name:MPMoviePlayerNowPlayingMovieDidChangeNotification object:nil];
        [_movieplayer prepareToPlay];
        [_movieplayer play];
//        _movieplayer.view需要提前加上否则会黑屏
        [self insertSubview:_movieplayer.view atIndex:0];
//        [self addSubview:_movieplayer.view];
        if (_startPlayBlock) {
            _startPlayBlock();
        }
    } failed:^(NSError *error) {
        [WaitingAlertView hideWaitingView];
        AIAI_LOG(@"UserHeaderView.error = %@",error);
    }];
}

#pragma mark -- 播放视频头像
-(void)playGroupHeaderWithArray:(NSArray*)memberArray{
    
    if (_startPlayBlock) {
        _startPlayBlock();
    }
    [self.animateImgView removeFromSuperview];
    NSMutableArray *keyArray = [[NSMutableArray alloc] init];
    for (int i = 0; i < [memberArray count] && i < ANIMATECOUNT; ++i) {
        IMPackageGroupMemberData* memberData = [memberArray objectAtIndex:i];
        [keyArray addObject:memberData.portraitKey? :@""];
    }
    
    NSMutableArray *imageArray = [[NSMutableArray alloc] init];
    __block int finishCount = 0;
    __weak UserHeaderView *weakSelf = self;
    for (int j = 0; j<[keyArray count]; ++j) {
        NSString *keyStr = [keyArray objectAtIndex:j];
        AsyImageView *asyView = [[AsyImageView alloc] init];
        [asyView loadImageWithPath:[AppUtils getImageNewUrlWithSrcImageUrl:keyStr width:0 height:0] andPlaceHolderName:DefalutHeadImage];
        asyView.finishLoadBlock = ^(AsyImageView *obj){
            
            dispatch_async(dispatch_get_main_queue(), ^{
                finishCount++;
                //首先要删去父类视图的imageView 否则会压盖子视图的view
                [obj.imageView removeFromSuperview];
                [imageArray addObject:obj.image];
                //如果全部子头像加载完成 或者到达最大数。那么开始上传图片服务器生成key
                if (finishCount == [keyArray count] || finishCount == ANIMATECOUNT) {
                    if (!weakSelf.animateImgView) {
                        weakSelf.animateImgView = [[UIImageView alloc] init];
                        weakSelf.animateImgView.frame = CGRectMake(0, 0, weakSelf.frame.size.width, weakSelf.frame.size.width);
                    }
                    [weakSelf addSubview:weakSelf.animateImgView];
                    weakSelf.animateImgView.animationImages = imageArray;
                    weakSelf.animateImgView.animationDuration = ANIMATEDURATION;
                    [weakSelf.animateImgView startAnimating];
                    [weakSelf performSelector:@selector(resetAnimating) withObject:nil afterDelay:ANIMATEDURATION];
                }
            });
        };
        [self addSubview:asyView];
    }
}

-(void)stopPlay{
    if (_movieplayer) {
        [_movieplayer stop];
        [_movieplayer.view removeFromSuperview];
    }
    if (self.animateImgView) {
        [self.animateImgView stopAnimating];
        [self.animateImgView removeFromSuperview];
    }
}

-(void)cancelAllDownLoadHttp{
    [[RequestServers share] cancelDownloadRequest];
}

#pragma mark -- 头像动画
-(void)resetAnimating{
    self.transform = CGAffineTransformMakeScale(1.0,1.0);
    [self.animateImgView stopAnimating];
    [self.animateImgView removeFromSuperview];
}

-(void)beginAnimating{
    self.transform = CGAffineTransformMakeScale(1.2, 1.2);
}

#pragma mark MPMovieDelegate
-(void)MPMoviePlayerPlaybackDidFinishNotification:(NSNotification*)notify{
    NSLog(@"play finish");
    self.transform = CGAffineTransformMakeScale(1.0,1.0);
    if (_stopPlayBlock) {
        _stopPlayBlock();
    }
    
    [self sendSubviewToBack:_movieplayer.view];
    [_movieplayer.view removeFromSuperview];
    [_movieplayer stop];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:nil];
}

- (void)moviePlayerReaderToPlaying:(NSNotification*)notification{
    
    [self addSubview:_movieplayer.view];
        // Remove observer
    [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerNowPlayingMovieDidChangeNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(MPMoviePlayerPlaybackDidFinishNotification:) name:MPMoviePlayerPlaybackDidFinishNotification object:nil];
    
}

@end
