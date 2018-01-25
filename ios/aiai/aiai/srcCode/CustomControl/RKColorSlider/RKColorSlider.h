//
//  RKColorSlider.h
//  Pods
//
//  Created by Richard Kirk on 1/11/15.
//
//

#import <UIKit/UIKit.h>

typedef void (^GetColor)(UIColor *currentColor);

@interface RKColorSlider : UIControl

@property(nonatomic, strong)GetColor getColorBlock;
@end
