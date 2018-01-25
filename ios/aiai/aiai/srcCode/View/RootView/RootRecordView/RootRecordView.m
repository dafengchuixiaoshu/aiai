//
//  RootRecordView.m
//  aiai
//
//  Created by zhangyang on 15/5/5.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "RootRecordView.h"
#import "DecreasingProgressView.h"
#import "GraffitiBtnView.h"
#import "FontBtnView.h"
#import "GraffitiView.h"

//#import "UITextField+DynamicFontSize.h"
#import "IISettingViewController+SavePhotoAndVideoManager.h"
#import <CoreText/CTFont.h>
#import <CoreText/CTStringAttributes.h>

@interface RootRecordView ()<UITextFieldDelegate,SBVideoRecorderDelegate, UIGestureRecognizerDelegate>{
    
}
//录制相关视图
@property (strong, nonatomic) SBVideoRecorder *recorder;
@property (strong, nonatomic) UIView *preview;

//视频相关数据
@property(nonatomic,copy)NSString* movePath;
@property(nonatomic,copy)NSNumber* moveTime;

//允许用户输入的文字
@property(nonatomic,strong)UITextField* inputTextField;
@property(nonatomic,assign)BOOL showKeyBord;                        //是否弹起键盘
@property(nonatomic,strong)UILabel* msgLabel;
//字体
@property(nonatomic,strong)UIButton* fontBtn;
@property(nonatomic,strong)UILabel* fontNamtLabel;
@property(nonatomic,assign)int fontIndex;

//主界面按钮
@property(nonatomic,strong)UIView *defaultView;
@property(nonatomic,strong)FontBtnView *fontView;
@property(nonatomic,strong)GraffitiBtnView* graffitiBtnView;
@property(nonatomic,strong)GraffitiView *graffitView;               //涂鸦

@property(nonatomic,strong)DecreasingProgressView* progressView;
@property(nonatomic,assign)CGFloat progress;
@property(nonatomic,strong)NSTimer* timer;

@end

@implementation RootRecordView

@synthesize msgLabel;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        
        _fontIndex = 0;
        //初始化当前界面UI
        [self initializationRcorder];
        [self initializationInputTextField];
        [self initializationProgressView];
        [self initializationKeyboardBtnWith_Y:self.frame.size.height];
        [self initFontNameLabel];
        [self initGraffitiView];
    }
    return self;
}

-(void)initializationRcorder{
    //创建录制视频相关UI
    self.backgroundColor = EveryView_BackGroundColor;
    
    self.preview = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, self.frame.size.width)];
    _preview.clipsToBounds = YES;
    [self addSubview:_preview];
    UITapGestureRecognizer* ges = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGestureRecognizerAction:)];
    [_preview addGestureRecognizer:ges];
    
    //创建录制对象之前先创建缓存文件夹
    [SaveManager createVideoFolderWithTmpDir];
    
    //创建录制对象
    self.recorder = [[SBVideoRecorder alloc] init];
    _recorder.delegate = self;
    _recorder.preViewLayer.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.width);
    [self.preview.layer addSublayer:_recorder.preViewLayer];
}

-(void)initializationInputTextField{
    
    //输入框
    _showKeyBord = NO;
    CGFloat inputTextField_Y = self.frame.size.width/2 - 20;
    _inputTextField = [[UITextField alloc] initWithFrame:CGRectMake(0, inputTextField_Y, self.frame.size.width, 88)];
    _inputTextField.delegate = self;
    _inputTextField.font = [UIFont systemFontOfSize:36.0];
    _inputTextField.textAlignment = NSTextAlignmentCenter;
    _inputTextField.textColor = [UIColor whiteColor];
    _inputTextField.borderStyle = UITextBorderStyleNone;
    _inputTextField.returnKeyType = UIReturnKeyDone;
    _inputTextField.adjustsFontSizeToFitWidth = YES;
    [self addSubview:_inputTextField];

    //输入框拖动
    UIPanGestureRecognizer *gesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
    gesture.delegate = self;
    [_inputTextField addGestureRecognizer:gesture];
    
}

