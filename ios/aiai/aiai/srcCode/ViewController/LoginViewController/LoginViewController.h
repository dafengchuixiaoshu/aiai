//
//  LoginViewController.h
//  F2CPlatform
//
//  Created by BJMD131464 on 13-9-3.
//  Copyright (c) 2013年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^loginLeftItemBtnAction)(void);

@interface LoginViewController : BaseViewController

@property(nonatomic,strong)loginLeftItemBtnAction leftItemBtnBlock;

//只有第一次走登录流程，其他情况直接登录IM即可
+(void)loginWithCacheUserName:(NSString*)userName password:(NSString*)passWord;

+ (void)loginWithUserName:(NSString*)userName password:(NSString*)passWord;
//第三方登录
+ (void)loginWithOpenPlatformID:(NSString*)openPlatformID OpenPlatformCode:(NSString*)sourceCode finished:(void (^)(void))finished;
@end
