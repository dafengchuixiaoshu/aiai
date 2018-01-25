//
//  ScouringTheSeaCommon.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/10/28.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#ifndef STSCommon_h
#define STSCommon_h

#import "SaveManager.h"
#import <AFNetworking/AFNetworking.h>
#import "AppUtils.h"
#import "STSCommon.h"
#import "MJExtension.h"
#import "UIColor+Hex.h"
#import "ConfigManager.h"
#import "GlobalUserInfo.h"
#import "RequestUrl.h"

//定义三种版本模式
#define DEBUG_MODE           0  //线下
#define RELEASE_MODE         1  //预发布
#define DESCRIPTION_MODE     2  //线上

//根据版本模式增加不同开关
#define URL_TYPE     DEBUG_MODE //修改URL入口的地方的地方
#define STS_LOG      DEBUG_MODE //修改是否打印log

//**********************开关实现部分****************************//

//线上线下的url逻辑
#if (URL_TYPE == DEBUG_MODE)

#define BASEURL @"http://10.0.247.44/rpm?"

#elif (URL_TYPE == RELEASE_MODE)

#define BASEURL @"192.168.1.1"

#else

#define BASEURL @"www.baidu.com"

#endif


//自定log 调试用

#if !STS_LOG
#    define STS_Log(...) NSLog(__VA_ARGS__)
#else
#    define STS_Log(...)
#endif

#endif /* STSCommon_h */
