//
//  SkinModel.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/7.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseModel.h"

//外皮json格式对象
@interface SkinModel : BaseModel

@property(nonatomic,strong)id returndata;
@property(nonatomic,strong)id status;
@property(nonatomic,copy)NSNumber* totalNum;
@property(nonatomic,copy)NSNumber* hasMore;

@end

@interface StatusModel : BaseModel

@property(nonatomic,copy)NSNumber* code;
@property(nonatomic,copy)NSString* desc;

@end