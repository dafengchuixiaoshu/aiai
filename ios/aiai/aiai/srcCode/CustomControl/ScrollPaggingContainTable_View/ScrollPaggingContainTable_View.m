//
//  ScrollPaggingContainTable_View.m
//  XHTwitterPaggingViewerExample
//
//  Created by zhangyang on 14/12/14.
//  Copyright (c) zhangyang
//

#import "ScrollPaggingContainTable_View.h"

#define PageControlHeight 23

@interface ScrollPaggingContainTable_View ()<UIScrollViewDelegate>

@property(nonatomic,strong)UIView* centerContainerView;
@property(nonatomic,strong)UIScrollView* paggingScrollView;
@property(nonatomic,strong)UIPageControl* pageControl;

@end

@implementation ScrollPaggingContainTable_View
@synthesize centerContainerView = _centerContainerView;
@synthesize paggingScrollView = _paggingScrollView;
@synthesize pageControl = _pageControl;

@synthesize views = _views;

#pragma mark - Propertys

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        self.isScroll = YES;
    }
    return self;
}

-(id)init{
    if (self = [super init]) {
        self.isScroll = YES;
    }
    return self;
}

#pragma mark overwrite
-(void)setIsScroll:(BOOL)isScroll{
    _isScroll = isScroll;
    
    _paggingScrollView.scrollEnabled = isScroll;
}

- (UIScrollView *)paggingScrollView {
    if (!_paggingScrollView) {
        _paggingScrollView = [[UIScrollView alloc] initWithFrame:self.bounds];
        _paggingScrollView.backgroundColor = [UIColor clearColor];
        _paggingScrollView.bounces = NO;
        _paggingScrollView.pagingEnabled = YES;
        _paggingScrollView.scrollEnabled = _isScroll;
        [_paggingScrollView setScrollsToTop:NO];
        _paggingScrollView.delegate = self;
        _paggingScrollView.showsVerticalScrollIndicator = NO;
        _paggingScrollView.showsHorizontalScrollIndicator = NO;
        [_paggingScrollView.panGestureRecognizer addTarget:self action:@selector(panGestureRecognizerHandle:)];
        [self addSubview:_paggingScrollView];
    }
    return _paggingScrollView;
}

-(UIPageControl*)pageControl{
    if (!_pageControl && self.views && [self.views count]>1) {
        _pageControl = [[UIPageControl alloc] initWithFrame:CGRectMake(0, _paggingScrollView.bounds.size.height - PageControlHeight, _paggingScrollView.bounds.size.width, PageControlHeight)];
        _pageControl.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleTopMargin;
        _pageControl.pageIndicatorTintColor = [UIColor whiteColor];
        _pageControl.currentPageIndicatorTintColor = Every_YellowColor;
        _pageControl.enabled = NO;
        [self addSubview:_pageControl];
    }
    return _pageControl;
}

#pragma mark - PanGesture Handle Method

- (void)pageControlPageDidChange:(NSInteger)index
{
    CGRect frame = _paggingScrollView.frame;
    frame.origin.x = frame.size.width * index;
    frame.origin.y = 0;
    [_paggingScrollView scrollRectToVisible:frame animated:YES];
    
    if (self.didChangedPageCompleted) {
        self.didChangedPageCompleted(self.currentPage,[self.views objectAtIndex:self.currentPage]);
    }
}

- (void)panGestureRecognizerHandle:(UIPanGestureRecognizer *)panGestureRecognizer {
    CGPoint contentOffset = self.paggingScrollView.contentOffset;
    
    CGSize contentSize = self.paggingScrollView.contentSize;
    
    CGFloat baseWidth = CGRectGetWidth(self.paggingScrollView.bounds);
    
    switch (panGestureRecognizer.state) {
        case UIGestureRecognizerStateBegan:
            
            break;
        case UIGestureRecognizerStateChanged:
        {
            if (contentOffset.x <= 0) {
                // 滑动到最左边
                [panGestureRecognizer setTranslation:CGPointZero inView:panGestureRecognizer.view];
            } else if (contentOffset.x >= contentSize.width - baseWidth) {
                // 滑动到最右边
                [panGestureRecognizer setTranslation:CGPointZero inView:panGestureRecognizer.view];
            }
            break;
        }
        case UIGestureRecognizerStateFailed:
        case UIGestureRecognizerStateEnded:
        case UIGestureRecognizerStateCancelled: {
            // 判断是否打开或关闭Menu
            break;
        }
        default:
            break;
    }
}

#pragma mark public Method
- (void)reloadData {
    if (!self.views.count) {
        return;
    }
    [self.paggingScrollView.subviews makeObjectsPerformSelector:@selector(removeFromSuperview)];
    
    [self.views enumerateObjectsUsingBlock:^(UIView* subview, NSUInteger idx, BOOL *stop) {
        
        CGRect rect = subview.frame;
        rect.origin.x = idx * self.paggingScrollView.frame.size.width;
        rect.origin.y = 0;
        rect.size.width = self.paggingScrollView.frame.size.width;
        rect.size.height = self.paggingScrollView.frame.size.height;
        subview.frame = rect;
        [self.paggingScrollView addSubview:subview];
    }];
    self.paggingScrollView.contentSize = CGSizeMake(self.paggingScrollView.frame.size.width * self.views.count, 0);
    self.pageControl.numberOfPages = [self.views count];
}

- (void)setCurrentPage:(NSInteger)currentPage {
    if (self.pageControl.currentPage == currentPage)
        return;
    self.pageControl.currentPage = currentPage;
    [self pageControlPageDidChange:currentPage];
}

-(NSInteger)currentPage{
    return self.pageControl.currentPage;
}

#pragma mark - UIScrollView Delegate

- (void)scrollViewWillBeginDecelerating:(UIScrollView *)scrollView {
    
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView {
    
}

- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView {
    // 得到每页宽度
    CGFloat pageWidth = scrollView.frame.size.width;
    
    // 根据当前的x坐标和页宽度计算出当前页数
    NSInteger currentPage = floor((scrollView.contentOffset.x - pageWidth/ 2) / pageWidth)+ 1;
    
    //设置currentpage
    [self setCurrentPage:currentPage];
}

#pragma mark overwrite setFrame
-(void)setFrame:(CGRect)frame{
    [super setFrame:frame];

    //需要把scrollview也重置frame
    [[self paggingScrollView] setFrame:CGRectMake(0, 0, frame.size.width, frame.size.height)];
}

@end
