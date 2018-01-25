//
//  TipsView.m
//  aiai
//
//  Created by wangnan on 15/10/23.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "TipsView.h"

@interface TipsView ()

@property(nonatomic, strong)UILabel *titleLabel;
@property(nonatomic, strong)UIScrollView *pageScroll;
@property(nonatomic, strong)UIPageControl *pageControl;

@end

@implementation TipsView

-(id)initWithFrame:(CGRect)frame{
    
    if (self = [super initWithFrame:frame]) {
        
        UIView *backGround = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width - 50, 0)];
        backGround.backgroundColor = [UIColor whiteColor];
        backGround.layer.cornerRadius = 8;
        backGround.layer.masksToBounds = YES;
        [self addSubview:backGround];
        
        CGFloat boomHigh = 30;
        //titleLabel
        _titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, backGround.frame.size.width, boomHigh)];
        _titleLabel.backgroundColor = [UIColor whiteColor];
        _titleLabel.text = @"小技巧";
        _titleLabel.textColor = [UIColor blackColor];
        _titleLabel.font = [UIFont systemFontOfSize:16];
        _titleLabel.textAlignment = NSTextAlignmentCenter;
        _titleLabel.userInteractionEnabled = YES;
        _titleLabel.enabled = YES;
        [backGround addSubview:_titleLabel];
        
        //完成按钮
        CGFloat btnWidth = _titleLabel.frame.size.height;
        UIButton *confirmBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        UIImage *btnImg = [UIImage imageNamed:@"done@2x.png"];
        [confirmBtn setImage:btnImg forState:UIControlStateNormal];
        [confirmBtn setImage:btnImg forState:UIControlStateHighlighted];
        confirmBtn.frame = CGRectMake(backGround.frame.size.width - 5 - btnWidth, 0, btnWidth, btnWidth);
        [confirmBtn addTarget:self action:@selector(removeTips) forControlEvents:UIControlEventTouchUpInside];
        [backGround addSubview:confirmBtn];
        
        //scrollerView
        _pageScroll = [[UIScrollView alloc] initWithFrame:CGRectMake(0, _titleLabel.frame.origin.y + _titleLabel.frame.size.height, backGround.frame.size.width, 280)];
        _pageScroll.pagingEnabled = YES;
        _pageScroll.showsHorizontalScrollIndicator = NO;
        _pageScroll.showsVerticalScrollIndicator = NO;
        _pageScroll.delegate = (id)self;
        _pageScroll.tag = 0;
        [backGround addSubview:_pageScroll];
        
        int pageCount = 3;
        for (int i = 0; i<pageCount; i++){
            UIImage* image = [UIImage imageNamed:[NSString stringWithFormat:@"tips%d.png",i+1]];
            UIImageView* imageView = [[UIImageView alloc]initWithImage:image];
            imageView.frame = CGRectMake(_pageScroll.frame.size.width*i, 0, _pageScroll.frame.size.width, _pageScroll.frame.size.height);
            [_pageScroll addSubview:imageView];
        }
        _pageScroll.contentSize = CGSizeMake(_pageScroll.frame.size.width * 3, _pageScroll.frame.size.height);
        
        //pageControl
        CGRect pageControlFrame = CGRectMake(0, _pageScroll.frame.origin.y + _pageScroll.frame.size.height, backGround.frame.size.width, boomHigh);
        _pageControl = [[UIPageControl alloc]initWithFrame:pageControlFrame];
        _pageControl.numberOfPages = pageCount;
        _pageControl.currentPage=0;
        _pageControl.pageIndicatorTintColor = [UIColor grayColor];
        _pageControl.currentPageIndicatorTintColor = Every_YellowColor;
        [backGround addSubview:self.pageControl];
        
        CGRect backFrame = backGround.frame;
        backFrame.size.height = _pageControl.frame.size.height + _pageControl.frame.origin.y;
        backFrame.size.width = _pageScroll.frame.size.width;
        backGround.frame = backFrame;
        backGround.center = self.center;
        //圆角
        self.backgroundColor = [UIColor whiteColor];
        
    }
    
    return self;
    
}

-(void)removeTips{
    
    [self removeFromSuperview];
}

-(void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
    [self removeTips];
}

#pragma mark - scollerView协议方法

-(void)scrollViewDidScroll:(UIScrollView *)scrollView

{
    
    //    self.navigationItem.title = [NSString stringWithFormat:@"(%.2f,%.2f)",scrollView.contentOffset.x,scrollView.contentOffset.y];
    
    
    
    NSInteger pageInt = scrollView.contentOffset.x / scrollView.frame.size.width;
    
    _pageControl.currentPage = pageInt;
    
    
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
