//
//  IntervalSlider.m
//
//
//  Created by xiehaiduo on 15/1/24.
//  Copyright (c) 2015年 yuanfeng. All rights reserved.
//

#import "IntervalSlider.h"

//视频最长时间13秒
#define TIME_LENGHT  13
//最多显示20格
#define MAX_SHOW_LINES  20
//精确到0.1秒
#define ONE_SECOND_SCORE 10
//每0.1秒有5个间隔
#define ONE_SECOND_SCORE_LINES  5

@interface IntervalSlider ()

@property(nonatomic,strong) NSMutableArray* linesArray;//间隔位置数组
@property(nonatomic,strong) NSMutableArray* timesArray;//间隔对应时间数值

@property(nonatomic)CGFloat curIndex;//当前选中Index
@property(nonatomic)CGFloat curPosition;//当前位置Index
@property(nonatomic)NSInteger leftIndex;//最左边Index
@property(nonatomic)NSInteger picNumber;//帧数
@property(nonatomic)NSInteger maxLines;//总可选间隔数
@property(nonatomic)CGPoint previousTouchPoint;//移动位移

@end

@implementation IntervalSlider

#pragma mark 定格动画
- (id)initWithFrame:(CGRect)frame andPictuerNumber:(NSInteger)picNumber
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        
        self.layer.delegate  = self;
        
        self.leftIndex = 0;
        self.picNumber = picNumber;
        self.maxLines = (TIME_LENGHT * ONE_SECOND_SCORE / self.picNumber - 1) * ONE_SECOND_SCORE_LINES;

        self.linesArray = [[NSMutableArray alloc] init];
        self.timesArray = [[NSMutableArray alloc] init];
        [self getLinesPositions];
        [self.layer setNeedsDisplay];
    }
    return self;
}

//得到间隔图标对应的坐标和时间
-(void)getLinesPositions{
    
    NSInteger count = MAX_SHOW_LINES;
    [self.linesArray removeAllObjects];
    [self.timesArray removeAllObjects];
    
    if (self.leftIndex + MAX_SHOW_LINES >= self.maxLines) {
        count = self.maxLines - self.leftIndex;
    }
    
    for (int i=0; i<count; i++) {
        CGFloat x = i * self.frame.size.width/MAX_SHOW_LINES;
        [self.linesArray addObject:[NSNumber numberWithFloat:x]];
        NSNumber* str = [NSNumber numberWithFloat:0.02*(i+self.leftIndex)+0.1];
        [self.timesArray addObject:str];
    }
    self.curIndex = self.maxLines < MAX_SHOW_LINES?self.maxLines/2:MAX_SHOW_LINES/2;
    self.curPosition = [[self.linesArray objectAtIndex:self.curIndex] floatValue]+15;
    if (self.maxLines < MAX_SHOW_LINES) {
        self.curPosition += (self.frame.size.width/MAX_SHOW_LINES)*(MAX_SHOW_LINES-self.maxLines)/2;
    }
    self.value = [[self.timesArray objectAtIndex:self.curIndex] floatValue];
}


