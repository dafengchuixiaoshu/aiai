//
//  ContactConllectionCell.m
//  aiai
//
//  Created by zhangyang on 14/12/14.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "ContactHeadLogoConllectionCell.h"
#import "IMPackageData.h"
#import "UserHeaderView.h"

//手势有效滑动距离
#define MINDISTANCE 300

@interface ContactHeadLogoConllectionCell ()<UIGestureRecognizerDelegate>{
    CGPoint beginPoint;    //手势开始按下的位置
    CGPoint endPoint;
}

@property(nonatomic,readwrite,strong)IMPackageSessionData* currentSessionData;
@property(nonatomic,strong)UIView* bgImageview;
@property(nonatomic,strong)UserHeaderView* headImageView;
@property(nonatomic,strong)UIView* msgSendStausView;
@property(nonatomic,strong)UILabel* nameLabel;
@property(nonatomic,strong)UILabel* unReadMsgLabel;
@property(nonatomic,strong)UIButton* deleteBtn;

@end

@implementation ContactHeadLogoConllectionCell
@synthesize headImageView = _headImageView;
@synthesize nameLabel = _nameLabel;
@synthesize deleteBtn = _deleteBtn;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        
        //联系人头像圆圈
//        CGFloat imageview_w = frame.size.width - LR_padding*2;
//        CGFloat imageview_h = imageview_w;
//        _bgImageview = [[UIView alloc] initWithFrame:CGRectMake(LR_padding, TB_padding, imageview_w, imageview_h)];
//        _bgImageview.backgroundColor = [UIColor whiteColor];
//        _bgImageview.layer.cornerRadius = _bgImageview.frame.size.width/2;
//        _bgImageview.layer.masksToBounds = YES;
//        [self addSubview:_bgImageview];
        
        __weak ContactHeadLogoConllectionCell* weakself = self;
        //添加头像
        _headImageView = [[UserHeaderView alloc] init];
        _headImageView.frame = CGRectMake(2, 2, CONTACTHEADLOGO_W, CONTACTHEADLOGO_W);
        __weak UserHeaderView *weakHeader = _headImageView;
        _headImageView.singleTapBlock = ^(UIGestureRecognizer* recognizer){
            [weakself headImageView1TapAction:recognizer];
        };
        _headImageView.doubleTapBlock = ^(UIGestureRecognizer* recognizer){
            [weakHeader playUserHeaderVideo];
        };
        _headImageView.startPlayBlock = ^(){
            [weakHeader beginAnimating];
        };
        _headImageView.stopPlayBlock = ^(){
            [weakHeader resetAnimating];
        };
        //头像添加长按事件
        UILongPressGestureRecognizer* recognizer = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressFromUp:)];
        recognizer.delegate = self;
        [_headImageView addGestureRecognizer:recognizer];
        
        [self addSubview:_headImageView];
        
        //未读消息显示
        CGFloat unReadMsgLabel_W = 16;
        CGFloat unReadMsgLabel_H = unReadMsgLabel_W;
        CGFloat unReadMsgLabel_X = _headImageView.frame.origin.x + _headImageView.frame.size.width - unReadMsgLabel_W;
        CGFloat unReadMsgLabel_Y = _headImageView.frame.origin.y + _headImageView.frame.size.height - unReadMsgLabel_H;
        _unReadMsgLabel = [[UILabel alloc] initWithFrame:CGRectMake(unReadMsgLabel_X, unReadMsgLabel_Y, unReadMsgLabel_W, unReadMsgLabel_H)];
        _unReadMsgLabel.backgroundColor = Every_YellowColor;
        _unReadMsgLabel.font = [UIFont systemFontOfSize:11];
        _unReadMsgLabel.textColor = [UIColor whiteColor];
        _unReadMsgLabel.text = @"0";
        _unReadMsgLabel.textAlignment = NSTextAlignmentCenter;
        _unReadMsgLabel.layer.cornerRadius = _unReadMsgLabel.frame.size.width / 2;
        _unReadMsgLabel.layer.masksToBounds = YES;
        [self addSubview:_unReadMsgLabel];
        
        //添加删除按钮
        _deleteBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        CGFloat btnW = 16;
        _deleteBtn.frame = CGRectMake(0, 0, btnW, btnW);
        [_deleteBtn setBackgroundImage:[UIImage imageNamed:@"照片删除@2x.png"] forState:UIControlStateNormal];
        [_deleteBtn addTarget:self action:@selector(deleteButtonPress:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_deleteBtn];
        _deleteBtn.hidden = YES;
        
        //添加用户名称
        CGFloat nameLabel_Y = _headImageView.frame.size.height + _headImageView.frame.origin.y + 8;
        CGFloat nameLabel_H = 14;
        _nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, nameLabel_Y, frame.size.width, nameLabel_H)];
        _nameLabel.textAlignment = NSTextAlignmentCenter;
        _nameLabel.textColor = [UIColor whiteColor];
        _nameLabel.font = [UIFont systemFontOfSize:11];
        _nameLabel.alpha = 0.70;
        _nameLabel.text = @"";
        [self addSubview:_nameLabel];
        
        UITapGestureRecognizer* panGes = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(bgImageviewAction:)];
        panGes.delegate = self;
        [panGes setNumberOfTapsRequired:2];
        [self addGestureRecognizer:panGes];
        
    }
    return self;
}

