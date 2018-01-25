//
//  BaseEntity.h
//  ZLYDoc
//
//  Created by Ryan on 14-4-3.
//  Copyright (c) 2014年 ZLY. All rights reserved.
//

#import <Foundation/Foundation.h>

#define SUCCESS 0

@interface BaseEntity : NSObject

//判断数据类型是否无效，目前只支持判断nil和null，YES无效，NO有效
+ (BOOL)checkResponseIsInvalid:(id)response;

//解析返回异常json
+(BaseEntity* )parseResponseErrorDic:(NSDictionary*)dic_json;
/**
 *  解析HTTP返回异常JSON
 *
 *  @param json
 *
 *  @return 
 */
+ (BaseEntity *)parseResponseErrorJSON:(id)json;

/**
 *  解析成功或失败状态JSON
 *
 *  @param json
 *
 *  @return 
 */
+ (BaseEntity *)parseResponseStatusJSON:(id)json;

/**
 *  解析版本号及升级URL JSON
 *
 *  @param json
 *
 *  @return 
 */
+ (BaseEntity *)parseResponseUpdateJSON:(id)json;

@end
