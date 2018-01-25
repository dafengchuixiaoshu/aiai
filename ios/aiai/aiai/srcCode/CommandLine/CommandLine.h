//
//  CommandLine.h
//  FFMpeg_CommandLineDemo
//
//  Created by zhangyang on 14-8-9.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CommandLine : NSObject

//release命令行返回的字符串
+(void)releaseCommandLineStr:(char**)commandLineStr size:(int)size;

//转换成原始编码
+(char**)ConversionYUVWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath;

//截取一张352x240尺寸大小的，格式为jpg的图片：
+(char**)GetFirstFrameThumbnailWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath;

//把视频的前３０帧转换成一个Animated Gif ：
+(char**)GetGifAnimatedWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath;

//在视频的第8.01秒处截取 320*240 的缩略图,//获取指定帧的缩略图
+(char**)GetSpecifyFrameWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath;

//把视频转换成图片
+(char**)GetAllFrameWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputDirPath;

//把图片转换成视频
+(char**)ImageConvertToVideoWithStringSize:(int*)commondSize inputAudioPath:(NSString*)audioFilePath inputImagePathDir:(NSString*)inputDir outputSrc:(NSString*)outputFilePath;

//添加滤镜指定时间从某个时间到某个时间
+(char**)AddFilterWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath filter:(NSString*)filterFilePath startTime:(int)startTime endTime:(int)endTime outputSrc:(NSString*)outputPath;

//mp4视频拼接
+(char**)concatMP4WithStringSize:(int*)commondSize inputSrc:(NSArray*)inputFileArray outputSrc:(NSString*)outputFilePath;

//多个视频拼接
+(char**)concatVideoWithStringSize:(int*)commondSize inputSrc:(NSArray*)inputFileArray outputSrc:(NSString*)outputFilePath;
//编码
+(char**)encodecWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath;

//多个视频和音频拼接
+(char**)moreConcatVideoWithStringSize:(int*)commondSize inputVideo:(NSArray*)inputVideoArray inputAudio:(NSArray*)inputAudioArray outputSrc:(NSString*)outputFilePath;
//原始音频和视频一起重新编码
+(char**)moreEncodecWithStringSize:(int*)commondSize inputVideo:(NSString*)inputVideo inputAudio:(NSString*)inputAudio outputSrc:(NSString*)outputFilePath;

//裁剪
+(char**)cutVideoStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath startTime:(NSString*)startTime endTime:(NSString*)endTime;

//剪切成需要的尺寸
+(char**)getVideoProvidedSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath;

//单视频叠加
+(char**)getVideoSuperpositionStingWithSize:(int*)commondSize inputSrc:(NSString*)inputFilePath filterSrc:(NSString*)filterFilePath outputSrc:(NSString*)outputFilePath;
//双视频叠加，原视频和遮罩
+(char**)getVideoSuperpositionStingWithSize:(int*)commondSize inputSrc:(NSString*)inputFilePath filter1:(NSString*)filter1 filter2:(NSString*)filter2 startTime:(NSString*)startTime endTime:(NSString*)endTime outputSrc:(NSString*)outputFilePath;

//往视频中插入图片
+(char**)blendVideoAndImagesWithSize:(int*)commondSize inputVideoPath:(NSString*)inputVideoPath waterMark:(NSString*)wmPath outputVideoPath:(NSString*)outputVideoPath;

//转换音频
+(char**)audioToaudioWithSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath;

//合并遮罩
+(char**)test1111:(int*)commondSize inputSrc:(NSString*)inputFilePath src:(NSString*)srcFilePath outputSrc:(NSString*)outputFilePath;
+(char**)test2222:(int*)commondSize inputSrc:(NSString*)inputFilePath src:(NSString*)srcFilePath outputSrc:(NSString*)outputFilePath;
@end
