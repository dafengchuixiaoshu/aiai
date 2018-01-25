//
//  AssetCell.m
//  TSAssetsPickerController
//
//  Created by Tomasz Szulc on 05.01.2014.
//  Copyright (c) 2014 Tomasz Szulc. All rights reserved.
//

#import "AssetCell.h"
#import <AssetsLibrary/AssetsLibrary.h>


@implementation AssetCell
@synthesize cellSelected = _cellSelected;

#pragma mark - Initialization
- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self setup];
    }
    return self;
}

- (id)init {
    self = [super init];
    if (self) {
        [self setup];
    }
    return self;
}

- (void)prepareForReuse {
    [super prepareForReuse];
    [_thumbnailImageView setImage:nil];
    [_movieMarkImageView setHidden:YES];
    _cellSelected = NO;
}

-(UIImageView *)thumbnailImageView {
    if (!_thumbnailImageView) {
        _thumbnailImageView = [[UIImageView alloc] initWithFrame:[[self class] preferedThumbnailRect]];
        [_thumbnailImageView setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
    }
    return _thumbnailImageView;
}

- (UIImageView *)movieMarkImageView {
//    if (!_movieMarkImageView) {
//        _movieMarkImageView = [[UIImageView alloc] initWithFrame:[[self class] preferedMovieMarkRect]];
//        [_movieMarkImageView setImage:[[self class] preferedMovieMarkImage]];
//        [_movieMarkImageView setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
//    }
//    return _movieMarkImageView;
    return nil;
}

-(UILabel*)timeLable{
    if (!_timeLable) {
        CGFloat lableHight = 20;
        _timeLable = [[UILabel alloc] initWithFrame:CGRectMake(0, self.frame.size.height-lableHight, self.frame.size.width, lableHight)];
        _timeLable.backgroundColor = [UIColor whiteColor];
        [_timeLable setFont:[UIFont systemFontOfSize:14.0]];
    }
    return _timeLable;
}


- (void)setup {
    _cellSelected = NO;
    
    CGRect rect = CGRectZero;
    rect.size = [[self class] preferedCellSize];
    self.frame = rect;
    
    [self addSubview:self.thumbnailImageView];
    [self addSubview:self.movieMarkImageView];
    [self addSubview:self.timeLable];
}


#pragma mark - Configuration
- (void)configure:(ALAsset *)asset {
    //设置timelabel内容
    NSNumber* timeLength = [asset valueForProperty:ALAssetPropertyDuration];
    NSString* timeString = [NSString stringWithFormat:@"%.2f",[timeLength doubleValue]/100];
    NSArray* array = [timeString componentsSeparatedByString:@"."];
    NSString* min = [array objectAtIndex:0];
    NSString* second = [array objectAtIndex:1];
    _timeLable.text = [NSString stringWithFormat:@"%@:%@",min,second];
}


#pragma mark - Modificators
- (void)markAsSelected:(BOOL)selected {
    _cellSelected = selected;

    UIColor *color;
    if (selected ) {
        color = [[self class] preferedBackgroundColorForStateSelected];
    } else {
        color = [[self class] preferedBackgroundColorForStateNormal];
    }
    
    [self setBackgroundColor:color];
    [_movieMarkImageView setBackgroundColor:color];
}


#pragma mark - Customization
+ (CGSize)preferedCellSize {
    return CGSizeMake(74, 74);
}

+ (CGRect)preferedThumbnailRect {
    return CGRectMake(5, 5, 64, 64);
}

+ (CGRect)preferedMovieMarkRect {
    return CGRectMake(46, 46, 20, 20);
}

+ (UIImage *)preferedMovieMarkImage {
    return [UIImage imageNamed:@"movieMark"];
}

+ (UIColor *)preferedBackgroundColorForStateNormal {
    return [UIColor colorWithWhite:0.7 alpha:0.3];
}

+ (UIColor *)preferedBackgroundColorForStateSelected {
//    return [UIColor colorWithRed:21.0f/255.0f green:150.0f/255.0f blue:210.0f/255.0f alpha:1.0f];
    return [UIColor blackColor];
}

@end



