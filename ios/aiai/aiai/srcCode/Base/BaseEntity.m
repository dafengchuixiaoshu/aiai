//
//  BaseEntity.m
//  ZLYDoc
//
//  Created by Ryan on 14-4-3.
//  Copyright (c) 2014年 ZLY. All rights reserved.
//

#import "BaseEntity.h"

@implementation BaseEntity

+ (BOOL)checkResponseIsInvalid:(id)response {
    if (nil == response) {
        return YES;
    }
    if ([response isEqual:[NSNull null]]) {
        return YES;
    }
    return NO;
}

+(BaseEntity* )parseResponseErrorDic:(NSDictionary*)dic_json
{
    if ([NSJSONSerialization isValidJSONObject:dic_json]) {
        BaseEntity *baseEntity = [[BaseEntity alloc] init];
        return baseEntity;
    }
    return nil;
}

+(BaseEntity* )parseResponseErrorJSON:(id)json
{
    NSString *responseJSON = [NSString stringWithFormat:@"%@",json];
    NSData *jsonData = [responseJSON dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *dic_json = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];
    if ([NSJSONSerialization isValidJSONObject:dic_json]) {
        BaseEntity *baseEntity = [[BaseEntity alloc] init];
        return baseEntity;
    }
    return nil;
}

+ (BaseEntity *)parseResponseStatusJSON:(id)json
{
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:json options:NSJSONWritingPrettyPrinted error:nil];
    //格式化打印输出至控制台
//    NSString *responseJSON = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    NSDictionary *dic_json = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];
    if ([NSJSONSerialization isValidJSONObject:dic_json]) {
        BaseEntity *baseEntity = [[BaseEntity alloc] init];
        return baseEntity;
    }
    return nil;
}

+(BaseEntity *)parseResponseUpdateJSON:(id)json
{
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:json options:NSJSONWritingPrettyPrinted error:nil];
    //格式化打印输出至控制台
//    NSString *responseJSON = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];

    NSDictionary *dic_json = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:nil];
    if ([NSJSONSerialization isValidJSONObject:dic_json]) {
        BaseEntity *baseEntity = [[BaseEntity alloc] init];
        return baseEntity;
    }
    return nil;
}

@end