-(void)initializationProgressView{
    //进度条,先创建进度条，便于计算坐标
    CGFloat progressView_H = 6;
    CGFloat progressView_Y = self.frame.origin.y+self.frame.size.height - progressView_H;
    self.progressView = [[DecreasingProgressView alloc] initWithFrame:CGRectMake(0, progressView_Y, self.frame.size.width, progressView_H)];
    [self addSubview:self.progressView];
    self.progressView.hidden = YES;
    self.progress = 1.0;
    
    //创建提示语label
    CGFloat msgLabel_H = 20;
    msgLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, self.progressView.frame.origin.y - msgLabel_H, self.frame.size.width, msgLabel_H)];
    msgLabel.text = @"上划取消，下划编辑";
    msgLabel.backgroundColor = [UIColor clearColor];
    msgLabel.textColor = [UIColor whiteColor];
    msgLabel.textAlignment = NSTextAlignmentCenter;
    msgLabel.font = [UIFont boldSystemFontOfSize:14];
    msgLabel.hidden = YES;
    [self addSubview:msgLabel];
}

-(void)initializationKeyboardBtnWith_Y:(CGFloat)y{
    
    CGFloat boomPadding = BOOMPADDING;
    _defaultView = [[UIView alloc] initWithFrame:CGRectMake(0, y - boomPadding, [UIScreen mainScreen].bounds.size.width, boomPadding)];
    _defaultView.backgroundColor = [UIColor clearColor];
    [self addSubview:_defaultView];
    //创建键盘按钮
    UIImage* keyboardImage = [UIImage imageNamed:@"键盘图标.png"];
    UIButton* keyboardBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [keyboardBtn setBackgroundImage:keyboardImage forState:UIControlStateNormal];
    [keyboardBtn addTarget:self action:@selector(keyboardButtonPress:) forControlEvents:UIControlEventTouchUpInside];
    CGFloat rightPadding = 27;
    CGFloat keyboardBtn_X = self.frame.size.width - keyboardImage.size.width - rightPadding;
    keyboardBtn.frame = CGRectMake(keyboardBtn_X, 0, keyboardImage.size.width, keyboardImage.size.height);
    [_defaultView addSubview:keyboardBtn];
    
    //创建涂鸦按钮
    UIButton* graffitiBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    UIImage* graffitiImage = [UIImage imageNamed:@"涂鸦.png"];
    [graffitiBtn setBackgroundImage:graffitiImage forState:UIControlStateNormal];
    [graffitiBtn addTarget:self action:@selector(showGraffitiAction:) forControlEvents:UIControlEventTouchUpInside];
    CGFloat graffitiBtn_X = keyboardBtn.frame.origin.x - graffitiImage.size.width - rightPadding;
    graffitiBtn.frame = CGRectMake(graffitiBtn_X, keyboardBtn.frame.origin.y, graffitiImage.size.width, graffitiImage.size.height);
    [_defaultView addSubview:graffitiBtn];
}

-(void)initFontNameLabel{
    
    UIImage* graffitiImage = [UIImage imageNamed:@"涂鸦.png"];
    CGFloat paddingToBoom = graffitiImage.size.height + BOOMPADDING;
    
    __weak RootRecordView *weakSelf = self;
    _fontView = [[FontBtnView alloc] initWithFrame:CGRectMake(0, self.frame.size.height, [UIScreen mainScreen].bounds.size.width, paddingToBoom)];
    _fontView.cancelBlock = ^(){
        [weakSelf cancelBtnAction];
    };
    _fontView.changeBlock = ^(){
        [weakSelf changeBtnAction];
    };
    _fontView.downLoadFontBlock = ^(){
        [weakSelf downLoadFont];
    };
    _fontView.confirmBlock = ^(){
        [weakSelf confirmAction];
    };
    _fontView.colorBlock = ^(UIColor *currentColor){
        weakSelf.inputTextField.textColor = currentColor;
    };
    [self addSubview:_fontView];
    
}