#pragma mark overwrite Calayer method
-(void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx{
    if (layer == self.layer) {
        
        for (int i=0; i<[self.linesArray count]; i++) {
            
            //如果间隔小于0.1秒，则不绘制间隔图标
            if (i+self.leftIndex < 0) {
                continue;
            }
            
            //如果间隔数小于20，则修正坐标
            double x = [[self.linesArray objectAtIndex:i] floatValue]+15;
            if (self.maxLines < MAX_SHOW_LINES) {
                x += (self.frame.size.width/MAX_SHOW_LINES)*(MAX_SHOW_LINES-self.maxLines)/2;
            }
            
            if ((i+self.leftIndex)%ONE_SECOND_SCORE_LINES == 0) { //如果是5的倍数，则加粗间隔图标，并标注时间值
                CGContextMoveToPoint(ctx, x, 20);
                CGContextAddLineToPoint(ctx, x, self.frame.size.height-20);
                CGContextClosePath(ctx);
                CGContextSetLineWidth(ctx, 3);
                CGContextSetLineCap(ctx, kCGLineCapRound);
                CGContextSetStrokeColorWithColor(ctx,[UIColor colorWithWhite:1.0 alpha:1.0].CGColor);
                CGContextStrokePath(ctx);
                
                UIFont* font = [UIFont systemFontOfSize:14];
                CGContextSelectFont(ctx, [[font fontName] cStringUsingEncoding:NSUTF8StringEncoding], [font pointSize], kCGEncodingMacRoman);
                CGContextSetTextDrawingMode(ctx, kCGTextFill);
                CGContextSetFillColorWithColor(ctx, [[UIColor whiteColor] CGColor]);
                
                CGContextSaveGState(ctx);
                CGContextTranslateCTM(ctx, 0, self.frame.size.height);
                CGContextScaleCTM(ctx, 1.0, -1.0);
                NSString* str = [NSString stringWithFormat:@"%0.1fs",[[self.timesArray objectAtIndex:i] floatValue]];
                CGContextShowTextAtPoint(ctx, x-12, self.frame.size.height-10, [str cStringUsingEncoding:[NSString defaultCStringEncoding]], str.length);
                CGContextRestoreGState(ctx);
                
            }else {
                CGContextMoveToPoint(ctx, x, 28);
                CGContextAddLineToPoint(ctx, x, self.frame.size.height-28);
                CGContextClosePath(ctx);
                CGContextSetLineWidth(ctx, 1);
                CGContextSetLineCap(ctx, kCGLineCapRound);
                CGContextSetStrokeColorWithColor(ctx,[UIColor colorWithWhite:1.0 alpha:1.0].CGColor);
                CGContextStrokePath(ctx);
            }
        }
        
         //在固定位置绘制选中图标
        CGContextMoveToPoint(ctx, self.curPosition, 33);
        CGContextAddLineToPoint(ctx, self.curPosition, self.frame.size.height-33);
        CGContextClosePath(ctx);
        CGContextSetLineWidth(ctx, 5);
        CGContextSetLineCap(ctx, kCGLineCapRound);
        CGContextSetStrokeColorWithColor(ctx,[UIColor colorWithRed:1 green:0 blue:0 alpha:1.0].CGColor);
        CGContextStrokePath(ctx);
    }
}



#pragma mark - overwrite UIControl methods
-(BOOL)beginTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    self.previousTouchPoint = [touch locationInView:self];
    
    if(CGRectContainsPoint(self.layer.bounds, _previousTouchPoint))
    {
        [self.layer setNeedsDisplay];
        return YES;
    }
    return NO;
}

- (BOOL)continueTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    CGPoint touchPoint = [touch locationInView:self];
    
    float deltaX = touchPoint.x - self.previousTouchPoint.x;
    
    if (fabs(deltaX) < 5) {
        return YES;
    }
    
    if (deltaX < 0) {//如果位移小于0，向右移
        self.leftIndex += 1;
        
        double x = [[self.linesArray lastObject] floatValue];
        if (self.maxLines < MAX_SHOW_LINES) {
            x += (self.frame.size.width/MAX_SHOW_LINES)*(MAX_SHOW_LINES-self.maxLines)/2;
        }
        if (x < self.curPosition) {
            self.leftIndex -= 1;
            return YES;
        }
        
    }else {//如果位移大于0，向左移
        self.leftIndex -= 1;
        if (self.leftIndex < 0) {
            double x = [[self.linesArray objectAtIndex:-self.leftIndex] floatValue]+15;
            if (self.maxLines < MAX_SHOW_LINES) {
                x += (self.frame.size.width/MAX_SHOW_LINES)*(MAX_SHOW_LINES-self.maxLines)/2;
            }
            if (x > self.curPosition) {
                self.leftIndex += 1;
                return YES;
            }
        }
    }
    self.previousTouchPoint = touchPoint;
    
    
    [self getLinesPositions];
    [self.layer setNeedsDisplay];
    
    self.value = [[self.timesArray objectAtIndex:self.curIndex] floatValue];
    [self sendActionsForControlEvents:UIControlEventValueChanged];
    
    return YES;
}

- (void)endTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    [self.layer setNeedsDisplay];
    
    [self sendActionsForControlEvents:UIControlEventValueChanged];
}

@end
