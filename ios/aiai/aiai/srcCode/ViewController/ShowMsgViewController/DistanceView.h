//
//  DistanceView.h
//  aiai
//
//  Created by wangnan on 15/5/16.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DistanceView : UIView

//设置昵称
-(void)setNickName:(NSString*)nickName;

//设置距离和时间
-(void)setDistanceWithBuddyLatitude:(double)buddyLatitude AndBuddyLongitude:(double)buddyLogitude AndTime:(NSString*)time;
@end
