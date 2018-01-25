//
//  FilterManager.m
//  aiai
//
//  Created by zhangyang on 15/6/4.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "FilterManager.h"
#import <IJKMediaPlayer/IJKMediaPlayer.h>

@implementation FilterManager
@synthesize filterImagePathArray = _filterImagePathArray;
@synthesize filterPathArray = _filterPathArray;
@synthesize filterTitleArray = _filterTitleArray;

@synthesize movieTitleArray = _movieTitleArray;
@synthesize movieImagePathArray = _movieImagePathArray;
@synthesize moviePathArray = _moviePathArray;

@synthesize preinstallEffectImagePathArray = _preinstallEffectImagePathArray;
@synthesize preinstallEffectPathArray = _preinstallEffectPathArray;
@synthesize preinstallEffectMaskPathArray = _preinstallEffectMaskPathArray;
@synthesize preinstallEffectTitleArray = _preinstallEffectTitleArray;

@synthesize effectImagePathArray = _effectImagePathArray;
@synthesize effectPathArray = _effectPathArray;
@synthesize effectMaskPathArray = _effectMaskPathArray;
@synthesize effectTitleArray = _effectTitleArray;


+(FilterManager*)share{
    static FilterManager *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

#pragma mark 资源读取接口
-(NSArray*)filterImagePathArray{
    if (_filterImagePathArray == nil) {
        _filterImagePathArray = [NSArray arrayWithObjects:
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_raw_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_bw_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_fuzzy_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_magic_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_nagative_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_vintage_Name,Preset_FilterImage_extern_Name]],nil];
    }
    return _filterImagePathArray;
}

-(NSArray*)filterPathArray{
    if (_filterPathArray == nil) {
        _filterPathArray = [NSArray arrayWithObjects:
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_raw_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_bw_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_fuzzy_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_magic_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_nagative_Name,Preset_FilterImage_extern_Name]],
                            [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_vintage_Name,Preset_FilterImage_extern_Name]],nil];
    }
    return _filterPathArray;
}

-(NSArray*)filterTitleArray{
    if (_filterTitleArray == nil) {
        _filterTitleArray = [NSArray arrayWithObjects:
                             Preset_FilterImage_raw_Name,
                             Preset_FilterImage_bw_Name,
                             Preset_FilterImage_fuzzy_Name,
                             Preset_FilterImage_magic_Name,
                             Preset_FilterImage_nagative_Name,
                             Preset_FilterImage_vintage_Name, nil];
    }
    return _filterTitleArray;
}

-(NSArray *)movieImagePathArray{
    
    if (_movieImagePathArray == nil) {
        
        _movieImagePathArray = [NSArray arrayWithObjects:
                                 [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_raw_Name,Preset_FilterImage_extern_Name]],
                                [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@.%@",Preset_FilterImage_Dir,Preset_FilterImage_bw_Name,Preset_FilterImage_extern_Name]],nil];
        
    }
    return _movieImagePathArray;
}

-(NSArray *)moviePathArray{
    
    if (_moviePathArray == nil) {
        
        _moviePathArray = [NSMutableArray arrayWithObjects:
                                           [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",MOVIE_RESOURCES_DIR,MOVIE_RESOURCES_0_PATH]],
                                           [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",MOVIE_RESOURCES_DIR,MOVIE_RESOURCES_1_PATH]],nil];
        
    }
    return _moviePathArray;
    
}

-(NSArray *)movieTitleArray{
    
    if (_movieTitleArray == nil) {
        
        _movieTitleArray = [NSArray arrayWithObjects:
                             Preset_FilterImage_raw_Name,
                            Preset_FilterImage_bw_Name,nil];
        
    }
    return _movieTitleArray;
    
}

-(NSArray*)preinstallEffectImagePathArray{
    if (_preinstallEffectImagePathArray == nil) {
        _preinstallEffectImagePathArray = [NSMutableArray arrayWithObjects:
                                      [SaveManager bundlePath:@"selectFilter_none.png"],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_1_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_2_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_3_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_4_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_5_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_6_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_7_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_8_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_9_Path,Preset_Resources_Pic_Name]],
                                      [SaveManager bundlePath:@"selectFilter_add.png"],nil];
    }
    return _preinstallEffectImagePathArray;
}

-(NSArray*)preinstallEffectPathArray{
    if (_preinstallEffectPathArray == nil) {
        _preinstallEffectPathArray = [NSMutableArray arrayWithObjects:
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_0_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_1_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_2_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_3_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_4_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_5_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_6_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_7_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_8_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_9_Path,Preset_Resources_Name]],
                                      [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_10_Path,Preset_Resources_Name]],nil];
    }
    return _preinstallEffectPathArray;
}