-(void)initGraffitiView{
    
    UIImage* graffitiImage = [UIImage imageNamed:@"涂鸦.png"];
    CGFloat paddingToBoom = graffitiImage.size.height + BOOMPADDING;
    
    //创建涂鸦板
    _graffitView = [[GraffitiView alloc] initWithFrame:self.frame];
    _graffitView.backgroundColor = [UIColor clearColor];
    _graffitView.hidden = YES;
    [self addSubview:_graffitView];
    
    
    __weak RootRecordView *weakSelf = self;
    _graffitiBtnView = [[GraffitiBtnView alloc] initWithFrame:CGRectMake(0, self.frame.size.height, [UIScreen mainScreen].bounds.size.width,paddingToBoom)];
    _graffitiBtnView.colorBlock = ^(UIColor *currentColor){
        
        [weakSelf.graffitView setCurrentColorWith:currentColor];
    };
    _graffitiBtnView.cancelBlock = ^(){
        [UIView animateWithDuration:ANIMATETIME animations:^{
            [weakSelf showDefaultBtnView];
        } completion:^(BOOL finished) {
            weakSelf.graffitiBtnView.hidden = YES;
            weakSelf.graffitView.hidden = YES;
            [weakSelf.graffitView resetGraffitiView];
        }];
    };
    _graffitiBtnView.confirmBlock = ^(){
        
        [weakSelf finishGraffitAction];
    };
    _graffitiBtnView.revokeBlock = ^(){
        
        [weakSelf.graffitView revokeAction];
        
    };
    _graffitiBtnView.strokeBlock = ^(){
        
        [weakSelf.graffitView strokeAction];
        [weakSelf.graffitiBtnView setImgWithWidth:weakSelf.graffitView.width];
    };
    [self addSubview:_graffitiBtnView];
}

-(void)startRunCapture{
    [_recorder startRunning];
    self.preview.hidden = NO;
}

-(void)stopRunCapture{
    [_recorder stopRunning];
    
    self.preview.hidden = YES;
}

-(void)startRecordView{
    //开始录制
    NSString* filePath = [SaveManager getVideoSaveFilePathString];
    [_recorder startRecordingToOutputFileURL:[NSURL fileURLWithPath:filePath]];
    
    //做进度条动画
    [self startProgressAnimation];
    //显示默认按钮
    [self finishGraffitAction];
}
-(void)stopRecordView{
    [_recorder stopCurrentVideoRecording];
    
    //停止进度条动画
    [self stopProgressAnimation];
    [self resetProgressView];
}

-(void)resumeRecordView{

    [_recorder resumeCurrentVideoRecording];
    //停止进度条动画
    [self stopProgressAnimation];
    [self resetProgressView];
}

-(void)capturePhone{
    [_recorder capturePhoto];
}

-(BOOL)isFrontCamera{
    return [_recorder isFrontCameraSupported];
}

-(void)switchCamera{
    [_recorder switchCamera];
}

-(void)openFlash:(BOOL)isOpen{
    [_recorder openTorch:isOpen];
}

//判断是否需要添加水印
-(BOOL)needAddMark{
    return [_graffitView hasEdit] || [_inputTextField.text length]>0?YES:NO;
}

#pragma mark 进度条动画
-(void)progressAction:(NSTimer*)timer{
    
    self.progress -= 0.0038f;
    if (self.progress < 0) {
        //停止进度条动画
        [self stopProgressAnimation];
        [self resetProgressView];
        //停止录制
        [self stopRecordView];
        //文字输入框回空
        [self resetInputText];
        return;
    }
    msgLabel.hidden = NO;
    self.progressView.progress = self.progress;
}

-(void)startProgressAnimation{
    
    self.progressView.hidden = NO;
    self.timer = [NSTimer scheduledTimerWithTimeInterval:0.05f target:self selector:@selector(progressAction:) userInfo:nil repeats:YES];
}

