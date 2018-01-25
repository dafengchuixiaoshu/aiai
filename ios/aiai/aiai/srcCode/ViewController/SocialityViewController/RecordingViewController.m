//
//  PostViewController.m
//  aiai
//
//  Created by wangnan on 15/11/3.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "RecordingViewController.h"
#import "SelectFilterViewController.h"

#import "IISettingViewController+SavePhotoAndVideoManager.h"

#import "RootRecordView.h"

@interface RecordingViewController ()<RootRecordViewDelegate>
//录制视图
@property(nonatomic,strong)RootRecordView* recordView;
@end

@implementation RecordingViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    //设置title
    self.centerBtnImageName = @"flashon_open@2x.png";
    self.rightBtnImageName = @"changecamera@2x.png";
    
    _recordView = [[RootRecordView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.width)];
    _recordView.delegate = self;
    [_recordView needShowTextBtn:NO];
    [self.view insertSubview:_recordView atIndex:0];//放在最下边，要不title会被遮盖
    
}

-(void)viewWillAppear:(BOOL)animated{
    //使用主线程延迟来获取音源句柄
    [UIView animateWithDuration:0.05 animations:^{
        _recordView.alpha = 1.0;
    } completion:^(BOOL finished) {
        if (_recordView) {
            [_recordView startRunCapture];
        }
    }];
}

-(void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    
    if (_recordView) {
        [_recordView stopRunCapture];
        _recordView.alpha = 0.0;
    }
}

#pragma mark -- titleBtnAction
-(void)centerItemClicked:(id)sender{
    if ([_recordView isFrontCamera]) {
        return;
    }
    UIButton* flashBtn = (UIButton*)sender;
    flashBtn.selected = !flashBtn.selected;
    if(!flashBtn.isSelected){
        self.centerBtnImageName = @"flashon_open.png";
    }else{
        self.centerBtnImageName = @"flashon_close.png";
    }
    
    [_recordView openFlash:flashBtn.selected];
    
}
-(void)rightItemClicked:(id)sender{
    [self.recordView switchCamera];
    
    self.centerBtnImageName = @"flashon_open.png";
}

#pragma mark -- 录制视频回调
-(void)didRecordFinish:(NSURL*)videoUrl videoTime:(CGFloat)time{
    //保存视频
    [IISettingViewController saveVideoWithSetting:videoUrl];
    
    //进入视频编辑界面
    SelectFilterViewController* con = [[SelectFilterViewController alloc] initWithVideoUrl:videoUrl andTime:time];
    [self.navigationController pushViewController:con animated:YES];
    
    
    //需要把输入框设置成空
//    [self.recordView resetInputText];
//    [self.recordView resetGraffitView];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
