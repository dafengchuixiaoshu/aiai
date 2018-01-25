//
//  STSRaidersModel.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/10.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

//海淘攻略 model

#import "BaseModel.h"

@interface STSRaidersModel : BaseModel

@property(nonatomic,copy)NSNumber* articleCid;
@property(nonatomic,copy)NSString* articlePic;
@property(nonatomic,copy)NSString* content;
@property(nonatomic,copy)NSNumber* id;
@property(nonatomic,strong)NSArray* ids;
@property(nonatomic,copy)NSNumber* insertTime;
@property(nonatomic,copy)NSString* keyword;
@property(nonatomic,copy)NSString* orderBySql;
@property(nonatomic,copy)NSNumber* pageSize;
@property(nonatomic,copy)NSNumber* publishStatus;
@property(nonatomic,copy)NSNumber* sort;
@property(nonatomic,copy)NSString* sqlStr;
@property(nonatomic,copy)NSNumber* start;
@property(nonatomic,copy)NSNumber* status;
@property(nonatomic,copy)NSString* title;
@property(nonatomic,copy)NSNumber* updateTime;
@property(nonatomic,copy)NSString* url;

@end
