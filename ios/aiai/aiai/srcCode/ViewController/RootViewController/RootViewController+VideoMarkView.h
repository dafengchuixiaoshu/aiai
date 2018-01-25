//
//  RootViewController+VideoMarkView.h
//  aiai
//
//  Created by wangnan on 15/10/14.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "RootViewController.h"

@interface RootViewController (VideoMarkView)

//视频加水印
-(void)blendVideoAndImagesWithImg:(UIImage*)img srcVedioPath:(NSURL*)srcPath targetData:(IMPackageSessionData*)targetData prepare:(void (^)(void))prepare complate:(void (^)(NSURL *outputPath, IMPackageSessionData *data))complate;

@end
