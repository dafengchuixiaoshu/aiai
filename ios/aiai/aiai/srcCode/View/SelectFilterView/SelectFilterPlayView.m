//
//  SelectFilterPlayView.m
//  aiai
//
//  Created by zhangyang on 15/3/27.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "SelectFilterPlayView.h"

#import "SBVideoRecorder.h"
#import "FilterManager.h"
#import <IJKMediaPlayer/IJKMediaPlayer.h>

#define  SCALE  1.5

@interface SelectFilterPlayView ()

@property(nonatomic,strong)NSURL* movePath;//视频路径
@property(nonatomic,assign)CGFloat moveTime;//视频时长

@property(nonatomic,assign)NSInteger filterIndex;//滤镜下标
@property(nonatomic,assign)NSInteger filterInsertTime;//滤镜插入时间
@property(nonatomic,assign)NSInteger effectsIndex;//特效下标
@property(nonatomic,assign)NSInteger effectsInsertTime;//特效插入时间
@property(nonatomic,assign)NSInteger movieIndex;//电影编辑下标
@property(nonatomic,assign)NSInteger movieInsertTime;//电影编辑插入时间

@property(nonatomic,strong)MPMoviePlayerController *movieplayer;//播放使用
@property(nonatomic,strong)UIButton* playButton;//播放按钮

@property(nonatomic,strong)NSMutableArray* playerArray;

@property(nonatomic,strong)dispatch_queue_t createFilterPalyerQueue;

@end

@implementation SelectFilterPlayView
@synthesize movieplayer = _movieplayer;
@synthesize playButton = _playButton;

-(id)initWithFrame:(CGRect)frame moviePath:(NSURL*)moviePath movieTime:(CGFloat)time{
    if (self = [super initWithFrame:frame]) {
        self.movePath = moviePath;
        self.moveTime = time;
        [self createSystemPlayerWithMoviePath:moviePath];

        //播放按钮
        _playButton = [UIButton buttonWithType:UIButtonTypeCustom];
        UIImage* playimage = [UIImage imageNamed:@"selectFilter_playbutton.png"];
        CGFloat x = (self.frame.size.width - playimage.size.width*1.5)/2;
        _playButton.frame = CGRectMake(x, x, playimage.size.width*1.5, playimage.size.height*1.5);
        [_playButton setBackgroundImage:playimage forState:UIControlStateNormal];
        [_playButton addTarget:self action:@selector(playMovie:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_playButton];
        
        //播放器数组
        self.playerArray = [[NSMutableArray alloc] init];
        //创建队列
        _createFilterPalyerQueue = dispatch_queue_create ( "com.dispatch.createFilterPalyerQueue.serial" , DISPATCH_QUEUE_SERIAL );
    }
    return self;
}

-(void)dealloc{
    AIAI_LOG(@"SelectFilterPlayView dealloc");
    
    [self releaseSystemPlayer];
    [self releaseFilterPlayer];
}

-(void)playMovie:(id)sender{
    _playButton.hidden = YES;
    [_movieplayer play];
    
    IJKFFMoviePlayerController* tmpFilterplayer = [self.playerArray lastObject];
    [tmpFilterplayer play];
}

#pragma mark createSystemPlayer
-(void)createSystemPlayerWithMoviePath:(NSURL*)moviePath{
    if (_movieplayer) {
        return;
    }
    _movieplayer = [[MPMoviePlayerController alloc] init];
    _movieplayer.movieSourceType = MPMovieSourceTypeFile;//必须设置，否则不能播放
    _movieplayer.contentURL = moviePath;
    _movieplayer.controlStyle = MPMovieControlStyleNone;//隐藏系统控件
    _movieplayer.shouldAutoplay = NO;
    _movieplayer.repeatMode = MPMovieRepeatModeNone;//这个属性不知道为啥不好用，所以只好设置成none，用消息来完成循环
    [_movieplayer prepareToPlay];
    _movieplayer.view.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.width);
    //如果ios6不能适配，需要增加uiview容器
    UIView* tmpPlayView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, self.frame.size.width)];
    [tmpPlayView addSubview:_movieplayer.view];
    [self addSubview:tmpPlayView];
    //注册通知
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(MPMoviePlayerPlaybackDidFinishNotification:) name:MPMoviePlayerPlaybackDidFinishNotification object:_movieplayer];
}

#pragma mark createFilterPlayer

