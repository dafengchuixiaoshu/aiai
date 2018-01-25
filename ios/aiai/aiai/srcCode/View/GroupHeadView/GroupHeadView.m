//  Created by xiehaiduo on 15/4/16.
//  Copyright (c) 2015年 xiehaiduo. All rights reserved.
//


#import "GroupHeadView.h"

#import "HeadView.h"

#define radians(degrees) ((degrees) * M_PI / 180)

@interface GroupHeadView ()

@property(nonatomic, strong)dispatch_block_t prepareBlock;

@property(nonatomic, strong)finishUpData finishBlock;

@end

@implementation GroupHeadView{
    
    NSArray *_images;
    CGSize _size;
    CGPoint centerPoint;
}

+ (instancetype)groupHeadViewWithFrame:(CGRect)frame images:(NSArray *)images prepare:(void (^)(void))prepare complete:(finishUpData)complete
{
    return [[self alloc] initWithFrame:frame images:images prepare:(void (^)(void))prepare complete:(finishUpData)complete];
}

- (instancetype)initWithFrame:(CGRect)frame images:(NSArray *)images prepare:(void (^)(void))prepare complete:(finishUpData)complete
{
    self = [super initWithFrame:frame];
    if (self) {
        
        self.finishBlock = complete;
        self.prepareBlock = prepare;
        _images = images;
        _size = frame.size;
        
        [self p_initViewData];
        self.backgroundColor = [EveryView_BackGroundColor colorWithAlphaComponent:0.90];
    }
    return self;
}

- (UIImage *)imageGroupHeadView
{
    switch ([_images count]) {
//        case 1:
//            return _images[0];
        default:
            return [self imageRenderView:self];
    }
    return nil;
}

- (void)p_initViewData
{
    if (self.prepareBlock) {
        self.prepareBlock();
    }
    
    centerPoint.x = _size.width / 2;
    centerPoint.y = _size.height / 2;
    
    [self setBackgroundColor:[UIColor clearColor]];
    
    switch ([_images count]) {
        case 0:
            return;
        case 1:
            [self p_headCardViewOne];
            break;
        case 2:
            [self p_headCardViewTwo];
            break;
        case 3:
            [self p_headCardViewThree];
            break;
        case 4:
            [self p_headCardViewFour];
            break;
        default:
            [self p_headCardViewMore];
            break;
    }
    
}

- (void)p_headCardViewOne
{
    float cubeRadius = _size.width / 2;//variable
    
    NSArray *angles = @[@(0)];
    CGPoint point0 = CGPointMake(_size.width / 2, _size.width / 2);
    NSArray *points = @[[NSValue valueWithCGPoint:point0]];
    
    [self addImagesPoints:points angles:angles size:cubeRadius];
}

- (void)p_headCardViewTwo
{
    float cubeRadius = _size.width / 3.8;//subsview width and height
    float radius = centerPoint.x - cubeRadius;
    float datUnit = radius * sin(radians(45));
    
    CGPoint point0 = CGPointMake(-datUnit, -datUnit);
    CGPoint point1 = CGPointMake(datUnit, datUnit);
    
    NSArray *angles = @[@(45),@(0)];
    
    NSArray *points = @[[NSValue valueWithCGPoint:point0],
                        [NSValue valueWithCGPoint:point1]];
    
    points = [self resetCycleCenterOffset:cubeRadius + radius points:points];
    [self addImagesPoints:points angles:angles size:cubeRadius];
}

- (void)p_headCardViewThree
{
    float cubeRadius = _size.width / 4.1;//subsview width and height
    float radius = centerPoint.x - cubeRadius;

    CGPoint point0 = CGPointMake(0, -radius);
    CGPoint point1 = CGPointMake(-radius * cos(radians(30)), radius * sin(radians(30)));
    CGPoint point2 = CGPointMake(radius * cos(radians(30)), radius * sin(radians(30)));
    
    NSArray *angles = @[@(120),@( 0),@(-120)];
    
    NSArray *points = @[[NSValue valueWithCGPoint:point0],
                        [NSValue valueWithCGPoint:point1],
                        [NSValue valueWithCGPoint:point2]];
    
    points = [self resetCycleCenterOffset:cubeRadius + radius points:points];
    [self addImagesPoints:points angles:angles size:cubeRadius];
}
- (void)p_headCardViewFour
{
    float cubeRadius = _size.width / 4.5;//subsview width and height
    float radius = centerPoint.x - cubeRadius;
    float datUnit = radius * sin(radians(45));
    
    CGPoint point0 = CGPointMake(-datUnit, -datUnit);
    CGPoint point1 = CGPointMake(-datUnit, datUnit);
    CGPoint point2 = CGPointMake(datUnit, datUnit);
    CGPoint point3 = CGPointMake(datUnit, -datUnit);
    
    NSArray *angles = @[@(90),@(0),@(-90),@(-180)];
    
    NSArray *points = @[[NSValue valueWithCGPoint:point0],
                        [NSValue valueWithCGPoint:point1],
                        [NSValue valueWithCGPoint:point2],
                        [NSValue valueWithCGPoint:point3]];
    
    points = [self resetCycleCenterOffset:cubeRadius + radius points:points];
    [self addImagesPoints:points angles:angles size:cubeRadius];
}

