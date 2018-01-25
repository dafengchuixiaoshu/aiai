//
//  AppUtils.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/1.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "AppUtils.h"
#include "sys/utsname.h"
#import <CommonCrypto/CommonDigest.h>
#import "SSKeychain.h"
#import <ImageIO/ImageIO.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES1/glext.h>

#import <CoreText/CTFontManager.h>
#import <CoreText/CTFont.h>

@implementation AppUtils

#pragma mark base64
+ (NSString*) base64StringDecode:(NSString *)string
{
    unsigned long ixtext, lentext;
    unsigned char ch, inbuf[4], outbuf[4];
    short i, ixinbuf;
    Boolean flignore, flendtext = false;
    const unsigned char *tempcstring;
    NSMutableData *theData;
    
    if (string == nil) {
        return @"";
    }
    
    ixtext = 0;
    
    tempcstring = (const unsigned char *)[string UTF8String];
    
    lentext = [string length];
    
    theData = [NSMutableData dataWithCapacity: lentext];
    
    ixinbuf = 0;
    
    while (true) {
        if (ixtext >= lentext){
            break;
        }
        
        ch = tempcstring [ixtext++];
        
        flignore = false;
        
        if ((ch >= 'A') && (ch <= 'Z')) {
            ch = ch - 'A';
        } else if ((ch >= 'a') && (ch <= 'z')) {
            ch = ch - 'a' + 26;
        } else if ((ch >= '0') && (ch <= '9')) {
            ch = ch - '0' + 52;
        } else if (ch == '+') {
            ch = 62;
        } else if (ch == '=') {
            flendtext = true;
        } else if (ch == '/') {
            ch = 63;
        } else {
            flignore = true;
        }
        
        if (!flignore) {
            short ctcharsinbuf = 3;
            Boolean flbreak = false;
            
            if (flendtext) {
                if (ixinbuf == 0) {
                    break;
                }
                
                if ((ixinbuf == 1) || (ixinbuf == 2)) {
                    ctcharsinbuf = 1;
                } else {
                    ctcharsinbuf = 2;
                }
                
                ixinbuf = 3;
                
                flbreak = true;
            }
            
            inbuf [ixinbuf++] = ch;
            
            if (ixinbuf == 4) {
                ixinbuf = 0;
                
                outbuf[0] = (inbuf[0] << 2) | ((inbuf[1] & 0x30) >> 4);
                outbuf[1] = ((inbuf[1] & 0x0F) << 4) | ((inbuf[2] & 0x3C) >> 2);
                outbuf[2] = ((inbuf[2] & 0x03) << 6) | (inbuf[3] & 0x3F);
                
                for (i = 0; i < ctcharsinbuf; i++) {
                    [theData appendBytes: &outbuf[i] length: 1];
                }
            }
            
            if (flbreak) {
                break;
            }
        }
    }
    NSString* deString = [[NSString alloc] initWithData:theData encoding:NSUTF8StringEncoding];
    return deString;
}

+ (NSString*) base64StringEncode:(NSString *)string
{
    static char base64EncodingTable[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
    };
    NSData* data = [string dataUsingEncoding:NSUTF8StringEncoding];
    unsigned long length = [data length];
    unsigned long ixtext, lentext;
    long ctremaining;
    unsigned char input[3], output[4];
    short i, charsonline = 0, ctcopy;
    const unsigned char *raw;
    NSMutableString *result;
    
    lentext = [data length];
    if (lentext < 1)
        return @"";
    result = [NSMutableString stringWithCapacity: lentext];
    raw = (unsigned char*)[data bytes];
    ixtext = 0;
    
    while (true) {
        ctremaining = lentext - ixtext;
        if (ctremaining <= 0)
            break;
        for (i = 0; i < 3; i++) {
            unsigned long ix = ixtext + i;
            if (ix < lentext)
                input[i] = raw[ix];
            else
                input[i] = 0;
        }
        output[0] = (input[0] & 0xFC) >> 2;
        output[1] = ((input[0] & 0x03) << 4) | ((input[1] & 0xF0) >> 4);
        output[2] = ((input[1] & 0x0F) << 2) | ((input[2] & 0xC0) >> 6);
        output[3] = input[2] & 0x3F;
        ctcopy = 4;
        switch (ctremaining) {
            case 1:
                ctcopy = 2;
                break;
            case 2:
                ctcopy = 3;
                break;
        }
        
        for (i = 0; i < ctcopy; i++)
            [result appendString: [NSString stringWithFormat: @"%c", base64EncodingTable[output[i]]]];
        
        for (i = ctcopy; i < 4; i++)
            [result appendString: @"="];
        
        ixtext += 3;
        charsonline += 4;
        
        if ((length > 0) && (charsonline >= length))
            charsonline = 0;
    }
    return result;
}

