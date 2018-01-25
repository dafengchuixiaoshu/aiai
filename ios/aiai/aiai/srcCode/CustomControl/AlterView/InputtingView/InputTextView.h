//
//  InputTextView   .h
//  F2CPlatform
//
//  Created by xiehaiduo on 13-12-31.
//  Copyright (c) 2013年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^Confirm)(NSString *inputText);

@interface InputTextView : UIView

- (id)initWithContent:(NSString *)content cancelButtonTitle:(NSString *)cancelTitle confirmButtonTitle:(NSString *)confirmTitle placeholder:(NSString *)placeholder;

- (void)show;

@property (nonatomic, copy) dispatch_block_t cancelBlock;
@property (nonatomic, copy) Confirm confirmBlock;
@end
