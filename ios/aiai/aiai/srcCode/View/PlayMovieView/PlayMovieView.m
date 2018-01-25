//
//  PlayMovieView.m
//  aiai
//
//  Created by zhangyang on 15/2/4.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "PlayMovieView.h"
#import <IJKMediaPlayer/IJKMediaPlayer.h>

@interface PlayMovieView ()
@property(nonatomic,strong)MPMoviePlayerController *movieplayer;//播放使用
//@property(nonatomic,strong)IJKFFMoviePlayerController* filterplayer;

@property(nonatomic,strong)UIImageView* firstFrameImageView;//第一帧图片
@property(nonatomic,strong)UIButton* playButton;//播放按钮

@end

@implementation PlayMovieView

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        //创建播放器
        _movieplayer = [[MPMoviePlayerController alloc] init];
        _movieplayer.movieSourceType = MPMovieSourceTypeFile;//必须设置，否则不能播放
        _movieplayer.controlStyle = MPMovieControlStyleNone;//隐藏系统控件
        _movieplayer.shouldAutoplay = NO;
        _movieplayer.repeatMode = MPMovieRepeatModeNone;//这个属性不知道为啥不好用，所以只好设置成none，用消息来完成循环
        _movieplayer.view.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.width);
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(MPMoviePlayerThumbnailImageRequestDidFinishNotification:) name:MPMoviePlayerThumbnailImageRequestDidFinishNotification object:_movieplayer];
        [self addSubview:_movieplayer.view];
        
        //第一帧图片
        _firstFrameImageView= [[UIImageView alloc] initWithFrame:frame];
        [self addSubview:_firstFrameImageView];
        
        //播放按钮
        _playButton = [UIButton buttonWithType:UIButtonTypeCustom];
        UIImage* playimage = [UIImage imageNamed:@"selectFilter_playbutton.png"];
        CGFloat w = frame.size.width/4;
        CGFloat h = frame.size.height/4;
        CGFloat x = (frame.size.width - w)/2;
        CGFloat y = (frame.size.height - h)/2;
        _playButton.frame = CGRectMake(x, y, w, h);
        _playButton.hidden = YES;
        [_playButton setBackgroundImage:playimage forState:UIControlStateNormal];
        [_playButton addTarget:self action:@selector(playMovie:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_playButton];
    }
    return self;
}

-(void)dealloc{
    AIAI_LOG(@"PlayMovieView dealloc");
}

#pragma mark buttonAction
-(void)playMovie:(id)sender{
    [self playMovie];
}

#pragma mark MPMoviePlayerPlaybackDidFinishNotification Mothed

- (void)MPMoviePlayerPlaybackDidFinishNotification:(NSNotification*)notification{
    _playButton.hidden = NO;
    [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:_movieplayer];
    if (self.complateBlock) {
        self.complateBlock();
    }
}

//请求第一帧图片
-(void)MPMoviePlayerThumbnailImageRequestDidFinishNotification:(NSNotification*)notification{
    _firstFrameImageView.image = [notification.userInfo objectForKey:MPMoviePlayerThumbnailImageKey];
    if (_loadFinishBlock) {
        _loadFinishBlock(_firstFrameImageView.image);
    }
}

#pragma mark public mothed

-(void)setMoviePath:(NSString *)moviePath{
    _moviePath = [moviePath copy];
    if (_movieplayer) {
        [_movieplayer stop];
        _movieplayer.contentURL = [NSURL fileURLWithPath:_moviePath];
        [_movieplayer requestThumbnailImagesAtTimes:[NSArray arrayWithObject:[NSNumber numberWithDouble:0.0]] timeOption:MPMovieTimeOptionNearestKeyFrame];
    }
}

-(void)playMovie{
    //添加播放器
    if (_movieplayer) {
        
        dispatch_queue_t queue = dispatch_queue_create("play", NULL);
        dispatch_async(queue, ^{
            [_movieplayer stop];
            
            [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(MPMoviePlayerPlaybackDidFinishNotification:) name:MPMoviePlayerPlaybackDidFinishNotification object:_movieplayer];
            
            dispatch_async(dispatch_get_main_queue(), ^{
                
                //准备结束进行播放
                _playButton.hidden = YES;
                _firstFrameImageView.hidden = YES;
                
                [_movieplayer play];
            });
        });
    }
}

-(void)stopMovie{
    [_movieplayer stop];
//    _playButton.hidden = NO;
}

-(void)releaseResource{
    if (_movieplayer) {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerThumbnailImageRequestDidFinishNotification object:_movieplayer];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:_movieplayer];
        [_movieplayer.view removeFromSuperview];
        [_movieplayer stop];
        _movieplayer = nil;
    }
}

//////IJKPlayer播放
//#pragma mark filterPlayBackDidFinishNotification
//-(void)playMoiveWithPath:(NSString*)videoPath{
//
//    [_filterplayer.view removeFromSuperview];
//    [_filterplayer stop];
//    _filterplayer = nil;
//
//    _filterplayer = [[IJKFFMoviePlayerController alloc] initWithContentURL:[NSURL URLWithString:videoPath] withOptions:nil];
//    _filterplayer.view.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.width);
//    [self addSubview:_filterplayer.view];
//
//    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(filterPlayBackDidFinish:) name:IJKMoviePlayerPlaybackDidFinishNotification object:_filterplayer];
//    [_filterplayer prepareToPlay];
//    [_filterplayer play];
//}
//
//#pragma mark IJKFFMoviePlayerController Mothed
//-(void)filterPlayBackDidFinish:(NSNotification*)notification{
//    [self bringSubviewToFront:_playButton];
//    _playButton.hidden = NO;
//}

@end