#pragma mark Md5
+(NSString *) md5: (NSString *) inPutText
{
    const char *cStr = [inPutText UTF8String];
    unsigned char result[CC_MD5_DIGEST_LENGTH];
    CC_MD5(cStr, (CC_LONG)strlen(cStr), result);
    
    return [[NSString stringWithFormat:@"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
             result[0], result[1], result[2], result[3],
             result[4], result[5], result[6], result[7],
             result[8], result[9], result[10], result[11],
             result[12], result[13], result[14], result[15]
             ] lowercaseString];
}

#pragma mark 验证手机号是否合法
+ (BOOL)checkPhoneNumber:(NSString *)phoneNumber{
    
    NSString * MOBILE = @"^1(3[0-9]|5[0-35-9]|8[025-9])\\d{8}$";
    NSString * CM = @"^1(34[0-8]|(3[5-9]|5[017-9]|8[278])\\d)\\d{7}$";
    NSString * CU = @"^1(3[0-2]|5[256]|8[56])\\d{8}$";
    NSString * CT = @"^1((33|53|8[09])[0-9]|349)\\d{7}$";
    
    NSPredicate *regextestmobile = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", MOBILE];
    NSPredicate *regextestcm = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", CM];
    NSPredicate *regextestcu = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", CU];
    NSPredicate *regextestct = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", CT];
    BOOL res1 = [regextestmobile evaluateWithObject:phoneNumber];
    BOOL res2 = [regextestcm evaluateWithObject:phoneNumber];
    BOOL res3 = [regextestcu evaluateWithObject:phoneNumber];
    BOOL res4 = [regextestct evaluateWithObject:phoneNumber];
    
    if (res1 || res2 || res3 || res4 )
    {
        return YES;
    }
    else
    {
        return NO;
    }
}

+(BOOL)checkMailbox:(NSString*)str{
    NSString *regex = @"[0-9]\\d{5}(?!\\d)";
    NSPredicate *pred = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex];
    
    BOOL isMatch = [pred evaluateWithObject:str];
    if (!isMatch) {
        return NO;
    }
    return YES;
}

// 处理由iPhone摄像头拍摄的图片，上传至服务器后，造成其它设备查看时图片显示方向不正确的问题
+ (UIImage *)fixOrientation:(UIImage*)srcImage {
    
    // No-op if the orientation is already correct
    if (srcImage.imageOrientation == UIImageOrientationUp) return srcImage;
    
    // We need to calculate the proper transformation to make the image upright.
    // We do it in 2 steps: Rotate if Left/Right/Down, and then flip if Mirrored.
    CGAffineTransform transform = CGAffineTransformIdentity;
    
    UIImageOrientation orient = srcImage.imageOrientation;
    
    switch (orient) {
        case UIImageOrientationDown:
        case UIImageOrientationDownMirrored:
            transform = CGAffineTransformTranslate(transform, srcImage.size.width, srcImage.size.height);
            transform = CGAffineTransformRotate(transform, M_PI);
            break;
            
        case UIImageOrientationLeft:
        case UIImageOrientationLeftMirrored:
            transform = CGAffineTransformTranslate(transform, srcImage.size.width, 0);
            transform = CGAffineTransformRotate(transform, M_PI_2);
            break;
            
        case UIImageOrientationRight:
        case UIImageOrientationRightMirrored:
            transform = CGAffineTransformTranslate(transform, 0, srcImage.size.height);
            transform = CGAffineTransformRotate(transform, -M_PI_2);
            break;
        default:
            break;
    }
    
    switch (orient) {
        case UIImageOrientationUpMirrored:
        case UIImageOrientationDownMirrored:
            transform = CGAffineTransformTranslate(transform, srcImage.size.width, 0);
            transform = CGAffineTransformScale(transform, -1, 1);
            break;
            
        case UIImageOrientationLeftMirrored:
        case UIImageOrientationRightMirrored:
            transform = CGAffineTransformTranslate(transform, srcImage.size.height, 0);
            transform = CGAffineTransformScale(transform, -1, 1);
            break;
        default:
            break;
    }
    
    // Now we draw the underlying CGImage into a new context, applying the transform
    // calculated above.
    CGContextRef ctx = CGBitmapContextCreate(NULL, srcImage.size.width, srcImage.size.height,
                                             CGImageGetBitsPerComponent(srcImage.CGImage), 0,
                                             CGImageGetColorSpace(srcImage.CGImage),
                                             CGImageGetBitmapInfo(srcImage.CGImage));
    CGContextConcatCTM(ctx, transform);
    switch (orient) {
        case UIImageOrientationLeft:
        case UIImageOrientationLeftMirrored:
        case UIImageOrientationRight:
        case UIImageOrientationRightMirrored:
            // Grr...
            CGContextDrawImage(ctx, CGRectMake(0,0,srcImage.size.height,srcImage.size.width), srcImage.CGImage);
            break;
            
        default:
            CGContextDrawImage(ctx, CGRectMake(0,0,srcImage.size.width,srcImage.size.height), srcImage.CGImage);
            break;
    }
    
    // And now we just create a new UIImage from the drawing context
    CGImageRef cgimg = CGBitmapContextCreateImage(ctx);
    UIImage *img = [UIImage imageWithCGImage:cgimg];
    CGContextRelease(ctx);
    CGImageRelease(cgimg);
    return img;
}


