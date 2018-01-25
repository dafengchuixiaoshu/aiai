//
//  AddressMapView.h
//  aiai
//
//  Created by zhangyang on 15/1/12.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^AddressMapViewOtherBtnAction)(void);

typedef void (^sendImgAction)(NSString *sessionId, UIGestureRecognizer *gesture);
typedef void (^sendVideoAction)(NSString *sessionId, UIGestureRecognizer *gesture ,EditState editState);
typedef void (^ChangeStateRecognizerBlock)(EditState state);

@interface AddressMapView : UIView
@property(nonatomic,strong) NSArray* nearFriends;
@property(nonatomic,copy)AddressMapViewOtherBtnAction openOrCloseMapBtnAction;
@property(nonatomic, strong) sendImgAction sendImgBlick;
@property(nonatomic, strong) sendVideoAction sendVideoBlock;
@property(nonatomic, strong) ChangeStateRecognizerBlock changeStateBlock;

//在不现实的情况下需要stop，否则太费电了，也占用系统资源
-(void)startLocation;
-(void)stopLocation;

-(void)openOrClose:(BOOL)isOpen;
@end
