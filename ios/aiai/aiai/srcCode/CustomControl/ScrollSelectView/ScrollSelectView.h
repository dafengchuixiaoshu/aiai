//
//  ScrollSelectView.h
//  drawTest
//
//  Created by zhangyang on 15/3/26.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ScrollImageView : UIView
@property(nonatomic,assign)BOOL isSelect;
@property(nonatomic,copy)NSString* imagePath;
@property(nonatomic,copy)NSString* imageName;
@end

@protocol ScrollSelectViewDelegate <NSObject>

-(void)scrollSelectViewTapActionWithSelectIndex:(NSInteger)selectIndex;

-(void)scrollSelectViewDidScrollWithSelectImagePath:(NSString*)selectImagePath;
-(void)scrollSelectViewDidEndDraggingWithSelectIndex:(NSInteger)selectIndex;
-(void)scrollSelectViewDidEndDeceleratingWithSelectIndex:(NSInteger)selectIndex;

@end

@interface ScrollSelectView : UIView

@property(nonatomic,weak)id<ScrollSelectViewDelegate> delegate;

@property(nonatomic,readonly,assign)NSInteger selectIndex;

//是否显示selectview中央的框
@property(nonatomic,assign)BOOL isShowSelectView; //default YES

//subview是否支持swape删除
@property(nonatomic,assign)BOOL isSwapeDeleteEnable;//default NO

-(void)setImagePathArray:(NSArray *)imagePathArray textArray:(NSArray*)textArray;

//已经选择的suvview数组
@property(nonatomic,strong,readonly)NSMutableArray* selectSubViews;

@end