+ (NSString *)getTime:(id)time {
    NSNumber* timeNumber;
    if ([time isKindOfClass:[NSString class]]) {
        timeNumber = [[NSNumber alloc] initWithLongLong:[time longLongValue]];
    }
    else if ([time isKindOfClass:[NSNumber class]]) {
        timeNumber = [[NSNumber alloc] initWithLongLong:[[time stringValue] longLongValue]];
    }
    else {
        return @"";
    }
    NSDate* dataTime = [NSDate dateWithTimeIntervalSince1970:timeNumber.doubleValue/1000.0];
    
    NSTimeInterval beforeDateInterval = [dataTime timeIntervalSince1970];
    NSTimeInterval nowDateInterval = [[NSDate date] timeIntervalSince1970];
    
    NSTimeInterval subDate = nowDateInterval - beforeDateInterval;
    
    NSString *res = nil;
    
    if (subDate / 3600 <= 1) {
        res = [NSString stringWithFormat:@"%f", subDate / 60];
        res = [res substringToIndex:res.length - 7];
        if (subDate < 60) {
            res = [NSString stringWithFormat:@"刚刚"];
        }
        else{
            res = [NSString stringWithFormat:@"%@分钟前", res];
        }
    }
    else if (subDate / 3600 > 1 && subDate / 86400 <= 1) {
        res = [NSString stringWithFormat:@"%f", subDate / 3600];
        res = [res substringToIndex:res.length - 7];
        res = [NSString stringWithFormat:@"%@小时前", res];
    }
    else if (subDate / 86400 > 1 && subDate / 86400 <= 2) {
        res = [NSString stringWithFormat:@"%f", subDate / 86400];
        res = [res substringToIndex:res.length - 7];
        res = [NSString stringWithFormat:@"%@天前", res];
    }
    else {
        NSDateFormatter* dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy-MM-dd"];
        res = [dateFormatter stringFromDate:dataTime];
    }
    
    return res;
}

+(CGFloat)SystemVertion{
    return [[[UIDevice currentDevice] systemVersion] floatValue];
}

//获取设备唯一标识
+ (NSString*)uuid {
    NSString *uuid = [SSKeychain passwordForService:@"com.izptec.f2c" account:@"UUID"];
    
    if (nil != uuid && 0 != uuid.length) {
        return uuid;
    }
    
    CFUUIDRef puuid = CFUUIDCreate( nil );
    CFStringRef uuidString = CFUUIDCreateString( nil, puuid );
    uuid = (__bridge NSString*)CFStringCreateCopy(NULL, uuidString);
    
    if (nil != uuid && 0 != uuid.length) {
        [SSKeychain setPassword:uuid forService:@"com.izptec.f2c" account:@"UUID"];
    }
    
    return uuid;
}

//获取设备语言
+ (NSString*)language {
    
    NSArray *languageArray = [NSLocale preferredLanguages];
    NSString *language = [languageArray objectAtIndex:0];
    
    return language;
}

+(void)closeKeyBoard{
    [[UIApplication sharedApplication].keyWindow endEditing:YES];
}

