//
//  FilterManager.h
//  aiai
//
//  Created by zhangyang on 15/6/4.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

//素材名称
#define Preset_Resources_Name       @"f.mp4"
#define Preset_Resources_Mask_Name  @"f_.mp4"
#define Preset_Resources_Pic_Name   @"pic.jpg"

//预置素材名称
#define Preset_Resources_0_Path     @"无"
#define Preset_Resources_1_Path     @"fart"
#define Preset_Resources_2_Path     @"gloryfist"
#define Preset_Resources_3_Path     @"bazooka"
#define Preset_Resources_4_Path     @"money"
#define Preset_Resources_5_Path     @"fireball"
#define Preset_Resources_6_Path     @"ghost"
#define Preset_Resources_7_Path     @"devil"
#define Preset_Resources_8_Path     @"pulp"
#define Preset_Resources_9_Path     @"rose"
#define Preset_Resources_10_Path    @"添加新特效"

//片头片尾
#define MOVIE_RESOURCES_DIR         @"movie"
#define MOVIE_RESOURCES_0_PATH      @"0.mp4"
#define MOVIE_RESOURCES_1_PATH      @"1.mp4"

//滤镜资源文件
#define Preset_FilterImage_Dir              @"filterImage"

#define Preset_FilterImage_raw_Name         @"raw"
#define Preset_FilterImage_bw_Name          @"bw"
#define Preset_FilterImage_fuzzy_Name       @"fuzzy"
#define Preset_FilterImage_magic_Name       @"magic"
#define Preset_FilterImage_nagative_Name    @"nagative"
#define Preset_FilterImage_vintage_Name     @"vintage"

#define Preset_FilterImage_extern_Name      @"jpg"

@interface FilterManager : NSObject

+(FilterManager*)share;

@property(nonatomic,strong,readonly)NSArray* filterImagePathArray;//滤镜数组
@property(nonatomic,strong,readonly)NSArray* filterPathArray;//滤镜数组
@property(nonatomic,strong,readonly)NSArray* filterTitleArray;//滤镜名称数组

@property(nonatomic,strong,readonly)NSArray* movieImagePathArray;//电影编辑图片数组
@property(nonatomic,strong,readonly)NSArray* moviePathArray;//电影编辑路径数组
@property(nonatomic,strong,readonly)NSArray* movieTitleArray;//电影编辑名称数组

@property(nonatomic,strong,readonly)NSArray* preinstallEffectImagePathArray;//预设特效图片数组
@property(nonatomic,strong,readonly)NSArray* preinstallEffectPathArray;//预设特效数组
@property(nonatomic,strong,readonly)NSArray* preinstallEffectMaskPathArray;//预设特效遮罩数组
@property(nonatomic,strong,readonly)NSArray* preinstallEffectTitleArray;

@property(nonatomic,strong,readonly)NSArray* effectImagePathArray;//特效数组
@property(nonatomic,strong,readonly)NSArray* effectPathArray;//特效数组
@property(nonatomic,strong,readonly)NSArray* effectMaskPathArray;//特效遮罩数组
@property(nonatomic,strong,readonly)NSArray* effectTitleArray;//特效名称数组

//卡顿
+(void)catonFilterPathWithScrollImageViewArray:(NSArray*)images srcVedioPath:(NSURL*)srcPath prepare:(void (^)(void))prepare complate:(void (^)(NSURL *outputPath))complate;
//视频叠加
+(void)videoSuperpositionWithMoviePath:(NSURL*)moviePath movieTime:(CGFloat)time index:(NSInteger)index prepare:(void (^)(void))prepare complate:(void (^)(NSURL *outputPath))complate;
//滤镜
+(void)videoAddFilterWithMoviePath:(NSURL*)moviePath movieTime:(CGFloat)time index:(NSInteger)index prepare:(void (^)(void))prepare complate:(void (^)(NSURL *outputPath))complate;
    

@end
