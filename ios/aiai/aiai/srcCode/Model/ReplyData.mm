//
//  ReplyData.m
//  aiai
//
//  Created by wangnan on 15/11/17.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "ReplyData.h"

@implementation ReplyData

-(void)setReplyDataWith:(tutorial::ContentInfo_Reply)data{
    _faceStr = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:data.face().c_str()]];
    _nickName = [NSString stringWithUTF8String:data.nickname().c_str()];
    _content = [NSString stringWithUTF8String:data.content().c_str()];
    _sendtime = [AppUtils getTime:[NSNumber numberWithLongLong:data.sendtime()]];
    _faceVideo = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:data.facevideo().c_str()]];
}
@end
