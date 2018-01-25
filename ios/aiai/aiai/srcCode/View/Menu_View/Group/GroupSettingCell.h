//
//  GroupSettingCell.h
//  aiai
//
//  Created by wangnan on 15/4/16.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

#define GROUPSETTINGCELLHEIGHT 58

typedef void (^newMesgAction)(BOOL on);
typedef void (^changeNameAction)(NSString *newName);

@interface GroupSettingCell : UITableViewCell

@property(nonatomic, strong)newMesgAction msgActionBlock;
@property(nonatomic, strong)changeNameAction changeNameBlock;

-(void)setTitleText:(NSString*)title subText:(NSString*)subTitle needSwitch:(BOOL)need andSwitchOn:(BOOL)on;

@end
