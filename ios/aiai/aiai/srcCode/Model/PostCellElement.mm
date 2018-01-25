//
//  CommunityData.m
//  aiai
//
//  Created by wangnan on 15/11/12.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "PostCellElement.h"

@implementation PostCellElement

-(void)setModelWithCModel:(tutorial::CommunityList_Community)data{
    
    _firstkey = [AppUtils getImageNewUrlWithSrcImageUrl:[NSString stringWithUTF8String:data.firstkey().c_str()] width:0 height:0];
    _face = [AppUtils getImageNewUrlWithSrcImageUrl:[NSString stringWithUTF8String:data.face().c_str()] width:0 height:0];
    _label = [NSString stringWithUTF8String:data.label().c_str()];
    _praisecount = [NSString stringWithFormat:@"%d",data.praisecount()? :0];
    int64_t time = data.sendtime();
    _sendtime = [AppUtils getTime:[NSNumber numberWithLongLong:time]];
    
    _sid = [NSString stringWithFormat:@"%d",data.sid()? :0];
}


-(void)setModelWithLabelModel:(tutorial::LabelList_Label)data{
    
    self.firstkey = [AppUtils getImageNewUrlWithSrcImageUrl:[NSString stringWithUTF8String:data.firstkey().c_str()] width:0 height:0];
    self.face = [AppUtils getImageNewUrlWithSrcImageUrl:[NSString stringWithUTF8String:data.face().c_str()] width:0 height:0];
    self.label = [NSString stringWithUTF8String:data.label().c_str()];
    self.praisecount = [NSString stringWithFormat:@"%d",data.praisecount()? :0];
    int64_t time = data.sendtime();
    self.sendtime = [AppUtils getTime:[NSNumber numberWithLongLong:time]];
    
    self.sid = [NSString stringWithFormat:@"%d",data.sid()? :0];
    
}

-(void)setModelWithworkModel:(tutorial::UserPage_Work)data{
    
    self.firstkey = [AppUtils getImageNewUrlWithSrcImageUrl:[NSString stringWithUTF8String:data.firstkey().c_str()] width:0 height:0];
    self.face = [AppUtils getImageNewUrlWithSrcImageUrl:[NSString stringWithUTF8String:data.face().c_str()] width:0 height:0];
    self.label = [NSString stringWithUTF8String:data.label().c_str()];
    self.praisecount = [NSString stringWithFormat:@"%d",data.praisecount()? :0];
    int64_t time = data.sendtime();
    self.sendtime = [AppUtils getTime:[NSNumber numberWithLongLong:time]];
    
    self.sid = [NSString stringWithFormat:@"%d",data.sid()? :0];
    
}
@end
