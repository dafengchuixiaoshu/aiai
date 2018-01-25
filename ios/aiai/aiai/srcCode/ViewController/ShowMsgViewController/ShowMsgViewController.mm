//
//  ShowMsgViewController.m
//  aiai
//
//  Created by zhangyang on 15/4/13.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ShowMsgViewController.h"
#import "IISettingViewController+SavePhotoAndVideoManager.h"
#import "SelectFilterViewController.h"
#import "RootViewController+InteractionLogic.h"
#import "ShowMsgPlayView+GestureRecognizer.h"
#import "IISettingViewController+SavePhotoAndVideoManager.h"

#import "ShowMsgPlayView.h"
#import "AddressMapHeadView.h"
#import "DistanceView.h"
#import "RootRecordView.h"
#import "OpenPlatformActionSheet.h"

#import "IMPackageData.h"

#define ACTIONSHEETTITLE    @"屏蔽此人消息"
#define ANIMATETIME  0.3

@interface ShowMsgViewController ()<UIGestureRecognizerDelegate,RootRecordViewDelegate,BMKMapViewDelegate,IMPackageEngineDelegate,SelectFilterViewControllerDelegate,OpenPlatformActionSheetDelegate>


@property(nonatomic,copy)NSString* movePath;
@property(nonatomic,copy)NSNumber* moveTime;

@property(nonatomic,strong)ShowMsgPlayView* playView;
@property(nonatomic,strong)RootRecordView* recordView;

@property(nonatomic,strong)DistanceView* disView;
@property(nonatomic,strong)BMKMapView* mapView;
@property(nonatomic,strong)AddressMapHeadView* addressMapHeadView;
@property(nonatomic,strong)AddressMapHeadAnnotation* userAnnotation;

@property(nonatomic,assign)BOOL addressMapHeadViewEnableLongPressAndTap;//头像是否支持单击和长按
@property(nonatomic,assign)BOOL isEdit; //判断是否在编辑

@property(nonatomic,strong)IMPackageSessionData* sessionData;
@property(nonatomic,strong)IMPackageMessageData* currentMsgData;
@property(nonatomic,strong)NSMutableArray* unReadMsgArray;
@end

@implementation ShowMsgViewController
@synthesize finishBlock;
@synthesize disView = _disView;
@synthesize recordView = _recordView;
@synthesize sendVideoBlock,sendImgBlock;

