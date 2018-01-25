//
//  ReplyData.h
//  aiai
//
//  Created by wangnan on 15/11/17.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "contentInfo.pb.h"

@interface ReplyData : NSObject

@property(nonatomic, copy)NSString *faceStr;            //头像
@property(nonatomic, copy)NSString *nickName;           //昵称
@property(nonatomic, copy)NSString *content;            //评论内容
@property(nonatomic, copy)NSString *sendtime;           //发布时间
@property(nonatomic, copy)NSString *faceVideo;          //头像视频
@property(nonatomic, copy)NSString *uid;                //用户id

-(void)setReplyDataWith:(tutorial::ContentInfo_Reply)data;

@end