-(void)stopProgressAnimation{
    [self.timer invalidate];
    self.progress = 1.0f;
    self.progressView.hidden = YES;
}

-(void)resetProgressView{
    [self.progressView resetProgress];
    msgLabel.hidden = YES;
    msgLabel.text = @"上划取消，下划编辑";
}

#pragma mark 按钮事件
-(void)tapGestureRecognizerAction:(UITapGestureRecognizer*)recognizer{
    CGPoint point = [recognizer locationInView:recognizer.view];
    //获取焦点
    [_recorder focusInPoint:point];
    
    WaitingAlertView* alert = [[WaitingAlertView alloc] initWithFrame:CGRectMake(0, 0, 50, 50) mode:WaitingAlertView_Mode_Full];
    alert.center = point;
    [recognizer.view addSubview:alert];
    [NSTimer scheduledTimerWithTimeInterval:ANIMATETIME target:self selector:@selector(timerFire:) userInfo:alert repeats:NO];
}

-(void)timerFire:(NSTimer*)timer{
    WaitingAlertView* alert = timer.userInfo;
    [alert removeFromSuperview];
}


-(void)keyboardButtonPress:(id)sender{
    _inputTextField.enabled = YES;
    //点击按钮需要弹起键盘
    _showKeyBord = YES;
    [_graffitView sendSubviewToBack:_inputTextField];
    [_inputTextField becomeFirstResponder];
    [UIView animateWithDuration:ANIMATETIME animations:^{
        [self showFontBtnView];
    }];
}

-(void)showGraffitiAction:(id)sender{
    
    _graffitView.hidden = NO;
    _graffitiBtnView.hidden = NO;
    [_inputTextField sendSubviewToBack:_graffitView];

    [UIView animateWithDuration:ANIMATETIME animations:^{
        
        [self showGraffitiBtnView];
        [_graffitiBtnView bringSubviewToFront:_graffitView];
        _graffitView.userInteractionEnabled = YES;
    }];
}

#pragma mark --font action
-(void)cancelBtnAction{
    
    _inputTextField.text = @"";
    
    [_inputTextField resignFirstResponder];
    //收起键盘时时enable不响应时间
    _inputTextField.enabled = NO;
    _showKeyBord = NO;
    [UIView animateWithDuration:ANIMATETIME animations:^{
        
        [self showDefaultBtnView];
        
    }];
}

-(void)downLoadFont{
    
    if (_downLoadFontBlock) {
        _downLoadFontBlock();
    }
}

static int fontIndex = 0;
-(void)changeBtnAction{
    
    NSArray *fontArray = [AppUtils getFontArray:FONTCACHE size:36];
    fontIndex++;
    UIFont *font = nil;
    if (fontIndex == [fontArray count]) {
        fontIndex = 0;
    }
    font = [fontArray objectAtIndex:fontIndex];
    _inputTextField.font = font;
}

-(void)confirmAction{
    
    [_inputTextField resignFirstResponder];
    //收起键盘时时enable不响应时间
    _inputTextField.enabled = NO;
    //收起键盘需要重置属性,此时用户点击textfield不能弹起键盘
    _showKeyBord = NO;
    [UIView animateWithDuration:ANIMATETIME animations:^{
        [self showDefaultBtnView];
    }];
}

