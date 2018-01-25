//
//  SelectFilterViewController.m
//  ThirteenSeconds
//
//  Created by zhangyang on 14-8-13.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "SelectFilterViewController.h"

@interface SelectFilterViewController ()<UIAlertViewDelegate>

@property(nonatomic,strong)SelectFilterView* seclectview;
//播放
@property(nonatomic,strong)NSURL* videoPath;
@property(nonatomic,assign)CGFloat videoTime;

@end

@implementation SelectFilterViewController
@synthesize videoPath = _videoPath;
@synthesize videoTime = _videoTime;

-(id)initWithVideoUrl:(NSURL*)path andTime:(CGFloat)time{
    if (self = [super init]) {
        self.videoPath = path;
        self.videoTime = time;
    }
    return self;
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    CGRect rect = self.view.frame;
    rect.origin.y = 0;
    rect.size.height -= rect.origin.y;
    
    __weak SelectFilterViewController* weakself = self;
    _seclectview = [[SelectFilterView alloc] initWithFrame:rect moviePath:self.videoPath movieTime:self.videoTime];
    _seclectview.sureBtnBlock = ^(SelectFilterView_SelectBtn btnType){
        
        if(weakself.delegate && [weakself.delegate respondsToSelector:@selector(SelectFilterViewControllerWillGetUserInfo:)]){
            NSMutableDictionary* userinfo = [[NSMutableDictionary alloc] init];
            [userinfo setObject:weakself.videoPath forKey:kMoviePathKey];
            [userinfo setObject:[NSNumber numberWithFloat:weakself.videoTime] forKey:kMovieTimeKey];
            [userinfo setObject:[NSNumber numberWithInteger:[weakself.seclectview getSelectIndex]] forKey:kSelectFilterIndex];
            [userinfo setObject:[weakself.seclectview getSelectSubViewImagePathArray] forKey:kSubViewImagePathArray];
            [userinfo setObject:[NSNumber numberWithInteger:btnType] forKey:kBtnType];
            [userinfo setObject:weakself.sessionData forKey:kUserSessionData];
            [weakself.delegate SelectFilterViewControllerWillGetUserInfo:userinfo];
        }
        [weakself leftItemClicked:nil];
    };
    _seclectview.cancleBtnBlock = ^(SelectFilterView_SelectBtn btnType){
        [weakself leftItemClicked:nil];
    };
    
    [self.view addSubview:_seclectview];
    [self.view sendSubviewToBack:_seclectview];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)dealloc{
    AIAI_LOG(@"SelectFilterViewController dealloc");
}

-(void)leftItemClicked:(id)sender{

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
       [_seclectview releaseAllSource];
    });

    [super leftItemClicked:sender];
}

#pragma mark UIAlertViewDelegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    if (buttonIndex == 0) {
        if(self.delegate && [self.delegate respondsToSelector:@selector(SelectFilterViewControllerDidEditMovieUrl:movieTime:sessionData:)]){
            [self.delegate SelectFilterViewControllerDidEditMovieUrl:self.videoPath movieTime:self.videoTime sessionData:self.sessionData];
            [self leftItemClicked:nil];
        }
    }else{
        //取消，啥都不干
    }
}

@end
