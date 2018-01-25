//
//  RegisterViewController.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/26.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "BaseViewController.h"

typedef void (^registerLeftItemBtnAction)(void);

@interface RegisterViewController : BaseViewController

@property(nonatomic,strong)registerLeftItemBtnAction leftItemBtnBlock;

//第三方登录使用
@property(nonatomic, copy)NSString *openPlatformId;
@property(nonatomic, copy)NSString *openPlatformcode;
@end