-(void)handleLongPressFromUp:(UIGestureRecognizer*)recognizer{
    switch (recognizer.state) {
        case UIGestureRecognizerStateBegan:
        {
            //添加按钮的sessionid为空,点击添加按钮的时候不需要放大
            if (self.currentSessionData.pSessionId != nil) {
                [self startAnimation];
            }
            
            beginPoint = [recognizer locationInView:self];
            //长按录制
            if (self.longPressAction) {
                self.longPressAction(recognizer,self.currentSessionData);
            }
        }
            break;
        case UIGestureRecognizerStateChanged:
        {
            if (self.stateChangeAction) {
                
                CGPoint point = [recognizer locationInView:self];
                //手势点在上 -- 取消
                if (point.y < 0) {
                    self.stateChangeAction(upState);
                }
                //手势点在下 -- 编辑
                else if (point.y > self.frame.size.height){
                    self.stateChangeAction(downState);
                }
                else{
                    self.stateChangeAction(normalState);
                }
                
            }
            
        }
            break;
        case UIGestureRecognizerStateEnded:
        {
            [self stopAnimation];
            
            endPoint = [recognizer locationInView:self];
            if (endPoint.y < beginPoint.y - 20) {
                //上划取消
                if (self.topSwipingAction) {
                    self.topSwipingAction(recognizer,self.currentSessionData);
                }
            }else if (endPoint.y > beginPoint.y + 20){
                //下划编辑
                if (self.bottomSwipingAction) {
                    self.bottomSwipingAction(recognizer,self.currentSessionData);
                }
            }else{
                //长按录制
                if (self.longPressAction) {
                    self.longPressAction(recognizer,self.currentSessionData);
                }
            }
        }
            break;
        default:
            break;
    }
}

-(void)headImageView1TapAction:(UIGestureRecognizer*)recognizer{

    if (self.headImageView1TapAction) {
        self.headImageView1TapAction(recognizer,self.currentSessionData);
    }
}

#pragma mark overwrite

-(void)setIsEditStatus:(BOOL)isEditStatus{
    if (isEditStatus && self.currentSessionData.pSessionId) {
        _deleteBtn.hidden = NO;
    }else{
        _deleteBtn.hidden = YES;
    }
}

-(void)setIMPackageSessionData:(IMPackageSessionData* )sessionData withSendingDic:(NSDictionary*)sendingDic{
    self.currentSessionData = sessionData;
    
    //设置名称
    _nameLabel.text = self.currentSessionData.pSessionName;
    //设置头像
    UIImage* logo = [UIImage imageNamed:self.currentSessionData.pPortraitKey];
    if (logo) {
        self.headImageView.image = logo;
    }else{
        NSArray* faceurlArray = [self.currentSessionData.pPortraitKey componentsSeparatedByString:@"###"];
        
        NSString* newUrl = [AppUtils getImageNewUrlWithSrcImageUrl:[faceurlArray objectAtIndex:0] width:self.headImageView.frame.size.width height:self.headImageView.frame.size.height];
        [self.headImageView loadImageWithPath:newUrl andPlaceHolderName:DefalutHeadImage];
        //如果是群组需要轮播头像
        if (IM_MESSAGE_TYPE_GROUP == sessionData.eType) {
            self.headImageView.groupMemberArray = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetGroupMemberListWithGroupID:sessionData.pSessionId];
        }else{
            self.headImageView.groupMemberArray = nil;
        }
        //先置空符合条件再赋值
        self.headImageView.headerVideoPath = nil;
        if ([faceurlArray count]==2){
            self.headImageView.headerVideoPath = [AppUtils compatibleImageKeyWithKey:[faceurlArray objectAtIndex:1]];
        }
    }
    if (sessionData.pSessionId) {
        if (sessionData.nUnreadNum == 0) {
            _unReadMsgLabel.hidden = YES;
        }else{
            _unReadMsgLabel.hidden = NO;
        }
        _unReadMsgLabel.text = [[NSNumber numberWithInteger:sessionData.nUnreadNum] stringValue];
        _headImageView.backgroundColor = [UIColor clearColor];
    }else{
        //如果没有sessionID 说明数据为空则把所有数据职位默认或空
        _unReadMsgLabel.hidden = YES;
        _headImageView.backgroundColor = EveryView_BackGroundColor;
        self.headImageView.headerVideoPath = nil;
        self.headImageView.groupMemberArray = nil;
    }
    
    [self.msgSendStausView removeFromSuperview];
    self.msgSendStausView = nil;
    //如果不为空则说明这个session 有消息正在发送
    if ([sendingDic objectForKey:sessionData.pSessionId]) {
        CGRect rect = self.headImageView.frame;
        rect.origin.x = 0;
        rect.origin.y = 0;
        rect.size.width = self.headImageView.frame.size.width + 4;
        rect.size.height = rect.size.width ;
        self.msgSendStausView = [[WaitingAlertView alloc] initWithFrame:rect mode:WaitingAlertView_Mode_Full];
        [self addSubview:self.msgSendStausView];
    }
    
    //停掉所有的播放
    [self.headImageView stopPlay];
}

