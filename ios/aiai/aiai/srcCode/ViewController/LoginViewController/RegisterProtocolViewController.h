//
//  BrandDetaileWebViewController.h
//  F2CPlatform
//
//  Created by zhangyang on 13-8-26.
//  Copyright (c) 2013年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface RegisterProtocolViewController : BaseViewController<UIWebViewDelegate>

@property (nonatomic,readonly,strong) UIWebView* F2CWebView;

-(void)setURL:(NSString *)tempUrl;
-(void)setURL:(NSString *)tempUrl withParame:(NSString*)parame;

@property(nonatomic, copy) NSString* controllerTitle;
@end
