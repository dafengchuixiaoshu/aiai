//
//  SelectFilterPlayView.h
//  aiai
//
//  Created by zhangyang on 15/3/27.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SelectFilterPlayView : UIView

-(id)initWithFrame:(CGRect)frame moviePath:(NSURL*)moviePath movieTime:(CGFloat)time;

//特效效果
- (void)selectEffectsWithIndex:(NSInteger)index insetFilterTime:(CGFloat)time;
//选择滤镜效果
- (void)selectFilterWithIndex:(NSInteger)index insetFilterTime:(CGFloat)time;
//选择片头片尾
- (void)selectMovieWithIndex:(NSInteger)index insetFilterTime:(CGFloat)time;

//播放filterMoive
-(void)stopFilterPlayer;

-(void)releaseSystemPlayer;
-(void)releaseFilterPlayer;
@end
