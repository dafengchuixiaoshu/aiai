//
//  FilterSting.h
//  ThirteenSeconds
//
//  Created by zhangyang on 14-11-5.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FilterSting : NSObject

//设置音量
+(NSString*)mioveVoiceSetting:(CGFloat)volume;

//视频混合到原有视频上
+(NSString*)blendMioveFilter:(NSString*)filterPath;

//融合1个透明视频或者黑白2个视频达到特效效果
+(NSString*)mergeMovieFilter:(NSString*)filterPath fromValue:(float)value movieTime:(int)movieTime;
+(NSString*)mergeMovieFilter:(NSString*)firstFilterPath secondFilter:(NSString*)secondFilterPath fromValue:(float)value movieTime:(int)movieTime;

//复古filter字符串
+(NSString*)retroFilterString;

//胶片filter字符串
+(NSString*)filmFilterString;

//黑白filter字符串
+(NSString*)blackAndWhiteFilterString;

//模糊filter字符串
+(NSString*)fuzzyFilterString;

//迷幻filter字符串
+(NSString*)psychedelicFilterString;

@end