-(void)handlePan:(UIPanGestureRecognizer*)recognizer{
    
    CGPoint translation = [recognizer translationInView:_inputTextField];
    recognizer.view.center = CGPointMake(recognizer.view.center.x + translation.x,
                                         recognizer.view.center.y + translation.y);
    [recognizer setTranslation:CGPointZero inView:_inputTextField];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch{
    
    // 上一个点的位置
    CGPoint previous = [touch previousLocationInView:self];
    // 当前点的位置
    CGPoint current = [touch locationInView:self];
    
    CGPoint center = _inputTextField.center;
    center.x += current.x - previous.x;
    center.y += current.y - previous.y;
    _inputTextField.center = center;
    
    return YES;
}

#pragma mark 输入框接口
-(NSString*)inputText{
    return _inputTextField.text;
}

-(void)resetInputText{
    _inputTextField.text = @"";
}

-(void)setMsgLabelText:(NSString*)text{
    self.msgLabel.text = text;
}


-(void)needShowTextBtn:(BOOL)show{
    _defaultView.hidden = !show;
    _fontView.hidden = !show;
    _graffitiBtnView.hidden = !show;
}

-(void)showDefaultBtnView{
    
    _defaultView.frame = CGRectMake(0, self.frame.size.height - BOOMPADDING, _defaultView.frame.size.width, _defaultView.frame.size.height);
    _fontView.frame = CGRectMake(0, self.frame.size.height, _fontView.frame.size.width, _fontView.frame.size.height);
    
    UIImage* graffitiImage = [UIImage imageNamed:@"涂鸦.png"];
    CGFloat paddingToBoom = graffitiImage.size.height + BOOMPADDING;
    _graffitiBtnView.frame = CGRectMake(0, self.frame.size.height, [UIScreen mainScreen].bounds.size.width,paddingToBoom);
    
    [_inputTextField resignFirstResponder];
}

-(void)showFontBtnView{
    
    _defaultView.frame = CGRectMake(0, self.frame.size.height, _defaultView.frame.size.width, _defaultView.frame.size.height);
    
    UIImage* graffitiImage = [UIImage imageNamed:@"涂鸦.png"];
    CGFloat paddingToBoom = graffitiImage.size.height + BOOMPADDING;
    
    _fontView.frame = CGRectMake(0, self.frame.size.height - paddingToBoom, _fontView.frame.size.width, _fontView.frame.size.height);
    _graffitiBtnView.frame = CGRectMake(0, self.frame.size.height, [UIScreen mainScreen].bounds.size.width,paddingToBoom);
}

-(UIImage*)getMarkImg{
    
    //如果不输入文字就可以不用加了
    UIImageView *inputView;
    if ([_inputTextField.text length] > 0) {
        UIImage *inputImg = [AppUtils imageWithUIView:_inputTextField];
        inputView = [[UIImageView alloc] initWithImage:inputImg];
        inputView.frame = _inputTextField.frame;
        [_graffitView addSubview:inputView];
    }
    
    UIImage *markImg = [AppUtils imageWithUIView:_graffitView];
    //移除 释放
    [inputView removeFromSuperview];
    inputView = nil;
    return markImg;
}

- (UIImage *)addlogoImage:(UIImage *)resizedImage
{
    UIGraphicsBeginImageContext(resizedImage.size);
    [resizedImage drawInRect:CGRectMake(0, 0, resizedImage.size.width, resizedImage.size.height)];
    
    UIImage *maskImage  = [self getMarkImg];
    
    [maskImage drawInRect:CGRectMake((resizedImage.size.width - maskImage.size.width)/2,
                                     (resizedImage.size.height - maskImage.size.height)/2,
                                     maskImage.size.width,
                                     maskImage.size.height)];
    
    UIImage *resultingImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return resultingImage;
    
}

//13911517702/05448166
-(void)showGraffitiBtnView{
    
    _fontView.frame = CGRectMake(0, self.frame.size.height, _fontView.frame.size.width, _fontView.frame.size.height);
    _defaultView.frame = CGRectMake(0, self.frame.size.height, _defaultView.frame.size.width, _defaultView.frame.size.height);
    
    CGFloat boomPadding = BOOMPADDING;
    UIImage *revokeImg = [UIImage imageNamed:@"撤销.png"];
    CGFloat paddingToBoom = revokeImg.size.height + boomPadding;
    _graffitiBtnView.frame = CGRectMake(0, self.frame.size.height - paddingToBoom, [UIScreen mainScreen].bounds.size.width,paddingToBoom);
}

-(void)finishGraffitAction{
    
    [UIView animateWithDuration:ANIMATETIME animations:^{
        [self showDefaultBtnView];
    } completion:^(BOOL finished) {
        _graffitiBtnView.hidden = NO;
        _graffitView.hidden = NO;
        _graffitView.userInteractionEnabled = NO;
    }];
    
}

-(void)resetGraffitView{
    
    _graffitView.hidden = YES;
    [_graffitView resetGraffitiView];
}

#pragma mark - SBVideoRecorderDelegate
- (void)videoRecorder:(SBVideoRecorder *)videoRecorder didFinishRecordingToOutPutFileAtURL:(NSURL *)outputFileURL duration:(CGFloat)videoDuration totalDur:(CGFloat)totalDur error:(NSError *)error
{
    if (totalDur < 1.0) {
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"提示" message:@"录制时间太短了" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil];
        [alert show];
        return;
    }
    //对视频进行融合和裁剪
    [_recorder mergeVideoFiles];

    //对视频进行备份
//    NSString* filePath = [outputFileURL absoluteString];
//    NSString* dirPath = [filePath stringByDeletingLastPathComponent];
//    NSURL* newUrl = [NSURL URLWithString:[dirPath stringByAppendingPathComponent:@"temp.mp4"]];
//    [[NSFileManager defaultManager] copyItemAtURL:outputFileURL toURL:newUrl error:nil];
    //通知代理
//    if (self.delegate && [self.delegate respondsToSelector:@selector(didRecordFinish:videoTime:)]) {
//        [self.delegate didRecordFinish:outputFileURL videoTime:videoDuration];
//    }
}

