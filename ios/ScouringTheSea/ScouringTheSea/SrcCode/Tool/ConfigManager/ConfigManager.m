//
//  ConfigManager.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/11/23.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "ConfigManager.h"
#import "SaveManager.h"
#import <objc/runtime.h>

NSString* const kConfigManagerFilePath = @"ConfigManager";

#define FONT_OFFSET 2

@implementation ConfigManager
/*
// 在类被加载到内存中时就交换两个方法的实现
+ (void)load
{
    // 交换方法
    Method oldMethod = class_getClassMethod([UIFont class], @selector(systemFontOfSize:));
    Method newMethod = class_getClassMethod([self class], @selector(sts_FontOfSize:));
    method_exchangeImplementations(oldMethod, newMethod);
}

+(UIFont*)sts_FontOfSize:(CGFloat)fontSize{
    
    return [UIFont systemFontOfSize:fontSize+FONT_OFFSET];
}
*/

+(id)readConfigManager{
    ConfigManager* configModel = [SaveManager readDataUnArchiverWithFilePath:[SaveManager libraryPath:kConfigManagerFilePath]];
    if (configModel == nil) {
        
        [self saveConfigManager];
    }
    return configModel;
}

+(void)saveConfigManager{
    [SaveManager saveDataArchiverWithModel:self filePath:[SaveManager libraryPath:kConfigManagerFilePath]];
}

@end
