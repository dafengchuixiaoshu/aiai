//
//  UIFont+Adapter.m
//  aiai
//
//  Created by zhangyang on 15/3/25.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "UIFont+Adapter.h"

@implementation UIFont (Adapter)

+(UIFont*)systemFontWithAdapter:(CGFloat)fontSize{
    CGFloat DeviceDeviation = 3;
    if (iPhone5) {
        return [UIFont systemFontOfSize:fontSize];
    }else if (iPhone6){
        return [UIFont systemFontOfSize:fontSize+DeviceDeviation];
    }else if (iPhone6_Plus){
        return [UIFont systemFontOfSize:fontSize+DeviceDeviation*2];
    }
    return [UIFont systemFontOfSize:fontSize];
}

+(UIFont*)boldFontWithAdapter:(CGFloat)fontSize{
    CGFloat DeviceDeviation = 3;
    if (iPhone5) {
        return [UIFont boldSystemFontOfSize:fontSize];
    }else if (iPhone6){
        return [UIFont boldSystemFontOfSize:fontSize+DeviceDeviation];
    }else if (iPhone6_Plus){
        return [UIFont boldSystemFontOfSize:fontSize+DeviceDeviation*2];
    }
    return [UIFont boldSystemFontOfSize:fontSize];
}

@end
