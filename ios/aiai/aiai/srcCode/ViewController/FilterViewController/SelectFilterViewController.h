//
//  SelectFilterViewController.h
//  ThirteenSeconds
//
//  Created by zhangyang on 14-8-13.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SelectFilterView.h"

//代理 userinfo参数key
static const NSString* kMoviePathKey =             @"kMoviePathKey";
static const NSString* kMovieTimeKey =             @"kMovieTimeKey";
static const NSString* kSelectFilterIndex =        @"kSelectFilterIndex";
static const NSString* kSubViewImagePathArray =    @"kSubViewImagePathArray";
static const NSString* kBtnType =                  @"kBtnType";
static const NSString* kUserSessionData =          @"kUserSessionData";

@class IMPackageSessionData;
@protocol SelectFilterViewControllerDelegate <NSObject>

@optional
//获取编辑之后的视频路径时间和userSession
-(void)SelectFilterViewControllerDidEditMovieUrl:(NSURL*)editMovieUrl movieTime:(CGFloat)time sessionData:(IMPackageSessionData*)sessionData;

//获取原视频路径，选中的filter下标还有按钮类型如特效，滤镜等,userSession
-(void)SelectFilterViewControllerWillGetUserInfo:(NSDictionary*)userInfo;

@end

@interface SelectFilterViewController : BaseViewController

-(id)initWithVideoUrl:(NSURL*)path andTime:(CGFloat)time;

@property(nonatomic,weak)id<SelectFilterViewControllerDelegate> delegate;
@property(nonatomic,strong)IMPackageSessionData* sessionData;

@end
