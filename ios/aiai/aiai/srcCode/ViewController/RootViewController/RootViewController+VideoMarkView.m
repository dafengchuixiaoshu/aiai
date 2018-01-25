//
//  RootViewController+VideoMarkView.m
//  aiai
//
//  Created by wangnan on 15/10/14.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "RootViewController+VideoMarkView.h"

@implementation RootViewController (VideoMarkView)

#pragma mark -- 视频中加水印
-(void)blendVideoAndImagesWithImg:(UIImage*)img srcVedioPath:(NSURL*)srcPath targetData:(IMPackageSessionData*)targetData prepare:(void (^)(void))prepare complate:(void (^)(NSURL *outputPath, IMPackageSessionData *data))complate{
    
    prepare();
    
    [[HandelDataQueue share] exe:^{
        
        NSData *imageData = UIImagePNGRepresentation(img);
        NSString* documentPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
        NSString* outimageName = @"output.png";
        NSString* markPath = [documentPath stringByAppendingPathComponent:outimageName];
        [imageData writeToFile:markPath atomically:NO];
        
        NSString* outputStr = [NSTemporaryDirectory() stringByAppendingPathComponent:@"output.MOV"];
        if ([[NSFileManager defaultManager] fileExistsAtPath:outputStr]) {
            [[NSFileManager defaultManager] removeItemAtPath:outputStr error:nil];
        }
        int size = 0;
        NSString *audioPath = [srcPath relativeString];
        char **commandLinestr = [CommandLine blendVideoAndImagesWithSize:&size inputVideoPath:audioPath waterMark:markPath outputVideoPath:outputStr];
        ffmpegMain(size,commandLinestr);
        [CommandLine releaseCommandLineStr:commandLinestr size:size];
        
        complate([NSURL fileURLWithPath:outputStr],targetData);
    }];
}

@end
