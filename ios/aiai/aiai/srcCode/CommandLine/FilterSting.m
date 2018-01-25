//
//  FilterSting.m
//  ThirteenSeconds
//
//  Created by zhangyang on 14-11-5.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "FilterSting.h"

@implementation FilterSting

+(NSString*)mioveVoiceSetting:(CGFloat)volume{
    return [NSString stringWithFormat:@"volume=volume=%f",volume];
}

//视频混合到原有视频上
+(NSString*)blendMioveFilter:(NSString*)filterPath{
    return [NSString stringWithFormat:@"movie=%@,setsar=sar=0[watermark];[watermark][in]blend=all_expr='if(between(A,0,128),B,A)'",[SaveManager bundlePath:filterPath]];
}

+(NSString*)mergeMovieFilter:(NSString*)filterPath fromValue:(float)value movieTime:(int)movieTime{
    return [NSString stringWithFormat:@"movie=%@,setsar=sar=0,fps=fps=10,setpts=PTS+%1f/TB[w];[in]fps=fps=10[vin];[vin][w]blend=all_expr='if(lte(B,128),A,B)':enable=between(t\\,%1f\\,%d)",filterPath,value,value,movieTime];
}

+(NSString*)mergeMovieFilter:(NSString*)firstFilterPath secondFilter:(NSString*)secondFilterPath fromValue:(float)value movieTime:(int)movieTime{
    NSString *firstFilterString  = [NSString stringWithFormat:@"movie=%@,setsar=0[v0]",firstFilterPath];
    NSString *secondFilterString = [NSString stringWithFormat:@"movie=%@,setsar=0[v1]",secondFilterPath];
    
    return [NSString stringWithFormat:@"%@,%@;[in]fps=fps=10[vin];[v0][v1]alphamerge,fps=fps=10,setpts=PTS+%1f/TB[alpha];[vin][alpha]overlay=0:0:enable=between(t\\,%1f\\,%d)",firstFilterString,secondFilterString,value,value,movieTime];
}

//复古filter字符串
+(NSString*)retroFilterString{
    return [NSString stringWithFormat:@"curves=vintage"];
}
//胶片filter字符串
+(NSString*)filmFilterString{
    return [NSString stringWithFormat:@"curves=negative"];
}
//黑白filter字符串
+(NSString*)blackAndWhiteFilterString{
    return [NSString stringWithFormat:@"lutyuv='u=128:v=128'"];
}
//模糊filter字符串
+(NSString*)fuzzyFilterString{
    return [NSString stringWithFormat:@"boxblur=1:10:4:10"];
}
//迷幻filter字符串
+(NSString*)psychedelicFilterString{
    return [NSString stringWithFormat:@"hue='H=2*PI*t:s=sin(2*PI*t)+1'"];
}


@end
