//
//  STSRequestUrl.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/8.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

//排序方式
typedef NS_ENUM(NSUInteger, SORT_MODE) {
    SORT_MODE_DEFAULT       =       0,          //默认
    SORT_MODE_SALESVOLUME   =       1,          //销量
    SORT_MODE_PRICE         =       2           //价格
};

//排序类型
typedef NS_ENUM(NSUInteger, SORT_TYPE) {
    SORT_TYPE_POSITIVESEQUENCE  =   0,          //正序
    SORT_TYPE_REVERSEORDER      =   1           //倒序
};


@interface RequestUrl : NSObject

//首页
+(NSString*)httpMothed_HomePageUrl;

//转运公司列表
+(NSString*)httpMothed_TransferCompanyListUrlFrom:(NSInteger)seq size:(NSInteger)size;

//转运公司详情
+(NSString*)httpMothed_TransferCompanyDetaileUrlWithId:(id)cid;

//转运公司评论,参数
+(NSString*)httpMothed_TransferCompanyCommentUrl;
+(NSDictionary*)httpMothed_TransferCompanyCommentUrlWithId:(id)cid cpName:(NSString*)cname cpTime:(NSString*)ctime allscore:(NSNumber*)allScore orderTime:(NSString*)orderTime seller:(NSString*)seller;

//转运公司评论列表
+(NSString*)httpMothed_TransferCompanyCommentListUrlWithId:(id)cid from:(NSInteger)seq size:(NSInteger)size;

//攻略列表
+(NSString*)httpMothed_StsRaidersListUrlFrom:(NSInteger)seq size:(NSInteger)size;

//攻略详情页
+(NSString*)httpMothed_StsRaidersDetaileUrlWithId:(id)cid;

//登录
+(NSString*)httpMothed_UserLoginUrlWithUsername:(NSString*)username pwd:(NSString*)pwd;

//注册
+(NSString*)httpMothed_UserRegisterUrlWithUsername:(NSString*)username pwd:(NSString*)pwd tel:(NSString*)tel;

//重置密码
+(NSString*)httpMothed_UserChangePassWordUrlWithPwd:(NSString*)pwd tel:(NSString*)tel;

//我的点评
+(NSString*)httpMothed_MyselfCommentUrlWithUserID:(id)userId from:(NSInteger)seq size:(NSInteger)size;


//******************************************方培云接口*****************************************************


//搜索页接口,返回热门品牌、热门网站、热词
+(NSString*)httpMothed_SearchPageUrl;

/*搜索页接口,返回热门品牌、热门网站、热词
* size：分页大小
* page：页码
* brand：品牌id
* website：站点id
* query：关键词
* sort：1：销量，2：价格，0：默认
* sortType：0：顺序，1：倒序
 */
+(NSString*)httpMothed_SearchListUrl;
+(NSDictionary*)httpMothed_SearchListUrlWithBrand:(id)brand website:(id)website query:(NSString*)query sort:(SORT_MODE)sort sortType:(SORT_TYPE)sortType page:(NSInteger)page size:(NSInteger)size;

//请求商品详情
+(NSString*)httpMothed_GoodsInfoUrlWithGoodsId:(id)goodid;

//请求商品评论列表
+(NSString*)httpMothed_GoodsCommentListUrlWithGoodsId:(id)goodid;

//发布评论
+(NSString*)httpMothed_GoodsPostCommentUrlWithGoodsId:(id)pId score:(NSInteger)score content:(NSString*)content userName:(NSString*)userName;

@end
