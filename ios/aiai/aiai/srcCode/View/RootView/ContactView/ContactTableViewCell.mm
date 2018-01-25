//
//  ContactTableViewCell.m
//  aiai
//
//  Created by zhangyang on 15/2/5.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ContactTableViewCell.h"


#define kMinimumVelocity  self.contentView.frame.size.width*1.5
#define kMinimumPan       60.0
#define kBOUNCE_DISTANCE  7.0

typedef enum {
    LMFeedCellDirectionNone=0,
    LMFeedCellDirectionRight,
    LMFeedCellDirectionLeft,
} LMFeedCellDirection;

typedef enum {
    kFeedStatusNormal = 0,
    kFeedStatusLeftExpanded,
    kFeedStatusLeftExpanding,
    kFeedStatusRightExpanded,
    kFeedStatusRightExpanding,
}kFeedStatus;

@interface ContactTableViewCell ()

//flag
@property (nonatomic,retain) UIPanGestureRecognizer *panGesture;
@property (nonatomic,assign) CGFloat initialHorizontalCenter;
@property (nonatomic,assign) CGFloat initialTouchPositionX;
@property (nonatomic,assign) BOOL revealing;

@property (nonatomic,assign) LMFeedCellDirection lastDirection;
@property (nonatomic,assign) kFeedStatus currentStatus;
@property (nonatomic,assign) CGFloat originalCenter;

//ui
@property (nonatomic,retain) UIView *bottomRightView;


@property(nonatomic, strong)AsyImageView* headImageView;
@property(nonatomic, strong)UILabel* nameLabel;
@property(nonatomic, strong)UIImageView *lineView;

@property(nonatomic,strong)IMPackageBuddyData* buddyEntity;
@end

@implementation ContactTableViewCell
@synthesize buddyEntity = _buddyEntity;
@synthesize lineView;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {

        //很关键的一句
        self.contentView.backgroundColor = EveryView_BackGroundColor;
        self.backgroundColor = EveryView_BackGroundColor;
        
        _currentStatus=kFeedStatusNormal;
        
        CGFloat padding_TB = 13;
        CGFloat padding_LR = 13;
        //头像
        _headImageView = [[AsyImageView alloc] init];
        CGFloat headImageView_H = 40;
        _headImageView.frame = CGRectMake(padding_LR, padding_TB, headImageView_H, headImageView_H);
        _headImageView.layer.cornerRadius = _headImageView.frame.size.width/2;
        _headImageView.layer.masksToBounds = YES;
        [self.contentView addSubview:_headImageView];
        
        //用户名称
        CGFloat nameLabel_X = _headImageView.frame.origin.x+_headImageView.frame.size.width+padding_LR;
        CGFloat nameLabel_Y = 0;
        CGFloat nameLabel_W = self.frame.size.width - _headImageView.frame.size.width - padding_LR*2;
        CGFloat nameLabel_H = ContactTableViewCell_Height;
        _nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(nameLabel_X, nameLabel_Y, nameLabel_W, nameLabel_H)];
        _nameLabel.textAlignment = NSTextAlignmentLeft;
        _nameLabel.textColor = [UIColor whiteColor];
        _nameLabel.font = [UIFont systemFontOfSize:16];
        _nameLabel.text = @"";
        [self.contentView addSubview:_nameLabel];
        
        lineView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        lineView.frame = CGRectMake(padding_LR, ContactTableViewCell_Height - 1, [UIScreen mainScreen].bounds.size.width - padding_LR*2, 1);
        lineView.alpha = 0.5;
        [self.contentView addSubview:lineView];

        _originalCenter=ceil(self.bounds.size.width / 2);
        
        _panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGestureHandle:)];
        _panGesture.delegate = self;
        [self addGestureRecognizer:_panGesture];
        
        
    }
    self.selectionStyle = UITableViewCellSelectionStyleNone;
    return self;
}



- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated{
    if (highlighted) {
        self.backgroundColor = Every_YellowColor;
        self.contentView.backgroundColor = Every_YellowColor;
    }else{
        self.backgroundColor = EveryView_BackGroundColor;
        self.contentView.backgroundColor = EveryView_BackGroundColor;
    }
}

-(void)setFrame:(CGRect)frame{
    [super setFrame:frame];
}

-(void)layoutSubviews{
    [super layoutSubviews];
}

#pragma mark public mothed
-(void)setCellWithData:(IMPackageBuddyData*)buddyEntity AndIsShowCutline:(BOOL)isShow{
    
    lineView.hidden = !isShow;
    
    _buddyEntity = buddyEntity;
    //设置名称
    _nameLabel.text = _buddyEntity.buddySubname && [_buddyEntity.buddySubname length] > 0?_buddyEntity.buddySubname : _buddyEntity.buddyNickName;
    //设置头像
    UIImage* logo = [UIImage imageNamed:_buddyEntity.buddyPortraitKey];
    if (logo) {
        self.headImageView.image = logo;
    }else{
        NSString* newUrl = [AppUtils getImageNewUrlWithSrcImageUrl:_buddyEntity.buddyPortraitKey width:self.headImageView.frame.size.width height:self.headImageView.frame.size.height];
        [self.headImageView loadImageWithPath:newUrl andPlaceHolderName:DefalutHeadImage];
    }
    
}

