//
//  ScrollSelectView.m
//  drawTest
//
//  Created by zhangyang on 15/3/26.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ScrollSelectView.h"

static const CGFloat PADDING = 10;
#define NAMELABEL_H getNamelabelHeight()

CGFloat getNamelabelHeight()
{
    CGFloat defaultH = 20;
    if (iPhone5) {
        defaultH = 20;
    }else if (iPhone6){
        defaultH = 26;
    }else if (iPhone6_Plus){
        defaultH = 32;
    }
    return defaultH;
}

@interface ScrollImageView ()

@property(nonatomic,strong)UIImageView* selectImageView;
@property(nonatomic,strong)UILabel* nameLabel;

@end

@implementation ScrollImageView

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        self.backgroundColor = [UIColor clearColor];
        self.isSelect = NO;
        
        CGFloat selectImageView_X = 0;
        CGFloat selectImageView_Y = 0;
        CGFloat selectImageView_W = frame.size.width;
        CGFloat selectImageView_H = frame.size.width;
        _selectImageView = [[UIImageView alloc] initWithFrame:frame];
        _selectImageView.frame = CGRectMake(selectImageView_X, selectImageView_Y, selectImageView_W, selectImageView_H);
        [self addSubview:_selectImageView];
        
        CGFloat nameLabel_Y = _selectImageView.frame.size.height+_selectImageView.frame.origin.y;
        _nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, nameLabel_Y, frame.size.width, NAMELABEL_H)];
        _nameLabel.textColor = [UIColor whiteColor];
        _nameLabel.backgroundColor = Every_YellowColor;
        _nameLabel.textAlignment = NSTextAlignmentCenter;
        _nameLabel.font = [UIFont systemFontWithAdapter:14];
        _nameLabel.text = @"无";
        [self addSubview:_nameLabel];
    }
    return self;
}

-(void)setImagePath:(NSString *)imagePath{
    _imagePath = [imagePath copy];
    _selectImageView.image = [UIImage imageWithContentsOfFile:_imagePath];
}

-(void)setImageName:(NSString *)imageName{
    _nameLabel.text = imageName;
}

@end



@interface ScrollSelectView ()<UIScrollViewDelegate,UIGestureRecognizerDelegate>

@property(nonatomic,strong)UIScrollView* scrollview;
@property(nonatomic,strong)UIView* selectView;

@property(nonatomic,assign)CGFloat selectView_W;
@property(nonatomic,assign)CGFloat selectView_H;

@property(nonatomic,readwrite,assign)NSInteger selectIndex;

@property(nonatomic,strong,readwrite)NSMutableArray* selectSubViews;

@property(nonatomic,strong)NSMutableArray* imagePathArray;
@property(nonatomic,strong)NSMutableArray* textArray;

@end

@implementation ScrollSelectView

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        _isShowSelectView = YES;
        _isSwapeDeleteEnable = NO;
        self.backgroundColor = [UIColor clearColor];
        
        _scrollview = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, frame.size.width, frame.size.height)];
        _scrollview.delegate = self;
        [self addSubview:_scrollview];
        [_scrollview setShowsHorizontalScrollIndicator:NO];
        
        _selectView_H = frame.size.height /*- 7*/;//如果要显示滚动条，-7则是最合适的位置
        _selectView_W = _selectView_H - NAMELABEL_H;
        CGFloat x = (frame.size.width - _selectView_W)/2;
        _selectView = [[UIView alloc] initWithFrame:CGRectMake(x, 0, _selectView_W, _selectView_H)];
        _selectView.backgroundColor = [UIColor clearColor];
        _selectView.layer.borderColor = Every_YellowColor.CGColor;
        _selectView.layer.borderWidth = 3.0;
        [self addSubview:_selectView];
        
    }
    return self;
}

-(void)setIsShowSelectView:(BOOL)isShowSelectView{
    _isShowSelectView = isShowSelectView;
    
    _selectView.hidden = !isShowSelectView;
}

-(void)reloadData{
    if (_selectSubViews == nil) {
        _selectSubViews = [NSMutableArray array];
    }else{
        [_selectSubViews removeAllObjects];
    }
    
    for (UIView* subview in _scrollview.subviews) {
        if ([subview isKindOfClass:[ScrollImageView class]]) {
            [subview removeFromSuperview];
        }
    }
    
    for (int i = 0; i<[_imagePathArray count]; i++) {
        CGFloat x = (PADDING + _selectView_W) * i + (_scrollview.frame.size.width - _selectView_W)/2;
        CGFloat y = 0;
        ScrollImageView* imageView = [[ScrollImageView alloc] initWithFrame:CGRectMake(x, y, _selectView_W, _selectView_H)];
        imageView.isSelect = NO;
        imageView.imagePath = [_imagePathArray objectAtIndex:i];
        imageView.imageName = [_textArray objectAtIndex:i];
        imageView.tag = i;
        
        //添加点击事件
        UITapGestureRecognizer* panGes = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapAction:)];
        panGes.delegate = self;
        [imageView addGestureRecognizer:panGes];
        
        if (_isSwapeDeleteEnable) {
            UISwipeGestureRecognizer* upSwipeGestureRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(deleteAction:)];
            upSwipeGestureRecognizer.direction = UISwipeGestureRecognizerDirectionUp;
            [imageView addGestureRecognizer:upSwipeGestureRecognizer];
            
            UISwipeGestureRecognizer* downSwipeGestureRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(deleteAction:)];
            downSwipeGestureRecognizer.direction = UISwipeGestureRecognizerDirectionDown;
            [imageView addGestureRecognizer:downSwipeGestureRecognizer];
            
            //替换子数组
            [_selectSubViews addObject:imageView];
        }
        [_scrollview addSubview:imageView];
    }
    
    CGFloat contentsize_w = (PADDING+_selectView_W) * [_imagePathArray count] + (_scrollview.frame.size.width - _selectView_W)/2*2;
    _scrollview.contentSize = CGSizeMake(contentsize_w, _scrollview.contentSize.height);
}

