//
//  BrandDetaileWebViewController.m
//  F2CPlatform
//
//  Created by zhangyang on 13-8-26.
//  Copyright (c) 2013年 xiehaiduo. All rights reserved.
//

#import "RegisterProtocolViewController.h"

@interface RegisterProtocolViewController ()<NSXMLParserDelegate>
@property (nonatomic,readwrite,strong) UIWebView* F2CWebView;

@end

@implementation RegisterProtocolViewController
@synthesize F2CWebView;

- (id)init
{
    self = [super init];
    if (self) {
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    if (IOS7_OR_LATER) {
        self.automaticallyAdjustsScrollViewInsets = NO;
    }
    self.centerBtnImageName = @"使用条款和隐私政策";
    
    CGRect rect = [UIScreen mainScreen].bounds;
    
    self.F2CWebView = [[UIWebView alloc] initWithFrame:CGRectMake(0, 64, rect.size.width, rect.size.height - 64)];
    self.F2CWebView.autoresizingMask = UIViewAutoresizingFlexibleTopMargin|UIViewAutoresizingFlexibleHeight| UIViewAutoresizingFlexibleBottomMargin;
    self.F2CWebView.delegate = self;
    self.F2CWebView.scrollView.bounces = NO;
    [self.view addSubview:self.F2CWebView];
    [self setURL:@"ii_protocol"];
}
-(void)viewDidDisappear:(BOOL)animated{
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark overwrite setOrget
-(void)setURL:(NSString *)tempUrl{
    NSURL* url = [NSURL URLWithString:tempUrl];
    if ([[url scheme] isEqualToString:@"http"] == NO)
    {
        NSString *encodedPath = [tempUrl stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding];
        url = [NSURL fileURLWithPath: [[NSBundle mainBundle]pathForResource:encodedPath ofType:@"html"] ];
    }else{
        url = [NSURL URLWithString:tempUrl];
    }
    
    NSURLRequest* request = [NSURLRequest requestWithURL:url];
    [self.F2CWebView loadRequest:request];
}

-(void)setURL:(NSString *)tempUrl withParame:(NSString*)parame{
    if (!tempUrl) {
        return;
    }
    NSURL* url = [NSURL URLWithString:tempUrl];
    if ([[url scheme] isEqualToString:@"http"] == NO)
    {
        NSString *encodedPath = [tempUrl stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding];
        NSString *encodedParame = [parame stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding];
        url = [NSURL URLWithString:[NSString stringWithFormat:@"file://%@?%@",encodedPath,encodedParame]];
    }else{
        url = [NSURL URLWithString:[NSString stringWithFormat:@"%@?%@",tempUrl,parame]];
    }
    
    NSURLRequest* request = [NSURLRequest requestWithURL:url];
    [self.F2CWebView loadRequest:request];
}


#pragma mark webView Delegate
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType{
    return YES;
}
- (void)webViewDidStartLoad:(UIWebView *)webView{
    
    NSLog(@"start");
}
- (void)webViewDidFinishLoad:(UIWebView *)webView{
    self.title = self.controllerTitle;
    NSLog(@"finish");
    if (self.controllerTitle) {
//        [self setCustomNavigationBarWithTitle:self.controllerTitle leftButton:[self backButton] rightButton:nil];
        return;
    }
//    [self setCustomNavigationBarWithTitle:@"帮助中心" leftButton:[self backButton] rightButton:nil];
}
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error{
    NSLog(@"error");
}

@end