-(void)resetCell{
    _revealing=NO;
}

-(void)setNameLabelText:(NSString*)nameStr{
    _nameLabel.text = nameStr;
    _buddyEntity.buddySubname = nameStr;
}

#pragma mark gester
- (void)panGestureHandle:(UIPanGestureRecognizer *)recognizer
{
    
    //begin pan...
    if (recognizer.state == UIGestureRecognizerStateBegan) {
        
        _initialTouchPositionX = [recognizer locationInView:self].x;
        _initialHorizontalCenter = self.contentView.center.x;
        if(_currentStatus==kFeedStatusNormal){
            [self layoutBottomView];
        }
        if (_swipBlock) {
            _swipBlock();
        }
        
    }else if (recognizer.state == UIGestureRecognizerStateChanged) { //status change
        
        
        CGFloat panAmount  = _initialTouchPositionX - [recognizer locationInView:self].x;
        CGFloat newCenterPosition     = _initialHorizontalCenter - panAmount;
        CGFloat centerX               = self.contentView.center.x;
        
        if(centerX<_originalCenter && _currentStatus!=kFeedStatusRightExpanding){
            _currentStatus = kFeedStatusRightExpanding;
            [self togglePanelWithFlag];
            
        }
        
        if (newCenterPosition > self.bounds.size.width + _originalCenter){
            newCenterPosition = self.bounds.size.width + _originalCenter;
        }
        else if (newCenterPosition < -_originalCenter){
            newCenterPosition = -_originalCenter;
        }
        
        _lastDirection = LMFeedCellDirectionLeft;
        if (panAmount > 0){
            CGPoint center = self.contentView.center;
            center.x = newCenterPosition;
            self.contentView.layer.position = center;
        }
        
    }
    else if (recognizer.state == UIGestureRecognizerStateEnded ||
             recognizer.state == UIGestureRecognizerStateCancelled){
        
        CGPoint translation = [recognizer translationInView:self];
        CGFloat velocityX = [recognizer velocityInView:self].x;
        
        //判断是否push view
        BOOL isNeedPush = (fabs(velocityX) > kMinimumVelocity);
        
        
        isNeedPush |= ((_lastDirection == LMFeedCellDirectionLeft && translation.x < -kMinimumPan) ||
                       (_lastDirection== LMFeedCellDirectionRight && translation.x > kMinimumPan));
        
        if (velocityX > 0 && _lastDirection == LMFeedCellDirectionLeft){
            isNeedPush = NO;
        }
        
        else if (velocityX < 0 && _lastDirection == LMFeedCellDirectionRight){
            isNeedPush = NO;
        }
        
        if (isNeedPush && !self.revealing) {
            
            if(_lastDirection==LMFeedCellDirectionRight){
                _currentStatus = kFeedStatusLeftExpanding;
                [self togglePanelWithFlag];
                
            }
            else{
                _currentStatus = kFeedStatusRightExpanding;
                [self togglePanelWithFlag];
            }
            [self _slideOutContentViewInDirection:_lastDirection];
            [self _setRevealing:YES];
            
        }
        else if (self.revealing && translation.x != 0) {
            
            LMFeedCellDirection direct = _currentStatus==kFeedStatusRightExpanding?LMFeedCellDirectionLeft:LMFeedCellDirectionRight;
            
            [self _slideInContentViewFromDirection:direct];
            [self _setRevealing:NO];
            
        }
        else if (translation.x != 0) {
            // Figure out which side we've dragged on.
            LMFeedCellDirection finalDir = LMFeedCellDirectionRight;
            if (translation.x < 0)
                finalDir = LMFeedCellDirectionLeft;
            [self _slideInContentViewFromDirection:finalDir];
            [self _setRevealing:NO];
        }
    }
    
}

-(void)layoutBottomView{
    if(!_bottomRightView){
        
        _currentStatus = kFeedStatusNormal;
        
        _bottomRightView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, self.bounds.size.width, self.bounds.size.height)];
        _bottomRightView.backgroundColor = EveryView_BackGroundColor;
        [self insertSubview:_bottomRightView atIndex:0];
        
        UIButton *deleteButton = [[UIButton alloc] init];
        //        [deleteButton setImage:[UIImage imageNamed:delete_button_name] forState:UIControlStateNormal];
        deleteButton.frame = CGRectMake(_bottomRightView.frame.size.width - 80 - 15, 15, 80, 40);
        [deleteButton setTitle:@"删除" forState:UIControlStateNormal];
        [deleteButton setTitle:@"删除" forState:UIControlStateHighlighted];
        [deleteButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [deleteButton setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
        [deleteButton setBackgroundColor:[UIColor clearColor]];
        deleteButton.layer.borderColor = [UIColor whiteColor].CGColor;
        deleteButton.layer.borderWidth = 1.0;
        deleteButton.layer.cornerRadius = deleteButton.frame.size.height/2;
        deleteButton.layer.masksToBounds = YES;
        [deleteButton addTarget:self action:@selector(deleteAction:) forControlEvents:UIControlEventTouchUpInside];
        
        [_bottomRightView addSubview:deleteButton];
    }
}

