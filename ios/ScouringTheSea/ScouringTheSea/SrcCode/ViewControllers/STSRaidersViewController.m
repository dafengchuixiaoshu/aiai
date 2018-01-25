//
//  STSRaidersViewController.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/11/19.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "STSRaidersViewController.h"

@interface STSRaidersViewController ()

@end

@implementation STSRaidersViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    UILabel* label = [[UILabel alloc] initWithFrame:CGRectMake(50, 50, 50, 50)];
    label.text = @"海淘攻略";
    label.textColor = [UIColor whiteColor];
    [self.view addSubview:label];
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
