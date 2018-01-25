//
//  AddressMapHeadView.m
//
//
//  Created by wzy on 15-03-07.
//  Copyright (c) 2014年 All rights reserved.
//

#import "AddressMapHeadView.h"
#import "UserHeaderView.h"

@implementation AddressMapHeadAnnotation
- (instancetype)initWithUserInfo:(GlobalUserInfo *)userInfo
{
    self = [super init];
    if (self) {
        self.headImageUrl = userInfo.userFaceUrl;
        self.headVedioUrl = userInfo.userFaceVedio;
        self.coordinate = CLLocationCoordinate2DMake(userInfo.userLatitude, userInfo.userLongitude);
        self.nearByName = userInfo.userName;
    }
    return self;
}

@end

@interface AddressMapHeadView ()
@property(nonatomic,strong)UserHeaderView* headImageView;

@property (nonatomic, assign) EditState editState;      //判断手势结束时是否在self里,区别取消发送和选择特效
@end

@implementation AddressMapHeadView
@synthesize editState;

- (id)initWithAnnotation:(id<BMKAnnotation>)annotation reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithAnnotation:annotation reuseIdentifier:reuseIdentifier];
    if (self) {
        self.frame = CGRectMake(0.f, 0.f, 70, 70);
        [self setBackgroundColor:[UIColor clearColor]];
        
        UIImage* image = [UIImage imageNamed:@"地图头像外边框.png"];
        UIImageView* biankuang = [[UIImageView alloc] initWithImage:image];
        biankuang.frame = self.frame;
        [self addSubview:biankuang];
        
        CGRect rect;
        rect.origin.y = 4;          //规范
        rect.size.height = 58;      //规范
        rect.size.width = rect.size.height;
        rect.origin.x = (self.frame.size.width - rect.size.width)/2;
        
        AddressMapHeadAnnotation* weakAnnotation = annotation;
        AddressMapHeadView *weakSelf = self;
        //加载头像
        _headImageView = [[UserHeaderView alloc] init];
        _headImageView.frame = rect;
        _headImageView.headerVideoPath = [AppUtils compatibleImageKeyWithKey:weakAnnotation.headVedioUrl];
        NSString* headImageUrl = [[AppUtils getImageNewUrlWithSrcImageUrl:weakAnnotation.headImageUrl width:self.frame.size.width height:self.frame.size.height] copy];
        [_headImageView loadImageWithPath:[AppUtils getImageNewUrlWithSrcImageUrl:headImageUrl width:50 height:50] andPlaceHolderName:DefalutHeadImage];
        _headImageView.singleTapBlock  = ^(UIGestureRecognizer* recognizer){
            if (weakSelf.tap1Block) {
                weakSelf.tap1Block(recognizer);
            }
        };
        _headImageView.doubleTapBlock = ^(UIGestureRecognizer* recognizer){
            if (weakSelf.tap2Block) {
                weakSelf.tap2Block(recognizer);
            }
        };
        _headImageView.startPlayBlock = ^(){
            weakSelf.transform = CGAffineTransformMakeScale(1.2, 1.2);
        };
        _headImageView.stopPlayBlock = ^(){
            weakSelf.transform = CGAffineTransformMakeScale(1.0, 1.0);
        };
        [self insertSubview:_headImageView belowSubview:biankuang];
        
        //添加键盘长按手势
        UILongPressGestureRecognizer* recognizer = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(headImageLongPressGestureRecognizer:)];
        recognizer.delegate = (id)self;
        recognizer.minimumPressDuration = 1.0;
        [self addGestureRecognizer:recognizer];
    }
    return self;
}

-(void)headImageLongPressGestureRecognizer:(UIGestureRecognizer*)sender{
    
    CGPoint point = [sender locationInView:self];
    
    if (sender.state == UIGestureRecognizerStateChanged) {
        if (self.changeStateBlock) {
            
            //手势点在上 -- 取消
            if (point.y < 0) {
                editState = upState;
            }
            //手势点在下 -- 编辑
            else if (point.y > self.frame.size.height){
                editState = downState;
            }
            else{
                editState = normalState;
            }
            self.changeStateBlock(editState);
        }
    }
    
    if (sender.state == UIGestureRecognizerStateEnded || sender.state == UIGestureRecognizerStateBegan) {
        if (self.longPressBlock) {
            self.longPressBlock(sender, editState);
        }
    }
}

-(void)playHeaderAudio{
    self.transform = CGAffineTransformMakeScale(1.1,1.1);
    [_headImageView playUserHeaderVideo];
}

- (CGPoint)offset
{
    return CGPointMake(0, -(self.frame.size.height - 12)/2);
}

@end
