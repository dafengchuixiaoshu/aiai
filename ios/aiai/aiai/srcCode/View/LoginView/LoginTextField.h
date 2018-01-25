//
//  LoginTextField.h
//  aiai
//
//  Created by zhangyang on 15/4/2.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, AIAI_KeyboardType) {
    AIAI_KeyboardTypeDefault = UIKeyboardTypeDefault,
    AIAI_KeyboardTypeASCIICapable = UIKeyboardTypeASCIICapable,
    AIAI_KeyboardTypeNumbersAndPunctuation = UIKeyboardTypeNumbersAndPunctuation,
    AIAI_KeyboardTypeURL = UIKeyboardTypeURL,
    AIAI_KeyboardTypeNumberPad = UIKeyboardTypeNumberPad,
    AIAI_KeyboardTypePhonePad = UIKeyboardTypePhonePad,
    AIAI_KeyboardTypeNamePhonePad = UIKeyboardTypeNamePhonePad,
    AIAI_KeyboardTypeEmailAddress = UIKeyboardTypeEmailAddress,
    AIAI_KeyboardTypeDecimalPad = UIKeyboardTypeDecimalPad,
    AIAI_KeyboardTypeTwitter = UIKeyboardTypeTwitter,
    AIAI_KeyboardTypeWebSearch = UIKeyboardTypeWebSearch,
    AIAI_KeyboardTypeAlphabet = UIKeyboardTypeAlphabet
};

@interface LoginTextField : UIView

-(id)initWithFrame:(CGRect)frame leftIconName:(NSString*)iconName placeholderText:(NSString*)placeholder;

@property(nonatomic,copy)NSString* text;

@property(nonatomic,assign)BOOL secureTextEntry;

@property(nonatomic,assign)AIAI_KeyboardType keyboardType;

-(void)resignFirstResponder;

@end
