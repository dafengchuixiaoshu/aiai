//
//  AppUtils.h
//  F2CPlatform
//
//  Created by zhangyang on 14/12/1.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface AppUtils : NSObject

//base64编码解码
+ (NSString*) base64StringDecode:(NSString *)string;
+ (NSString*) base64StringEncode:(NSString *)string;

//md5加密
+(NSString *) md5: (NSString *) inPutText;

//颜色转image
+ (UIImage *)imageWithColor:(UIColor *)color rect:(CGRect)rect;

//验证手机号是否合法
+ (BOOL)checkPhoneNumber:(NSString *)phoneNumber;
//检测邮箱是否合法
+(BOOL)checkMailbox:(NSString*)str;

// 处理由iPhone摄像头拍摄的图片，上传至服务器后，造成其它设备查看时图片显示方向不正确的问题
+ (UIImage *)fixOrientation:(UIImage*)srcImage;

//获取系统版本号
+(CGFloat)SystemVertion;

//获取设备唯一标识
+ (NSString*)uuid;

//获取设备语言
+ (NSString*)language;

+(void)closeKeyBoard;

//判断是否是系统表情
+ (BOOL)stringContainsEmoji:(NSString *)string;

//把图片接口添加宽和高，以保证得到最合适的图片大小
+ (NSString*)getImageNewUrlWithSrcImageUrl:(NSString*)srcImageUrl width:(CGFloat)width height:(CGFloat)height;

//时间解析
+ (NSString *)getTime:(id)time;

//获取表情字符字典
+ (NSDictionary*)faceNameDictionary;

//获取富文本中表情的位置
+(NSRange)getFaceRange:(NSString *)text andLocation:(NSUInteger)location;

//把符号表情替换成文字
+ (NSString*)replaceExpression:(NSString*)srcContent;

//去除字符串末尾空格
+(NSString *)trimmingEndSpace:(NSString *)text;

//释放所有子控件
+(void)releaseSubView:(UIView *)view NoReleaseView:(UIView *)noRView;

//判断字符串是否为空
-(BOOL)isEmptyWithString:(NSString*)string;

//视图转换图片
+(UIImage*) imageWithUIView:(UIView*) view;

//获取视频第一帧图片
+(UIImage*)firstFrameImageWithVideoPath:(NSString*)videoPath;

//保存文件
+(void)writeVideoWithPath:(NSString*)videoFilePath savePath:(NSString*)savePath;

//时间戳转换成字符串
+(NSString*)stringFromTimeData:(id)date;

//根据视频url获取缩略图
+(UIImage *)getImage:(NSURL *)videoURL;

//加载gif图片
+(UIView*)loadGifImagePathForResource:(NSString*)name ofType:(NSString*)ext;

//根据视频路径获得视频的时间长度，单位秒(仅限于已存储到相册中的视频)
+(int64_t)getMoveTimeWithUrlString:(NSString*)urlPath;

//拼接文件文成，用来兼容文件文件名的接口，比如，有的本地文件，有的url，有的只有key
+(NSString*)compatibleImageKeyWithKey:(NSString*)imageKey;

//从一张图片中扣取一张子图片
+(UIImage*)getSubImage:(UIImage *)image mCGRect:(CGRect)mCGRect centerBool:(BOOL)centerBool;

//截屏
+(UIImage *)ScreenShotWithView:(UIView*)shotView;
//gl截屏
+(UIImage *) glToUIImage;
+(UIImage*)snapshot:(UIView*)eaglview;

//加入购物车动画
+ (void)CaptureAnimationWithImageView:(UIImage*)fromImage startPoint:(CGPoint)startP endPoint:(CGPoint)endP runView:(UIView*)runview;

//获取本地字体及系统默认字体
+(NSArray*)getFontArray:(NSString*)path size:(CGFloat)size;

//获取视频
+(void)videoPathWitSourceUrl:(NSString*)sourceUrl ready:(void (^)(void))ready complate:(void (^)(NSString* videoPath))complate failed:(void (^)(NSError *error))failed;
@end
