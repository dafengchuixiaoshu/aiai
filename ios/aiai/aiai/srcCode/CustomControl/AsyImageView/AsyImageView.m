//
//  AsyImageView.m
//  AsyImageDownloader
//
//  Created by andy on 13-5-31.
//  Copyright (c) 2013年 ChinaWidth. All rights reserved.
//

#import "AsyImageView.h"

@interface AsyImageView ()


- (void)layoutNow;
- (CGRect)getRectWithType:(NSInteger)type WithImage:(UIImage *)image;

- (void)AsyImageStartLoading;
- (void)AsyImageStopLoading;

@end

@implementation AsyImageView

@synthesize delegate;

@synthesize imageView;

@synthesize image = _image;

@synthesize placeHolder = _placeHolder;
@synthesize placeHolderName;

@synthesize loadingView;

@synthesize imageType = _imageType;


@synthesize imageLoader;

@synthesize finishLoadBlock;

#define WaitingView_annimationImage     @"WaitAlter.png"

-(UIImageView*)createAlertViewWithFrame:(CGRect)frame{
    UIImage* annimationImage = [UIImage imageNamed:WaitingView_annimationImage];
    UIImageView* annimationImageview = [[UIImageView alloc] initWithImage:annimationImage];
    CGRect rect = annimationImageview.frame;
    rect.size.width = frame.size.width<rect.size.width?frame.size.width:rect.size.width;
    rect.size.height = rect.size.width;
    rect.origin.x = fabs((frame.size.width - rect.size.width))/2;
    rect.origin.y = fabs((frame.size.height - rect.size.height))/2;
    
    annimationImageview.frame = rect;
    [self addSubview:annimationImageview];
    
    CABasicAnimation* rotationAnimation;
    rotationAnimation = [CABasicAnimation animationWithKeyPath:@"transform.rotation.z"];
    rotationAnimation.toValue = [NSNumber numberWithFloat: M_PI * 2.0 ];
    [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
    rotationAnimation.duration = 2;
    rotationAnimation.repeatCount = INT_MAX;//你可以设置到最大的整数值
    rotationAnimation.cumulative = NO;
    rotationAnimation.removedOnCompletion = NO;
    rotationAnimation.fillMode = kCAFillModeForwards;
    [annimationImageview.layer addAnimation:rotationAnimation forKey:@"Rotation"];
    
    return annimationImageview;
}

- (id)init
{
    self = [super init];
    if (self) {
        _imageType = AsyImageViewTypeScaled;
        self.imageView = [[UIImageView alloc] init];
        self.clipsToBounds = YES;
        [self addSubview:self.imageView];
        
//        self.loadingView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhite];
        self.loadingView = [self createAlertViewWithFrame:CGRectMake(0, 0, 50, 50)];
        self.loadingView.center = CGPointMake(self.bounds.size.width/2, self.bounds.size.height/2);
        self.loadingView.hidden = YES;
        [self addSubview:self.loadingView];
        
        //210，210，210
        self.backgroundColor = [UIColor colorWithRed:210.0/255 green:210.0/255 blue:210.0/255 alpha:210.0/255];
    }
    return self;
}

-(id)initWithDelegate:(id<AsyImageViewDelegate>)dele{
    self = [self init];
    if (self) {
        self.delegate = dele;
    }
    return self;
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        _imageType = AsyImageViewTypeScaled;
        self.imageView = [[UIImageView alloc] init];
        self.clipsToBounds = YES;
        [self addSubview:self.imageView];
        
        self.loadingView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhite];
        self.loadingView.center = CGPointMake(self.bounds.size.width/2, self.bounds.size.height/2);
        [self addSubview:self.loadingView];
        
//        self.backgroundColor = [UIColor clearColor];
        self.backgroundColor = [UIColor colorWithRed:210.0/255 green:210.0/255 blue:210.0/255 alpha:210.0/255];
    }
    return self;
}

