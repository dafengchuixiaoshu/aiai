//
//  NewMessageData.m
//  aiai
//
//  Created by wangnan on 15/11/23.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "NewMessageData.h"

@implementation NewMessageData

-(void)setWithNewMessageModel:(tutorial::NewMessageList_NewMessage)message{
    
    _nickName = [NSString stringWithUTF8String:message.nickname().c_str()];
    _sid = [NSString stringWithFormat:@"%d",message.sid()];
    _content = [NSString stringWithUTF8String:message.content().c_str()];
    _timeStr = [NSString stringWithFormat:@"%@",[AppUtils getTime:[NSNumber numberWithLongLong:message.sendtime()]]];
    _faceStr = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:message.face().c_str()]];
    _firstKey = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:message.firstkey().c_str()]];
    
}
@end
