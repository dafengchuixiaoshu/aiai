//
//  RootRecordView.h
//  aiai
//
//  Created by zhangyang on 15/5/5.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SBVideoRecorder.h"

@protocol RootRecordViewDelegate <NSObject>

-(void)didRecordFinish:(NSURL*)videoUrl videoTime:(CGFloat)time;

@optional
-(void)didTakePhotoFinish:(UIImage*)photo;

@end

@interface RootRecordView : UIView

@property(nonatomic,weak)id<RootRecordViewDelegate> delegate;

@property(nonatomic, copy)dispatch_block_t downLoadFontBlock;

//开始/停止运行摄像头
-(void)startRunCapture;
-(void)stopRunCapture;

//开始/停止录制
-(void)startRecordView;
-(void)stopRecordView;
-(void)resumeRecordView;

//拍照
-(void)capturePhone;
//切换前后摄像头
-(void)switchCamera;
-(BOOL)isFrontCamera;
//打开关闭闪光灯
-(void)openFlash:(BOOL)isOpen;

//开启进度条动画
-(void)startProgressAnimation;
-(void)stopProgressAnimation;
-(void)resetProgressView;

//界面操作
-(NSString*)inputText;
-(void)resetInputText;
-(void)setMsgLabelText:(NSString*)text;
-(void)needShowTextBtn:(BOOL)show;
-(void)finishGraffitAction;
-(void)resetGraffitView;

//得到水印和输入的文字
-(UIImage*)getMarkImg;
//判断是否需要添加水印
-(BOOL)needAddMark;
//图片叠加
- (UIImage *)addlogoImage:(UIImage *)resizedImage;
@end
