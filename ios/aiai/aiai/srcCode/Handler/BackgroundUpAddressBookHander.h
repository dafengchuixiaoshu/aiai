//
//  BackgroundUpAddressBookHander.h
//  aiai
//
//  Created by wangnan on 15/4/21.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseHandler.h"

#define AddressBookPath @"AddressBookPath.plist"

@interface BackgroundUpAddressBookHander : BaseHandler

+(void)upAddressBook;           //上传通讯录;

+(NSDictionary *)getAddressBooks;//获取本地联系人列表
@end