//  Created by xiehaiduo on 15/4/16.
//  Copyright (c) 2015年 xiehaiduo. All rights reserved.
//


#import <UIKit/UIKit.h>

@interface HeadView : AsyImageView


@property(nonatomic, strong)UIImage *subImage;

/*!
 *  Create and returns an HeadView object with the image
 *
 *  @param rect  frame
 *  @param image image
 *
 *  @return HeadView
 */
+ (instancetype)headViewWithFrame:(CGRect)rect image:(id)image;

/*!
 *  instalizes returns an HeadView object with the image
 *
 *  @param rect  frame
 *  @param image image
 *
 *  @return HeadView
 */
- (instancetype)initWithFrame:(CGRect)rect image:(id)image;


/**
 * cycleRadius
 */
@property(nonatomic, assign) CGFloat cycleRadius;

/**
 * cycleStroke
 */
@property(nonatomic, assign) CGFloat cycleStroke;

/**
 * cycleClipAngle
 */
@property(nonatomic, assign) CGFloat cycleClipAngle;

/**
 * rotateAngle
 */
@property(nonatomic, assign) CGFloat rotateAngle;

@end
