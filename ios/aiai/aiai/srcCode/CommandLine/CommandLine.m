//
//  CommandLine.m
//  FFMpeg_CommandLineDemo
//
//  Created by zhangyang on 14-8-9.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

//const char *filter_descr3 = "drawbox=x=10:y=20:w=200:h=60:color=red@0.5";//能用

const char *filter_descr4 = "fade=in:0:30";
const char *filter_descr5 = "hue='s=min(t/2\\,1)'";

//const char *filter_descr7 = "colorbalance=rs=.7";//色彩平衡
//const char *filter_descr7 = "colorchannelmixer=.3:.4:.3:0:.3:.4:.3:0:.3:.4:.3";//色彩通道混合，可以变黑白等自定义颜色
//const char *filter_descr7 = "crop=w=300:h=300:x=10:y=20";//剪裁视频，不能等比例
//const char *filter_descr7 = "curves=blue='0.1/0.58'";//曲线,变种那个颜色的比例？效果和色彩通道混合差不多
//const char *filter_descr7 = "drawgrid=width=100:height=100:thickness=2:color=red@0.5";//绘制网格
//const char *filter_descr7 = "edgedetect=low=0.1:high=0.4";//边缘检测，描边显示
//const char *filter_descr7 = "fade=in:0:30";//褪色,先黑再白
//const char *filter_descr7 = "fieldmatch=order=tff:combmatch=full, yadif=deint=interlaced, decimate";//不明白
//const char *filter_descr7 = "format=pix_fmts=yuv420p";//转换格式?
//const char *filter_descr7 = "fps=fps=15";//"fps=fps=film:round=near";//改变帧数
//const char *filter_descr7 = "frei0r=filter_name=distort0r:filter_params=0.5|0.01";//没有
//const char *filter_descr7 = "geq=128 + 100*sin(2*(PI/100)*(cos(PI/3)*(X-50*T) + sin(PI/3)*Y)):128:128";//没有
//const char *filter_descr7 = /*"gradfun=3.5:8";*/"gradfun=radius=8";//没看出效果
//const char *filter_descr7 = "hue=h=90:s=1";//色调
//const char *filter_descr7 = "kerndeint=thresh=10:map=0:order=0:sharp=0:twoway=0";//没有
//const char *filter_descr7 = "noformat=pix_fmts=yuv420p,vflip";//Force libavfilter not to use any of the specified pixel formats for the input to the next filter.
//const char *filter_descr7 = "noise=alls=20:allf=t+u";//Add noise on video input frame.增加噪音
//const char *filter_descr7 = "color=color=red@.3:size=WxH [over]; [in][over] overlay [out]";//overlay不能用,命令貌似有问题
//const char *filter_descr7 = "rotate=PI/6";//转换角度
//const char *filter_descr7 = "scale=w=200:h=100";//变模糊了
//const char *filter_descr7 = "setdar=dar=16/9";//setsar=sar=10/11;//改变宽高比？
//const char *filter_descr7 = "vignette=PI/4";//偷拍效果

#import "CommandLine.h"

@implementation CommandLine

+(void)releaseCommandLineStr:(char**)commandLineStr size:(int)size{
    for (int i = 0; i<size; i++) {
        free(commandLineStr[i]);
    }
    free(commandLineStr);
}