-(NSArray*)preinstallEffectMaskPathArray{
    if (_preinstallEffectMaskPathArray == nil) {
        _preinstallEffectMaskPathArray = [NSMutableArray arrayWithObjects:
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_0_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_1_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_2_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_3_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_4_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_5_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_6_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_7_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_8_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_9_Path,Preset_Resources_Mask_Name]],
                                             [SaveManager bundlePath:[NSString stringWithFormat:@"%@/%@",Preset_Resources_10_Path,Preset_Resources_Mask_Name]],nil];
    }
    return _preinstallEffectMaskPathArray;
}

-(NSArray*)preinstallEffectTitleArray{
    if (_preinstallEffectTitleArray == nil) {
        _preinstallEffectTitleArray = [NSMutableArray arrayWithObjects:
                                       [NSString stringWithFormat:@"%@",@"无"],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_1_Path],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_2_Path],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_3_Path],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_4_Path],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_5_Path],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_6_Path],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_7_Path],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_8_Path],
                                       [NSString stringWithFormat:@"%@",Preset_Resources_9_Path],
                                       [NSString stringWithFormat:@"%@",@"添加新特效"],nil];
    }
    return _preinstallEffectTitleArray;
}

-(NSArray*)effectImagePathArray{
    NSArray* downloadDirName = [FilterManager readEffectsCacheWithDownloadPath:FILTERCACHE];
    if ([downloadDirName count] + [self.preinstallEffectImagePathArray count] + 1 > [_effectImagePathArray count]) {
        NSMutableArray* newArray = [[NSMutableArray alloc] initWithArray:self.preinstallEffectImagePathArray];
        for (NSString* oneName in downloadDirName) {
            NSString* path = [NSString stringWithFormat:@"%@/%@/%@",FILTERCACHE,oneName,Preset_Resources_Pic_Name];
            NSInteger insertIndex = [newArray count]-1;
            [newArray insertObject:path atIndex:insertIndex];
        }
        
        _effectImagePathArray = newArray;
    }
    return _effectImagePathArray;
}

-(NSArray*)effectPathArray{
    NSArray* downloadDirName = [FilterManager readEffectsCacheWithDownloadPath:FILTERCACHE];
    if ([downloadDirName count] + [self.preinstallEffectPathArray count] > [_effectPathArray count]) {
        NSMutableArray* newArray = [[NSMutableArray alloc] initWithArray:self.preinstallEffectPathArray];
        for (NSString* oneName in downloadDirName) {
            NSString* path = [NSString stringWithFormat:@"%@/%@/%@",FILTERCACHE,oneName,Preset_Resources_Name];
            NSInteger insertIndex = [newArray count]-1;
            [newArray insertObject:path atIndex:insertIndex];
        }
        _effectPathArray = newArray;
    }
    return _effectPathArray;
}

-(NSArray*)effectMaskPathArray{
    NSArray* downloadDirName = [FilterManager readEffectsCacheWithDownloadPath:FILTERCACHE];
    if ([downloadDirName count] + [self.preinstallEffectMaskPathArray count] > [_effectMaskPathArray count]) {
        NSMutableArray* newArray = [[NSMutableArray alloc] initWithArray:self.preinstallEffectMaskPathArray];
        for (NSString* oneName in downloadDirName) {
            NSString* path = [NSString stringWithFormat:@"%@/%@/%@",FILTERCACHE,oneName,Preset_Resources_Mask_Name];
            NSInteger insertIndex = [newArray count]-1;
            [newArray insertObject:path atIndex:insertIndex];
        }
        
        _effectMaskPathArray = newArray;
    }
    return _effectMaskPathArray;
}

-(NSArray*)effectTitleArray{
    
    NSArray* downloadDirName = [FilterManager readEffectsCacheWithDownloadPath:FILTERCACHE];
    if ([downloadDirName count] + [self.preinstallEffectTitleArray count] + 1 > [_effectTitleArray count]) {
        NSMutableArray* newArray = [[NSMutableArray alloc] initWithArray:self.preinstallEffectTitleArray];
        id obj = [newArray lastObject];
        [newArray removeLastObject];
        [newArray addObjectsFromArray:downloadDirName];
        [newArray addObject:obj];
        
        _effectTitleArray = newArray;
    }
    return _effectTitleArray;
}

+(NSArray*)readEffectsCacheWithDownloadPath:(NSString*)path{
    NSFileManager* fileManager = [NSFileManager defaultManager];
    BOOL isDir;
    BOOL isSuc = [fileManager fileExistsAtPath:path isDirectory:&isDir];
    if (isSuc && isDir) {
        NSArray *contentOfFolder = [fileManager contentsOfDirectoryAtPath:FILTERCACHE error:NULL];
        return contentOfFolder;
    }
    return nil;
}

