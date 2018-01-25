//
//  BaseImageView.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/10/28.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "EGOImageView.h"

typedef void(^TapAction)(void);

//程序使用异步加载的图片都使用这个类，方便扩展

//egoimageview的一个bug是如果再次setImageUrl的时候必须放在主线程里才可以，否则有问题，尽量避免此种方式
@interface BaseImageView : EGOImageView

@property(nonatomic,copy)TapAction tapAction;

@end
