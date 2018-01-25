//
//  BaseViewController.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/10/28.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"

@interface BaseViewController ()

@end

@implementation BaseViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    //设置背景颜色
    self.view.backgroundColor = [UIColor colorWithHexString:STS_GRAY_BACKGOUND_COLOR];
    
    //预设的视图frame如果没有被设置，则默认为当前视图frame，否则把frame设置为预设值
    if (CGRectEqualToRect(_presetViewFrame, CGRectZero)) {
        _presetViewFrame = self.view.frame;
    }else{
        self.view.frame = _presetViewFrame;
    }
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
