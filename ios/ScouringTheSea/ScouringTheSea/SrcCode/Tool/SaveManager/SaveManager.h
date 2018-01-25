//
//  GlobalDataInfo.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/5.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SaveManager : NSObject

//userdefault存储
+(void)saveValue:(id)value forKey:(NSString *)key;
+(id)valueWithKey:(NSString *)key;

//获取文件路径
+(NSString *)bundlePath:(NSString *)fileName;
+(NSString *)libraryPath:(NSString *)fileName;
+(NSString *)cachesPath:(NSString *)fileName;
+(NSString *)documentsPath:(NSString *)fileName;
+(NSString *)tmpPath:(NSString *)fileName;

//文件存储
+(BOOL)saveToFileWithString:(NSString*)dataString filePath:(NSString*)path;
+(NSString*)readToStringWithFilePath:(NSString*)path;

+(BOOL)saveToFileWithData:(NSData*)data filePath:(NSString*)path;
+(NSData*)readToDataWithFilePath:(NSString*)path;

+(BOOL)saveToFileWithArray:(NSArray*)array filePath:(NSString*)path;
+(NSArray*)readToArrayWithFilePath:(NSString*)path;

+(BOOL)saveToFileWithDictionary:(NSDictionary*)dic filePath:(NSString*)path;
+(NSDictionary*)readToDictionaryWithFilePath:(NSString*)path;


//归档存储,配合basemodel一起使用
+ (BOOL)saveDataArchiverWithModel:(id)model filePath:(NSString*)path;
+ (id)readDataUnArchiverWithFilePath:(NSString*)path;


@end