-(IJKFFMoviePlayerController*)createFilterPalyerWithFirstFilterPath:(NSString*)firstFilterPath secondFilterPath:(NSString*)secondFilterPath filterType:(Filter_Type)filterType insetFilterTime:(CGFloat)insetFilterTime{
    
    NSURL* videoURL = self.movePath;
    CGFloat videoTime = self.moveTime;
    
    IJKFFMoviePlayerController* tmpFilterplayer = [[IJKFFMoviePlayerController alloc] initWithContentURL:videoURL withOptions:nil];
    tmpFilterplayer.view.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.width);
    //如果第二个参数为空则认为只单个filter的效果
    if (firstFilterPath && [firstFilterPath length]>0 ) {
        if (secondFilterPath && [secondFilterPath length]>0) {
            FilterData* data = [[FilterData alloc] init];
            data.firstMoviePath = firstFilterPath;
            data.secondMoviePath = secondFilterPath;
            data.startTime = insetFilterTime;
            data.endTime = data.startTime + videoTime;
            data.isOpenVol = YES;
            data.type =  filterType;
            [tmpFilterplayer setFilterData:data];
        }else{
            FilterData* data = [[FilterData alloc] init];
            data.firstMoviePath = firstFilterPath;
            data.startTime = insetFilterTime;
            data.endTime = data.startTime + videoTime;
            data.isOpenVol = YES;
            data.type =  filterType;
            [tmpFilterplayer setFilterData:data];
        }
    }else {
        if (secondFilterPath && [secondFilterPath length]>0) {
            AIAI_LOG(@"特效参数有错误");
            return nil;
        }else{
            if (Filter_Type_Raw != filterType) {
                FilterData* data = [[FilterData alloc] init];
                data.type =  filterType;
                [tmpFilterplayer setFilterData:data];
            }else{
                AIAI_LOG(@"没传入有效特效参数，按照原视频播放");
            }
        }
    }
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(filterPlayBackStateChange:) name:IJKMoviePlayerPlaybackStateDidChangeNotification object:tmpFilterplayer];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(filterPlayBackDidFinish:) name:IJKMoviePlayerPlaybackDidFinishNotification object:tmpFilterplayer];
    
    return tmpFilterplayer;
}

-(void)createFilterPalyerWithMoviePath:(NSURL*)moviePath withCompleteBlock:(void (^)(IJKFFMoviePlayerController *player))handler{
    
    NSURL *movieUrl = [NSURL fileURLWithPath:[[FilterManager share].moviePathArray objectAtIndex:self.movieIndex]];
    
    [SBVideoRecorder mergeAndExportVideosAtFileURLs:[NSArray arrayWithObjects:movieUrl, moviePath, nil] withScale:SCALE withCompleteBlock:^(NSURL *path, CGFloat length) {
        
//        IJKFFOptions *options = [IJKFFOptions optionsByDefault];
//        options.skipFrame = IJK_AVDISCARD_NONE;
//        options.skipLoopFilter = IJK_AVDISCARD_NONE;
        
        IJKFFMoviePlayerController* tmpFilterplayer = [[IJKFFMoviePlayerController alloc] initWithContentURL:path withOptions:nil];
        tmpFilterplayer.view.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.width);
        [tmpFilterplayer.view setContentMode:UIViewContentModeLeft];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(filterPlayBackStateChange:) name:IJKMoviePlayerPlaybackStateDidChangeNotification object:tmpFilterplayer];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(filterPlayBackDidFinish:) name:IJKMoviePlayerPlaybackDidFinishNotification object:tmpFilterplayer];
        
        handler(tmpFilterplayer);
    }];
    
    
}

#pragma mark PlayBackDidFinishNotifition
//systempPlayBackDidFinishNotifition
- (void)MPMoviePlayerPlaybackDidFinishNotification:(NSNotification*)notification{
    [self bringSubviewToFront:_playButton];
    _playButton.hidden = NO;
}

//filterPlayBackDidFinishNotifition
-(void)filterPlayBackStateChange:(NSNotification*)notification{
    IJKFFMoviePlayerController* tmpFilterplayer = [self.playerArray lastObject];
    if (tmpFilterplayer.playbackState == MPMoviePlaybackStatePlaying) {
        [WaitingAlertView hideWaitingView];
    }
}
-(void)filterPlayBackDidFinish:(NSNotification*)notification{
    [self bringSubviewToFront:_playButton];
    _playButton.hidden = NO;
}

