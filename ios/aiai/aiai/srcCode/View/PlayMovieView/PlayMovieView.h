//
//  PlayMovieView.h
//  aiai
//
//  Created by zhangyang on 15/2/4.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^PlayMovieViewPlayComplateBlock)(void);
typedef void (^LoadImgFinishBlock)(UIImage *finishImg);

@interface PlayMovieView : UIView

@property(nonatomic,copy)NSString* moviePath;
-(void)playMovie;
-(void)stopMovie;
//释放一些资源文件，通知等影响内存泄露的东西
-(void)releaseResource;

@property(nonatomic,strong)PlayMovieViewPlayComplateBlock complateBlock;
@property(nonatomic,strong)LoadImgFinishBlock loadFinishBlock;

@end
