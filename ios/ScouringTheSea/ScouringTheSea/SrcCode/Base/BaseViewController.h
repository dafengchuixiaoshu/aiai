//
//  BaseViewController.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/10/28.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BaseViewController : UIViewController

//为了避免出现controller.view调用的时候提前调用viewdidload产生bug
@property(nonatomic)CGRect presetViewFrame;//预设视图的frame

@end
