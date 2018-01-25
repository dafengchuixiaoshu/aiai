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

+(BOOL)saveToFileWithString:(NSString*)dataString filePath:(NSString*)path{
    return [dataString writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
}

+(BOOL)saveToFileWithData:(NSData*)data filePath:(NSString*)path{
    return [data writeToFile:path atomically:YES];
}

+(BOOL)saveToFileWithArray:(NSArray*)array filePath:(NSString*)path{
    return [array writeToFile:path atomically:YES];
}
+(BOOL)saveToFileWithDictionary:(NSDictionary*)dic filePath:(NSString*)path{
    return [dic writeToFile:path atomically:YES];
}

+(NSString*)readToStringWithFilePath:(NSString*)path{
    return [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
}

+(NSData*)readToDataWithFilePath:(NSString*)path{
    return [NSData dataWithContentsOfFile:path];
}

+(NSArray*)readToArrayWithFilePath:(NSString*)path{
    
    return [NSArray arrayWithContentsOfFile:path];
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

#pragma mark //创建vedio文件夹
+ (BOOL)createVideoFolderWithTmpDir
{
    NSString* folderName = [SaveManager tmpPath:VIDEO_FOLDER_NAME];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL isDir = YES;
    BOOL isDirExist = [fileManager fileExistsAtPath:folderName isDirectory:&isDir];
    
    if(isDirExist == NO)
    {
        BOOL bCreateDir = [fileManager createDirectoryAtPath:folderName withIntermediateDirectories:YES attributes:nil error:nil];
        if(!bCreateDir){
            NSLog(@"创建图片文件夹失败");
            return NO;
        }
        return YES;
    }
    return YES;
}

+ (NSString *)getVideoMergeFilePathString
{
    NSString* folderName = [SaveManager tmpPath:VIDEO_FOLDER_NAME];
    
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyyMMddHHmmss";
    NSString *nowTimeStr = [formatter stringFromDate:[NSDate dateWithTimeIntervalSinceNow:0]];
    
    NSString *fileName = [[folderName stringByAppendingPathComponent:nowTimeStr] stringByAppendingString:@"merge.mp4"];
    
    return fileName;
}

+ (NSString *)getVideoSaveFilePathString
{
    NSString* folderName = [SaveManager tmpPath:VIDEO_FOLDER_NAME];
    
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyyMMddHHmmss";
    NSString *nowTimeStr = [formatter stringFromDate:[NSDate dateWithTimeIntervalSinceNow:0]];
    
    NSString *fileName = [[folderName stringByAppendingPathComponent:nowTimeStr] stringByAppendingString:@".mp4"];
    
    return fileName;
}

+ (NSString *)getVideoFolderPathString
{
    NSString* folderName = [SaveManager tmpPath:VIDEO_FOLDER_NAME];
    
    return folderName;
}

@end