//判断是否为emoji
+ (BOOL)stringContainsEmoji:(NSString *)string
{
    __block BOOL returnValue = NO;
    [string enumerateSubstringsInRange:NSMakeRange(0, [string length])
                               options:NSStringEnumerationByComposedCharacterSequences
                            usingBlock:^(NSString *substring, NSRange substringRange, NSRange enclosingRange, BOOL *stop) {
                                const unichar hs = [substring characterAtIndex:0];
                                // surrogate pair
                                if (0xd800 <= hs && hs <= 0xdbff) {
                                    if (substring.length > 1) {
                                        const unichar ls = [substring characterAtIndex:1];
                                        const int uc = ((hs - 0xd800) * 0x400) + (ls - 0xdc00) + 0x10000;
                                        if (0x1d000 <= uc && uc <= 0x1f77f) {
                                            returnValue = YES;
                                        }
                                    }
                                } else if (substring.length > 1) {
                                    const unichar ls = [substring characterAtIndex:1];
                                    if (ls == 0x20e3) {
                                        returnValue = YES;
                                    }
                                    
                                } else {
                                    // non surrogate
                                    if (0x2100 <= hs && hs <= 0x27ff) {
                                        returnValue = YES;
                                    } else if (0x2B05 <= hs && hs <= 0x2b07) {
                                        returnValue = YES;
                                    } else if (0x2934 <= hs && hs <= 0x2935) {
                                        returnValue = YES;
                                    } else if (0x3297 <= hs && hs <= 0x3299) {
                                        returnValue = YES;
                                    } else if (hs == 0xa9 || hs == 0xae || hs == 0x303d || hs == 0x3030 || hs == 0x2b55 || hs == 0x2b1c || hs == 0x2b1b || hs == 0x2b50) {
                                        returnValue = YES;
                                    }
                                }
                            }];
    
    return returnValue;
}

+ (BOOL)isPureInt:(NSString*)string{
    NSScanner* scan = [NSScanner scannerWithString:string];
    int val;
    return [scan scanInt:&val] && [scan isAtEnd];
}

+ (NSDictionary*)faceNameDictionary{
    NSDictionary *faceNameDictionary = @{@"000":@"[微笑]", @"001":@"[开心]", @"002":@"[吐舌]", @"003":@"[汗]", @"004":@"[偷笑]", @"005":@"[挥手]", @"006":@"[无奈]", @"007":@"[囧]", @"008":@"[猪头]", @"009":@"[生气]", @"010":@"[海带泪]", @"011":@"[流泪]", @"012":@"[嘘]", @"013":@"[酷]", @"014":@"[抓狂]", @"015":@"[委屈]", @"016":@"[便便]", @"017":@"[炸弹]", @"018":@"[菜刀]", @"019":@"[可爱]", @"020":@"[花痴]", @"021":@"[红晕]", @"022":@"[得意]", @"023":@"[汗颜]", @"024":@"[闭嘴]", @"025":@"[花朵]", @"026":@"[小汗]", @"027":@"[大汗]", @"028":@"[呆萌]", @"029":@"[爱心]", @"030":@"[红唇]", @"031":@"[哦哦]", @"032":@"[愤怒]", @"033":@"[糗]", @"034":@"[严厉]", @"035":@"[疑惑]", @"036":@"[睡觉]", @"037":@"[么么]", @"038":@"[哈皮]", @"039":@"[心心]", @"040":@"[尴尬]", @"041":@"[敲头]", @"042":@"[呵呵]", @"043":@"[奋斗]", @"044":@"[呆呆]", @"045":@"[右哼哼]", @"046":@"[抱抱]", @"047":@"[哈哈]", @"048":@"[喜欢]", @"049":@"[鄙视]", @"050":@"[晕]", @"051":@"[吃]", @"052":@"[可怜]", @"053":@"[凋谢]", @"054":@"[要]", @"055":@"[灰机]", @"056":@"[握手]", @"057":@"[耶]", @"058":@"[佩服]", @"059":@"[饭饭]", @"060":@"[蛋糕]", @"061":@"[柠檬]", @"062":@"[果汁]", @"063":@"[小虫]", @"064":@"[勾引]", @"065":@"[OK]", @"066":@"[爱你]", @"067":@"[咖啡]", @"068":@"[月亮]", @"069":@"[刀]", @"070":@"[剪刀]", @"071":@"[差劲]", @"072":@"[心碎]", @"073":@"[太阳]", @"074":@"[钥匙]", @"075":@"[篮球]", @"076":@"[骷髅]", @"077":@"[电话]", @"078":@"[闪电]", @"079":@"[饿了]", @"080":@"[怒骂]", @"081":@"[折磨]", @"082":@"[抠鼻]", @"083":@"[鼓掌]", @"084":@"[大囧]", @"085":@"[左哼哼]", @"086":@"[哈欠]", @"087":@"[不开森]", @"088":@"[灯泡]", @"089":@"[手机]", @"090":@"[奖杯]", @"091":@"[LOW]", @"092":@"[赞]", @"093":@"[邮件]", @"094":@"[礼物]", @"095":@"[时钟]", @"096":@"[云朵]", @"097":@"[冰淇淋]", @"098":@"[购物]", @"099":@"[伞]", @"100":@"[衣服]", @"101":@"[暗喜]", @"102":@"[吹口哨]", @"103":@"[调皮MM]", @"104":@"[调皮GG]"};
    return faceNameDictionary;
}

