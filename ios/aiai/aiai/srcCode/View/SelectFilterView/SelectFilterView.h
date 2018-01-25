//
//  SelectFilterView.h
//  aiai
//
//  Created by zhangyang on 15/3/27.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, SelectFilterView_SelectBtn) {
    Select_TeXiaoBtn,
    Select_LvJingBtn,
    Select_TeXiaoTimeBtn,
    Select_KaDunBtn,
    Select_MovieBtn
};

typedef void (^SelectFilterBtnAction)(SelectFilterView_SelectBtn btnType);

@interface SelectFilterView : UIView

-(id)initWithFrame:(CGRect)frame moviePath:(NSURL*)moviePath movieTime:(CGFloat)time;

-(NSInteger)getSelectIndex;
-(NSArray*)getSelectSubViewImagePathArray;

@property(nonatomic,strong)SelectFilterBtnAction sureBtnBlock;
@property(nonatomic,strong)SelectFilterBtnAction cancleBtnBlock;

-(void)releaseAllSource;

@end
