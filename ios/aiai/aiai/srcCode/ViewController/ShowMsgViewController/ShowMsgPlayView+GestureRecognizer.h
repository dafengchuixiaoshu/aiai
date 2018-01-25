//
//  ShowMsgPlayView+GestureRecognizer.h
//  aiai
//
//  Created by zhangyang on 15/4/26.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ShowMsgPlayView.h"

@interface ShowMsgPlayView (GestureRecognizer)

-(void)addUIGestureRecognizer;

//点击头像使用
- (void)changeCaptureState;
@end
