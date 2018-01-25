//
//  ContactEntity.h
//  aiai
//
//  Created by zhangyang on 15/1/12.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseEntity.h"
#import "IMPackageData.h"

@interface ContactEntity : BaseEntity

@property(nonatomic,copy)NSString* localID;

@property(nonatomic,copy)NSString* buddyUserName;//主要用来排序用，所有数据buddydata里都有

@property(nonatomic,strong)IMPackageBuddyData* buddyData;

-(id)initWithContactEntity:(ContactEntity*)entity;

//buddylist转contact
+(NSDictionary*)firstLetterContactDictionaryWithBuddyList:(NSArray*)buddyList;
+(NSArray*)firstLetterArray:(NSDictionary*)contactBuddyDic;

@end