+(NSRange)getFaceRange:(NSString *)text andLocation:(NSUInteger)location{
    NSString* expression_begin_flag  = @"/f";
    NSString* expression_end_flag  = @"/f";
    NSInteger expression_length  = 7;
    
    NSRange beginFaceRange=[text rangeOfString: expression_begin_flag];
    if (beginFaceRange.length>0 && text.length - beginFaceRange.location >= expression_length) {
        NSString *str = [text substringWithRange:NSMakeRange(beginFaceRange.location+expression_begin_flag.length, expression_length-expression_begin_flag.length)];
        if ([str hasSuffix: expression_end_flag]) {//有效
            str = [str substringToIndex:str.length-expression_end_flag.length];
            if ([AppUtils isPureInt:str] && str.intValue < [[AppUtils faceNameDictionary] count]) {
                return NSMakeRange(beginFaceRange.location+location, beginFaceRange.length);
            }
        }
        NSUInteger beginLocation = beginFaceRange.location+beginFaceRange.length;
        if (text.length>beginLocation) {
            return [AppUtils getFaceRange:[text substringFromIndex:beginLocation] andLocation:beginLocation + location];
        }
    }
    return NSMakeRange(NSNotFound, 0);
}

+ (NSString*)replaceExpression:(NSString*)srcContent {
    if (nil == srcContent) {
        return nil;
    }
    
    NSString *resultStr = [NSString stringWithString:srcContent];
    
    while (1) {
        NSRange range = [AppUtils getFaceRange:resultStr andLocation:0];
        
        if (NSNotFound == range.location) {
            break;
        }
        
        NSString *numStr = [resultStr substringWithRange:NSMakeRange(range.location + 2, 3)];
        
        NSString *faceName = [[AppUtils faceNameDictionary] objectForKey:numStr];
        
        resultStr = [resultStr stringByReplacingCharactersInRange:NSMakeRange(range.location, 7) withString:faceName];
    }
    
    return resultStr;
}

+(NSString *)trimmingEndSpace:(NSString *)text{
    if (text.length>0) {
        if ([text hasSuffix:@" "]) {
            text = [text substringToIndex:text.length-1];
            text = [AppUtils trimmingEndSpace:text];
        }
    }
    return text;
}

+(void)releaseSubView:(UIView *)view NoReleaseView:(UIView *)noRView{
    UIView *subView;
    for (subView in view.subviews) {
        if (noRView && [subView isEqual:noRView]){
            continue;
        }
        if (subView.subviews.count>0) {
            [AppUtils releaseSubView:subView NoReleaseView:nil];
        }
        [subView removeFromSuperview];
        subView = nil;
    }
}

