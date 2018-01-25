//
//  MyTextView.h
//  F2CPlatform
//
//  Created by tianjiaxin on 14-7-19.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

#define MessageTextViewHeight 33

@interface MyTextView : UITextView

//文本框最大高度
@property(nonatomic,assign,readonly)CGFloat maxHeight;

-(id)initWithFrame:(CGRect)frame;
//自适应高
- (void)adjustTextViewHeightBy:(CGFloat)changeInHeight;
//判断是否需要改变高度并返回
+ (CGFloat)measureHeightOfUITextView:(UITextView *)textView;
@end
