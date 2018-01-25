//
//  NSString+isEmpty.m
//  F2CPlatform
//
//  Created by zhangyang on 14-6-13.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "NSString+isEmpty.h"

@implementation NSString (isEmpty)

-(BOOL)isEmpty{
    if (!self) {
        return true;
    } else {
        //A character set containing only the whitespace characters space (U+0020) and tab (U+0009) and the newline and nextline characters (U+000A–U+000D, U+0085).
        NSCharacterSet *set = [NSCharacterSet whitespaceAndNewlineCharacterSet];
        
        //Returns a new string made by removing from both ends of the receiver characters contained in a given character set.
        NSString *trimedString = [self stringByTrimmingCharactersInSet:set];
        
        if ([trimedString length] == 0) {
            return YES;
        } else {
            if ([trimedString isEqualToString:@""]) {
                return YES;
            }
            return NO;
        }
    }
}

-(BOOL)offNormalUpper:(NSInteger)upper AndLower:(NSInteger)lower{
    if (self.length<lower||self.length*2>upper) {
        int length = 0;
        for(int i=0; i< [self length];i++){
            int a = [self characterAtIndex:i];
            if( a > 0x4e00 && a < 0x9fff)
                length+=2;
            else
                length++;
        }
        return length>=lower&&length<=upper;
    }
    return YES;
}

-(BOOL)checkFormatWithUpper:(NSInteger)upper AndLower:(NSInteger)lower{
    
    BOOL allNum = NO;

    int length = 0;
    for(int i=0; i< [self length];i++){
        int a = [self characterAtIndex:i];
        
//        if( a > 0x4e00 && a < 0x9fff)
//            length+=2;
//        else
            length++;
        
        if (length>upper) {
            return NO;
        }
        //判断数字0-9
        if (a >=48 && a<=57) {
            continue;
        }else{
            allNum = YES;
        }
    }
    return length>=lower&&length<=upper&&allNum;
    
}

-(BOOL)voiceRecognitioning{
    __block BOOL returnValue = NO;
    [self enumerateSubstringsInRange:NSMakeRange(0, [self length]) options:NSStringEnumerationByComposedCharacterSequences usingBlock:
     ^(NSString *substring, NSRange substringRange, NSRange enclosingRange, BOOL *stop) {
         const unichar hs = [substring characterAtIndex:0];
         if (hs==0xfffc) {
             returnValue = YES;
         }
     }];
    return returnValue;
}

-(int)isEmoji{
    for (int i =1; i<=3; i++) {
        if (self.length>=i) {
            NSString *emoji = [self substringFromIndex:self.length - i];
            if ([AppUtils stringContainsEmoji:emoji]) {
                return i;
            }
        }
    }
    return 0;
}

+ (NSInteger)calcStringLength:(NSString*)srcStr {
    NSInteger length = 0;
    length = [srcStr lengthOfBytesUsingEncoding:NSUnicodeStringEncoding];
    
    return length;
}

@end
