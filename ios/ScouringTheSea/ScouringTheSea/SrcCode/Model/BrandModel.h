//
//  BrandModel.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/11/25.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseModel.h"

@interface BrandModel : BaseModel

@property(nonatomic,copy)NSNumber* brandId;
@property(nonatomic,copy)NSString* brandName;
@property(nonatomic,strong)NSArray* ids;
@property(nonatomic,copy)NSString* imgUrl;
@property(nonatomic,copy)NSString* logoUrl;
@property(nonatomic,copy)NSString* orderBySql;
@property(nonatomic,copy)NSNumber* outId;
@property(nonatomic,copy)NSNumber* pageSize;
@property(nonatomic,copy)NSNumber* sameId;
@property(nonatomic,copy)NSString* sqlStr;
@property(nonatomic,copy)NSNumber* start;
@property(nonatomic,copy)NSNumber* status;

@end
