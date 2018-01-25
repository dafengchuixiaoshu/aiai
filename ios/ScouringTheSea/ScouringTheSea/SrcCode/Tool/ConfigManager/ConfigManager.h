//
//  ConfigManager.h
//  ScouringTheSea
//
//  Created by zhangyang on 15/11/23.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BaseModel.h"

//颜色设定
#define STS_BLACK_COLOR                     @"#000000"  //主色调，用于导航栏或者主要文字
#define STS_YELLOW_COLOR                    @"#c9aa83"  //辅助色，商品价格，评分和一些按钮等
#define STS_GRAY_TITLE_COLOR                @"#666666"  //未选中文字状态分类名称
#define STS_GRAY_CONTENTTEXT_COLOR          @"#999999"  //辅助内容文字
#define STS_GRAY_CUTLINE_COLOR              @"#dddddd"  //分割线
#define STS_GRAY_BACKGOUND_COLOR            @"#eeeeee"  //背景色和模块背景
#define STS_BLUE_COLOR                      @"#4eade8"  //查看全部评论

#define STS_FONT_MAX_SIZE            18
#define STS_FONT_MIDDLE_LARGE_SIZE   16
#define STS_FONT_MIDDLE_SMALL_SIZE   14
#define STS_FONT_MIN_SIZE            12

@interface ConfigManager : BaseModel

+(id)readConfigManager;
+(void)saveConfigManager;

@end
