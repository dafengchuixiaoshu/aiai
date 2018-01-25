//
//  BestSellersModel.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/11/25.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseModel.h"

@interface BestSellersModel : BaseModel

@property(nonatomic,strong)NSArray* ids;
@property(nonatomic,copy)NSString* logoPic;
@property(nonatomic,copy)NSString* orderBySql;
@property(nonatomic,copy)NSNumber* pageSize;
@property(nonatomic,copy)NSNumber* siteId;
@property(nonatomic,copy)NSString* siteName;
@property(nonatomic,copy)NSString* siteUrl;
@property(nonatomic,copy)NSString* sqlStr;
@property(nonatomic,copy)NSNumber* start;
@property(nonatomic,copy)NSNumber* status;

@end
