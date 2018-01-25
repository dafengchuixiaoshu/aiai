//
//  CommunityData.h
//  aiai
//
//  Created by wangnan on 15/11/12.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "communityList.pb.h"
#import "labelList.pb.h"
#import "userPage.pb.h"

@interface PostCellElement : NSObject

@property(nonatomic, copy)NSString *firstkey;           //第一帧图片
@property(nonatomic, copy)NSString *sendtime;           //发布时间
@property(nonatomic, copy)NSString *face;               //头像
@property(nonatomic, copy)NSString *label;              //标签
@property(nonatomic, copy)NSString *praisecount;        //赞的数量
//id
@property(nonatomic, copy)NSString *sid;                //帖子id

-(void)setModelWithCModel:(tutorial::CommunityList_Community)data;

-(void)setModelWithLabelModel:(tutorial::LabelList_Label)data;

-(void)setModelWithworkModel:(tutorial::UserPage_Work)data;

@end