-(id)initWithIMPackageSessionData:(IMPackageSessionData*)sessionData{
    if (self = [super init]) {
        self.sessionData = sessionData;
        self.unReadMsgArray = [[NSMutableArray alloc] initWithArray:[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetMessageListWithSessionID:sessionData.pSessionId LastMsgID:@"" pageSize:sessionData.nUnreadNum]];
        self.currentMsgData = [self.unReadMsgArray objectAtIndex:0];
        self.addressMapHeadViewEnableLongPressAndTap = NO;
        //添加代理，需要使用收发消息代理
        [[IMPackageEngine sharedInstanceIMPackageEngine] addDelegate:self];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Do any additional setup after loading the view.
    self.centerBtnImageName = @"flashon_open@2x.png";
    //如果是个人消息则能够屏蔽此人消息
    if (IM_MESSAGE_TYPE_PERSONAL == self.currentMsgData.eType) {
        self.rightBtnImageName = @"更多";
    }
    
    __weak ShowMsgViewController* weakself = self;
    //初始化视频播放视图
    _playView = [[ShowMsgPlayView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.width)];
    _playView.unReadMsgCount = self.sessionData.nUnreadNum;
    _playView.unReadMsgButtonBlock = ^{
        [weakself unReadMsgButtonAction];
    };
    _playView.shareBlock = ^(){
        
        OpenPlatformActionSheet *sheet = [[OpenPlatformActionSheet alloc] initWithTitle:nil delegate:weakself cancelButtonTitle:nil ShareButtonTitles:[NSArray arrayWithObjects:@"微信好友",@"朋友圈" ,@"微博", @"QQ空间", nil] withShareButtonImagesName:[NSArray arrayWithObjects:@"weixin_friend_able.png", @"weixin_circle_able.png",@"sina_weibo_able.png" ,@"qq_zone_able.png" , nil]];
        [sheet showInView:weakself.view];
        
    };
    _playView.saveBlock = ^(NSString *videoPath){
        //保存
        if (IM_SENDMSG_TYPE_IMAGE == weakself.currentMsgData.eCategory) {
            [IISettingViewController saveImageToAlbum:weakself.playView.showImg];
        }
        else if (IM_SENDMSG_TYPE_VIDEO){
            [IISettingViewController saveVedioToAlbum:[NSURL fileURLWithPath:videoPath]];
        }

    };
    if (self.currentMsgData.pVideoKey == nil) {
        NSString* imageUrl = [AppUtils compatibleImageKeyWithKey:self.currentMsgData.pImgKey];
        [_playView setImageUrl:imageUrl];
    }else{
        NSString* vedioUrl = [AppUtils compatibleImageKeyWithKey:self.currentMsgData.pVideoKey];
        [_playView setVedioUrl:vedioUrl];
    }
    [_playView setContentText:self.currentMsgData.ptext];
    [_playView shareBtnHidden:self.currentMsgData.pIsShare == 1?NO:YES];
    [self.view insertSubview:_playView atIndex:0];
    
    //初始化地图
    CGFloat mapView_X = 0;
    CGFloat mapView_Y = _playView.frame.size.height+_playView.frame.origin.y;
    CGFloat mapView_W = self.view.frame.size.width;
    CGFloat mapView_H = self.view.frame.size.height - _playView.frame.size.height;
    _mapView = [[BMKMapView alloc]initWithFrame:CGRectMake(mapView_X, mapView_Y, mapView_W, mapView_H)];
    _mapView.delegate = (id)self;
    _mapView.scrollEnabled = NO;
    _mapView.zoomEnabledWithTap = NO;
    _mapView.zoomEnabled = NO;
    _mapView.rotateEnabled = NO;
    _mapView.overlookEnabled = NO;
    _mapView.zoomLevel = MAPZOOMLEVEL;
    CLLocationCoordinate2D loc;
    loc.longitude = self.currentMsgData.longitude;
    loc.latitude = self.currentMsgData.latitude;
    [_mapView setCenterCoordinate:loc animated:NO];
    [self.view insertSubview:_mapView atIndex:0];
    
    UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(doubleTap:)];
    [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
    [_mapView addGestureRecognizer:doubleTapGestureRecognizer];
    
    //创建距离的view
    _disView = [[DistanceView alloc] initWithFrame:CGRectMake(_mapView.frame.origin.x, _mapView.frame.origin.y, 100, 100)];
    [_disView setNickName:self.currentMsgData.pSenderName];
    [_disView setDistanceWithBuddyLatitude:self.currentMsgData.latitude AndBuddyLongitude:self.currentMsgData.longitude AndTime:self.currentMsgData.pTime];
    [self.view insertSubview:_disView atIndex:1];
    [_disView bringSubviewToFront:_mapView];
    
    _userAnnotation = [[AddressMapHeadAnnotation alloc] init];
    _userAnnotation.coordinate = loc;
    _userAnnotation.title = self.currentMsgData.pSender;
    NSArray* faceurlArray = [self.sessionData.pPortraitKey componentsSeparatedByString:@"###"];
    NSString* newUrl = [AppUtils getImageNewUrlWithSrcImageUrl:[faceurlArray objectAtIndex:0] width:_addressMapHeadView.frame.size.width height:_addressMapHeadView.frame.size.height];
    _userAnnotation.headImageUrl = newUrl;
    if ([faceurlArray count]==2){
        _userAnnotation.headVedioUrl = [faceurlArray objectAtIndex:1];
    }
    [_mapView addAnnotation:_userAnnotation];
    
    _addressMapHeadView = [[AddressMapHeadView alloc] initWithAnnotation:_userAnnotation reuseIdentifier:nil];
    
    CGRect rect = _addressMapHeadView.frame;
    rect.origin.x = _mapView.frame.origin.x+_mapView.frame.size.width/2-rect.size.width/2;
    rect.origin.y = _mapView.frame.origin.y+_mapView.frame.size.height/2-rect.size.height;
    _addressMapHeadView.frame = rect;
    _addressMapHeadView.hidden = YES;
    [self.view addSubview:_addressMapHeadView];
    
    __weak AddressMapHeadView *weakMapHeader = _addressMapHeadView;
    _addressMapHeadView.longPressBlock = ^(UIGestureRecognizer* recognizer, EditState state){
        if (weakself.addressMapHeadViewEnableLongPressAndTap == NO) {
            return;
        }
        
        //好友头像长按事件响应：
        if (recognizer.state == UIGestureRecognizerStateBegan) {
            //录制
            [weakself.recordView startRecordView];
            
        }else if (recognizer.state == UIGestureRecognizerStateEnded){
            
            [weakself.recordView stopProgressAnimation];
            [weakself.recordView resetProgressView];
            switch (state) {
                case normalState:
                {
                    //停止录制
                    [weakself.recordView stopRecordView];
                }
                    break;
                case upState:
                {
                    //重置摄像头
//                    [weakself.recordView resumeCapture];
                }
                    break;
                case downState:
                {
                    //停止录制,回调会进入编辑界面
                    [weakself.recordView stopRecordView];
                }
                    break;
                    
                default:
                    break;
            }
            if (state == normalState) {
                
            }
        }
        
    };
    
    __weak ShowMsgPlayView *weakPlayView = _playView;
    _addressMapHeadView.tap1Block = ^(UIGestureRecognizer* recognizer){
        if (weakself.addressMapHeadViewEnableLongPressAndTap == NO) {
            
            //如果为查看图片状态 点击头像进入拍照界面
            [weakPlayView changeCaptureState];
            [weakself.recordView startRecordView];
            weakself.addressMapHeadViewEnableLongPressAndTap = YES;
            [weakself.view bringSubviewToFront:weakself.addressMapHeadView];
            return;
        }
        
        //切换摄像头type，进行视频截图
        [weakself.recordView capturePhone];
    };
    _addressMapHeadView.tap2Block = ^(UIGestureRecognizer* recognizer){
        [weakMapHeader playHeaderAudio];
    };
    _addressMapHeadView.changeStateBlock = ^(EditState state){
        [weakself changeMsgTextWithState:state];
    };
    
    _playView.moveBottomBlock = ^(BOOL isBottom){
        if (isBottom) {
            weakMapHeader.hidden = NO;
        }else{
            weakMapHeader.hidden = YES;
        }
        weakself.addressMapHeadViewEnableLongPressAndTap = isBottom;
        [weakself.view bringSubviewToFront:weakself.addressMapHeadView];
    };
    
    //创建完视图把title提到最前边
    [self bringTitleViewToFront];
}

-(void)viewDidAppear:(BOOL)animated{
    [super viewDidAppear:animated];
    //添加录制视图
    _recordView = [[RootRecordView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.width)];
    [_recordView needShowTextBtn:NO];
    _recordView.delegate = self;
    [self.view insertSubview:_recordView atIndex:0];
    [_recordView startRunCapture];
}

-(void)viewWillDisappear:(BOOL)animated{
    
    [super viewWillDisappear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] removeDelegate:self];
    
    [_recordView stopRunCapture];
    [_recordView removeFromSuperview];
}

