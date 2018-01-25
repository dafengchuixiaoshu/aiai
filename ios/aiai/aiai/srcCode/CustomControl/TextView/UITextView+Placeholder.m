//
//  UITextView+Placeholder.m
//  F2CPlatform
//
//  Created by xiehaiduo on 14-1-10.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "UITextView+Placeholder.h"

@implementation UITextView (Placeholder)

-(void)setPlaceholder:(NSString*)placeholdertext
{
    for (id view in self.subviews) {
        if ([view isKindOfClass:[UILabel class]]) {
            [view removeFromSuperview];
        }
    }

    UILabel* label = [[UILabel alloc] init];
    label.frame =CGRectMake(10, 5, self.frame.size.width-10, 20);
    label.textColor = [UIColor lightGrayColor];
    label.font = self.font;
    label.text = placeholdertext;
    label.enabled = NO;
    label.backgroundColor = [UIColor clearColor];
    label.tag = 123;
    [self addSubview:label];
    
    if (self.text.length > 0){
        label.hidden = YES;
    }
}

-(void)changePlaceholder
{
    UILabel* label = (UILabel*)[self viewWithTag:123];
    label.hidden = self.text.length > 0;
}

-(void)setViewText:(NSString *)text{
    self.text = text;
    [self changePlaceholder];
    if (text.length>0) {
        CGFloat bottom = self.contentSize.height-self.frame.size.height;
        if (bottom>0) {
            [self setContentOffset:CGPointMake(self.contentOffset.x, bottom)];
        }
    }
}
@end
