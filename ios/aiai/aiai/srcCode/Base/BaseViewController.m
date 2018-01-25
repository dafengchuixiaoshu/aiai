//
//  BaseViewController.m
//  zlydoc-iphone
//  Parent View Controller
//  Created by Ryan on 14-5-23.
//  Copyright (c) 2014年 zlycare. All rights reserved.
//

#import "BaseViewController.h"

#define LeftDefaultImageName @"back.png"

@interface BaseViewController ()<UIGestureRecognizerDelegate>
@property(nonatomic,strong)UIView* titleView;
@property(nonatomic,strong)UIButton* leftBtn;
@property(nonatomic,strong)UIButton* centerBtn;
@property(nonatomic,strong)UIButton* rightBtn;

@end

@implementation BaseViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.view.backgroundColor = EveryView_BackGroundColor;
    [self createTitleBar];
    
    self.leftBtnImageName = @"back.png";
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    [[RequestServers share] cancelRequestWithClassName:[self class]];
}

-(void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];

}

-(void)viewDidAppear:(BOOL)animated{
    [super viewDidAppear:animated];
    //进行百度界面统计
    NSString *class = [NSString stringWithFormat:@"%@",[self class]];
    [BaiduStatistics baidu_pageviewStartWithName:class];

}

-(void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    
    NSString *class = [NSString stringWithFormat:@"%@",[self class]];
    [BaiduStatistics baidu_pageviewEndWithName:class];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark TitleBar
-(void)createTitleBar{
    _titleView = [[UIView alloc] initWithFrame:CGRectMake(0, 20, self.view.frame.size.width, 44)];
    _titleView.backgroundColor = [UIColor clearColor];
    
    CGFloat LR_Padding = 0;
    CGFloat image_Y = 0;
    CGFloat image_W = 44;
    CGFloat image_H = 44;
    
    self.leftBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.leftBtn.frame = CGRectMake(LR_Padding, image_Y, image_W, image_H);
    self.leftBtn.titleLabel.font = Every_SystemFontSize_14;
    [self.leftBtn setTitleColor:[[UIColor whiteColor] colorWithAlphaComponent:0.8] forState:UIControlStateNormal];
    [self.leftBtn setTitleColor:[[UIColor whiteColor] colorWithAlphaComponent:0.8] forState:UIControlStateHighlighted];
    [self.leftBtn addTarget:self action:@selector(leftItemClicked:) forControlEvents:UIControlEventTouchUpInside];
    self.leftBtn.contentHorizontalAlignment = UIControlContentHorizontalAlignmentRight;
    [_titleView addSubview:self.leftBtn];
    
    CGFloat centerBtn_X = (self.view.frame.size.width- image_W)/2;
    self.centerBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.centerBtn.frame = CGRectMake(centerBtn_X, image_Y, image_W, image_H);
    [self.centerBtn addTarget:self action:@selector(centerItemClicked:) forControlEvents:UIControlEventTouchUpInside];
    [_titleView addSubview:self.centerBtn];
    
    CGFloat rightBtn_X = self.view.frame.size.width- LR_Padding - image_W;
    self.rightBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.rightBtn.frame = CGRectMake(rightBtn_X, image_Y, image_W, image_H);
    self.rightBtn.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
    self.rightBtn.titleLabel.font = Every_SystemFontSize_14;
    [self.rightBtn setTitleColor:[[UIColor whiteColor] colorWithAlphaComponent:0.8] forState:UIControlStateNormal];
    [self.rightBtn setTitleColor:[[UIColor whiteColor] colorWithAlphaComponent:0.8] forState:UIControlStateHighlighted];
    [self.rightBtn addTarget:self action:@selector(rightItemClicked:) forControlEvents:UIControlEventTouchUpInside];
    [_titleView addSubview:self.rightBtn];
    
    [self.view addSubview:_titleView];
}

-(void)setLeftBtnImageName:(NSString *)leftBtnImageName{
    
    if ([[leftBtnImageName lowercaseString] hasSuffix:@".png"]||[[leftBtnImageName lowercaseString] hasSuffix:@".jpg"]) {
        [self.leftBtn setImage:[UIImage imageNamed:leftBtnImageName] forState:UIControlStateNormal];
        [self.leftBtn setImage:[UIImage imageNamed:leftBtnImageName] forState:UIControlStateHighlighted];
    }else{
        [self.leftBtn setImage:nil forState:UIControlStateNormal];
        [self.leftBtn setImage:nil forState:UIControlStateHighlighted];
        [self.leftBtn setTitle:leftBtnImageName forState:UIControlStateNormal];
        [self.leftBtn setTitle:leftBtnImageName forState:UIControlStateHighlighted];
    }
}

-(void)setCenterBtnImageName:(NSString *)centerBtnImageName{
    if ([[centerBtnImageName componentsSeparatedByString:@"."] count] == 1) {
        self.centerBtn.hidden = NO;
        CGRect rect = self.centerBtn.frame;
        rect.size.width = 200;
        rect.origin.x = (_titleView.frame.size.width - rect.size.width)/2;
        self.centerBtn.frame = rect;
        [self.centerBtn setTitle:centerBtnImageName forState:UIControlStateNormal];
        return;
    }
    if (!centerBtnImageName) {
        self.centerBtn.hidden = YES;
    }else {
        self.centerBtn.hidden = NO;
    }
    [self.centerBtn setImage:[UIImage imageNamed:centerBtnImageName] forState:UIControlStateNormal];
}

-(void)setRightBtnImageName:(NSString *)rightBtnImageName{
    if (!rightBtnImageName) {
        self.rightBtn.hidden = YES;
    }else {
        self.rightBtn.hidden = NO;
        if ([[rightBtnImageName lowercaseString] hasSuffix:@".png"]||[[rightBtnImageName lowercaseString] hasSuffix:@".jpg"]) {
            [self.rightBtn setImage:[UIImage imageNamed:rightBtnImageName] forState:UIControlStateNormal];
            [self.rightBtn setImage:[UIImage imageNamed:rightBtnImageName] forState:UIControlStateHighlighted];
        }else{
            [self.rightBtn setImage:nil forState:UIControlStateNormal];
            [self.rightBtn setImage:nil forState:UIControlStateHighlighted];
            [self.rightBtn setTitle:rightBtnImageName forState:UIControlStateNormal];
            [self.rightBtn setTitle:rightBtnImageName forState:UIControlStateHighlighted];
        }
    }
}

- (void)leftItemClicked:(id)sender {
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)centerItemClicked:(id)sender {
    
}

- (void)rightItemClicked:(id)sender {
    
}

-(void)bringTitleViewToFront{
    [self.view bringSubviewToFront:_titleView];
}

@end