-(void)dealloc{
    AIAI_LOG(@"ShowMsgViewController dealloc");
    [_playView releaseResource];
}

-(void)changeMsgTextWithState:(EditState)state{
    switch (state) {
        case normalState:
        {
            self.isEdit = NO;
            [_recordView setMsgLabelText:@"上划取消，下划编辑"];
        }
            break;
        case upState:
        {
            self.isEdit = YES;
            [_recordView setMsgLabelText:@"松开取消"];
        }
            break;
        case downState:
        {
            self.isEdit = YES;
            [_recordView setMsgLabelText:@"松开编辑"];
        }
            break;
        default:
            break;
    }
}


#pragma mark title按钮响应
-(void)leftItemClicked:(id)sender{
    
    //点返回按钮 当前的这条消息不用删除同android
    
    //通知主界面要离开了，需要主界面刷新
    if (self.delegate && [self.delegate respondsToSelector:@selector(willDisappear:)]) {
        [self.delegate willDisappear:self];
    }
    //地图代理置空
    _mapView.delegate = nil;

    //释放资源文件
    [_playView releaseResource];
    //离开
    [super leftItemClicked:sender];
}

-(void)centerItemClicked:(id)sender{
    if ([self.recordView isFrontCamera]) {
        return;
    }
    UIButton* flashBtn = (UIButton*)sender;
    flashBtn.selected = !flashBtn.selected;
    if(!flashBtn.isSelected){
        self.centerBtnImageName = @"flashon_open.png";
    }else{
        self.centerBtnImageName = @"flashon_close.png";
    }
    
    [self.recordView openFlash:flashBtn.selected];
    
}
-(void)rightItemClicked:(id)sender{
//    [self.recordView switchCamera];
//    
//    self.centerBtnImageName = @"flashon_open.png";
    
    UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:nil delegate:(id)self cancelButtonTitle:@"取消" destructiveButtonTitle:nil otherButtonTitles:ACTIONSHEETTITLE, nil];
    [sheet showInView:self.view];
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex{
    if ([[actionSheet buttonTitleAtIndex:buttonIndex] isEqualToString:ACTIONSHEETTITLE]) {
        
        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_SetBlackListWithId:self.currentMsgData.pSessionId IM_BlackListType:IM_BL_ADD];
        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_DeleteBuddyWihUserID:self.currentMsgData.pSessionId];
    }
}