#pragma mark releasePlayer
-(void)releaseSystemPlayer{
    if (_movieplayer) {
        [_movieplayer stop];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [_movieplayer.view removeFromSuperview];
            [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:nil];
            _movieplayer = nil;
        });
    }
}

-(void)releaseFilterPlayer{
    IJKFFMoviePlayerController* releasePlayer = [self.playerArray lastObject];
    [self.playerArray removeObject:releasePlayer];
    [self releaseFilterPlayer:releasePlayer];
}

-(void)releaseFilterPlayer:(IJKFFMoviePlayerController*)filterPlayer{
    if (filterPlayer) {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:IJKMoviePlayerPlaybackDidFinishNotification object:filterPlayer];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:IJKMoviePlayerPlaybackStateDidChangeNotification object:filterPlayer];
        [filterPlayer stop];
        [filterPlayer shutdown];
        [filterPlayer stop];
        filterPlayer = nil;
    }
}

-(void)stopFilterPlayer{
    IJKFFMoviePlayerController* tmpFilterplayer = [self.playerArray lastObject];
    [tmpFilterplayer stop];
}

//选择特效效果
- (void)selectEffectsWithIndex:(NSInteger)index insetFilterTime:(CGFloat)time
{
    NSArray* effectArray = [FilterManager share].effectPathArray;
    if (index >= [effectArray count]) {
        return;
    }
    
    self.effectsIndex = index;
    self.effectsInsertTime = time;
    
    [self playSelectFilter];
}

//选择滤镜效果
- (void)selectFilterWithIndex:(NSInteger)index insetFilterTime:(CGFloat)time
{
    NSArray* filterArray = [FilterManager share].filterPathArray;
    if (index >= [filterArray count]) {
        return;
    }
    
    self.filterIndex = index;
    self.filterInsertTime = time;
    
    [self playSelectFilter];
}

//选择片头片尾
- (void)selectMovieWithIndex:(NSInteger)index insetFilterTime:(CGFloat)time{
    
    NSArray* movieArray = [FilterManager share].moviePathArray;
    if (index >= [movieArray count]) {
        return;
    }
    
    self.movieIndex = index;
    self.movieInsertTime = time;
    
    [self playSelectFilter];
}

-(void)playSelectFilter{
    
    dispatch_async(dispatch_get_main_queue(), ^{
        //显示提示框
        [WaitingAlertView showWaitingViewAtView:self];
        _playButton.hidden = YES;
        
    });
    
    dispatch_async(_createFilterPalyerQueue, ^{
        //先释放，再创建
        [self releaseSystemPlayer];
        [self releaseFilterPlayer];

        dispatch_barrier_async(_createFilterPalyerQueue, ^{
        
            NSString* firstFilterPath = nil;
            NSString* secondFilterPath = nil;
            if(self.effectsIndex > 0){
                firstFilterPath = [[FilterManager share].effectPathArray objectAtIndex:self.effectsIndex];
                secondFilterPath = [[FilterManager share].effectMaskPathArray objectAtIndex:self.effectsIndex];
            }
//            firstFilterPath = [[FilterManager share].moviePathArray objectAtIndex:0];
            __weak SelectFilterPlayView *weakSelf = self;
            //根据不同的类型生成不同的视频对象
//            if (self.movieIndex >= 0) {
            
//                [weakSelf playMovie];
            
//            }else{
                
                IJKFFMoviePlayerController* newFilterplayer = [self createFilterPalyerWithFirstFilterPath:firstFilterPath secondFilterPath:secondFilterPath filterType:self.filterIndex insetFilterTime:self.effectsInsertTime];
                
                [self.playerArray addObject:newFilterplayer];
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    [WaitingAlertView hideWaitingView];
                    
                    [self addSubview:newFilterplayer.view];
                    [newFilterplayer prepareToPlay];
                    [newFilterplayer play];
                    
                    NSLog(@"frame = %@",NSStringFromCGRect(newFilterplayer.view.frame));
                });
//            }
            
        });
    });
}

-(void)playMovie{
    
    [self createFilterPalyerWithMoviePath:self.movePath withCompleteBlock:^(IJKFFMoviePlayerController *player) {
        
        [self.playerArray addObject:player];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [WaitingAlertView hideWaitingView];
            
            [self addSubview:player.view];
            [player prepareToPlay];
            [player play];
            
            NSLog(@"frame = %@",NSStringFromCGRect(player.view.frame));
        });
        self.movieIndex = -1;
    }];
    
}

@end
