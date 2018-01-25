//
//  STSRequestUrl.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/12/8.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "RequestUrl.h"

@implementation RequestUrl

//首页
+(NSString*)httpMothed_HomePageUrl{
    return [NSString stringWithFormat:@"%@%@",BASEURL,@"action=firstpage"];
}

//转运公司列表
+(NSString*)httpMothed_TransferCompanyListUrlFrom:(NSInteger)seq size:(NSInteger)size{
    return [NSString stringWithFormat:@"%@%@%ld%@%ld",BASEURL,@"action=companyList&from_seq=",seq,@"&page_size=",size];
}

//转运公司详情
+(NSString*)httpMothed_TransferCompanyDetaileUrlWithId:(id)cid{
    return [NSString stringWithFormat:@"%@%@%@",BASEURL,@"action=viewCompanyDetail&id=",cid];
}

//转运公司评论
+(NSString*)httpMothed_TransferCompanyCommentUrl{
    return [NSString stringWithFormat:@"%@%@",BASEURL,@"action=saveTalk"];
}
+(NSDictionary*)httpMothed_TransferCompanyCommentUrlWithId:(id)cid cpName:(NSString*)cname cpTime:(NSString*)ctime allscore:(NSNumber*)allScore orderTime:(NSString*)orderTime seller:(NSString*)seller{
    NSDictionary* parmarDic = [NSDictionary dictionaryWithObjectsAndKeys:
                               cid,@"companyid",
                               cname,@"companyname",
                               ctime,@"commenttime",
                               allScore,@"allScore",
                               orderTime,@"orderTime",
                               seller,@"seller",
                               nil];
    return parmarDic;
//    return [NSString stringWithFormat:@"%@%@%@%@%@",BASEURL,@"action=saveTalk&companyid=",cid,@"&companyname=",ctime,@"&commenttime=",ctime,@"&allScore=",ctime@"&orderTime=",ctime,@"&seller=",ctime,@"&userName=",ctime];
}

//转运公司评论列表
+(NSString*)httpMothed_TransferCompanyCommentListUrlWithId:(id)cid from:(NSInteger)seq size:(NSInteger)size{
    return [NSString stringWithFormat:@"%@%@%@%@%ld%@%ld",BASEURL,@"action=queryCompanyTalkList&companyid=",cid,@"&from_seq=",seq,@"&page_size=",size];
}

//攻略列表
+(NSString*)httpMothed_StsRaidersListUrlFrom:(NSInteger)seq size:(NSInteger)size{
    return [NSString stringWithFormat:@"%@%@%ld%@%ld",BASEURL,@"action=queryGongluoList&from_seq=",seq,@"&page_size=",size];
}

//攻略详情页
+(NSString*)httpMothed_StsRaidersDetaileUrlWithId:(id)cid{
    return [NSString stringWithFormat:@"%@%@%@",BASEURL,@"action=veiwGongluoDetail&id=",cid];
}

//登录
+(NSString*)httpMothed_UserLoginUrlWithUsername:(NSString*)username pwd:(NSString*)pwd{
    return [NSString stringWithFormat:@"%@%@%@%@%@",BASEURL,@"action=login&password=",pwd,@"&accountNumber=",username];
}

//注册
+(NSString*)httpMothed_UserRegisterUrlWithUsername:(NSString*)username pwd:(NSString*)pwd tel:(NSString*)tel{
    return [NSString stringWithFormat:@"%@%@%@%@%@%@%@",BASEURL,@"action=register&username=",username,@"&password=",pwd,@"&tel=",tel];
}

//重置密码
+(NSString*)httpMothed_UserChangePassWordUrlWithPwd:(NSString*)pwd tel:(NSString*)tel{
    return [NSString stringWithFormat:@"%@%@%@%@%@",BASEURL,@"action=resertPassword&password=",pwd,@"&tel=",tel];
}

//我的点评
+(NSString*)httpMothed_MyselfCommentUrlWithUserID:(id)userId from:(NSInteger)seq size:(NSInteger)size{
    return [NSString stringWithFormat:@"%@%@%@%@%ld%@%ld",BASEURL,@"action=myTalk&userId=",userId,@"&from_seq=",seq,@"&page_size=",size];
}


//******************************************方培云接口*****************************************************

//搜索页接口,返回热门品牌、热门网站、热词
+(NSString*)httpMothed_SearchPageUrl{
    return [NSString stringWithFormat:@"%@%@",BASEURL,@"action=searchPage"];
}

//搜索页接口,返回热门品牌、热门网站、热词
+(NSString*)httpMothed_SearchListUrl{
    return [NSString stringWithFormat:@"%@%@",BASEURL,@"action=searchListPage"];
}

+(NSDictionary*)httpMothed_SearchListUrlWithBrand:(id)brand website:(id)website query:(NSString*)query sort:(SORT_MODE)sort sortType:(SORT_TYPE)sortType page:(NSInteger)page size:(NSInteger)size{

    NSMutableDictionary* parmarDic = [NSMutableDictionary dictionary];
    if (brand) {
        [parmarDic setValue:brand forKey:@"brand"];
    }
    if (website) {
        [parmarDic setValue:website forKey:@"website"];
    }
    if (query) {
        [parmarDic setValue:query forKey:@"query"];
    }
    if (sort==0||sort==1||sort==2) {
        [parmarDic setValue:[NSNumber numberWithInteger:sort] forKey:@"sort"];
    }else{
        [parmarDic setValue:[NSNumber numberWithInteger:0] forKey:@"sort"];
    }
    if (sortType==0||sortType==1) {
        [parmarDic setValue:[NSNumber numberWithInteger:sortType] forKey:@"sortType"];
    }else{
        [parmarDic setValue:[NSNumber numberWithInteger:0] forKey:@"sortType"];
    }
    if (page >= 0) {
        [parmarDic setValue:[NSNumber numberWithInteger:page] forKey:@"page"];
    }else{
        [parmarDic setValue:[NSNumber numberWithInteger:0] forKey:@"page"];
    }
    if (size > 0) {
        [parmarDic setValue:[NSNumber numberWithInteger:size] forKey:@"size"];
    }else{
        [parmarDic setValue:[NSNumber numberWithInteger:1] forKey:@"size"];
    }
    return parmarDic;
}

//请求商品详情
+(NSString*)httpMothed_GoodsInfoUrlWithGoodsId:(id)goodid{
    return [NSString stringWithFormat:@"%@%@%@",BASEURL,@"action=goodsInfo&pId=",goodid];
}

//请求商品评论列表
+(NSString*)httpMothed_GoodsCommentListUrlWithGoodsId:(id)goodid{
    return [NSString stringWithFormat:@"%@%@%@",BASEURL,@"action=goodsCommList&pId=",goodid];
}

//发布评论
+(NSString*)httpMothed_GoodsPostCommentUrlWithGoodsId:(id)pId score:(NSInteger)score content:(NSString*)content userName:(NSString*)userName{
    return [NSString stringWithFormat:@"%@%@%@%@%ld%@%@%@%@",BASEURL,@"action=goodsComment&pId=",pId,@"&score=",score,@"&content=",content,@"&userName=",userName];
}


@end