-(void)setImagePathArray:(NSArray *)imagePathArray textArray:(NSArray*)textArray{
    self.imagePathArray = [NSMutableArray arrayWithArray:imagePathArray];
    self.textArray = [NSMutableArray arrayWithArray:textArray];
    
    [self reloadData];
}

#pragma mark 单击事件
-(void)tapAction:(UIGestureRecognizer*)recognizer{
    NSInteger selectIndex = recognizer.view.tag;
    
    [UIView animateWithDuration:0.3 animations:^{
        [_scrollview setContentOffset:CGPointMake((_selectView_W+PADDING)*selectIndex, 0)];
    } completion:^(BOOL finished) {
        if (self.delegate && [self.delegate respondsToSelector:@selector(scrollSelectViewTapActionWithSelectIndex:)]) {
            [self.delegate scrollSelectViewTapActionWithSelectIndex:selectIndex];
        }
    }];
}

-(void)deleteAction:(UIGestureRecognizer*)recognizer{
    NSInteger selectIndex = recognizer.view.tag;
    UIView*subview = [_selectSubViews objectAtIndex:selectIndex];
    
    [UIView animateWithDuration:0.3 animations:^{
        CGRect rect = subview.frame;
        if (((UISwipeGestureRecognizer*)recognizer).direction == UISwipeGestureRecognizerDirectionUp) {
            rect.origin.y = 0- rect.size.height;
        }else{
            rect.origin.y = [UIScreen mainScreen].bounds.size.height;
        }
        subview.frame = rect;
    } completion:^(BOOL finished) {
        [subview removeFromSuperview];
        [_selectSubViews removeObjectAtIndex:selectIndex];
        [_imagePathArray removeObjectAtIndex:selectIndex];
        [_textArray removeObjectAtIndex:selectIndex];
        [self animationMoveSubViewWithSelectIndex:selectIndex];
    }];
}

-(void)animationMoveSubViewWithSelectIndex:(NSInteger)selectIndex{
    if (selectIndex == [_selectSubViews count]) {
        [UIView animateWithDuration:0.3 animations:^{
            CGFloat contentsize_w = (PADDING+_selectView_W) * [_imagePathArray count] + (_scrollview.frame.size.width - _selectView_W)/2*2;
            _scrollview.contentSize = CGSizeMake(contentsize_w, _scrollview.contentSize.height);
        }];
    }else{
        for (NSInteger i =selectIndex ; i<[_selectSubViews count] ; i++) {
            ScrollImageView* imageView = [_selectSubViews objectAtIndex:i];
            imageView.tag --;
            [UIView animateWithDuration:0.3 animations:^{
                CGRect rect = imageView.frame;
                rect.origin.x -= rect.size.width+PADDING;
                imageView.frame = rect;
            } completion:^(BOOL finished) {
                CGFloat contentsize_w = (PADDING+_selectView_W) * [_imagePathArray count] + (_scrollview.frame.size.width - _selectView_W)/2*2;
                _scrollview.contentSize = CGSizeMake(contentsize_w, _scrollview.contentSize.height);
            }];
        }
    }
}

#pragma mark UIScrollViewDelegate
- (void)scrollViewDidScroll:(UIScrollView *)scrollView{
    _selectIndex = scrollView.contentOffset.x /(_selectView_W+PADDING) + 0.5;
    if (_selectIndex == [_imagePathArray count]) {
        return;
    }
    NSString* selectImagePath = [_imagePathArray objectAtIndex:_selectIndex];
    if (self.delegate && [self.delegate respondsToSelector:@selector(scrollSelectViewDidScrollWithSelectImagePath:)]) {
        [self.delegate scrollSelectViewDidScrollWithSelectImagePath:selectImagePath];
    }
}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate{
    if (decelerate == NO) {
//        NSInteger selectIndex = scrollView.contentOffset.x /(_selectView_W+PADDING) + 0.5;
        
        [UIView animateWithDuration:0.3 animations:^{
            [_scrollview setContentOffset:CGPointMake((_selectView_W+PADDING)*_selectIndex, 0)];
        } completion:^(BOOL finished) {
//            if (self.delegate && [self.delegate respondsToSelector:@selector(scrollSelectViewDidEndDraggingWithSelectIndex:)]) {
//                [self.delegate scrollSelectViewDidEndDraggingWithSelectIndex:selectIndex];
//            }
        }];
    }
}

- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView{
//    NSInteger selectIndex = scrollView.contentOffset.x /(scrollView.frame.size.height+PADDING) + 0.5;
    
    [UIView animateWithDuration:0.3 animations:^{
        [_scrollview setContentOffset:CGPointMake((_selectView_W+PADDING)*_selectIndex, 0)];
    } completion:^(BOOL finished) {
//        if (self.delegate && [self.delegate respondsToSelector:@selector(scrollSelectViewDidEndDeceleratingWithSelectIndex:)]) {
//            [self.delegate scrollSelectViewDidEndDeceleratingWithSelectIndex:selectIndex];
//        }
    }];
}

@end
