//
//  VedioIntervalEditViewController.m
//  aiai
//
//  Created by zhangyang on 15/2/3.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "VedioIntervalEditViewController.h"
#import "PlayMovieView.h"
#import "VedioIntervalEditView.h"

@interface VedioIntervalEditViewController ()

@property(nonatomic,strong)PlayMovieView* playView;

@property(nonatomic,copy)NSString* moviePath;

@end

@implementation VedioIntervalEditViewController

-(id)initWithVideoPath:(NSString*)path{
    if (self = [super init]) {
        _moviePath = [path copy];
    }
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    
    self.playView = [[PlayMovieView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.width)];
    self.playView.moviePath = _moviePath;
    [self.view insertSubview:self.playView atIndex:0];
    
    [self createStopMotion_AddFilterView];
}

-(void)dinggeFilterBtnPress:(id)sender{
    
}

-(void)addFilterBtnPress:(id)sender{
    [self createSelectFilterTimeView];
}

#pragma mark 定格与添加特效按钮界面
-(void)createStopMotion_AddFilterView{
    UIView* bgview = [[UIView alloc] initWithFrame:CGRectMake(0, self.view.frame.size.width, self.view.frame.size.width, self.view.frame.size.height - self.view.frame.size.width)];
    
    UIButton* btn1 = [UIButton buttonWithType:UIButtonTypeCustom];
    UIImage* image1 = [UIImage imageNamed:@"stopmotion@2x.png"];
    [btn1 setImage:image1 forState:UIControlStateNormal];
    [btn1 addTarget:self action:@selector(dinggeFilterBtnPress:) forControlEvents:UIControlEventTouchUpInside];
    btn1.frame = CGRectMake(0, 100, image1.size.width, image1.size.height);
    [bgview addSubview:btn1];
    
    UIButton* btn2 = [UIButton buttonWithType:UIButtonTypeCustom];
    UIImage* image2 = [UIImage imageNamed:@"添加特效@2x.png"];
    [btn2 setImage:image2 forState:UIControlStateNormal];
    [btn2 addTarget:self action:@selector(addFilterBtnPress:) forControlEvents:UIControlEventTouchUpInside];
    btn2.frame = CGRectMake(200, 100, image2.size.width, image2.size.height);
    [bgview addSubview:btn2];
    
    [self.view addSubview:bgview];
}

#pragma mark 特效选择时间界面
-(void)createSelectFilterTimeView{
    CGRect rect = CGRectMake(0, self.view.frame.size.width, self.view.frame.size.width, self.view.frame.size.height - self.view.frame.size.width);
    VedioIntervalEditView* bgview = [[VedioIntervalEditView alloc] initWithFrame:rect andPictuerNumber:10];
    [self.view addSubview:bgview];
}

@end