- (id)initWithImage:(UIImage *)aImage
{
    self = [self init];
    if (self) {
        self.image = aImage;
    }
    return self;
}

- (void)dealloc
{
    [self stopLoadImage];
    self.imageView = nil;
    self.loadingView = nil;
}

- (CGRect)getRectWithType:(NSInteger)type WithImage:(UIImage *)image
{
    CGRect result = CGRectMake(0, 0, 0, 0);
    assert(image !=nil);
    if (image) {
        switch (type) {
            case AsyImageViewTypeFilled:{
                result.size = self.bounds.size;
            }break;
            case AsyImageViewTypeScaled:{
                CGFloat wholeWHRat = self.bounds.size.width/self.bounds.size.height;
                CGFloat tempWHRat = image.size.width/image.size.height;
                
                if (tempWHRat > wholeWHRat) {
                    result.size.width = self.bounds.size.width;
                    result.size.height = self.bounds.size.width/tempWHRat;
                }
                else {
                    result.size.height = self.bounds.size.height;
                    result.size.width = self.bounds.size.height*tempWHRat;
                }
            }break;
            case AsyImageViewTypeCentralRect:{
                CGFloat scalewidth = image.size.width/self.bounds.size.width;
                CGFloat scaleheight = image.size.height/self.bounds.size.height;
                CGFloat scale = scalewidth<scaleheight ? scalewidth : scaleheight;
                result.size.width = image.size.width/scale;
                result.size.height = image.size.height/scale;
                
            }break;
            case AsyImageViewTypeFixedWidth:{
            }break;
            case AsyImageViewTypeFixedHeight:{
            }break;
            case AsyImageViewTypeFit:
            {
                CGFloat selfMinWH = self.bounds.size.width<self.bounds.size.height ? self.bounds.size.width : self.bounds.size.height;
                CGFloat imgMinWH = image.size.width<image.size.height ? image.size.width : image.size.height;
                if (imgMinWH > selfMinWH) {
                    CGFloat wholeWHRat = self.bounds.size.width/self.bounds.size.height;
                    CGFloat tempWHRat = image.size.width/image.size.height;
                    
                    if (tempWHRat > wholeWHRat) {
                        result.size.width = self.bounds.size.width;
                        result.size.height = self.bounds.size.width/tempWHRat;
                    }
                    else {
                        result.size.height = self.bounds.size.height;
                        result.size.width = self.bounds.size.height*tempWHRat;
                    }
                }else{
                    result.size = image.size;
                }
            }
                break;
            default:
                break;
        }
    }
    return result;
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    [self.loadingView setCenter:CGPointMake(self.bounds.size.width/2, self.bounds.size.height/2)];
    if (_image) {
        [self.imageView setFrame:[self getRectWithType:_imageType WithImage:_image]];
    }
    else if (_placeHolder) {
        [self.imageView setFrame:[self getRectWithType:_imageType WithImage:_placeHolder]];
    }
    [self.imageView setCenter:CGPointMake(self.bounds.size.width/2, self.bounds.size.height/2)];
}

//- (void)layoutSubviews
//{
//    [super layoutSubviews];
//    if (_image) {
//        [self.imageView setFrame:[self getRectWithType:_imageType WithImage:_image]];
//    }
//    else if (_placeHolder) {
//        [self.imageView setFrame:[self getRectWithType:AsyImageViewTypeFit WithImage:_placeHolder]];
//    }
//    [self.imageView setCenter:CGPointMake(self.bounds.size.width/2, self.bounds.size.height/2)];
//}

- (void)layoutNow
{
    [self setNeedsLayout];
    [self layoutIfNeeded];
}

- (void)setImageType:(NSInteger)aImageType
{
    _imageType = aImageType;
    [self layoutNow];
}

- (void)setPlaceHolderName:(NSString *)aPlaceHolderName
{
    _placeHolderName = aPlaceHolderName;
    [self setPlaceHolder:[UIImage imageNamed:aPlaceHolderName]];
}

