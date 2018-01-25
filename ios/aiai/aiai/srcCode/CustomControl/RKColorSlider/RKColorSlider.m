//
//  RKColorSlider.m
//  Pods
//
//  Created by Richard Kirk on 1/11/15.
//
//

#import "RKColorSlider.h"

double projectNormal(double n, double start, double end)
{
    return start + (n * (end - start));
}

double normalize(double value, double startValue, double endValue)
{
    return (value - startValue) / (endValue - startValue);
}

double solveLinearEquation(double input, double startValue, double endValue, double outputStart, double outputEnd)
{
    return projectNormal(MAX(0, MIN(1, normalize(input, startValue, endValue))), outputStart, outputEnd);
}


@interface RKColorSlider()
@property (strong, nonatomic) UIImageView *sliderImageView;
@property (strong, nonatomic) UIImageView *markImgView;
@property (strong, nonatomic) UIColor *selectedColor;
@end

@implementation RKColorSlider


- (instancetype)initWithFrame:(CGRect)frame
{
    CGRect rect = frame;
    rect.size.width = [UIScreen mainScreen].bounds.size.width;
    rect.size.height = 10;//[UIImage imageNamed:@"slider@2x"].size.height;
    frame = rect;
    
    if (self = [super initWithFrame:frame]) {
        [self setup];
        
        UIImage *arrowImg = [UIImage imageNamed:@"箭头@2x.png"];
        NSLog(@"%@",NSStringFromCGSize(arrowImg.size));
        _markImgView = [[UIImageView alloc] initWithImage:arrowImg];
        _markImgView.frame = CGRectMake(0, - arrowImg.size.height, arrowImg.size.width, arrowImg.size.height);
        [self addSubview:_markImgView];
        
    }
    return self;
}


- (id)initWithCoder:(NSCoder *)aDecoder
{
    if (self = [super initWithCoder:aDecoder]) {
        [self setup];
    }
    return self;
}


- (void)setup
{
    self.sliderImageView = [[UIImageView alloc] initWithFrame:self.bounds];
    self.sliderImageView.image = [UIImage imageNamed:@"slider@2x"];

    [self addSubview:self.sliderImageView];
    
    self.clipsToBounds = NO;
}

- (BOOL)beginTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    [super beginTrackingWithTouch:touch withEvent:event];
    CGPoint point = [touch locationInView:self];
    CGFloat hue = solveLinearEquation(point.x, 90, self.frame.size.width, 0, 1);
    CGFloat s = solveLinearEquation(point.x, 30, 75, 0, 1);
    CGFloat b = solveLinearEquation(point.x, 0, 30, 0, 1);
    
    _selectedColor = [UIColor colorWithHue:hue saturation:s brightness:b alpha:1.0];
    //改变mark的位置
    [self setMarkViewPoint:point];
    if (_getColorBlock) {
        _getColorBlock(_selectedColor);
    }
    
    [self sendActionsForControlEvents:UIControlEventValueChanged];
    
    return YES;
}

- (BOOL)continueTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    [super continueTrackingWithTouch:touch withEvent:event];
    CGPoint point = [touch locationInView:self];
    CGFloat hue = solveLinearEquation(point.x, 90, self.frame.size.width, 0, 1);
    CGFloat s = solveLinearEquation(point.x, 30, 75, 0, 1);
    CGFloat b = solveLinearEquation(point.x, 0, 30, 0, 1);
    
    _selectedColor = [UIColor colorWithHue:hue saturation:s brightness:b alpha:1.0];
    //改变mark的位置
    [self setMarkViewPoint:point];
    if (_getColorBlock) {
        _getColorBlock(_selectedColor);
    }
    
    [self sendActionsForControlEvents:UIControlEventValueChanged];
    
    return YES;
}


- (void)endTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    [super endTrackingWithTouch:touch withEvent:event];
    
    if (_getColorBlock) {
        _getColorBlock(_selectedColor);
    }
    
    CGPoint point = [touch locationInView:self];
    //改变mark的位置
    [self setMarkViewPoint:point];
    
}

-(void)setMarkViewPoint:(CGPoint)point{
    
    _markImgView.frame = CGRectMake(point.x - _markImgView.frame.size.width/2, _markImgView.frame.origin.y, _markImgView.frame.size.width, _markImgView.frame.size.height);
    
}

@end
