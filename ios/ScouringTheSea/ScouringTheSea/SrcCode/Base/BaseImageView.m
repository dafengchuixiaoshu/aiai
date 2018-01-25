//
//  BaseImageView.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/10/28.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BaseImageView.h"

@implementation BaseImageView

- (id)initWithPlaceholderImage:(UIImage*)anImage {
    return [self initWithPlaceholderImage:anImage delegate:nil];
}

- (id)initWithPlaceholderImage:(UIImage*)anImage delegate:(id<EGOImageViewDelegate>)aDelegate {
    if((self = [super initWithPlaceholderImage:anImage delegate:aDelegate])) {
        
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapAction:)];
        [self addGestureRecognizer:tap];
    }
    
    return self;
}

-(void)tapAction:(UITapGestureRecognizer*)recognizer{
    if (self.tapAction) {
        self.tapAction();
    }
}

-(void)setImageURL:(NSURL *)imageURL{
    dispatch_async(dispatch_get_main_queue(), ^{
        [super setImageURL:imageURL];
    });
}
/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
