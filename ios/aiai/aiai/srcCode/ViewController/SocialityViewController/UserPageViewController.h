//
//  UserPageViewController.h
//  aiai
//
//  Created by wangnan on 15/11/18.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"

@interface UserPageViewController : BaseViewController

-(id)initWithUid:(NSString*)uid;

@property(nonatomic, copy)NSString *titleStr;       //用来设置title;

@end
