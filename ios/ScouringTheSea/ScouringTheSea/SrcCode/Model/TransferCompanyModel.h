//
//  TransferCompanyModel.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/7.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseModel.h"

@interface TransferCompanyModel : BaseModel


@property(nonatomic,copy)NSString* address;
@property(nonatomic,copy)NSNumber* appendPound;
@property(nonatomic,copy)NSNumber* avgDays;
@property(nonatomic,copy)NSString* email;
@property(nonatomic,copy)NSString* fax;
@property(nonatomic,copy)NSNumber* firstPound;
@property(nonatomic,copy)NSNumber* id;
@property(nonatomic,strong)NSArray* ids;
@property(nonatomic,copy)NSString* introduction;
@property(nonatomic,copy)NSString* logo;
@property(nonatomic,copy)NSString* name;
@property(nonatomic,copy)NSString* orderBySql;
@property(nonatomic,copy)NSString* outUrl;
@property(nonatomic,copy)NSNumber* packageLeve;
@property(nonatomic,copy)NSNumber* pageSize;
@property(nonatomic,copy)NSNumber* serviceLeve;
@property(nonatomic,copy)NSNumber* speedLevel;
@property(nonatomic,copy)NSString* sqlStr;
@property(nonatomic,copy)NSNumber* start;
@property(nonatomic,copy)NSNumber* status;
@property(nonatomic,copy)NSString* telphone;

@end
