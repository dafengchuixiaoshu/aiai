//
//  GoodsDetailModel.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/17.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "GoodsDetailModel.h"

@implementation GoodsDetailModel

+ (NSDictionary *)replacedKeyFromPropertyName{
    return @{@"nowSiteCids":@"newSiteCids"};
}

@end
