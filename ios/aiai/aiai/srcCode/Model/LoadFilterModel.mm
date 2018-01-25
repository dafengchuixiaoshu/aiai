//
//  LoadFilterModel.m
//  aiai
//
//  Created by wangnan on 15/5/30.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "LoadFilterModel.h"

@implementation LoadFilterModel

-(id)init{
    if (self = [super init]) {
        self.loadReceivedData = [[NSMutableData alloc] init];
        self.loadState = NODownLoad;
        self.isInQueue = NO;
    }
    return self;
}

-(void)setFilterModel:(::com::mm::pb::po::FilterResult)result{
    
    NSDictionary *nameDic = [[NSString stringWithUTF8String:result.desc().c_str()] myObjectFromJSONString];
    self.loadName = [nameDic objectForKey:@"name"];
    
    self.loadimagePath = [AppUtils getImageNewUrlWithSrcImageUrl:[NSString stringWithUTF8String:result.descphoto().c_str()] width:0 height:0];
    
    self.loadURL = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:result.key().c_str()]];
    
    self.upLoadAuth = [nameDic objectForKey:@"auth"]?:[NSString stringWithFormat:@"屁屁"];
    
    self.loadType = downLoad_filter;
}


-(void)setFontModel:(::com::mm::pb::po::FilterResult)result{
    
    NSDictionary *nameDic = [[NSString stringWithUTF8String:result.desc().c_str()] myObjectFromJSONString];
    self.loadName = [nameDic objectForKey:@"name"];
    
    self.loadimagePath = [AppUtils getImageNewUrlWithSrcImageUrl:[NSString stringWithUTF8String:result.descphoto().c_str()] width:0 height:0];
    
    self.loadURL = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:result.key().c_str()]];
    
    self.upLoadAuth = [nameDic objectForKey:@"auth"]?:[NSString stringWithFormat:@"屁屁"];
    
    self.loadType = downLoad_font;
    
}

@end