-(void)deleteAction:(id)sender{
    if (_deleteBlock) {
        _deleteBlock();
    }
}

- (void)togglePanelWithFlag{
    switch (_currentStatus) {
        case kFeedStatusLeftExpanding:
        {
            _bottomRightView.alpha=0.0f;
        }
            break;
        case kFeedStatusNormal:{
            [_bottomRightView removeFromSuperview];
            self.bottomRightView=nil;
        }
        default:
            break;
    }
    
}

#pragma mark -
#pragma mark revealing setter
- (void)setRevealing:(BOOL)revealing
{
    if (_revealing == revealing) {
        return;
    }
    [self _setRevealing:revealing];
    
    if (self.revealing) {
        [self _slideOutContentViewInDirection:_lastDirection];
    } else {
        [self _slideInContentViewFromDirection:_lastDirection];
    }
}

- (void)_setRevealing:(BOOL)revealing
{
    _revealing=revealing;
}



#pragma mark
#pragma mark - ContentView Sliding
- (void)_slideInContentViewFromDirection:(LMFeedCellDirection)direction
{
    
    CGFloat bounceDistance;
    
    if (self.contentView.center.x == _originalCenter)
        return;
    
    switch (direction) {
        case LMFeedCellDirectionRight:
            bounceDistance = kBOUNCE_DISTANCE;
            break;
        case LMFeedCellDirectionLeft:
            bounceDistance = -kBOUNCE_DISTANCE;
            break;
        default:
            bounceDistance = kBOUNCE_DISTANCE;
            break;
    }
    
    [UIView animateWithDuration:0.1
                          delay:0
                        options:UIViewAnimationOptionCurveEaseOut|UIViewAnimationOptionAllowUserInteraction
                     animations:^{
                         self.contentView.center = CGPointMake([UIScreen mainScreen].bounds.size.width/2, self.contentView.center.y);
                     }
                     completion:^(BOOL f) {
                         [UIView animateWithDuration:0.1
                                               delay:0
                                             options:UIViewAnimationOptionCurveEaseOut
                                          animations:^{
                                              self.contentView.frame = CGRectOffset(self.contentView.frame, bounceDistance, 0);
                                          }
                                          completion:^(BOOL f) {
                                              [UIView animateWithDuration:0.1
                                                                    delay:0
                                                                  options:UIViewAnimationOptionCurveEaseIn
                                                               animations:^{ self.contentView.frame = CGRectOffset(self.contentView.frame, -bounceDistance, 0); }
                                                               completion:^(BOOL f){
                                                                   _currentStatus=kFeedStatusNormal;
                                                                   [self togglePanelWithFlag];
                                                                   [UIView animateWithDuration:0.1 animations:^{
                                                                       self.contentView.frame = CGRectMake(0, 0, self.contentView.frame.size.width, self.contentView.frame.size.height);
                                                                   }];
                                                               }];
                                          }];
                     }];
}



- (void)_slideOutContentViewInDirection:(LMFeedCellDirection)direction;
{
    CGFloat newCenterX;
    CGFloat bounceDistance;
    switch (direction) {
        case LMFeedCellDirectionLeft:{
            newCenterX = [UIScreen mainScreen].bounds.size.width/2 - (15*2+80);
            bounceDistance = -kBOUNCE_DISTANCE;
            _currentStatus=kFeedStatusLeftExpanded;
        }
            break;
        default:
            break;
    }
    
    [UIView animateWithDuration:0.1
                          delay:0
                        options:UIViewAnimationOptionCurveEaseOut
                     animations:^{
                         self.contentView.center = CGPointMake(newCenterX, self.contentView.center.y);
                     }
                     completion:^(BOOL f) {
                         [UIView animateWithDuration:0.1 delay:0
                                             options:UIViewAnimationOptionCurveEaseIn
                                          animations:^{
                                              self.contentView.frame = CGRectOffset(self.contentView.frame, -bounceDistance, 0);
                                          }
                                          completion:^(BOOL f) {
                                              [UIView animateWithDuration:0.1 delay:0
                                                                  options:UIViewAnimationOptionCurveEaseIn
                                                               animations:^{
                                                                   self.contentView.frame = CGRectOffset(self.contentView.frame, bounceDistance, 0);
                                                               }
                                                               completion:NULL];
                                          }];
                     }];
}




#pragma mark
#pragma mark - UIGestureRecognizerDelegate
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    if (gestureRecognizer == _panGesture) {
        UIScrollView *superview = (UIScrollView *)self.superview;
        CGPoint translation = [(UIPanGestureRecognizer *)gestureRecognizer translationInView:superview];
        // Make it scrolling horizontally
        return ((fabs(translation.x) / fabs(translation.y) > 1) ? YES : /* DISABLES CODE */ (NO) &&
                (superview.contentOffset.y == 0.0 && superview.contentOffset.x == 0.0));
    }
    return YES;
}





@end