#pragma mark public mothed
+(void)catonFilterPathWithScrollImageViewArray:(NSArray*)images srcVedioPath:(NSURL*)srcPath prepare:(void (^)(void))prepare complate:(void (^)(NSURL *outputPath))complate{
    
    prepare();
    dispatch_queue_t mergeImageToVedioQueue = dispatch_queue_create("mergeImageToVedioQueue", NULL);
    dispatch_async(mergeImageToVedioQueue, ^{
        //在当前文件夹下创建新的临时文件夹来存放重命名的图片文件
        NSString* creatNewDir = [NSTemporaryDirectory() stringByAppendingPathComponent:@"tmpOutVedioDic"];
        NSFileManager *temFM = [NSFileManager defaultManager];
        if ([temFM fileExistsAtPath:creatNewDir]){
            [temFM removeItemAtPath:creatNewDir error:nil];
        }
        BOOL createOutputResult = [temFM createDirectoryAtPath:creatNewDir withIntermediateDirectories:YES attributes:nil error:nil];
        if (createOutputResult == NO) {
            AIAI_LOG(@"creatNewDir faild");
        }
        
        //把所有选择的图片全部按规则拷贝到指定的文件夹中
        for (int i = 0;i < [images count];i++) {
            NSString* tmpoutputPath = [NSString stringWithFormat:@"%@/img%03d.jpg",creatNewDir,i+1];//图片命名需要从1开始
            if([temFM copyItemAtPath:[images objectAtIndex:i] toPath:tmpoutputPath error:nil]){
                AIAI_LOG(@"copy success");
            }
        }
        
        NSString* outputPath = [SaveManager tmpPath:@"tmpKadun.mp4"];
        int size = 0;
        NSString* audioPath = [srcPath relativeString];
        char** commandLinestr = [CommandLine ImageConvertToVideoWithStringSize:&size inputAudioPath:audioPath inputImagePathDir:creatNewDir outputSrc:outputPath];
        ffmpegMain(size,commandLinestr);
        [CommandLine releaseCommandLineStr:commandLinestr size:size];
        
        complate([NSURL fileURLWithPath:outputPath]);
    });
}

//视频叠加
+(void)videoSuperpositionWithMoviePath:(NSURL*)moviePath movieTime:(CGFloat)time index:(NSInteger)index prepare:(void (^)(void))prepare complate:(void (^)(NSURL *outputPath))complate{
    
    prepare();
    dispatch_queue_t mergeImageToVedioQueue = dispatch_queue_create("mergeImageToVedioQueue", NULL);
    dispatch_async(mergeImageToVedioQueue, ^{
        if (index == 0) {
            complate(moviePath);
            return;
        }
        NSString* filter1Path = [[FilterManager share].effectPathArray objectAtIndex:index];
        NSString* filter2Path = [[FilterManager share].effectMaskPathArray objectAtIndex:index];
        
        NSString* saveFilePath =[SaveManager documentsPath:@"sendVedio.mp4"];
        int size = 0;
        char** commandLinestr = [CommandLine getVideoSuperpositionStingWithSize:&size inputSrc:[moviePath absoluteString] filter1:filter1Path filter2:filter2Path startTime:@"0" endTime:[NSString stringWithFormat:@"%.1f",time] outputSrc:saveFilePath];
        ffmpegMain(size,commandLinestr);
        [CommandLine releaseCommandLineStr:commandLinestr size:size];
        
        complate([NSURL fileURLWithPath:saveFilePath]);
    });
}

+(void)videoAddFilterWithMoviePath:(NSURL*)moviePath movieTime:(CGFloat)time index:(NSInteger)index prepare:(void (^)(void))prepare complate:(void (^)(NSURL *outputPath))complate{
    
    prepare();
    dispatch_queue_t mergeImageToVedioQueue = dispatch_queue_create("mergeImageToVedioQueue", NULL);
    dispatch_async(mergeImageToVedioQueue, ^{
        if(index == Filter_Type_Raw){
            complate(moviePath);
            return;
        }
        NSString* filterString = nil;
        switch (index) {
            case Filter_Type_BW:
            {
                filterString = @"lutyuv='u=128:v=128'";
            }
                break;
            case Filter_Type_Fuzzy:
            {
                filterString = @"boxblur=1:10:4:10";
            }
                break;
            case Filter_Type_Magic:
            {
                filterString = @"hue='H=2*PI*t:s=sin(2*PI*t)+1'";
            }
                break;
            case Filter_Type_Nagative:
            {
                filterString = @"curves=negative";
            }
                break;
            case Filter_Type_Vintage:
            {
                filterString = @"curves=vintage";
            }
                break;
            default:
                break;
        }
        
        NSString* saveFilePath =[SaveManager documentsPath:@"sendVedio.mp4"];
        int size = 0;
        char** commandLinestr = [CommandLine AddFilterWithStringSize:&size inputSrc:[moviePath absoluteString] filter:filterString startTime:0 endTime:time outputSrc:saveFilePath];
        ffmpegMain(size,commandLinestr);
        [CommandLine releaseCommandLineStr:commandLinestr size:size];
        
        complate([NSURL fileURLWithPath:saveFilePath]);
    });
}


@end
