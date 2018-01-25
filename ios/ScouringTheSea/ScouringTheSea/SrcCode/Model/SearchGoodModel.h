//
//  GoodModel.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/11/25.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseModel.h"

@interface SearchGoodSkinModel : BaseModel

@property(nonatomic,copy)NSNumber* _id;
@property(nonatomic,copy)NSString* _index;
@property(nonatomic,copy)NSNumber* _score;
@property(nonatomic,strong)NSDictionary* _source;
@property(nonatomic,copy)NSString* _type;

@end

@interface SearchGoodModel : BaseModel

@property(nonatomic,copy)NSNumber* brand_id;
@property(nonatomic,copy)NSString* brand_id_name;
@property(nonatomic,copy)NSString* brand_img_url;
@property(nonatomic,copy)NSString* brand_name;
@property(nonatomic,copy)NSString* category;
@property(nonatomic,copy)NSString* category_1;
@property(nonatomic,copy)NSString* category_2;
@property(nonatomic,copy)NSString* category_3;
@property(nonatomic,copy)NSString* category_id_name_1;
@property(nonatomic,copy)NSString* category_id_name_2;
@property(nonatomic,copy)NSString* category_id_name_3;
@property(nonatomic,copy)NSNumber* cid_1;
@property(nonatomic,copy)NSNumber* cid_2;
@property(nonatomic,copy)NSNumber* cid_3;
@property(nonatomic,copy)NSNumber* comment_num;
@property(nonatomic,copy)NSString* corewords;
@property(nonatomic,copy)NSString* currency_code;
@property(nonatomic,copy)NSNumber* db_type;
@property(nonatomic,copy)NSNumber* goods_score;
@property(nonatomic,strong)NSArray* keywords;
@property(nonatomic,copy)NSNumber* last_price;
@property(nonatomic,copy)NSNumber* original_price;
@property(nonatomic,copy)NSString* pic_url;
@property(nonatomic,copy)NSNumber* postfee;
@property(nonatomic,copy)NSNumber* price;
@property(nonatomic,copy)NSNumber* prod_id;
@property(nonatomic,copy)NSNumber* sales_volume;
@property(nonatomic,copy)NSString* sell;
@property(nonatomic,copy)NSString* title;
@property(nonatomic,copy)NSString* title_zh;
@property(nonatomic,copy)NSNumber* unified_price;
@property(nonatomic,copy)NSNumber* update_time;
@property(nonatomic,copy)NSString* url;
@property(nonatomic,copy)NSString* website;
@property(nonatomic,copy)NSNumber* website_id;
@property(nonatomic,copy)NSString* website_id_name;
@property(nonatomic,copy)NSString* website_url;

@end

