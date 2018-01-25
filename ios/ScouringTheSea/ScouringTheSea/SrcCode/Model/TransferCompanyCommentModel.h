//
//  TransferCompanyCommentModel.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/8.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseModel.h"

@interface TransferCompanyCommentModel : BaseModel

@property(nonatomic,copy)NSNumber* allscore;
@property(nonatomic,copy)NSString* commenttime;
@property(nonatomic,copy)NSString* commenturl;
@property(nonatomic,copy)NSNumber* companyid;
@property(nonatomic,copy)NSString* companyname;
@property(nonatomic,copy)NSNumber* discount;
@property(nonatomic,copy)NSString* goodstype;
@property(nonatomic,copy)NSNumber* id;
@property(nonatomic,strong)NSArray* ids;
@property(nonatomic,copy)NSString* istax;
@property(nonatomic,copy)NSNumber* istrue;
@property(nonatomic,copy)NSString* opdate;
@property(nonatomic,copy)NSString* orderBySql;
@property(nonatomic,copy)NSString* ordertime;
@property(nonatomic,copy)NSNumber* pageSize;
@property(nonatomic,copy)NSString* receivedate;
@property(nonatomic,copy)NSString* seller;
@property(nonatomic,copy)NSString* servicecomment;
@property(nonatomic,copy)NSNumber* servicescore;
@property(nonatomic,copy)NSString* speedcomment;
@property(nonatomic,copy)NSNumber* speedscore;
@property(nonatomic,copy)NSString* sqlStr;
@property(nonatomic,copy)NSNumber* start;
@property(nonatomic,copy)NSString* storename;
@property(nonatomic,copy)NSNumber* trandays;
@property(nonatomic,copy)NSString* transline;
@property(nonatomic,copy)NSNumber* translineId;
@property(nonatomic,copy)NSString* userlable;
@property(nonatomic,copy)NSString* username;

@end
