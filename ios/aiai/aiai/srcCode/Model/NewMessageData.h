//
//  NewMessageData.h
//  aiai
//
//  Created by wangnan on 15/11/23.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "newMessage.pb.h"

@interface NewMessageData : NSObject

@property(nonatomic, copy)NSString *sid;
@property(nonatomic, copy)NSString *nickName;
@property(nonatomic, copy)NSString *content;
@property(nonatomic, copy)NSString *timeStr;
@property(nonatomic, copy)NSString *faceStr;
@property(nonatomic, copy)NSString *firstKey;

-(void)setWithNewMessageModel:(tutorial::NewMessageList_NewMessage)message;

@end
