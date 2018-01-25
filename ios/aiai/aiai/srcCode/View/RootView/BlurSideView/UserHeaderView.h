//
//  UserHeaderView.h
//  aiai
//
//  Created by zhangyang on 15/4/9.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AsyImageView.h"

@class UserHeaderView;
typedef void (^UserHeaderViewTapAction)(UIGestureRecognizer* recognizer);
typedef void (^UserHeaderViewBtnAction)(/*NSArray* unReadMsgArray*/void);

@interface UserHeaderView : AsyImageView

@property(nonatomic,copy)UserHeaderViewTapAction singleTapBlock;
@property(nonatomic,copy)UserHeaderViewTapAction doubleTapBlock;
@property(nonatomic,copy)dispatch_block_t startPlayBlock;
@property(nonatomic,copy)dispatch_block_t stopPlayBlock;

@property(nonatomic,copy)NSString *headerVideoPath;
@property(nonatomic,copy)NSArray *groupMemberArray;        //群组成员数组 用于播放群组头像

/*播放视频*/
-(void)playUserHeaderVideo;
/*停止视频*/
-(void)stopPlay;
/*终止所有下载*/
-(void)cancelAllDownLoadHttp;
/*开始动画*/
-(void)beginAnimating;
/*结束动画*/
-(void)resetAnimating;

@end