-(BOOL)isEmptyWithString:(NSString*)string{
    if (!string) {
        return true;
    } else {
        //A character set containing only the whitespace characters space (U+0020) and tab (U+0009) and the newline and nextline characters (U+000A–U+000D, U+0085).
        NSCharacterSet *set = [NSCharacterSet whitespaceAndNewlineCharacterSet];
        
        //Returns a new string made by removing from both ends of the receiver characters contained in a given character set.
        NSString *trimedString = [string stringByTrimmingCharactersInSet:set];
        
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

+(UIImage*) imageWithUIView:(UIView*) view
{
    UIGraphicsBeginImageContext(view.bounds.size);
//    UIGraphicsBeginImageContextWithOptions(view.bounds.size, YES,  [UIScreen mainScreen].scale);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [view.layer renderInContext:ctx];
    UIImage* tImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return tImage;
}

+(void)writeVideoWithPath:(NSString*)videoFilePath savePath:(NSString*)savePath{
    //编辑之后的视频写入文件
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error;
    
    NSData *videoData = [NSData dataWithContentsOfURL:[NSURL URLWithString:videoFilePath]];
    
    BOOL exist = [fileManager fileExistsAtPath:savePath];
    if(exist) {
        exist = [fileManager removeItemAtPath:savePath error:&error];
    }
    [videoData writeToFile:savePath atomically:YES];
}

+(NSString*)stringFromTimeData:(id)date{
    NSString *timestr = [NSString stringWithFormat:@"%@",date];
    NSRange rang;
    rang.location = 0;
    rang.length = 10;
    NSString *needtime = [timestr substringWithRange:rang];
    return needtime;
}

/**
 * 截取部分图像
 *
 **/
+(UIImage*)getSubImage:(UIImage *)image mCGRect:(CGRect)mCGRect centerBool:(BOOL)centerBool
{
    /*如若centerBool为Yes则是由中心点取mCGRect范围的图片*/
    float imgwidth = image.size.width;
    float imgheight = image.size.height;
    float viewwidth = mCGRect.size.width;
    float viewheight = mCGRect.size.height;
    CGRect rect;
    if(centerBool)
        rect = CGRectMake((imgwidth-viewwidth)/2, (imgheight-viewheight)/2, viewwidth, viewheight);
    else{
        if (viewheight < viewwidth) {
            if (imgwidth <= imgheight) {
                rect = CGRectMake(0, 0, imgwidth, imgwidth*viewheight/viewwidth);
            }else {
                float width = viewwidth*imgheight/viewheight;
                float x = (imgwidth - width)/2 ;
                if (x > 0) {
                    rect = CGRectMake(x, 0, width, imgheight);
                }else {
                    rect = CGRectMake(0, 0, imgwidth, imgwidth*viewheight/viewwidth);
                }
            }
        }else {
            if (imgwidth <= imgheight) {
                float height = viewheight*imgwidth/viewwidth;
                if (height < imgheight) {
                    rect = CGRectMake(0, 0, imgwidth, height);
                }else {
                    rect = CGRectMake(0, 0, viewwidth*imgheight/viewheight, imgheight);
                }
            }else {
                float width = viewwidth*imgheight/viewheight;
                if (width < imgwidth) {
                    float x = (imgwidth - width)/2 ;
                    rect = CGRectMake(x, 0, width, imgheight);
                }else {
                    rect = CGRectMake(0, 0, imgwidth, imgheight);
                }
            }
        }
    }
    
    CGImageRef subImageRef = CGImageCreateWithImageInRect(image.CGImage, rect);
    CGRect smallBounds = CGRectMake(0, 0, CGImageGetWidth(subImageRef), CGImageGetHeight(subImageRef));
    
    UIGraphicsBeginImageContext(smallBounds.size);
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextDrawImage(context, smallBounds, subImageRef);
    UIImage* smallImage = [UIImage imageWithCGImage:subImageRef];
    UIGraphicsEndImageContext();
    
    return smallImage;
}

static int i=0;
+(UIImage *)ScreenShotWithView:(UIView*)shotView{
    UIGraphicsBeginImageContextWithOptions(shotView.frame.size, YES, [UIScreen mainScreen].scale);     //设置截屏大小
    [[shotView layer] renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *viewImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
//    CGImageRef imageRef = viewImage.CGImage;
//    CGRect rect = CGRectMake(0, 0, shotView.frame.size.width, shotView.frame.size.height);//这里可以设置想要截图的区域
//    CGImageRef imageRefRect =CGImageCreateWithImageInRect(imageRef, rect);
//    UIImage *sendImage = [[UIImage alloc] initWithCGImage:imageRefRect];
//    UIImageWriteToSavedPhotosAlbum(sendImage, nil, nil, nil);//保存图片到照片库
//    NSData *imageViewData = UIImagePNGRepresentation(sendImage);
//    CGImageRelease(imageRefRect);
    
    NSData *imageViewData = UIImagePNGRepresentation(viewImage);
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *pictureName= [NSString stringWithFormat:@"screenShow_%d.png",i];
    NSString *savedImagePath = [documentsDirectory stringByAppendingPathComponent:pictureName];
    NSLog(@"%@", savedImagePath);
    [imageViewData writeToFile:savedImagePath atomically:YES];
    
    return viewImage;
}



+(UIImage *) glToUIImage {
    NSInteger screenWidth = [UIScreen mainScreen].bounds.size.width;
    NSInteger screenHeight = [UIScreen mainScreen].bounds.size.height;
    NSInteger myDataLength = screenHeight * screenWidth * 4;
    
    // allocate array and read pixels into it.
    GLubyte *buffer = (GLubyte *) malloc(myDataLength);
    glReadPixels(0, 0, (GLsizei)screenHeight, (GLsizei)screenWidth, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    
    // gl renders "upside down" so swap top to bottom into new array.
    // there's gotta be a better way, but this works.
    GLubyte *buffer2 = (GLubyte *) malloc(myDataLength);
    for(int y = 0; y <screenWidth; y++)
    {
        for(int x = 0; x <screenHeight * 4; x++)
        {
            buffer2[(screenWidth-1 - y) * screenHeight * 4 + x] = buffer[y * 4 * screenHeight + x];
        }
    }
    
    // make data provider with data.
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer2, myDataLength, NULL);
    
    // prep the ingredients
    int bitsPerComponent = 8;
    int bitsPerPixel = 32;
    long bytesPerRow = 4 * screenHeight;
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    
    // make the cgimage
    CGImageRef imageRef = CGImageCreate(screenHeight, screenWidth, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpaceRef, bitmapInfo, provider, NULL, NO, renderingIntent);
    
    // then make the uiimage from that
    UIImage *myImage = [UIImage imageWithCGImage:imageRef];
    return myImage;
}

+(UIImage*)snapshot:(UIView*)eaglview
{
    GLint backingWidth, backingHeight;
    
    // Bind the color renderbuffer used to render the OpenGL ES view
    // If your application only creates a single color renderbuffer which is already bound at this point,
    // this call is redundant, but it is needed if you're dealing with multiple renderbuffers.
    // Note, replace "viewRenderbuffer" with the actual name of the renderbuffer object defined in your class.
//    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    // Get the size of the backing CAEAGLLayer
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    GLsizei x = 0, y = 0, width = backingWidth, height = backingHeight;
    NSInteger dataLength = width * height * 4;
    GLubyte *data = (GLubyte*)malloc(dataLength * sizeof(GLubyte));
    
    // Read pixel data from the framebuffer
    
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    
    // Create a CGImage with the pixel data
    
    // If your OpenGL ES content is opaque, use kCGImageAlphaNoneSkipLast to ignore the alpha channel
    
    // otherwise, use kCGImageAlphaPremultipliedLast
    
    CGDataProviderRef ref = CGDataProviderCreateWithData(NULL, data, dataLength, NULL);
    
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    
    CGImageRef iref = CGImageCreate(width, height, 8, 32, width * 4, colorspace, kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast,ref, NULL, true, kCGRenderingIntentDefault);
    // OpenGL ES measures data in PIXELS
    
    // Create a graphics context with the target size measured in POINTS

    CGFloat scale = eaglview.contentScaleFactor;
    NSInteger widthInPoints = width / scale;
    NSInteger heightInPoints = height / scale;
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(widthInPoints, heightInPoints), NO, scale);

    CGContextRef cgcontext = UIGraphicsGetCurrentContext();
    // UIKit coordinate system is upside down to GL/Quartz coordinate system
    // Flip the CGImage by rendering it to the flipped bitmap context
    // The size of the destination area is measured in POINTS
    CGContextSetBlendMode(cgcontext, kCGBlendModeCopy);
    CGContextDrawImage(cgcontext, CGRectMake(0.0, 0.0, widthInPoints, heightInPoints), iref);
    
    // Retrieve the UIImage from the current context
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    // Clean up
    free(data);
    CFRelease(ref);
    CFRelease(colorspace);
    CGImageRelease(iref);
    return image;
}

#pragma mark 发送图片视频动画
#define DURATIONTIME        0.300
+ (void)CaptureAnimationWithImageView:(UIImage*)fromImage startPoint:(CGPoint)startP endPoint:(CGPoint)endP runView:(UIView*)runview{
    CGSize screenSize = [UIScreen mainScreen].bounds.size;
    UIImageView *imageView=[[UIImageView alloc]initWithFrame:CGRectMake(0, 0, screenSize.width, screenSize.width)];
    imageView.contentMode=UIViewContentModeScaleToFill;
    imageView.image=fromImage;
    CALayer *layer=[[CALayer alloc]init];
    layer.contents=imageView.layer.contents;
    layer.frame=imageView.frame;
    layer.opacity=1;
    [runview.layer addSublayer:layer];
    
    //动画 终点 都以sel.view为参考系
    CGPoint endpoint= endP;
    UIBezierPath *path=[UIBezierPath bezierPath];
    //动画起点
    CGPoint startPoint=startP;
    [path moveToPoint:startPoint];
    //贝塞尔曲线中间点
    float sx=startPoint.x;
    float sy=startPoint.y;
    float ex=endpoint.x;
    float ey=endpoint.y;
    float x=sx+(ex-sx)/3;
    float y=sy+(ey-sy)*0.5-400;
    CGPoint centerPoint=CGPointMake(x,y);
    [path addQuadCurveToPoint:endpoint controlPoint:centerPoint];
    
    CAKeyframeAnimation *moveAnim=[CAKeyframeAnimation animationWithKeyPath:@"position"];
    moveAnim.path = path.CGPath;
    moveAnim.removedOnCompletion = YES;
    moveAnim.fillMode = kCAFillModeForwards;
    moveAnim.duration=DURATIONTIME;
    moveAnim.autoreverses= NO;
    moveAnim.removedOnCompletion = YES;
    moveAnim.timingFunction=[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseIn];
    
    CABasicAnimation *scaleAnim = [CABasicAnimation animationWithKeyPath:@"transform"];
    scaleAnim.fromValue = [NSValue valueWithCATransform3D:CATransform3DIdentity];
    scaleAnim.toValue = [NSValue valueWithCATransform3D:CATransform3DMakeScale(0.1, 0.1, 1.0)];
    scaleAnim.removedOnCompletion = YES;
    
    CAAnimationGroup *animGroup = [CAAnimationGroup animation];
    animGroup.animations = [NSArray arrayWithObjects:moveAnim, scaleAnim, nil];
    animGroup.duration = DURATIONTIME;
    [animGroup setValue:layer forKey:@"toViewKey"];
    animGroup.removedOnCompletion = YES;
    [layer addAnimation:animGroup forKey:nil];
    
    double delayInSeconds = DURATIONTIME+0.130;
    dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
    dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
       [layer removeFromSuperlayer];
    });
}