//转换成原始编码
+(char**)ConversionYUVWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath{
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-i",inputFilePath,@"-f",@"rawvideo",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//截取一张352x240尺寸大小的，格式为jpg的图片：
//"ffmpeg.exe -i starwar.avi -f image2 -ss 10 -s 400*300 -vframes 1 1.jpg";
//ffmpeg -i test.asf -y -f image2 -t 0.001 -s 352x240 a.jpg
+(char**)GetFirstFrameThumbnailWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath{
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-i",inputFilePath,@"-y",@"-f",@"image2",@"-t",@"0.001",@"-s",@"480x480",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//把视频的前３０帧转换成一个Animated Gif ：
//ffmpeg -i test.asf -vframes 30 -y -f gif a.gif
+(char**)GetGifAnimatedWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath{
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-i",inputFilePath,@"-vframes",@"30",@"-y",@"-f",@"gif",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//在视频的第8.01秒处截取 320*240 的缩略图,//获取指定帧的缩略图
//ffmpeg -i test.flv -y -f mjpeg -ss 3 -t 0.001 -s 320x240 test.jpg
+(char**)GetSpecifyFrameWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath{
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-i",inputFilePath,@"-y",@"-f",@"mjpeg",@"-ss",@"3",@"-t",@"0.001",@"-s",@"480x480",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

+(char**)GetAllFrameWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputDirPath{

    NSString* tmpOutDir = [outputDirPath stringByAppendingPathComponent:@"img%3d.jpg"];
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-i",inputFilePath,@"-y",@"-f",@"image2",@"-qscale",@"1",@"-r",@"6",tmpOutDir, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//图片转换成视频
+(char**)ImageConvertToVideoWithStringSize:(int*)commondSize inputAudioPath:(NSString*)audioFilePath inputImagePathDir:(NSString*)inputDir outputSrc:(NSString*)outputFilePath{
//ffmpeg,-y,-framerate,1/%s,-i,img%d.jpg-i,sound.mp3,-filter_complex,[1:a]volume=volume=0.01,-r,6,-t,%s,tmp.mp4
    
    NSArray* tmpPhotoArray = nil;
    NSFileManager *temFM = [NSFileManager defaultManager];//创建文件管理器
    if ([temFM fileExistsAtPath:inputDir]){
        tmpPhotoArray = [temFM subpathsAtPath:inputDir];
    }
    CGFloat oneFramePlayTime = 0.5;//一帧持续播放时间
    NSString* oneFrameTimeString = [NSString stringWithFormat:@"1/%f",oneFramePlayTime];
    NSString* allFrameTimeString = [NSString stringWithFormat:@"%f",[tmpPhotoArray count]*oneFramePlayTime];
    NSString* tmpInputDir = [inputDir stringByAppendingPathComponent:@"img%03d.jpg"];

    NSArray* commandLineArray = nil;
    if (audioFilePath) {
        commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y",@"-framerate",oneFrameTimeString,@"-i",tmpInputDir,@"-i",audioFilePath,@"-filter_complex",@"[1:a]volume=volume=0.01",@"-r",@"6",@"-t",allFrameTimeString,outputFilePath, nil];
    }else{
        commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y",@"-framerate",oneFrameTimeString,@"-i",tmpInputDir,@"-filter_complex",@"-r",@"-t",allFrameTimeString,outputFilePath, nil];
    }
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//添加滤镜指定时间从某个时间到某个时间
+(char**)AddFilterWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath filter:(NSString*)filterFilePath startTime:(int)startTime endTime:(int)endTime outputSrc:(NSString*)outputPath{
  
    NSString* filter_descrString = [NSString stringWithFormat:@"%@",filterFilePath];
    
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y",@"-i",inputFilePath,@"-vf",filter_descrString ,@"-c:v",@"libx264",@"-profile:v", @"baseline", @"-preset", @"ultrafast", @"-c:a", @"libfaac" ,@"-ar", @"44100", @"-f", @"mp4", @"-movflags", @"faststart",outputPath, nil];
    
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//mp4视频拼接
+(char**)concatMP4WithStringSize:(int*)commondSize inputSrc:(NSArray*)inputFileArray outputSrc:(NSString*)outputFilePath{
    
    NSMutableArray* commandLineArray = [NSMutableArray arrayWithObjects:@"ffmpeg",@"-y",nil];
    
    if (inputFileArray && [inputFileArray count]==1) {
        NSString* inputfile = [inputFileArray objectAtIndex:0];
        
        [commandLineArray addObject:@"-i"];
        [commandLineArray addObject:inputfile];
        [commandLineArray addObject:@"-c:v"];
        [commandLineArray addObject:@"libx264"];
        [commandLineArray addObject:@"-profile:v"];
        [commandLineArray addObject:@"baseline"];
        
        [commandLineArray addObject:@"-preset"];
        [commandLineArray addObject:@"ultrafast"];
        [commandLineArray addObject:outputFilePath];
    }else{
        NSString* concatString = [NSString stringWithFormat:@"concat=n=%zi:v=1:a=1",[inputFileArray count]];
        
        for (NSString* inputfile in inputFileArray) {
            [commandLineArray addObject:@"-i"];
            [commandLineArray addObject:inputfile];
        }
        [commandLineArray addObject:@"-filter_complex"];
        [commandLineArray addObject:concatString];
        
        [commandLineArray addObject:@"-c:v"];
        [commandLineArray addObject:@"libx264"];
        [commandLineArray addObject:@"-profile:v"];
        [commandLineArray addObject:@"baseline"];

        [commandLineArray addObject:@"-preset"];
        [commandLineArray addObject:@"ultrafast"];
        [commandLineArray addObject:outputFilePath];
    }
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//多个视频拼接
+(char**)concatVideoWithStringSize:(int*)commondSize inputSrc:(NSArray*)inputFileArray outputSrc:(NSString*)outputFilePath{
    
    if ([inputFileArray count]>1) {
        NSMutableString* concatStr = [[NSMutableString alloc] initWithString:@"concat:"];
        for (int i = 0;i<[inputFileArray count];i++) {
            NSString* oneFilePath = [inputFileArray objectAtIndex:i];
            [concatStr appendString:oneFilePath];
            if (i < [inputFileArray count]-1) {
                [concatStr appendString:@"|"];
            }
        }
        return [CommandLine encodecWithStringSize:commondSize inputSrc:concatStr outputSrc:outputFilePath];
    }else if([inputFileArray count] == 1){
        NSString* outputFile = [inputFileArray objectAtIndex:0];
        return [CommandLine encodecWithStringSize:commondSize inputSrc:outputFile outputSrc:outputFilePath];
    }
    
    return NULL;
}
//编码
+(char**)encodecWithStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath{

    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y",@"-s",@"480x480",@"-i",inputFilePath,@"-c:v",@"libx264",@"-profile:v", @"baseline", @"-preset", @"ultrafast", @"-c:a", @"libfaac" ,@"-ar", @"44100", @"-f", @"mp4", @"-movflags", @"faststart",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//多个视频和音频拼接
+(char**)moreConcatVideoWithStringSize:(int*)commondSize inputVideo:(NSArray*)inputVideoArray inputAudio:(NSArray*)inputAudioArray outputSrc:(NSString*)outputFilePath{
    
    if ([inputVideoArray count]>1) {
        NSMutableString* VideoConcatStr = [[NSMutableString alloc] initWithString:@"concat:"];
        NSMutableString* AudioConcatStr = [[NSMutableString alloc] initWithString:@"concat:"];
        for (int i = 0;i<[inputVideoArray count];i++) {
            NSString* oneVideoPath = [inputVideoArray objectAtIndex:i];
            NSString* oneAudioPath = [inputAudioArray objectAtIndex:i];
            [VideoConcatStr appendString:oneVideoPath];
            [AudioConcatStr appendString:oneAudioPath];
            if (i < [inputVideoArray count]-1) {
                [VideoConcatStr appendString:@"|"];
                [AudioConcatStr appendString:@"|"];
            }
        }
        return [CommandLine moreEncodecWithStringSize:commondSize inputVideo:VideoConcatStr inputAudio:AudioConcatStr outputSrc:outputFilePath];
    }else if([inputVideoArray count] == 1){
        NSString* oneVideoPath = [inputVideoArray objectAtIndex:0];
        NSString* oneAudioPath = [inputAudioArray objectAtIndex:0];
        return [CommandLine moreEncodecWithStringSize:commondSize inputVideo:oneVideoPath inputAudio:oneAudioPath outputSrc:outputFilePath];
    }
    
    return NULL;
}

//把视频和音频一起编码编码
+(char**)moreEncodecWithStringSize:(int*)commondSize inputVideo:(NSString*)inputVideo inputAudio:(NSString*)inputAudio outputSrc:(NSString*)outputFilePath{
    
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y",@"-s",@"480x480",@"-i",inputVideo,@"-f",@"s16le",@"-ar",@"44100",@"-ac",@"1",@"-i", inputAudio,@"-c:v",@"libx264",@"-profile:v", @"baseline", @"-preset", @"ultrafast", @"-c:a", @"libfaac" ,@"-ar", @"44100", @"-f", @"mp4", @"-movflags", @"faststart",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//裁剪
+(char**)cutVideoStringSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath startTime:(NSString*)startTime endTime:(NSString*)endTime{
   
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-ss",startTime,@"-i",inputFilePath ,@"-t",endTime,@"-acodec",@"copy",@"-vcodec",@"copy",inputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//剪切成需要的尺寸
+(char**)getVideoProvidedSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath{
    
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y",@"-i",inputFilePath ,@"-vf",@"crop=out_w=in_h,fps=fps=15,scale=480:480,transpose=1"/*,"-b","200k"*/,outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//视频叠加
+(char**)getVideoSuperpositionStingWithSize:(int*)commondSize inputSrc:(NSString*)inputFilePath filter1:(NSString*)filter1 filter2:(NSString*)filter2 startTime:(NSString*)startTime endTime:(NSString*)endTime outputSrc:(NSString*)outputFilePath{

//    movie=%s[wm],movie=%s,setsar=0[v0],movie=%s,setsar=0[v1];[in]fps=fps=%d[vin];[v0][v1]alphamerge,fps=fps=%d,setpts=PTS+%.1f/TB[alpha];[vin][alpha]overlay=0:0:enable=between(t\\,%.1f\\,%.1f)[ef];[ef][wm]overlay=10:10";
    
    NSString *afilter = [NSString stringWithFormat:@"amovie=%@,volume=volume=%d,adelay=%.1f|%.1f[w];[in]volume=volume=%d[ain];[ain][w]amix",filter1,1,1.0,1.0,1];
//    NSString *filter_descr0 = @"movie=%s[wm],";//这里可以叠加水印
    NSString *filter_descr1 = @"movie=";
    NSString *filter_descr2 = @",setsar=0[v0];movie=";
    NSString *filter_descr3 = @",setsar=0[v1];[in]fps=fps=10[vin];[v0][v1]alphamerge,fps=fps=10,setpts=PTS+0/TB[alpha];[vin][alpha]overlay=0:0:enable=";
    NSString *filter_descr4 = [NSString stringWithFormat:@"between(t\\,%@\\,%@)",startTime,endTime];
//    NSString *filter_descr5 = @"[ef];[ef][wm]overlay=10:10";
    NSString* filter_descrString = [NSString stringWithFormat:@"%@%@%@%@%@%@",filter_descr1,filter1,filter_descr2,filter2,filter_descr3,filter_descr4];
    
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y",@"-i",inputFilePath,@"-vf",filter_descrString , @"-af",afilter,@"-c:v",@"libx264",@"-profile:v", @"baseline", @"-preset", @"ultrafast", @"-c:a", @"libfaac" ,@"-ar", @"44100", @"-f", @"mp4", @"-movflags", @"faststart",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

+(char**)blendVideoAndImagesWithSize:(int*)commondSize inputVideoPath:(NSString*)inputVideoPath waterMark:(NSString*)wmPath outputVideoPath:(NSString*)outputVideoPath{
    
    //    ffmpeg -i haha.mp4 -vf 'movie=waitAlter.png [logo];[in][logo]overlay=0:0[out]' test.mp4
    
    NSMutableArray* commandLineArray = [[NSMutableArray alloc] init];
    [commandLineArray addObject:@"ffmpeg"];
    [commandLineArray addObject:@"-i"];
    [commandLineArray addObject:inputVideoPath];
    [commandLineArray addObject:@"-vf"];
    
    NSString* filterString = [NSString stringWithFormat:@"movie=%@ [logo];[in][logo]overlay=0:0[out]",wmPath];
    [commandLineArray addObject:filterString];
    [commandLineArray addObject:outputVideoPath];
    
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//视频叠加
+(char**)getVideoSuperpositionStingWithSize:(int*)commondSize inputSrc:(NSString*)inputFilePath filterSrc:(NSString*)filterFilePath outputSrc:(NSString*)outputFilePath{
    NSString *filter_descr = @"movie=";
    NSString *filter_descr2 = @",setsar=sar=0[watermark];[watermark][in]blend=all_expr='if(between(A,0,128),B,A)'";
    NSString* filter_descrString = [NSString stringWithFormat:@"%@%@%@",filter_descr,filterFilePath,filter_descr2];
    
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y",@"-i",inputFilePath,@"-vf",filter_descrString ,@"-c:v",@"libx264",@"-profile:v", @"baseline", @"-preset", @"ultrafast", @"-c:a", @"libfaac" ,@"-ar", @"44100", @"-f", @"mp4", @"-movflags", @"faststart",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

//转换成原始音频
+(char**)audioToaudioWithSize:(int*)commondSize inputSrc:(NSString*)inputFilePath outputSrc:(NSString*)outputFilePath{
    
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-i",inputFilePath , @"-vn",@"-acodec",@"pcm_s16le",@"-f",@"s16le",@"-ar",@"44100",@"-ac",@"1",outputFilePath, nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

+(char**)test1111:(int*)commondSize inputSrc:(NSString*)inputFilePath src:(NSString*)srcFilePath outputSrc:(NSString*)outputFilePath{
    NSString* comond = @"\"[1:0] setsar=sar=1,format=rgba [1sared]; [0:0]format=rgba [0rgbd]; [0rgbd][1sared]blend=all_mode='addition':repeatlast=1:all_opacity=1,format=yuva422p10le\"";
    
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y" ,@"-i" ,inputFilePath, @"-i", srcFilePath, @"-filter_complex",comond ,@"-map", @"1:a", @"-c:v", @"libx264", @"-preset", @"slow" ,@"-tune", @"film -pix_fmt" ,@"yuv420p" ,outputFilePath ,nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

+(char**)test2222:(int*)commondSize inputSrc:(NSString*)inputFilePath src:(NSString*)srcFilePath outputSrc:(NSString*)outputFilePath{
    
    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y" ,@"-i" ,inputFilePath,@"-i", srcFilePath,@"-filter_complex",@"[0:0]setsar=sar=0,format=rgba[v0];[1:0]format=rgba[v1];[v0][v1]blend=all_mode=addition:all_opacity=1,format=rgba",@"-pix_fmt",@"yuv420p",outputFilePath ,nil];
    
//    NSArray* commandLineArray = [NSArray arrayWithObjects:@"ffmpeg",@"-y" ,@"-i" ,inputFilePath, @"-i", srcFilePath, @"-filter_complex" ,@"blend=all_mode=overlay",outputFilePath ,nil];
    *commondSize = (int)[commandLineArray count];
    
    char** array = (char**)malloc(sizeof(char*)*[commandLineArray count]);
    memset(array, 0, sizeof(char*)*[commandLineArray count]);
    for (int i = 0; i<[commandLineArray count]; i++) {
        NSString* oneString = [commandLineArray objectAtIndex:i];
        array[i] = (char*)malloc([oneString length]+1);
        memset(array[i], 0, [oneString length]+1);
        strcpy(array[i], [oneString UTF8String]);
    }
    return array;
}

@end
