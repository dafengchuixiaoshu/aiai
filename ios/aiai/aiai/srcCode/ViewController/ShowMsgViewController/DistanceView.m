//
//  DistanceView.m
//  aiai
//
//  Created by wangnan on 15/5/16.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "DistanceView.h"

@interface DistanceView ()

@property(nonatomic, strong)UILabel *nickNameLabel;
@property(nonatomic, strong)UILabel *distanceLabel;

@end

@implementation DistanceView
@synthesize nickNameLabel, distanceLabel;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        
        CGFloat labelHeight = 16;
        nickNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 13, [UIScreen mainScreen].bounds.size.width, labelHeight)];
        nickNameLabel.textColor = [UIColor whiteColor];
        nickNameLabel.font = [UIFont systemFontOfSize:16];
        nickNameLabel.backgroundColor = [UIColor clearColor];
        nickNameLabel.textAlignment = NSTextAlignmentLeft;
        nickNameLabel.alpha = 1.0;
        [self addSubview:nickNameLabel];
        
        distanceLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, nickNameLabel.frame.origin.y + nickNameLabel.frame.size.height + 2, [UIScreen mainScreen].bounds.size.width, labelHeight)];
        distanceLabel.textColor = [UIColor whiteColor];
        distanceLabel.font = [UIFont systemFontOfSize:14];
        distanceLabel.textAlignment = NSTextAlignmentLeft;
        distanceLabel.alpha = 1.0;
        [self addSubview:distanceLabel];
        
        self.backgroundColor = [UIColor clearColor];
        self.frame = CGRectMake(frame.origin.x, frame.origin.y, [UIScreen mainScreen].bounds.size.height, distanceLabel.frame.origin.y + distanceLabel.frame.size.height +10);
        
        UIImageView *bgImgView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"距离背景图@2x.png"]];
        bgImgView.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.height);
        [self addSubview:bgImgView];
        [self sendSubviewToBack:bgImgView];
        
    }
    return self;
}

-(void)setNickName:(NSString*)nickName{
    nickNameLabel.text = nickName? :@"";
}


-(void)setDistanceWithBuddyLatitude:(double)buddyLatitude AndBuddyLongitude:(double)buddyLogitude AndTime:(NSString*)time{
    
    //计算距离
    double myLatitude = [GlobalUserInfo myselfInfo].userLatitude;
    double myLogitude = [GlobalUserInfo myselfInfo].userLongitude;
    
    CLLocationCoordinate2D coors[2] = {0};
    //我所在的点
    coors[0].latitude = myLatitude;
    coors[0].longitude = myLogitude;
    //好友所在的点
    coors[1].latitude = buddyLatitude;
    coors[1].longitude = buddyLogitude;
    
    BMKMapPoint pmine = BMKMapPointForCoordinate(coors[0]);
    BMKMapPoint pbuddy = BMKMapPointForCoordinate(coors[1]);
    
    CLLocationDistance dis = BMKMetersBetweenMapPoints(pmine, pbuddy);
    NSString *disText ;
    //如果大于1公里显示公里 小于1公里显示米
    if (dis/1000 >= 1.0) {
        disText = [NSString stringWithFormat:@"距离:%.2f公里",dis/1000];
    }else{
        disText = [NSString stringWithFormat:@"距离:%.2f米",dis];
    }
    
    //计算时间
    NSString *timeText = [AppUtils getTime:time];
    
    distanceLabel.text = [NSString stringWithFormat:@"%@, %@",disText,timeText];
    
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/



@end
