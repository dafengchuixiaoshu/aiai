//
//  ScrollPaggingContainTable_View.h
//  XHTwitterPaggingViewerExample
//
//  Created by zhangyang on 14/12/14.
//  Copyright (c)
//

#import <UIKit/UIKit.h>

typedef void(^DidChangedPageBlock)(NSInteger currentPage, UIView* subView);

/**
 scrollview包含子视图的控件,子视图可以是任何视图
 */
@interface ScrollPaggingContainTable_View : UIView

@property(nonatomic, strong) NSArray* views;

@property (nonatomic, copy) DidChangedPageBlock didChangedPageCompleted;

@property (nonatomic, assign) NSInteger currentPage;

@property (nonatomic, assign) BOOL isScroll;

- (void)reloadData;

@end