-(void)setSendMsgImageWithStatus:(SendMsgStatus)status{
    
    UIImage* image = nil;
    
    //发送前回复到正常大小
    [self stopAnimation];
    
    CGRect rect = self.headImageView.frame;
    rect.origin.x = 0;
    rect.origin.y = 0;
    rect.size.width = self.headImageView.frame.size.width + 4;
    rect.size.height = rect.size.width;
    [self.msgSendStausView removeFromSuperview];
    
    switch (status) {
        case SendMsgStatus_willSend:
        {
            //所有的显示UI全要放在未读消息的下边
            self.msgSendStausView = [[WaitingAlertView alloc] initWithFrame:rect mode:WaitingAlertView_Mode_Full];
            [self insertSubview:self.msgSendStausView belowSubview:_unReadMsgLabel];
        }
            break;
        case SendMsgStatus_didSendSuccess:
        {
            self.msgSendStausView = nil;
            image= [UIImage imageNamed:@"发送完成.png"];
            self.msgSendStausView = [[UIImageView alloc] initWithImage:image];
            self.msgSendStausView.frame= rect;
            [self insertSubview:self.msgSendStausView belowSubview:_unReadMsgLabel];
            [self.msgSendStausView performSelector:@selector(removeFromSuperview) withObject:nil afterDelay:0.5];
        }
            break;
        case SendMsgStatus_didSendFaild:
        {
            [self.msgSendStausView removeFromSuperview];
            self.msgSendStausView = nil;
            image= [UIImage imageNamed:@"发送失败.png"];
            self.msgSendStausView = [[UIImageView alloc] initWithImage:image];
            self.msgSendStausView.frame= rect;
            [self insertSubview:self.msgSendStausView belowSubview:_unReadMsgLabel];
            [self.msgSendStausView performSelector:@selector(removeFromSuperview) withObject:nil afterDelay:0.5];
        }
            break;
        default:
            break;
    }
    
    
}

#pragma mark Action
-(void)bgImageviewAction:(UIGestureRecognizer*)recognizer{
    if (self.bgImageViewAction) {
        self.bgImageViewAction(recognizer,self.currentSessionData);
    }
}

-(void)unReadMsgButtonAction:(id)sender{
    if (self.unReadMsgBtnAction) {
        self.unReadMsgBtnAction(self.currentSessionData,nil);
    }
}

-(void)deleteButtonPress:(id)sender{
    if (self.deleteBtnAction) {
        self.deleteBtnAction(self.currentSessionData);
    }
}

#pragma mark Animation
-(void)startAnimation{
//    [UIView animateWithDuration:0.3 animations:^{
        _headImageView.transform = CGAffineTransformMakeScale(1.2,1.2);
//    }];
}
-(void)stopAnimation{
    _headImageView.transform = CGAffineTransformMakeScale(1.0,1.0);
}

-(void)makeAnimation:(BOOL)make{
    if (make) {
        [UIView animateWithDuration:0.3 animations:^{
            self.transform = CGAffineTransformMakeScale(1.2,1.2);
        } completion:^(BOOL finished) {
            self.transform = CGAffineTransformMakeScale(1.0,1.0);
        }];
    }
}
@end