- (void)p_headCardViewMore
{
    float cubeRadius = _size.width / 5;//subsview width and height
    float radius = centerPoint.x - cubeRadius;//
    
    CGPoint point0 = CGPointMake(0, -radius);
    CGPoint point1 = CGPointMake(radius * -cos(radians(18)), radius * -sin(radians(18)));
    CGPoint point2 = CGPointMake(radius * -cos(radians(54)), radius * sin(radians(54)));
    CGPoint point3 = CGPointMake(radius * cos(radians(54)), radius * sin(radians(54)));
    CGPoint point4 = CGPointMake(radius * cos(radians(18)), radius * -sin(radians(18)));
    
    NSArray *angles = @[@(36 * 4),@(36 * 2),@(0),@(36 * -2),@(36 * -4)];
    
    NSArray *points = @[[NSValue valueWithCGPoint:point0],
                        [NSValue valueWithCGPoint:point1],
                        [NSValue valueWithCGPoint:point2],
                        [NSValue valueWithCGPoint:point3],
                        [NSValue valueWithCGPoint:point4]];
    
    points = [self resetCycleCenterOffset:cubeRadius + radius points:points];
    [self addImagesPoints:points angles:angles size:cubeRadius];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - Method
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

- (void)addImagesPoints:(NSArray *)points angles:(NSArray *)angles size:(CGFloat)cubeRadius
{
    
    __block int finishCount = 0;
    for (int num=0; num<[angles count]; num++) {
        
        CGFloat angle =[angles[num] floatValue];
        CGPoint point =[points[num] CGPointValue];
        
        __weak GroupHeadView *weakSelf = self;
        CGRect cardViewRect = CGRectMake(0, 0, cubeRadius * 2, cubeRadius * 2);
        HeadView *cardImage = [HeadView headViewWithFrame:cardViewRect image:_images[num]];
        cardImage.backgroundColor = [UIColor clearColor];
        cardImage.cycleRadius = cubeRadius;
        cardImage.rotateAngle = angle;
        cardImage.center = point;
        cardImage.finishLoadBlock = ^(AsyImageView *obj){
        
            dispatch_async(dispatch_get_main_queue(), ^{
                finishCount++;
                HeadView *temp = (HeadView*)obj;
                //首先要删去父类视图的imageView 否则会压盖子视图的view
                [obj.imageView removeFromSuperview];
                temp.subImage = obj.image;
                [obj setNeedsDisplay];
                //如果全部子头像加载完成 或者到达最大数。那么开始上传图片服务器生成key
                if (finishCount == [_images count] || finishCount == [GroupHeadView MaxCount]) {
                    [weakSelf startHttpUpData];
                }
            });
        };
        [self addSubview:cardImage];
        
        if ([_images count] == 1 || ([_images count] == 2 && num == 1)){
            cardImage.cycleClipAngle = 1;
        }
    }
}

-(void)startHttpUpData{
    
    UIImage *headerImg = [self imageGroupHeadView];
    NSData* headerData = UIImageJPEGRepresentation(headerImg, 1.0);
    
    [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[self class] data:headerData prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSString *fileName = [dic objectForKey:@"fileName"];
        if (self.finishBlock) {
            self.finishBlock(fileName);
        }
    } failed:^(NSError *error) {
        
    }];
    
}

- (UIImage *)imageRenderView:(UIView *)view
{
    UIGraphicsBeginImageContext(view.bounds.size);
    CGContextRef currnetContext = UIGraphicsGetCurrentContext();
    [view.layer renderInContext:currnetContext];
    UIImage* image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image;
}

- (NSArray *)resetCycleCenterOffset:(CGFloat)offset points:(NSArray *)points
{
    NSMutableArray *tempPoints = [points mutableCopy];
    
    for (int num=0; num<[tempPoints count]; num++) {
        
        CGPoint point = [tempPoints[num] CGPointValue];
        point.x += offset;
        point.y += offset;
        
        NSValue *pointValue = [NSValue valueWithCGPoint:point];
        [tempPoints replaceObjectAtIndex:num withObject:pointValue];
    }
    return tempPoints;
}


//返回子头像最大数
+(int)MaxCount{
    return 5;
}

@end
