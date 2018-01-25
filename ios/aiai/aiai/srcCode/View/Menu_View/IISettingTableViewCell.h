//
//  IISettingTableViewCell.h
//  aiai
//
//  Created by wangnan on 15/4/30.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

#define IISETTINGCELLHEIGHT 58

typedef NS_ENUM(NSInteger, settingType) {
    
    saveType = 0,
    loadType = 1,
    playType = 2
    
};

typedef void (^action)(BOOL on);

@interface IISettingTableViewCell : UITableViewCell


@property(nonatomic, strong)action actionBlock;

-(void)setTitleText:(NSString*)title subText:(NSString*)subTitle needSwitch:(BOOL)need andSwitchOn:(BOOL)on;

@end