- (void)setImage:(UIImage *)aImage
{
    _image = aImage;
    [self.imageView setImage:aImage];
    
    [self.imageView setFrame:[self getRectWithType:_imageType WithImage:aImage]];
    [self.imageView setCenter:CGPointMake(self.bounds.size.width/2, self.bounds.size.height/2)];
    
    [self layoutNow];
    [self.imageView setAlpha:1.0f];
}

- (void)setPlaceHolder:(UIImage *)aPlaceHolder
{
    _placeHolder = aPlaceHolder;
    [self.imageView setImage:aPlaceHolder];
    [self.imageView setFrame:[self getRectWithType:AsyImageViewTypeFit WithImage:_placeHolder]];
    [self.imageView setCenter:CGPointMake(self.bounds.size.width/2, self.bounds.size.height/2)];
    [self layoutNow];
}

- (void)AsyImageStartLoading
{
//    [self.loadingView startAnimating];
    [self.loadingView setHidden:NO];
}

- (void)AsyImageStopLoading
{
//    [self.loadingView stopAnimating];
    [self.loadingView setHidden:YES];
}

- (void)loadImageWithPath:(NSString *)path andPlaceHolderName:(NSString *)phName
{
    [self stopLoadImage];
    
    self.imageDisplayStyle = 0;
    
    if (_image) {
        _image = nil;
    }
    
    if (phName) {
        [self setPlaceHolderName:phName];
    }
    
    if (self.imageLoader == nil) {
        self.imageLoader = [[AsyImageLoader alloc] init];
    }
    [self AsyImageStartLoading];
    [self.imageLoader loadImageWithDelegate:self WithPath:path];
}

- (void)loadImageWithPath:(NSString *)path andStyle:(NSInteger)style andPlaceHolderName:(NSString *)phName
{
    [self stopLoadImage];
    
    self.imageDisplayStyle = 0;
    self.imageType = AsyImageViewTypeCentralRect;
    
    if (_image) {
        _image = nil;
    }
    
    if (phName) {
        [self setPlaceHolderName:phName];
    }
    
    if (self.imageLoader == nil) {
        self.imageLoader = [[AsyImageLoader alloc] initWithStyle:style];
    }
    [self.imageLoader loadImageWithDelegate:self WithPath:path];
}

//防止tableview重用cell的时候会混乱
-(void)stopLoadImage{
    [self.imageLoader stopLoading];
    self.imageLoader.delegate = nil;
    self.imageLoader = nil;
}

#pragma mark - AsyImageLoaderDelegate <NSObject>
- (void)AsyImageLoader:(AsyImageLoader *)loader finishedLoadingImage:(UIImage *)image Style:(NSInteger)style
{
    _image = image;
    
    [self.imageView setFrame:[self getRectWithType:_imageType WithImage:image]];
    [self.imageView setImage:image];
    [self.imageView setCenter:CGPointMake(self.bounds.size.width/2, self.bounds.size.height/2)];
    
    [UIView animateWithDuration:0.2f animations:^{
        [self.imageView setAlpha:1.0f];
    }];
    
    [self AsyImageStopLoading];
    
    if (finishLoadBlock) {
        finishLoadBlock(self);
    }
    if (self.delegate && [self.delegate respondsToSelector:@selector(AsyImageView:withSize:)]) {
        if (image) {
            [self.delegate AsyImageView:self withSize:image.size];
        }
    }
}

- (void)AsyImageLoaderFailLoadingImage:(AsyImageLoader *)loader
{
    if (_placeHolderName) {
        [self setPlaceHolderName:_placeHolderName];
        [self AsyImageStopLoading];
    }
    
    if (finishLoadBlock) {
        finishLoadBlock(self);
    }
    if (self.delegate && [self.delegate respondsToSelector:@selector(AsyImageView:withSize:)]) {
        if (self.placeHolder) {
            [self.delegate AsyImageView:self withSize:self.placeHolder.size];
        }
    }
}
@end
