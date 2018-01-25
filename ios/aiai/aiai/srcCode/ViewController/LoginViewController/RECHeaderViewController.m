//
//  RECHeaderViewController.m
//  aiai
//
//  Created by zhangyang on 15/4/3.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "RECHeaderViewController.h"

#import "RootRecordView.h"

@interface RECHeaderViewController ()<UIGestureRecognizerDelegate,RootRecordViewDelegate>{
    CGPoint beginPoint;    //手势开始按下的位置
    CGPoint endPoint;
}
//录制视图
@property(nonatomic,strong)RootRecordView* recordView;
@property(nonatomic,strong)UILabel* REC_Label;//录制label

@property(nonatomic,copy)NSString* movePath;
@property(nonatomic,copy)NSNumber* moveTime;
@end

@implementation RECHeaderViewController

-(id)init{
    if (self = [super init]) {

    }
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    
    self.centerBtnImageName = @"flashon_open@2x.png";
    self.rightBtnImageName = @"changecamera@2x.png";
    
    //创建录制视频相关UI
    _recordView = [[RootRecordView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.width)];
    _recordView.delegate = self;
    [_recordView setMsgLabelText:@"上划取消"];
    [_recordView needShowTextBtn:NO];
    [_recordView startRunCapture];
    [self.view insertSubview:_recordView atIndex:0];
    
    CGFloat label_W = 120;
    CGFloat label_H = 40;
    CGFloat label_X = (_recordView.frame.size.width - label_W) /2;
    CGFloat label_Y = _recordView.frame.origin.y+_recordView.frame.size.height + 30;
    _REC_Label = [[UILabel alloc] initWithFrame:CGRectMake(label_X, label_Y, label_W, label_H)];
    _REC_Label.layer.cornerRadius = label_H/2;
    _REC_Label.layer.masksToBounds = YES;
    _REC_Label.layer.borderColor = [UIColor whiteColor].CGColor;
    _REC_Label.layer.borderWidth = 1.0;
    _REC_Label.font = [UIFont systemFontWithAdapter:14];
    _REC_Label.text = @"长按录制";
    _REC_Label.textAlignment = NSTextAlignmentCenter;
    _REC_Label.textColor = [UIColor whiteColor];
    _REC_Label.userInteractionEnabled = YES;
    [self.view addSubview:_REC_Label];
    
    //头像添加长按事件
    UILongPressGestureRecognizer* recognizer = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressFromUp:)];
    recognizer.delegate = self;
    [_REC_Label addGestureRecognizer:recognizer];
}


-(void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    [_recordView stopRunCapture];
}

#pragma mark titleButtonAction

-(void)centerItemClicked:(id)sender{
    if ([self.recordView isFrontCamera]) {
        return;
    }
    UIButton* flashBtn = (UIButton*)sender;
    flashBtn.selected = !flashBtn.selected;
    if(!flashBtn.isSelected){
        self.centerBtnImageName = @"flashon_open.png";
    }else{
        self.centerBtnImageName = @"flashon_close.png";
    }
    
    [self.recordView openFlash:flashBtn.selected];
    
}
-(void)rightItemClicked:(id)sender{
    [self.recordView switchCamera];
    
    self.centerBtnImageName = @"flashon_open.png";
}

#pragma mark gesture
-(void)handleLongPressFromUp:(UIGestureRecognizer*)recognizer{
    switch (recognizer.state) {
        case UIGestureRecognizerStateBegan:
        {
            beginPoint = [recognizer locationInView:self.view];
            
            [_recordView startRecordView];
        }
            break;
        case UIGestureRecognizerStateChanged:
        {
            CGPoint currentPoint = [recognizer locationInView:_REC_Label];
            
            if (currentPoint.y < 0) {
                
                [_recordView setMsgLabelText:@"松开取消"];
            }else{
                [_recordView setMsgLabelText:@"上划取消"];
            }
        }
            break;
        case UIGestureRecognizerStateEnded:
        {
            endPoint = [recognizer locationInView:_REC_Label];
            if (endPoint.y < 0) {
                //上划取消
                [_recordView setMsgLabelText:@"上划取消"];
                //重置摄像头
                [_recordView stopRecordView];
            }else{
                //长按录制
                [_recordView stopRecordView];
            }
        }
            break;
        default:
            break;
    }
}

#pragma mark RecordViewDelegate
-(void)didRecordFinish:(NSURL*)videoUrl videoTime:(CGFloat)time{
    
    if (self.delegate && [self.delegate respondsToSelector:@selector(REC_VideoPath:videoTime:)]) {
        [self.delegate REC_VideoPath:videoUrl videoTime:time];
    }
    
    [self leftItemClicked:nil];
}

@end
