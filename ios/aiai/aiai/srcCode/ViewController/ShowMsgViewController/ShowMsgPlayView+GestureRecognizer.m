//
//  ShowMsgPlayView+GestureRecognizer.m
//  aiai
//
//  Created by zhangyang on 15/4/26.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ShowMsgPlayView+GestureRecognizer.h"
#import "ShowMsgPlayView.h"

@implementation ShowMsgPlayView (GestureRecognizer)

#pragma mark 添加手势
-(void)addUIGestureRecognizer{
    UIPanGestureRecognizer * panGestureRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(addViewPanAction:)];
    [self addGestureRecognizer:panGestureRecognizer];
    
    UITapGestureRecognizer * tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(addViewTapAction:)];
    [self addGestureRecognizer:tapGestureRecognizer];
    
    UISwipeGestureRecognizer* upSwipeGestureRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(addViewSwipeAction:)];
    upSwipeGestureRecognizer.direction = UISwipeGestureRecognizerDirectionUp;
    [self addGestureRecognizer:upSwipeGestureRecognizer];
    
    UISwipeGestureRecognizer* downSwipeGestureRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(addViewSwipeAction:)];
    downSwipeGestureRecognizer.direction = UISwipeGestureRecognizerDirectionDown;
    [self addGestureRecognizer:downSwipeGestureRecognizer];
    
//    UISwipeGestureRecognizer* leftSwipeGestureRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(addViewleftSwipeAction:)];
//    leftSwipeGestureRecognizer.direction = UISwipeGestureRecognizerDirectionLeft;
//    [self addGestureRecognizer:leftSwipeGestureRecognizer];
    
    [panGestureRecognizer requireGestureRecognizerToFail:upSwipeGestureRecognizer];
    [panGestureRecognizer requireGestureRecognizerToFail:downSwipeGestureRecognizer];
//    [panGestureRecognizer requireGestureRecognizerToFail:leftSwipeGestureRecognizer];
}

CGFloat begin_Y;
- (void)addViewPanAction:(UIPanGestureRecognizer *)recognizer{

    [self.superview bringSubviewToFront:self];
    if (recognizer.state == UIGestureRecognizerStateBegan) {
        begin_Y = [recognizer translationInView:self].y;
    }else if (recognizer.state == UIGestureRecognizerStateChanged){
         CGPoint point = [recognizer translationInView:self];
        CGRect rect = self.frame;
        rect.origin.y += point.y - begin_Y;
        begin_Y = point.y;
        
        if (rect.origin.y <= 0.0) {
            rect.origin.y = 0;
            if(self.moveBottomBlock){
                self.moveBottomBlock(NO);
            }
        }else if (rect.origin.y >= [[UIScreen mainScreen] bounds].size.width){
            rect.origin.y = [[UIScreen mainScreen] bounds].size.width;
            if(self.moveBottomBlock){
                self.moveBottomBlock(YES);
            }
        }
        self.frame = rect;
    }else if (recognizer.state == UIGestureRecognizerStateEnded){
        CGPoint point = [recognizer translationInView:self];
        CGRect rect = self.frame;
        rect.origin.y += point.y - begin_Y;
        if (rect.origin.y >= [[UIScreen mainScreen] bounds].size.width/2) {
            rect.origin.y = [[UIScreen mainScreen] bounds].size.width;
            if(self.moveBottomBlock){
                self.moveBottomBlock(YES);
            }
        }else{
            rect.origin.y = 0;
            if(self.moveBottomBlock){
                self.moveBottomBlock(NO);
            }
        }
        [UIView animateWithDuration:0.3 animations:^{
            self.frame = rect;
        }];
    }
}

- (void)addViewTapAction:(UITapGestureRecognizer *)recognizer{
    
    CGRect rect = self.frame;
    if (rect.origin.y == [[UIScreen mainScreen] bounds].size.width) {
        rect.origin.y = 0;
        if(self.moveBottomBlock){
            self.moveBottomBlock(NO);
        }
    }else{
        rect.origin.y = [[UIScreen mainScreen] bounds].size.width;
        if(self.moveBottomBlock){
            self.moveBottomBlock(YES);
        }
    }
    
    [UIView animateWithDuration:0.3 animations:^{
        self.frame = rect;
    }];
}

-(void)addViewSwipeAction:(UISwipeGestureRecognizer *)recognizer{
    
     CGRect rect = self.frame;
    
     if (recognizer.direction==UISwipeGestureRecognizerDirectionUp) {
         rect.origin.y = 0;
         if(self.moveBottomBlock){
             self.moveBottomBlock(NO);
         }
     }else if (recognizer.direction==UISwipeGestureRecognizerDirectionDown){
         rect.origin.y = [[UIScreen mainScreen] bounds].size.width;
         if(self.moveBottomBlock){
             self.moveBottomBlock(YES);
         }
     }
    [UIView animateWithDuration:0.3 animations:^{
        self.frame = rect;
    }];
}

//左划手势
-(void)addViewleftSwipeAction:(UISwipeGestureRecognizer *)recognizer{
    if (self.unReadMsgButtonBlock) {
        self.unReadMsgButtonBlock();
    }
}

//点击头像使用
- (void)changeCaptureState{
    
    [self.superview bringSubviewToFront:self];
    
    CGRect rect = self.frame;
    if (rect.origin.y == [[UIScreen mainScreen] bounds].size.width) {
        rect.origin.y = 0;
        
    }else{
        rect.origin.y = [[UIScreen mainScreen] bounds].size.width;
    }
    
    [UIView animateWithDuration:0.3 animations:^{
        self.frame = rect;
    }];
    
}

@end
