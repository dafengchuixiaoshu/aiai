//
//  GlobalUrlHeader.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/4.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

//上传crash的url
#define UPLOAD_CRASHLOG_URL @""
//非苹果商店版本
//#define NO_APP_STORE_VERTION
//进入程序后消息数统一置为0
#define BadgeNumberZero


#define Debug_URL           0
#define Release_URL         1
#define Distribution_URL    2

//是否有日志
//#define AIAI_DEBUG

//同否通过服务器进行登录，不定义的话使用默认账户登录
//+ (void)loginWithUserName:(NSString*)userName password:(NSString*)passWord
#define LOGINSUCCESS   

//修改URL的地方
#define URL_TYPE Distribution_URL

#if (URL_TYPE == Debug_URL)//测试



#elif (URL_TYPE == Release_URL)//ii正式IP

//红姐接口的Url,//注册登录注销修改信息相关接口url
#define LOGIN_MODEL_SERVICE_URL @"http://182.92.189.13:9080/SpreeCMS/"
//sns接口
#define SNS_SERVICE_URL     @"http://182.92.189.13:9980/httpProxy"
//图片服务器url
#define IMAGE_SERVICE_URL   @"http://123.57.251.23:4869/"
//版本检测
#define CHECKSOFT_URL       @"http://123.57.251.23:10080/wbportal/soft/select_version.do?"
//短信邀请
#define MESSAGE_DOWNLOAD_URL       @"http://dwz.cn/24DS7y"
//IM接口
#define CMMP_BASE_IP    "123.57.251.23"
#define CMMP_BASE_PORT  "8000"
#define CMMP_IMG_IP     "123.57.251.23"
#define CMMP_IMG_PORT   4869

#else//正式

//红姐接口的Url,//注册登录注销修改信息相关接口url
#define LOGIN_MODEL_SERVICE_URL @"http://sns.xmatrix.com.cn:9080/SpreeCMS/"
//sns接口
#define SNS_SERVICE_URL     @"http://182.92.189.13:9980/httpProxy"
//图片服务器url
#define IMAGE_SERVICE_URL   @"http://img.xmatrix.com.cn:80/"
//版本检测
#define CHECKSOFT_URL       @"http://sns.xmatrix.com.cn:10080/wbportal/soft/select_version.do?"
//短信邀请
#define MESSAGE_DOWNLOAD_URL       @"http://dwz.cn/24DS7y"
//IM接口
#define CMMP_BASE_IP    "123.57.251.23"
#define CMMP_BASE_PORT  "8000"
#define CMMP_IMG_IP     "123.57.251.23"
#define CMMP_IMG_PORT   4869

#endif