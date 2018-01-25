//  Created by xiehaiduo on 15/4/16.
//  Copyright (c) 2015年 xiehaiduo. All rights reserved.
//


#import <UIKit/UIKit.h>

typedef void (^finishUpData)(NSString *imgKey);

@interface GroupHeadView : UIView

/*!
 *  Create and returns an GroupHeadView object with the images
 *
 *  @param frame  self frame
 *  @param images images an array of pictures
 *
 *  @return GroupHeadView
 */
+ (instancetype)groupHeadViewWithFrame:(CGRect)frame images:(NSArray *)images prepare:(void (^)(void))prepare complete:(finishUpData)complete;

/*!
 *  get GroupHeadView image
 *
 *  @return UIImage
 */
- (UIImage *)imageGroupHeadView;

//返回允许头像的最大数
+(int)MaxCount;

@end