- (void)videoRecorder:(SBVideoRecorder *)videoRecorder didFinishMergingVideosToOutPutFileAtURL:(NSURL *)outputFileURL totalDur:(CGFloat)totalDur
{

//    NSString* inputStr = outputFileURL.absoluteString;
//    NSString* imageName = [SaveManager bundlePath:@"aiaiLogo.jpg"];
//    NSString* insetTimeStr = [NSString stringWithFormat:@"%f",totalDur];
//    NSString* outputStr = [SaveManager tmpPath:@"blendImageVedio.mp4"];
//    if ([[NSFileManager defaultManager] fileExistsAtPath:outputStr]) {
//        [[NSFileManager defaultManager] removeItemAtPath:outputStr error:nil];
//    }
//    
//    int size = 0;
//    char** commandLinestr = [CommandLine blendVideoAndImagesWithSize:&size inputVideoPath:inputStr waterMark:imageName insetTime:insetTimeStr outputVideoPath:outputStr];
//    ffmpegMain(size,commandLinestr);
//    [CommandLine releaseCommandLineStr:commandLinestr size:size];
    
    //通知代理
    if (self.delegate && [self.delegate respondsToSelector:@selector(didRecordFinish:videoTime:)]) {

        [self.delegate didRecordFinish:outputFileURL videoTime:totalDur];

    }
}

- (void)videoRecorder:(SBVideoRecorder *)videoRecorder didRecordingToOutPutFileAtURL:(NSURL *)outputFileURL duration:(CGFloat)videoDuration recordedVideosTotalDur:(CGFloat)totalDur
{
    
    
}

-(void)stillImage:(SBVideoRecorder *)videoRecorder didCapturePhotoWithImage:(UIImage *)photo{
    //通知代理
    if (self.delegate && [self.delegate respondsToSelector:@selector(didTakePhotoFinish:)]) {
        [self.delegate didTakePhotoFinish:photo];
    }
}

#pragma mark - 允许用户输入文字的输入框

-(BOOL)textFieldShouldReturn:(UITextField *)textField{
    
    [_inputTextField resignFirstResponder];
    //收起键盘时时enable不响应时间
    _inputTextField.enabled = NO;
    _showKeyBord = NO;
    [UIView animateWithDuration:ANIMATETIME animations:^{
        
        [self showDefaultBtnView];
        
    }];
   
    return NO;
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
    return _showKeyBord;
}

@end
