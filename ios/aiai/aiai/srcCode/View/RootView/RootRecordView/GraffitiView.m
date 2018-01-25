//
//  MyView.m
//  UI-9-11-Tuyax


#import "GraffitiView.h"

@interface GraffitiView ()
@property(nonatomic, strong)NSMutableArray *lines;
@property(nonatomic, strong)NSMutableArray *histoyrLines;

@property(nonatomic, strong)UIBezierPath *path;
@property(nonatomic, strong)UIColor *lineColor;
@property(nonatomic, assign)FontWidth width;

@end

@implementation GraffitiView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self) {
        
        self.lines = [NSMutableArray array];
        self.histoyrLines = [NSMutableArray array];
        _width = 3.0f;
        
    }
    return self;
}

-(void)setCurrentColorWith:(UIColor*)currentColor{
    _lineColor = currentColor;
}

-(void)revokeAction{
    
    if ([self.lines lastObject]){
        [self.histoyrLines addObject:[self.lines lastObject]];
        [self.lines removeObject:[self.lines lastObject]];
        
        [self setNeedsDisplay];
    }
}

-(void)resetGraffitiView{
    
    [self.lines removeAllObjects];
    [self.histoyrLines removeAllObjects];
    [self setNeedsDisplay];
}

-(void)strokeAction{
    
    _width += 2;
    if (_width > font_high) {
        _width = font_low;
    }
}

-(BOOL)hasEdit{
    if ([self.lines count] > 0) {
        return YES;
    }else{
        return NO;
    }
}

#pragma mark touchDelegate
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch * touch = [touches anyObject];
    CGPoint point = [touch locationInView:self];
    UIBezierPath * path = [UIBezierPath bezierPath];
    [path setLineWidth:_width];
    [path moveToPoint:point];
    GraffitiView * l = [[GraffitiView alloc]init];
    l.path = path;
    l.lineColor = _lineColor;
    [self.lines addObject:l];
    
}
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch * touch = [touches anyObject];
    CGPoint point = [touch locationInView:self];
    GraffitiView * l = [self.lines lastObject];
    [l.path addLineToPoint:point];
    [self setNeedsDisplay];
}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
//    [_path stroke];
    
    for (GraffitiView * l in self.lines)
    {
        [l.lineColor setStroke];
        [l.path stroke];
    }
}


@end