#pragma mark IM相关未读等
-(void)unReadMsgButtonAction{
    //删除已经阅读过的消息
    [self deleteReadedMsgWithIMPackageMessageData:self.currentMsgData];
    
    //获取下一条消息
    if ([self.unReadMsgArray count] == 0) {
        dispatch_async(dispatch_get_main_queue(), ^{
            //读完最后一条消息后清空session
            [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_ClearSession:self.currentMsgData.pSessionId];
            
            //通知主界面刷新
            if (finishBlock) {
                finishBlock(self.currentMsgData);
            }
            [self.navigationController popViewControllerAnimated:YES];
        });
        return;
    }
    self.currentMsgData = [self.unReadMsgArray objectAtIndex:0];
    
    //重新设置昵称,距离和时间
    [_disView setNickName:self.currentMsgData.pSenderName];
    [_disView setDistanceWithBuddyLatitude:self.currentMsgData.latitude AndBuddyLongitude:self.currentMsgData.longitude AndTime:self.currentMsgData.pTime];
    
    //设置地图中心点
    [_mapView setCenterCoordinate:CLLocationCoordinate2DMake(self.currentMsgData.latitude, self.currentMsgData.longitude) animated:YES];
    
    //调整anntionview的位置
    [_mapView removeAnnotation:_userAnnotation];
    _userAnnotation.coordinate = CLLocationCoordinate2DMake(self.currentMsgData.latitude, self.currentMsgData.longitude);
    _userAnnotation.title = self.currentMsgData.pSender;
    [_mapView addAnnotation:_userAnnotation];
    
    //设置消息是否可分享
    [_playView shareBtnHidden:self.currentMsgData.pIsShare == 1?NO:YES];
    
    //进行播放，图片，视频和文字
    if (self.currentMsgData.pVideoKey == nil) {
        NSString* imageUrl = [AppUtils compatibleImageKeyWithKey:self.currentMsgData.pImgKey];
        [_playView setImageUrl:imageUrl];
    }else{
        NSString* vedioUrl = [AppUtils compatibleImageKeyWithKey:self.currentMsgData.pVideoKey];
        [_playView setVedioUrl:vedioUrl];
    }
    [_playView setContentText:self.currentMsgData.ptext];
    
    //已读信息减少一个
    _playView.unReadMsgCount -= 1;
    self.sessionData.nUnreadNum -= 1;
}

