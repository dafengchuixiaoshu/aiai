//
//  MyTextView.m
//  F2CPlatform
//
//  Created by tianjiaxin on 14-7-19.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "MyTextView.h"
#define MaxLines 3.7f

@interface MyTextView()

@end

@implementation MyTextView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.font = [UIFont systemFontOfSize:16];
        self.returnKeyType = UIReturnKeySend;
        self.layer.masksToBounds = YES;
        self.layer.borderColor = [UIColor colorWithRed:170/255.0 green:170/255.0 blue:170/255.0 alpha:1].CGColor;
        self.layer.borderWidth = 0.3f;
        self.layer.cornerRadius = 4.0f;
    }
    return self;
}

-(CGFloat)maxHeight{
    return self.font.lineHeight*MaxLines;
}

#pragma mark - Setters

-(void)setText:(NSString *)text{
    [super setText:text];
    if (text.length>0) {
        CGFloat bottom = self.contentSize.height-self.frame.size.height-self.font.lineHeight/2;//露半行字
        if (bottom>0) {
            [self setContentOffset:CGPointMake(self.contentOffset.x, bottom)];
        }
    }
}

-(void)setFrame:(CGRect)frame{
    if (frame.size.height<MessageTextViewHeight) {
        frame = CGRectMake(frame.origin.x, frame.origin.y, frame.size.width, MessageTextViewHeight);
    }
    [super setFrame:frame];
}

#pragma mark - Message input view
- (void)adjustTextViewHeightBy:(CGFloat)changeInHeight
{
    CGRect prevFrame = self.frame;
    self.frame = CGRectMake(prevFrame.origin.x,
                            prevFrame.origin.y,
                            prevFrame.size.width,
                            prevFrame.size.height + changeInHeight);
}

+ (CGFloat)measureHeightOfUITextView:(UITextView *)textView
{
    if ([textView respondsToSelector:@selector(snapshotViewAfterScreenUpdates:)])
    {
        CGRect frame = textView.bounds;
        
        UIEdgeInsets textContainerInsets = textView.textContainerInset;
        UIEdgeInsets contentInsets = textView.contentInset;
        
        CGFloat leftRightPadding = textContainerInsets.left + textContainerInsets.right + textView.textContainer.lineFragmentPadding * 2 + contentInsets.left + contentInsets.right;
        CGFloat topBottomPadding = textContainerInsets.top + textContainerInsets.bottom + contentInsets.top + contentInsets.bottom;
        
        frame.size.width -= leftRightPadding;
        frame.size.height -= topBottomPadding;
        
        NSString *textToMeasure = textView.text;
        if ([textToMeasure hasSuffix:@"\n"])
        {
            textToMeasure = [NSString stringWithFormat:@"%@-", textView.text];
        }
        
        NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc] init];
        [paragraphStyle setLineBreakMode:NSLineBreakByWordWrapping];
        
        NSDictionary *attributes = @{ NSFontAttributeName: textView.font, NSParagraphStyleAttributeName : paragraphStyle };
        
        CGRect size = [textToMeasure boundingRectWithSize:CGSizeMake(CGRectGetWidth(frame), MAXFLOAT)
                                                  options:NSStringDrawingUsesLineFragmentOrigin
                                               attributes:attributes
                                                  context:nil];
        
        CGFloat measuredHeight = ceilf(CGRectGetHeight(size) + topBottomPadding);
        return measuredHeight;
    }
    else
    {
        return textView.contentSize.height;
    }
}

@end
