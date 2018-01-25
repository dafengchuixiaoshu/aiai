//
//  NSString+JSON.m
//  F2CPlatform
//
//  Created by zhangyang on 13-12-14.
//  Copyright (c) 2013年 zhangyang. All rights reserved.
//

#import "JSONFormat.h"

@implementation NSString (JSON)

-(id)myObjectFromJSONString
{
    id obj;
     obj  = [NSJSONSerialization JSONObjectWithData:[self dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingMutableContainers error:nil];
    return obj;
}
-(NSData*)myJSONData
{
    return [NSJSONSerialization dataWithJSONObject:self options:NSJSONWritingPrettyPrinted error:nil];
}
@end

@implementation NSData (JSON)
-(id)myObjectFromJSONData
{
    id obj;
    
    obj  = [NSJSONSerialization JSONObjectWithData:self options:NSJSONReadingMutableContainers error:nil];
    
    return obj;
}
@end


@implementation NSArray (JSON)
-(NSData*)myJSONData
{
    return [NSJSONSerialization dataWithJSONObject:self options:NSJSONWritingPrettyPrinted error:nil];
}
@end


@implementation NSDictionary (JSON)
-(NSData*)myJSONData
{
    return [NSJSONSerialization dataWithJSONObject:self options:NSJSONWritingPrettyPrinted error:nil];
}
@end