-(void)deleteReadedMsgWithIMPackageMessageData:(IMPackageMessageData*)deleteData{
    NSString* imagePath = [NSString stringWithFormat:@"%@/%@",IM_SAVEPATH,deleteData.pImgKey];
    NSString* vedioPath = [NSString stringWithFormat:@"%@/%@",IM_SAVEPATH,deleteData.pVideoKey];
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_DeleteOneMessageWithSessionID:deleteData.pSessionId msgID:deleteData.pMsgId imageKey:imagePath vedioKey:vedioPath];
    //同时删除数组中的数据
    [self.unReadMsgArray removeObject:deleteData];
}

- (void)IMPackageRecvMessageType:(int)detaileType msgData:(IMPackageMessageData*)msgModel{
    
    AIAI_LOG(@"收到消息");
    
    dispatch_async(dispatch_get_main_queue(), ^{
        //在此界面收到新消息
        [_unReadMsgArray addObject:msgModel];
        _playView.unReadMsgCount += 1;
    });
}

#pragma mark implement BMKMapViewDelegate
// 根据anntation生成对应的View
- (BMKAnnotationView *)mapView:(BMKMapView *)mapView viewForAnnotation:(id <BMKAnnotation>)annotation
{
    NSString *AnnotationViewID = @"AnnotationViewID";
    AddressMapHeadView* annotationView = (AddressMapHeadView *)[mapView dequeueReusableAnnotationViewWithIdentifier:AnnotationViewID];
    if (annotationView == nil) {
        annotationView = [[AddressMapHeadView alloc] initWithAnnotation:annotation reuseIdentifier:AnnotationViewID];
        annotationView.canShowCallout = NO;
        annotationView.centerOffset = annotationView.offset;
    }
    return annotationView;
}

-(void)doubleTap:(UIGestureRecognizer*)gesture{
    
    //全屏
    if (_mapView.frame.size.height > _mapView.frame.size.width) {
        
        [UIView animateWithDuration:ANIMATETIME animations:^{
            CGFloat close_y = _playView.frame.size.height+_playView.frame.origin.y;
            _mapView.frame = CGRectMake(0, close_y, self.view.frame.size.width, self.view.frame.size.height - close_y);
            _disView.frame = CGRectMake(0, close_y, _disView.frame.size.width, _disView.frame.size.height);
        } completion:^(BOOL finished) {
            [self.view sendSubviewToBack:_mapView];
            [self.view bringSubviewToFront:_disView];
            _mapView.scrollEnabled = NO;
            _mapView.zoomEnabledWithTap = NO;
            _mapView.zoomEnabled = NO;
            [_mapView setCenterCoordinate:CLLocationCoordinate2DMake(self.currentMsgData.latitude, self.currentMsgData.longitude) animated:YES];
        }];
    }
    else{
        [UIView animateWithDuration:ANIMATETIME animations:^{
            CGFloat open_y = 20;
            _mapView.frame = CGRectMake(0, open_y, self.view.frame.size.width, self.view.frame.size.height - open_y);
            _disView.frame = CGRectMake(0, open_y, _disView.frame.size.width, _disView.frame.size.height);
            [self.view bringSubviewToFront:_mapView];
        } completion:^(BOOL finished) {
            [self.view bringSubviewToFront:_disView];
            _mapView.scrollEnabled = YES;
            _mapView.zoomEnabled = YES;
            _mapView.zoomEnabledWithTap = NO;
            [_mapView setCenterCoordinate:CLLocationCoordinate2DMake(self.currentMsgData.latitude, self.currentMsgData.longitude) animated:YES];
        }];
    }
}

