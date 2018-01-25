//
// OpenPlatformActionSheet.h
//  LXActivityDemo
//
//  Created by lixiang on 14-3-17.
//  Copyright (c) 2014年 lcolco. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol OpenPlatformActionSheetDelegate <NSObject>
- (void)didClickOnImageIndex:(NSInteger)imageIndex;
@optional
- (void)didClickOnCancelButton;
@end

@interface OpenPlatformActionSheet : UIView

- (id)initWithTitle:(NSString *)title delegate:(id<OpenPlatformActionSheetDelegate>)delegate;

- (id)initWithTitle:(NSString *)title delegate:(id<OpenPlatformActionSheetDelegate>)delegate cancelButtonTitle:(NSString *)cancelButtonTitle ShareButtonTitles:(NSArray *)shareButtonTitlesArray withShareButtonImagesName:(NSArray *)shareButtonImagesNameArray;
- (void)showInView:(UIView *)view;

@end
