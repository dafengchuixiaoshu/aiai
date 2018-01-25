//
//  InputTextView.m
//  F2CPlatform
//
//  Created by xiehaiduo on 13-12-31.
//  Copyright (c) 2013年 xiehaiduo. All rights reserved.
//

#import "InputTextView.h"
#import "LoginViewController.h"
#import "UITextView+Placeholder.h"
#import "NSString+isEmpty.h"

#define kAlertWidth [UIScreen mainScreen].bounds.size.width
#define kAlertHeight 160.0f

@interface InputTextView ()

@property (nonatomic, strong) UITextView* textView;
@property (nonatomic, strong) UIButton *cancelBtn;
@property (nonatomic, strong) UIButton *confirmBtn;
@property (nonatomic, strong) UIView *backImageView;

@end

@implementation InputTextView

+ (CGFloat)alertWidth
{
    return kAlertWidth;
}

+ (CGFloat)alertHeight
{
    return kAlertHeight;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

#define kTitleYOffset 15.0f
#define kTitleHeight 25.0f

#define kContentOffset 30.0f
#define kBetweenLabelOffset 20.0f

- (id)initWithContent:(NSString *)content cancelButtonTitle:(NSString *)cancelTitle confirmButtonTitle:(NSString *)confirmTitle placeholder:(NSString *)placeholder
{
    if (self = [super init]) {
        self.backgroundColor = [UIColor whiteColor];
        
        CGFloat contentLabelWidth = kAlertWidth - 40;
        self.textView = [[UITextView alloc] initWithFrame:CGRectMake((kAlertWidth - contentLabelWidth) * 0.5, 10, contentLabelWidth, 100)];
        self.textView.textColor = [UIColor blackColor];
        self.textView.font = Every_SystemFontSize_14;
        [self.textView setPlaceholder:placeholder];
        self.textView.text = content;
        self.textView.delegate = (id)self;
        [self addSubview:self.textView];
        [self.textView becomeFirstResponder];
        
        CGRect leftBtnFrame;
        CGRect rightBtnFrame;
#define kCoupleButtonWidth 160.0f
#define kButtonHeight 40.0f
        
        leftBtnFrame = CGRectMake((kAlertWidth - 2 * kCoupleButtonWidth) * 0.5, kAlertHeight - kButtonHeight, kCoupleButtonWidth, kButtonHeight);
        rightBtnFrame = CGRectMake(CGRectGetMaxX(leftBtnFrame), kAlertHeight - kButtonHeight, kCoupleButtonWidth, kButtonHeight);
        self.cancelBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        self.confirmBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        self.cancelBtn.frame = leftBtnFrame;
        self.confirmBtn.frame = rightBtnFrame;
        
        
        [self.confirmBtn setTitle:confirmTitle forState:UIControlStateNormal];
        [self.cancelBtn setTitle:cancelTitle forState:UIControlStateNormal];
        self.cancelBtn.titleLabel.font = self.confirmBtn.titleLabel.font = Every_SystemFontSize_14;
        [self.confirmBtn setTitleColor:[UIColor colorWithRed:243/255.0 green:80/255.0 blue:80/255.0 alpha:1] forState:UIControlStateNormal];
        [self.cancelBtn setTitleColor:[UIColor colorWithRed:67/255.0 green:74/255.0 blue:84/255.0 alpha:1] forState:UIControlStateNormal];
        
        [self.cancelBtn addTarget:self action:@selector(cancelClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.confirmBtn addTarget:self action:@selector(confirmClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:self.cancelBtn];
        [self addSubview:self.confirmBtn];
        
        UILabel* line1 = [[UILabel alloc] initWithFrame:CGRectMake(0, kAlertHeight - kButtonHeight, kAlertWidth, 1)];
        line1.backgroundColor = [UIColor lightGrayColor];
        [self addSubview:line1];
        
        UILabel* line2 = [[UILabel alloc] initWithFrame:CGRectMake(CGRectGetMaxX(leftBtnFrame), kAlertHeight - kButtonHeight, 1, kButtonHeight)];
        line2.backgroundColor = [UIColor lightGrayColor];
        [self addSubview:line2];
        
    }
    return self;
}

- (void)confirmClicked:(id)sender
{
    if (self.confirmBlock) {
        self.confirmBlock(self.textView.text);
    }
    [self dismissAlert];
}

- (void)cancelClicked:(id)sender
{
    if (_cancelBlock) {
        _cancelBlock();
    }
    [self dismissAlert];
}

- (void)show
{
    UIWindow *shareWindow = [UIApplication sharedApplication].keyWindow;
    self.frame = CGRectMake((CGRectGetWidth(shareWindow.bounds) - kAlertWidth) * 0.5, - kAlertHeight - 30, kAlertWidth, kAlertHeight);
    [shareWindow addSubview:self];
}

- (void)dismissAlert
{
    [self.textView resignFirstResponder];
    [self removeFromSuperview];
}

- (void)removeFromSuperview
{
    [self.backImageView removeFromSuperview];
    self.backImageView = nil;
    [super removeFromSuperview];
}

- (void)willMoveToSuperview:(UIView *)newSuperview
{
    if (newSuperview == nil) {
        return;
    }
    UIWindow *shareWindow = [UIApplication sharedApplication].keyWindow;
    
    if (!self.backImageView) {
        self.backImageView = [[UIView alloc] initWithFrame:shareWindow.bounds];
        
        //添加键盘关闭手势
        UITapGestureRecognizer* recognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(bgTapGestureRecognizer:)];
        recognizer.delegate = (id)self;
        [self.backImageView addGestureRecognizer:recognizer];
    }
    self.backImageView.backgroundColor = [UIColor blackColor];
    self.backImageView.alpha = 0.6f;
    [shareWindow addSubview:self.backImageView];
    self.transform = CGAffineTransformMakeRotation(-M_1_PI / 2);
    CGRect afterFrame = CGRectMake((CGRectGetWidth(shareWindow.bounds) - kAlertWidth) * 0.5, 76, kAlertWidth, kAlertHeight);
    self.transform = CGAffineTransformMakeRotation(0);
    self.frame = afterFrame;
    self.center = CGPointMake(shareWindow.center.x, shareWindow.center.y - 50);
    [super willMoveToSuperview:newSuperview];
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text {
    
    if([text isEqualToString:@"\n"]){
        return NO;
    }
    NSString * aString = [textView.text stringByReplacingCharactersInRange:range withString:text];
    if ([text length] != 0) {
        if ([NSString calcStringLength:aString] > 20) {
            return NO;
        }
    }else{
        return YES;
    }
    
    return YES;
}
- (void)textViewDidChange:(UITextView *)textView
{
    [self.textView changePlaceholder];
}


#pragma mark Action
-(void)bgTapGestureRecognizer:(id)sender{
    [AppUtils closeKeyBoard];
}


-(void)dealloc{
    [[RequestServers share] cancelRequestWithClassName:[self class]];
}

@end

