//
//  GlobalDataInfo.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/5.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>

//常用的且需要保存的一些数据
#define USERNAME_KEY                        @"UserName"
#define USERPASSWORD_KEY                    @"UserPassword"
#define USER_ID_KEY                         @"userID"
#define USER_NICKNAME_KEY                   @"userNickName"
#define USER_FACEURL_KEY                    @"userFaceUrl"
#define USER_FACEVIDEO_KEY                  @"userFaceVedio"
#define USER_GENDER_KEY                     @"userGender"

#define USERTELPHONE_KEY                    @"UserTelPhone"
#define USEROPENPLATFORMCODE                @"openPlatformcode"
#define USEROPENPLATFORMID                  @"openPlatformID"

#define USER_LONGITUDE                      @"longitude"
#define USER_LATITUDE                       @"latitude"

#define USERFIRSTSTARTUP_KEY                @"UserFirstStartup_Key" //第一次启动App
#define NEEDSHOWTIPS                        @"ShowTips"         //是否需要显示tips
#define ISFOREGROUND_KEY                    @"isForeground" //程序是否在后台运行

#define IM_SAVEPATH                         [SaveManager libraryPath:@"IM_savePath"]
#define VEDIOCACHEPATH                      [SaveManager libraryPath:@"videoHeaderCache"]
#define VIDEO_FOLDER_NAME                   @"VideoFolder"

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
+(BOOL)saveToFileWithData:(NSData*)data filePath:(NSString*)path;
+(BOOL)saveToFileWithArray:(NSArray*)array filePath:(NSString*)path;
+(BOOL)saveToFileWithDictionary:(NSDictionary*)dic filePath:(NSString*)path;

//读取到数据中
+(NSString*)readToStringWithFilePath:(NSString*)path;
+(NSData*)readToDataWithFilePath:(NSString*)path;
+(NSArray*)readToArrayWithFilePath:(NSString*)path;
+(NSDictionary*)readToDictionaryWithFilePath:(NSString*)path;

//归档存储,配合basemodel一起使用
+ (BOOL)saveDataArchiverWithModel:(id)model filePath:(NSString*)path;
+ (id)readDataUnArchiverWithFilePath:(NSString*)path;

//创建vedio文件夹
+ (BOOL)createVideoFolderWithTmpDir;
+ (NSString *)getVideoMergeFilePathString;
+ (NSString *)getVideoSaveFilePathString;
+ (NSString *)getVideoFolderPathString;

@end