#pragma mark RecordViewDelegate
-(void)didRecordFinish:(NSURL*)videoUrl videoTime:(CGFloat)time{
    //保存视频
    [IISettingViewController saveVideoWithSetting:videoUrl];
    //发送消息
    IMPackageSessionData* sessionData = self.sessionData;
    NSString* videoTitle = [NSString stringWithFormat:@"%@发送",[GlobalUserInfo myselfInfo].userName];
    NSString* text = _recordView.inputText;
    
    if (self.isEdit) {
        //进入视频编辑界面
        SelectFilterViewController* con = [[SelectFilterViewController alloc] initWithVideoUrl:videoUrl andTime:time];
        con.delegate = self;
        con.sessionData = self.sessionData;
        [self.navigationController pushViewController:con animated:YES];
    }
    else{
        if (sendVideoBlock) {
            sendVideoBlock(videoUrl,time,videoTitle,text,sessionData.eType);
        }
        [AppUtils CaptureAnimationWithImageView:[AppUtils getImage:videoUrl] startPoint:self.recordView.center endPoint:self.addressMapHeadView.center runView:self.view];
    }
    //需要把输入框设置成空
    [self.recordView resetInputText];
    [self.recordView resetGraffitView];
}

-(void)didTakePhotoFinish:(UIImage *)photo{
    //保存照片
    [IISettingViewController saveImageWithSetting:photo];
    [AppUtils CaptureAnimationWithImageView:photo startPoint:self.recordView.center endPoint:self.addressMapHeadView.center runView:self.view];
    //发送消息
    IMPackageSessionData* sessionData = self.sessionData;
    NSString* text = _recordView.inputText;
    if (sendImgBlock) {
        sendImgBlock(photo, text, sessionData.eType);
    }
    //需要把输入框设置成空
    [self.recordView resetInputText];
    [self.recordView resetGraffitView];
}

#pragma mark SelectFilterViewControllerDelegate
-(void)SelectFilterViewControllerDidEditMovieUrl:(NSURL*)editMovieUrl movieTime:(CGFloat)time sessionData:(IMPackageSessionData*)sessionData{
    
    //发送消息
    NSURL* vedioPath = editMovieUrl;
    CGFloat vedioTime = time;
    NSString* videoTitle = [NSString stringWithFormat:@"%@发送",[GlobalUserInfo myselfInfo].userName];
    NSString* text = self.recordView.inputText;
    [RootViewController sendVedioWithUserinfo:sessionData vedioUrl:vedioPath vedioTime:vedioTime videoTitle:videoTitle text:text msgType:sessionData.eType completion:nil];
}

#pragma mark 第三方分享
- (void)didClickOnImageIndex:(NSInteger)imageIndex {
    
    NSString *webpageUrl = [NSString stringWithFormat:@"%@shareMsg.pb?msgId=%@", @"http://182.92.189.13:11080/SpreeCMS/", self.currentMsgData.pMsgId];
    //微信好友
    if (0 == imageIndex) {
        [OpenPlatformServers shareToWeiXinLinkContent:@"" title:@"来自ii客户端" thumbImage:self.playView.showImg webpageUrl:webpageUrl scene:0];
    }
    //微信朋友圈
    else if (1 == imageIndex) {
        [OpenPlatformServers shareToWeiXinLinkContent:@"" title:@"来自ii客户端" thumbImage:self.playView.showImg webpageUrl:webpageUrl scene:1];
    }
    //微博
    else if (2 == imageIndex) {
        [OpenPlatformServers shareToWeiBoWithMessage:@"来自ii客户端" image:self.playView.showImg mediaURL:webpageUrl];
    }
    //qq空间
    else if (3 == imageIndex) {
        [OpenPlatformServers shareToQQLinkContent:@"" title:@"来自ii客户端" thumbImage:self.playView.showImg webpageUrl:webpageUrl type:0];
    }
}
@end
