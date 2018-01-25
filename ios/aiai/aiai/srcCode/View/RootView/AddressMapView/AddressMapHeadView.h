//
//  AddressMapHeadView.h
//  aiai
//
//  Created by zhangyang on 15/1/19.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//


#import <BaiduMapAPI/BMKAnnotationView.h>
#import "PlayMovieView.h"
#import "ContactHeadLogoConllectionCell.h"

@interface AddressMapHeadAnnotation : BMKPointAnnotation
- (instancetype)initWithUserInfo:(GlobalUserInfo *)userInfo;

@property (nonatomic, copy) NSString* headImageUrl;
@property (nonatomic, copy) NSString* headVedioUrl;
@property (nonatomic, copy) NSString *nearByName;   //附近人的用户名  有可能是个人有可能是群组
@end


typedef void (^LongPressGestureRecognizerBlock)(UIGestureRecognizer* recognizer, EditState state);
typedef void (^Tap1GestureRecognizerBlock)(UIGestureRecognizer* recognizer);
typedef void (^Tap2GestureRecognizerBlock)(UIGestureRecognizer* recognizer);
typedef void (^ChangeStateRecognizerBlock)(EditState state);

@interface AddressMapHeadView : BMKAnnotationView

@property (nonatomic, readonly, assign) CGPoint offset;
@property (nonatomic, assign) double distance;      //距离我的位置的距离

@property(nonatomic, strong)LongPressGestureRecognizerBlock longPressBlock;
@property(nonatomic, strong)Tap1GestureRecognizerBlock tap1Block;
@property(nonatomic, strong)Tap2GestureRecognizerBlock tap2Block;
@property(nonatomic, strong)ChangeStateRecognizerBlock changeStateBlock;

//播放头像视频
-(void)playHeaderAudio;
@end
