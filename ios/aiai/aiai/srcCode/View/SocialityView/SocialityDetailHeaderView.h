//
//  SocialityDetailHeaderView.h
//  aiai
//
//  Created by wangnan on 15/11/13.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "UserHeaderView.h"

@interface SocialityDetailHeaderView : UIView

@property(nonatomic,strong)UserHeaderView *perHeader;     //头像

@property(nonatomic, copy)NSString *videoUrl;           //视频地址
@property(nonatomic, copy)NSString *nickName;            //昵称
@property(nonatomic, copy)NSString *praiseCount;         //赞
@property(nonatomic, copy)NSString *commentCount;        //评论
@property(nonatomic, copy)NSString *perText;             //文字内容
@property(nonatomic, copy)NSString *sendTime;            //时间
@property(nonatomic, copy)NSString *firstImgUrl;         //第一帧图片

@property(nonatomic, copy)dispatch_block_t praiseBlock;     //赞按钮
@end
