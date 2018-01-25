//
//  GlobalDataInfo.m
//  F2CPlatform
//
//  Created by zhangyang on 14/12/5.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "SaveManager.h"

#define GlobalDataSavePath_Key @"GlobalData"

@implementation SaveManager

+(void)saveValue:(id)value forKey:(NSString *)key{
    NSString* saveFilePath = [SaveManager libraryPath:GlobalDataSavePath_Key];
    NSDictionary* oldDic = [SaveManager readToDictionaryWithFilePath:saveFilePath];
    if (oldDic) {
        NSMutableDictionary* tmpDic = [NSMutableDictionary dictionaryWithDictionary:oldDic];
        [tmpDic setObject:value forKey:key];
        NSString* saveFilePath = [SaveManager libraryPath:GlobalDataSavePath_Key];
        [SaveManager saveToFileWithDictionary:tmpDic filePath:saveFilePath];
    }else{
        NSMutableDictionary* newDic = [NSMutableDictionary dictionary];
        [newDic setObject:value forKey:key];
        NSString* saveFilePath = [SaveManager libraryPath:GlobalDataSavePath_Key];
        [SaveManager saveToFileWithDictionary:newDic filePath:saveFilePath];
    }
}

+(id)valueWithKey:(NSString *)key{
    NSString* saveFilePath = [SaveManager libraryPath:GlobalDataSavePath_Key];
    NSDictionary* oldDic = [SaveManager readToDictionaryWithFilePath:saveFilePath];
    if (oldDic) {
        return [oldDic objectForKey:key];
    }
    return nil;
}

#pragma mark 获取常用文件路径
+(NSString *)bundlePath:(NSString *)fileName{
    return [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:fileName];
}

+(NSString *)libraryPath:(NSString *)fileName{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    NSString *libraryDirectory = [paths objectAtIndex:0];
    return [libraryDirectory stringByAppendingPathComponent:fileName];
}

+(NSString *)documentsPath:(NSString *)fileName{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docDirectory = [paths objectAtIndex:0];
    return [docDirectory stringByAppendingPathComponent:fileName];
}

+(NSString *)cachesPath:(NSString *)fileName{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cachesDirectory = [paths objectAtIndex:0];
    return [cachesDirectory stringByAppendingPathComponent:fileName];
}

+(NSString *)tmpPath:(NSString *)fileName{
    return [NSTemporaryDirectory() stringByAppendingPathComponent:fileName];
}

#pragma mark 文件存储读取
+(BOOL)saveToFileWithString:(NSString*)dataString filePath:(NSString*)path{
    return [dataString writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
}

+(NSString*)readToStringWithFilePath:(NSString*)path{
    return [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
}

+(BOOL)saveToFileWithData:(NSData*)data filePath:(NSString*)path{
    return [data writeToFile:path atomically:YES];
}

+(NSData*)readToDataWithFilePath:(NSString*)path{
    return [NSData dataWithContentsOfFile:path];
}

+(BOOL)saveToFileWithArray:(NSArray*)array filePath:(NSString*)path{
    return [array writeToFile:path atomically:YES];
}

+(NSArray*)readToArrayWithFilePath:(NSString*)path{
    return [NSArray arrayWithContentsOfFile:path];
}

+(BOOL)saveToFileWithDictionary:(NSDictionary*)dic filePath:(NSString*)path{
    return [dic writeToFile:path atomically:YES];
}

+(NSDictionary*)readToDictionaryWithFilePath:(NSString*)path{
    return [NSDictionary dictionaryWithContentsOfFile:path];
}

#pragma mark 归档存储
+ (BOOL)saveDataArchiverWithModel:(id)model filePath:(NSString*)path{

    return [NSKeyedArchiver archiveRootObject:model toFile:path];
}


+ (id)readDataUnArchiverWithFilePath:(NSString*)path{
    if ([[NSFileManager defaultManager] fileExistsAtPath:path] == NO) {
        return nil;
    }
    id obj = [NSKeyedUnarchiver unarchiveObjectWithFile:path];
    
    return obj;
}


@end