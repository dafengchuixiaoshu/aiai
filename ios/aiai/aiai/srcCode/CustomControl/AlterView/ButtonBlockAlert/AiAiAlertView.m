//
//  HXAlertView.m
//  F2CPlatform
//
//  Created by zhangyang on 14-6-27.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "AiAiAlertView.h"

@implementation AiAiAlertView {
    btn_block confirmBlock;
    btn_block cancelBlock;
}

+ (AiAiAlertView*)share {
    static AiAiAlertView *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (id)init {
    if (self = [super init]) {
    }
    return self;
}

- (void)showAlertViewWithTitle:(NSString*)title message:(NSString*)message confirmTitle:(NSString*)confirmTitle cancelTitle:(NSString *)cancelTitle confirm:(btn_block)confirm cancel:(btn_block)cancel {
    
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:title message:message delegate:self cancelButtonTitle:cancelTitle otherButtonTitles:confirmTitle, nil];
    [alertView show];
    
    confirmBlock = confirm;
    cancelBlock  = cancel;
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    if ([[alertView buttonTitleAtIndex:buttonIndex] isEqualToString:@"取消"]) {
        if (cancelBlock) {
            cancelBlock();
        }
    }
    else {
        if (confirmBlock) {
            confirmBlock();
        }
    }
}

@end