- (void)animationDidStop:(CAAnimation *)anim finished:(BOOL)flag{
    //通过代理实现该函数会闪，没有好的解决办法，暂时使用perform
    CALayer* transitionLayer = [anim valueForKey:@"toViewKey"];
    [transitionLayer removeFromSuperlayer];
}

-(void)animationFinish:(CALayer*)layer{
    [layer removeFromSuperlayer];
}

//获取第三方字体
+(NSArray*)getFontArray:(NSString*)path size:(CGFloat)size
{
    NSMutableArray *fontArray = [[NSMutableArray alloc] initWithObjects:[UIFont systemFontOfSize:size], nil];
    NSArray *file = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath: path error:nil];
    for (NSString *fileName in file) {
        
        NSString *filePath = [NSString stringWithFormat:@"%@/%@",path,fileName];
//        [self registerFont:filePath];
        
        NSURL *fontUrl = [NSURL fileURLWithPath:filePath];
        CGDataProviderRef fontDataProvider = CGDataProviderCreateWithURL((__bridge CFURLRef)fontUrl);
        CGFontRef fontRef = CGFontCreateWithDataProvider(fontDataProvider);

        CGDataProviderRelease(fontDataProvider);
        CTFontManagerRegisterFontsForURL((__bridge CFURLRef)fontUrl, kCTFontManagerScopeNone, NULL);
        NSString *fontName = CFBridgingRelease(CGFontCopyPostScriptName(fontRef));
        if (fontName) {
            UIFont *font = [UIFont fontWithName:fontName size:size];
            if(font){
                [fontArray addObject:font];
            }
        }
        CTFontManagerUnregisterFontsForURL((__bridge CFURLRef)fontUrl, kCTFontManagerScopeNone, NULL);
        CGFontRelease(fontRef);
         
    }
    return fontArray;
}

+ (void)registerFont:(NSString *)fontPath
{
    NSData *dynamicFontData = [NSData dataWithContentsOfFile:fontPath];
    if (!dynamicFontData)
    {
        return;
    }
    CFErrorRef error;
    CGDataProviderRef providerRef = CGDataProviderCreateWithCFData((__bridge CFDataRef)dynamicFontData);
    CGFontRef font = CGFontCreateWithDataProvider(providerRef);
    if (! CTFontManagerRegisterGraphicsFont(font, &error))
    {
        //注册失败
        CFStringRef errorDescription = CFErrorCopyDescription(error);
        NSLog(@"Failed to load font: %@", errorDescription);
        CFRelease(errorDescription);
    }
    CGFontRelease(font);
    CGDataProviderRelease(providerRef);
    CTFontManagerUnregisterGraphicsFont(font, NULL);
}

@end
