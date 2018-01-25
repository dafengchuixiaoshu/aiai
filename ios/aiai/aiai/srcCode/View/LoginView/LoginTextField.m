//
//  LoginTextField.m
//  aiai
//
//  Created by zhangyang on 15/4/2.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "LoginTextField.h"

@interface LoginTextField ()
@property(nonatomic,strong)UITextField* inputTextField;
@end

@implementation LoginTextField

-(id)initWithFrame:(CGRect)frame leftIconName:(NSString*)iconName placeholderText:(NSString*)placeholder{
    if (self = [super initWithFrame:frame]) {
        self.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.05];
//        self.alpha = 0.3;
        self.layer.cornerRadius = frame.size.height/2;
        self.layer.masksToBounds = YES;
        
        CGFloat Control_X = 0;
        CGFloat Control_Y = 0;
        CGFloat Control_W = 0;
        CGFloat Control_H = 0;
        
        UIImage* icon = [UIImage imageNamed:iconName];
        Control_X = 15;
        Control_Y = (frame.size.height - icon.size.height)/2;
        UIImageView* iconImageview = [[UIImageView alloc] initWithImage:icon];
        iconImageview.frame = CGRectMake(Control_X, Control_Y, icon.size.width, icon.size.height);
        [self addSubview:iconImageview];
        
        Control_X = iconImageview.frame.origin.x + iconImageview.frame.size.width + 10;
        Control_Y = 0;
        Control_W = frame.size.width - Control_X;
        Control_H = frame.size.height;
        
        _inputTextField = [[UITextField alloc] initWithFrame:CGRectMake(Control_X, Control_Y, Control_W , Control_H)];
        _inputTextField.placeholder =  placeholder;
        [_inputTextField setValue:[[UIColor whiteColor] colorWithAlphaComponent:0.3] forKeyPath:@"_placeholderLabel.textColor"];
        _inputTextField.textColor = [UIColor whiteColor];
        [self addSubview:_inputTextField];
    }
    return self;
}

-(void)resignFirstResponder{
    [_inputTextField resignFirstResponder];
}

-(NSString*)text{
    return _inputTextField.text;
}

-(void)setText:(NSString *)inputText{
    _inputTextField.text = inputText;
}

-(void)setSecureTextEntry:(BOOL)secureTextEntry{
    _secureTextEntry = secureTextEntry;
    _inputTextField.secureTextEntry = secureTextEntry;
}

-(void)setKeyboardType:(AIAI_KeyboardType)keyboardType{
    _inputTextField.keyboardType = (UIKeyboardType)keyboardType;
}

@end
