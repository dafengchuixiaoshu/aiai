//
//  BaseViewController.h
//  zlydoc-iphone
//
//  Created by Ryan on 14-5-23.
//  Copyright (c) 2014年 zlycare. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BaseViewController : UIViewController

@property(nonatomic,copy)NSString* leftBtnImageName;
@property(nonatomic,copy)NSString* centerBtnImageName;
@property(nonatomic,copy)NSString* rightBtnImageName;

- (void)leftItemClicked:(id)sender;
- (void)centerItemClicked:(id)sender;
- (void)rightItemClicked:(id)sender;

-(void)bringTitleViewToFront;

@end