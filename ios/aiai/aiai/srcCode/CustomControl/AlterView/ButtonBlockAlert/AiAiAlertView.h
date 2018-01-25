//
//  HXAlertView.h
//  F2CPlatform
//
//  Created by Circle Liu on 14-6-27.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^btn_block)(void);

@interface AiAiAlertView : UIView

+ (AiAiAlertView*)share;

- (void)showAlertViewWithTitle:(NSString*)title message:(NSString*)message confirmTitle:(NSString*)confirmTitle cancelTitle:(NSString *)cancelTitle confirm:(btn_block)confirm cancel:(btn_block)cancel;

@end
