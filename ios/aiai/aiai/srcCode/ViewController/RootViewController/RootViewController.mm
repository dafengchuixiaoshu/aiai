//
//  RootViewController.m
//  aiai
//
//  Created by zhangyang on 14/12/13.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "RootViewController.h"
#import "RootViewController+InteractionLogic.h"
#import "RootViewController+VideoMarkView.h"
#import "MenuViewController.h"
#import "DownLoadFontViewController.h"
#import "VedioIntervalEditViewController.h"
#import "SelectFilterViewController.h"
#import "ShowMsgViewController.h"
#import "TSAssetsPickerController.h"

#import "RootRecordView.h"
#import "ScrollPaggingContainTable_View.h"

#import "AddressMapView.h"
#import "PlayMovieView.h"
#import "ContactHeadLogoConllectionCell.h"
#import "TipsView.h"

#import "IISettingViewController+SavePhotoAndVideoManager.h"
#import "FilterManager.h"


#import "PostViewController.h"

@interface RootViewController ()<UIGestureRecognizerDelegate,SelectFilterViewControllerDelegate,RootRecordViewDelegate>

//录制视图
@property(nonatomic,strong)RootRecordView* recordView;

//联系人，地图视图
@property(nonatomic,strong)ScrollPaggingContainTable_View* containView;
@property(nonatomic,strong)AddressMapView* addressMapView;

//头像
@property(nonatomic,strong)PlayMovieView* headPlayView;

//菜单
@property(nonatomic,strong) MenuViewController* menuSidebarVc;

//开始录制的时候需要保存一下用户数据，记录当前的用户
@property(nonatomic,strong)IMPackageSessionData* clickSessionData;
//记录是否需要编辑，判断是直接发送还是进入编辑界面
@property(nonatomic,assign)BOOL isEdit;

//记录是否为主动分享
@property(nonatomic,assign)BOOL isActiveShare;

//获取联系人的回调判断,im获取联系人的回调很多地儿都会有,为了避免错误加参数区分
@property(nonatomic,assign)BOOL isInsertSession;

//动画使用
@property(nonatomic, assign)CGPoint currentPoint;

@end

@implementation RootViewController

-(id)init{
    if (self = [super init]) {
        
        [self createLoginLogic];
        _isInsertSession = NO;
        
    }
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    
    self.leftBtnImageName = @"menu.png";
    self.centerBtnImageName = @"flashon_open@2x.png";
    self.rightBtnImageName = @"changecamera@2x.png";
    _isActiveShare = NO;
    
    
    
    __weak RootViewController* weakself = self;
    
    self.recordView = [[RootRecordView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.width)];
    self.recordView.delegate = self;
    [self.recordView needShowTextBtn:YES];
    self.recordView.downLoadFontBlock = ^(){
        
        DownLoadFontViewController *con = [[DownLoadFontViewController alloc] init];
        [weakself.navigationController pushViewController:con animated:YES];
        
    };
//    [self.recordView startRunCapture];
    [self.view insertSubview:self.recordView atIndex:0];//放在最下边，要不title会被遮盖

    //创建联系人相关UI
    //创建容器视图
    CGFloat containView_Y = self.recordView.frame.origin.y + self.recordView.frame.size.height ;
    CGFloat containView_W = self.view.frame.size.width;
    CGFloat containView_H = self.view.frame.size.height - containView_Y;
    _containView = [[ScrollPaggingContainTable_View alloc] initWithFrame:CGRectMake(0, containView_Y, containView_W, containView_H)];
    _containView.backgroundColor = EveryView_BackGroundColor;
    _containView.isScroll = YES;
    _containView.didChangedPageCompleted = ^(NSInteger currentPage, UIView* subView){
        //滑动显示地图
        if ([LoginMsgCenter isLogin] && currentPage == 1 && [weakself.addressMapView.nearFriends count] == 0){
            //获取附近人信息
            [LoginMsgCenter getNearFriendsWithUserInfo:[GlobalUserInfo myselfInfo] comlate:^(NSArray* friendsList){
                //在地图上更新附近的好友
                weakself.addressMapView.nearFriends = friendsList;
            }];
        }
    };
    
    NSMutableArray* viewArray = [[NSMutableArray alloc] initWithCapacity:2];
    //创建联系人视图
    _contactView = [[ContactParentView alloc] initWithFrame:CGRectMake(0, 0, _containView.frame.size.width, _containView.frame.size.height-10)];//为了视图本身避开pagecontrol
    _contactView.isScroll = NO;
    _contactView.contactArray = [NSMutableArray array];
    ContactParentView *weakContactView = _contactView;
    _contactView.bgImageBtnBlock = ^{
        if (weakself.containView.frame.size.height>weakself.containView.frame.size.width) {
            [weakself openOrCloseContactView:NO];
            [weakself openOrCloseMapView:NO];
        }else{
            [weakself openOrCloseContactView:YES];
            [weakself openOrCloseMapView:YES];
        }
    };
    _contactView.topSwipingBlock = ^(UIGestureRecognizer *gesture,IMPackageSessionData* sessionData,NSIndexPath* indexPath){
        AIAI_LOG(@"取消录制");
        //重置摄像头
        [weakself.recordView resumeRecordView];
    };
    _contactView.bottomSwipingBtnBlock = ^(UIGestureRecognizer *gesture,IMPackageSessionData* sessionData,NSIndexPath* indexPath){
        
        AIAI_LOG(@"进行编辑");
        //下滑需要编辑
        weakself.isEdit = YES;
        
        //停止录制,回调会进入编辑界面
        [weakself.recordView stopRecordView];
    };
    _contactView.longPressBtnBlock = ^(UIGestureRecognizer *gesture,IMPackageSessionData* sessionData,NSIndexPath* indexPath){
        
        //首先重置变量
        _isActiveShare = NO;
        
        if (sessionData.pSessionId == nil) {
            if ([sessionData.pSessionName isEqualToString:ContactEntity_Text_NewBuildChat]) {
                
                return ;
            }else{
                //为主动分享
                _isActiveShare = YES;
            }
        }
        
        if (!weakContactView.isEditStatus){
            weakself.currentPoint = [gesture locationInView:weakself.view];
            if (gesture.state == UIGestureRecognizerStateBegan) {
                //记录一下正在录制的用户
                weakself.clickSessionData = sessionData;
                
                [weakself.recordView startRecordView];
                
            }else if (gesture.state == UIGestureRecognizerStateEnded){
                
                AIAI_LOG(@"发送视频");
                //直接抬起不需要编辑
                weakself.isEdit = NO;
                //停止录制
                [weakself.recordView stopRecordView];
            }
        }
    };
    _contactView.tap1Block = ^(UIGestureRecognizer *gesture,IMPackageSessionData* sessionData,NSIndexPath* indexPath){
        
        //拍照前重置主界面按钮
        [weakself.recordView finishGraffitAction];
        //首先重置变量
        _isActiveShare = NO;
        
        if (sessionData.pSessionId == nil) {
            
            if ([sessionData.pSessionName isEqualToString:ContactEntity_Text_NewBuildChat]) {
                //添加联系人
                AIAI_LOG(@"添加联系人");
                [weakself presentContactViewController];
            }else{
                //为主动分享
                _isActiveShare = YES;
            }
            
        }
        
        if (weakContactView.isEditStatus) {
            
            for (IMPackageSessionData* tempSessionData in weakContactView.contactArray) {
                if (sessionData.pSessionId == nil && ![sessionData.pSessionName isEqualToString:ContactEntity_Text_NewBuildChat]) {
                    if ([tempSessionData.pSessionName isEqualToString:sessionData.pSessionName]) {
                        
                        [weakContactView.contactArray removeObject:tempSessionData];
                        [weakContactView.contactArray insertObject:tempSessionData atIndex:0];
                        [weakContactView.myCollectionView reloadData];
                        return ;
                    }
                }
                if ([tempSessionData.pSessionId isEqualToString:sessionData.pSessionId]) {
                    
                    [weakContactView.contactArray removeObject:tempSessionData];
                    [weakContactView.contactArray insertObject:tempSessionData atIndex:0];
                    [weakContactView.myCollectionView reloadData];
                    return ;
                }
            }
            
        }else{
            weakself.currentPoint = [gesture locationInView:weakself.view];
            //sessionID == nil 说明是添加按钮
            if (sessionData.pSessionId != nil || (sessionData.pSessionId == nil && ![sessionData.pSessionName isEqualToString:ContactEntity_Text_NewBuildChat])){
                if (sessionData.nUnreadNum > 0) {
                    AIAI_LOG(@"查看未读消息");
                    ShowMsgViewController* vc = [[ShowMsgViewController alloc] initWithIMPackageSessionData:sessionData];
                    vc.finishBlock = ^(IMPackageMessageData* currentMsgData){
                        [weakself.contactView readFinishCollectionCellWithIMPackageMessageData:currentMsgData];
                    };
                    vc.sendVideoBlock = ^(NSURL *videoUrl, CGFloat time, NSString *videoTitle, NSString *text, IM_MESSAGE_TYPE type)
                    {
                        [weakself sendVideoWithPath:videoUrl AndLength:time];
                    };
                    vc.sendImgBlock = ^(UIImage *sendImg, NSString *text, IM_MESSAGE_TYPE type){
                        [RootViewController sendImageWithUserinfo:sessionData firstFrameImage:sendImg text:text msgType:type completion:^{
             
                        }];
                    };
                    vc.delegate = weakself;
                    [weakself.navigationController pushViewController:vc animated:YES];
                }else{
                    AIAI_LOG(@"单击发送图片");
                    
                    //记录一下正在录制的用户
                    weakself.clickSessionData = sessionData;
                    //切换摄像头type，进行视频截图
                    [weakself.recordView capturePhone];
                }
            }
        }
    };
    //双击事件不用实现，头像自带了
    
    //删除按钮响应事件
    _contactView.deleteBlock = ^(IMPackageSessionData* sessionData){
        AIAI_LOG(@"deleteBlock");
        [weakself deleteSessionWithSessionData:sessionData];
    };
    _contactView.unReadMsgBlock = ^(IMPackageSessionData* sessionData,NSArray* unReadMsgArray){
        AIAI_LOG(@"unReadMsgBlock");
        ShowMsgViewController* vc = [[ShowMsgViewController alloc] initWithIMPackageSessionData:sessionData];
        [weakself.navigationController pushViewController:vc animated:YES];
    };
    
    _contactView.stateChangeBlock = ^(EditState state){
        [weakself changeMsgTextWithState:state];
    };
    [viewArray addObject:_contactView];
    
    //创建联系人在地图上的位置的视图
    _addressMapView = [[AddressMapView alloc] initWithFrame:_contactView.frame];
    _addressMapView.backgroundColor = [UIColor clearColor];
    _addressMapView.openOrCloseMapBtnAction = ^{
        if (weakself.containView.frame.size.height>weakself.containView.frame.size.width) {
            [weakself openOrCloseContactView:NO];
            [weakself openOrCloseMapView:NO];
        }else{
            [weakself openOrCloseContactView:YES];
            [weakself openOrCloseMapView:YES];
        }
    };
    _addressMapView.sendImgBlick = ^(NSString *sessionId, UIGestureRecognizer *gesture){
        
        //首先重置变量
        _isActiveShare = NO;
        weakself.currentPoint = [gesture locationInView:weakself.view];
        
        //创建sessionModel 只需要set这两个参数
        IMPackageSessionData *sessionData = [[IMPackageSessionData alloc] init];
        sessionData.pSessionId = sessionId;
        sessionData.eType = IM_MESSAGE_TYPE_PERSONAL;
        weakself.clickSessionData = sessionData;
        
        //加入新的session
        [weakself insertNewSession:sessionId];
        
        //切换摄像头type，进行视频截图
        [weakself.recordView capturePhone];
    };
    _addressMapView.sendVideoBlock = ^(NSString *sessionId, UIGestureRecognizer* gesture, EditState editState){
        weakself.currentPoint = [gesture locationInView:weakself.view];
        //首先重置变量
        _isActiveShare = NO;
        if (gesture.state == UIGestureRecognizerStateBegan) {
            //记录一下正在录制的用户
            weakself.clickSessionData = nil;
            weakself.clickSessionData = [[IMPackageSessionData alloc] init];
            weakself.clickSessionData.pSessionId = sessionId;
            weakself.clickSessionData.eType = IM_MESSAGE_TYPE_PERSONAL;
            
            //加入新的session
            [weakself insertNewSession:sessionId];
            
            [weakself.recordView startRecordView];
            
        }else if (gesture.state == UIGestureRecognizerStateEnded){
            switch (editState) {
                case normalState:
                {
                    AIAI_LOG(@"发送视频");
                    //直接抬起不需要编辑
                    weakself.isEdit = NO;
                    //停止录制
                    [weakself.recordView stopRecordView];
                    
                    //需要把输入框设置成空
                    [weakself.recordView resetInputText];
                }
                    break;
                case upState:
                {
                    //重置摄像头
                    [weakself.recordView resumeRecordView];
                }
                    break;
                case downState:
                {
                    //下滑需要编辑
                    weakself.isEdit = YES;
                    
                    //停止录制,回调会进入编辑界面
                    [weakself.recordView stopRecordView];
                }
                    break;
                    
                default:
                    break;
            }
            
        }
    };
    _addressMapView.changeStateBlock = ^(EditState state){
        
        [weakself changeMsgTextWithState:state];
    };
    [viewArray addObject:_addressMapView];
    
    _containView.views = viewArray;
    [_containView reloadData];
    self.view.backgroundColor = [UIColor clearColor];
    [self.view addSubview:_containView];

    //左菜单手势
    UIPanGestureRecognizer* panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panDetected:)];
    panGesture.delegate = self;
    [self.view addGestureRecognizer:panGesture];
    
    //刷新联系人
    [self reloadSessionContactView];
    
    //添加左菜单
    self.menuSidebarVc = [[MenuViewController alloc] init];
    [self.menuSidebarVc setBgR:0 G:0 B:0];
    [self.view addSubview:self.menuSidebarVc.view];
    self.menuSidebarVc.view.frame = self.view.bounds;
    
    //添加和好友聊天的通知
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(insertNewBuddySessionNotify:) name:SENDMSG_NOTIFY object:nil];
    
    //添加设置项
    [self setImgSetting];
    
    //第一次登录提示
    if (![SaveManager valueWithKey:NEEDSHOWTIPS]) {
        [SaveManager saveValue:[NSNumber numberWithBool:YES] forKey:NEEDSHOWTIPS];
        //添加tips
        TipsView *tips = [[TipsView alloc] initWithFrame:self.view.bounds];
        tips.backgroundColor = [UIColor clearColor];
        [self.view addSubview:tips];   
    }
    
}


-(void)viewWillAppear:(BOOL)animated{
    //使用主线程延迟来获取音源句柄
    [UIView animateWithDuration:0.05 animations:^{
        self.recordView.alpha = 1.0;
    } completion:^(BOOL finished) {
        if (self.recordView) {
            [self.recordView startRunCapture];
        }
    }];
    
    //显示操作按钮
    [self.recordView needShowTextBtn:YES];
}

-(void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    
    if (self.recordView) {
        [self.recordView stopRunCapture];
        self.recordView.alpha = 0.0;
    }
}

-(void)insertNewSession:(NSString*)sessionId{
    
    //从服务器获取buddy数据
    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetContactWithUserId:sessionId isFromServer:YES];
    
    //im获取联系人的回调很多地儿都会有,为了避免错误加参数区分
    _isInsertSession = YES;
}

//在回调中获取数据
-(void)IMPackageGetBuddyDataSuccess:(IMPackageBuddyData*)buddyData{
    
    if (buddyData && _isInsertSession) {
        dispatch_async(dispatch_get_main_queue(), ^{
            _isInsertSession = NO;
            IMPackageSessionData* newSession = [[IMPackageSessionData alloc] initWithIMPackageBuddyData:buddyData];
            NSMutableArray* tmpArray = [NSMutableArray arrayWithArray:self.contactView.contactArray];
            BOOL isExist = NO;
            for (int i = 0;i<[tmpArray count];i++) {
                IMPackageSessionData* oneData = [tmpArray objectAtIndex:i];
                if ([oneData.pSessionId isEqualToString:newSession.pSessionId]) {
                    [tmpArray exchangeObjectAtIndex:i withObjectAtIndex:0];
                    isExist = YES;
                    break;
                }
            }
            if (isExist == NO) {
                [tmpArray insertObject:newSession atIndex:0];
            }
            self.contactView.contactArray = tmpArray;
            [self.contactView.myCollectionView reloadData];
        });
    }
    
}

#pragma mark UIGestureRecognizer
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    if (self.menuSidebarVc.isSidebarShown) {
        return YES;
    }
    CGPoint point = [gestureRecognizer locationInView:self.view];
    if (point.x > 30) {
        return NO;
    }
    return  YES;
}

-(void)panDetected:(UIPanGestureRecognizer*)recoginzer{
    
    [self.menuSidebarVc panDetected:recoginzer];
}

-(void)changeMsgTextWithState:(EditState)state{
    switch (state) {
        case normalState:
        {
            [self.recordView setMsgLabelText:@"上划取消，下划编辑"];
        }
            break;
        case upState:
        {
            [self.recordView setMsgLabelText:@"松开取消"];
        }
            break;
        case downState:
        {
            [self.recordView setMsgLabelText:@"松开编辑"];
        }
            break;
        default:
            break;
    }
}

#pragma mark -- OpenPlatformShare
-(void)OpenPlatformShareWithImg:(UIImage*)sendImg WithVideoPath:(NSURL*)videoPath{
    
    NSData* uploadData = nil;
    if (sendImg) {
        uploadData = UIImagePNGRepresentation(sendImg);
    }else{
        uploadData = [NSData dataWithContentsOfURL:videoPath];
    }
    
    //上传图片服务器获取key
    [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[self class] data:uploadData  prepare:^{
        [WaitingAlertView showWaitingViewAtView:self.recordView];
    } successed:^(NSDictionary *dic) {
        
        [WaitingAlertView hideWaitingView];
        if ([[dic objectForKey:@"result"] isEqualToString:@"error"]){
            
            return;
        }
        
        //获取分享的key
        NSString *shareImgKey = [dic objectForKey:@"fileName"];
        
        //如果是视频还需要上传视频第一帧给图片服务器
        if (videoPath) {
            UIImage *firstImg = [AppUtils getImage:videoPath];
            NSData* uploadData = UIImagePNGRepresentation(firstImg);
            [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[self class] data:uploadData prepare:^{
                
            } successed:^(NSDictionary *dic) {
                
                [WaitingAlertView hideWaitingView];
                if ([[dic objectForKey:@"result"] isEqualToString:@"error"]){
                    
                    return;
                }
                //第三方主动分享
                //获取分享的key
                NSString *shareKey = [dic objectForKey:@"fileName"];
                //分享显示的缩略图
                UIImage *parameterImg = [AppUtils getImage:videoPath];
                //拼接请求url
                NSString *parameters = [NSString stringWithFormat:@"video=%@&photoImg=%@",shareImgKey,shareKey];
                [self openPlatformWithParams:parameters AndParameteImg:parameterImg];
                
            } failed:^(NSError *error) {
                [WaitingAlertView hideWaitingView];
            }];
        }else{
            //第三方主动分享
            //拼接请求url
            NSString *parameters = [NSString stringWithFormat:@"img=%@",shareImgKey];
            [self openPlatformWithParams:parameters AndParameteImg:sendImg];
        }
    } failed:^(NSError *error) {
        [WaitingAlertView hideWaitingView];
    }];
}

-(void)openPlatformWithParams:(NSString*)parameters AndParameteImg:(UIImage*)parameterImg{
    NSString *webpageUrl = [NSString stringWithFormat:@"%@shareMyMsg.pb?%@", @"http://182.92.189.13:11080/SpreeCMS/", parameters];
    if ([self.clickSessionData.pSessionName isEqualToString:ContactEntity_Text_WXFriendShare]) {
        [OpenPlatformServers shareToWeiXinLinkContent:@"" title:@"来自ii客户端" thumbImage:parameterImg webpageUrl:webpageUrl scene:0];
    }else if ([self.clickSessionData.pSessionName isEqualToString:ContactEntity_Text_WXCircleShare]){
        [OpenPlatformServers shareToWeiXinLinkContent:@"" title:@"来自ii客户端" thumbImage:parameterImg webpageUrl:webpageUrl scene:1];
    }else if ([self.clickSessionData.pSessionName isEqualToString:ContactEntity_Text_QQZoneShare]){
        [OpenPlatformServers shareToQQLinkContent:@"" title:@"来自ii客户端" thumbImage:parameterImg webpageUrl:webpageUrl type:0];
    }else if ([self.clickSessionData.pSessionName isEqualToString:ContactEntity_Text_WBShare]){
        [OpenPlatformServers shareToWeiBoWithMessage:@"来自ii客户端" image:parameterImg mediaURL:webpageUrl];
    }
}

#pragma mark titleButtonAction
-(void)leftItemClicked:(id)sender{
    //点击左菜单时，键盘隐藏。
    [_recordView finishGraffitAction];
    
    [self.menuSidebarVc showHideSidebar];
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
   [self.recordView switchCamera];
    
    self.centerBtnImageName = @"flashon_open.png";
}

#pragma mrak 逻辑处理
//展开或者回复联系人视图
-(void)openOrCloseContactView:(BOOL)isOpen{
    
    if (isOpen==NO) {
        [UIView animateWithDuration:0.3 animations:^{
            //先做个下拉动画
            CGFloat containView_Y = self.recordView.frame.origin.y + self.recordView.frame.size.height;
            CGFloat containView_W = self.view.frame.size.width;
            CGFloat containView_H = self.view.frame.size.height - containView_Y;
            _containView.frame = CGRectMake(0, containView_Y, containView_W, containView_H);
            
            _contactView.frame = CGRectMake(0, 0, _containView.frame.size.width, _containView.frame.size.height);
        } completion:^(BOOL finished) {
            
            _containView.isScroll = YES;
            _contactView.isScroll = NO;
            //通知contactview现在是可以编辑的状态
        }];
    }else{
        [UIView animateWithDuration:0.3 animations:^{
            _containView.frame = CGRectMake(0, 20, self.view.frame.size.width, self.view.frame.size.height-20);
            _containView.isScroll = NO;
            _contactView.frame = CGRectMake(0, 0, _containView.frame.size.width, _containView.frame.size.height);
            _contactView.isScroll = YES;
        } completion:^(BOOL finished) {
            //通知contactview现在是不可编辑状态
        }];
    }
}

//展开或者缩起地图
-(void)openOrCloseMapView:(BOOL)isOpen{
    
    if (isOpen==NO) {
        [UIView animateWithDuration:0.3 animations:^{
            CGFloat containView_Y = self.recordView.frame.origin.y + self.recordView.frame.size.height;
            CGFloat containView_W = self.view.frame.size.width;
            CGFloat containView_H = self.view.frame.size.height - containView_Y;

            _containView.frame = CGRectMake(0, containView_Y, containView_W, containView_H);
            _containView.isScroll = YES;
            _addressMapView.frame = CGRectMake(_addressMapView.frame.origin.x, 0, containView_W, containView_H);
        } completion:^(BOOL finished) {
            [_addressMapView openOrClose:isOpen];
        }];
    }else{
        [UIView animateWithDuration:0.3 animations:^{
            _containView.frame = CGRectMake(0, 20, self.view.frame.size.width, self.view.frame.size.height-20);
            _containView.isScroll = NO;
            _addressMapView.frame = CGRectMake(_addressMapView.frame.origin.x, 0, _containView.frame.size.width, _containView.frame.size.height);
        } completion:^(BOOL finished) {
            [_addressMapView openOrClose:isOpen];
        }];
    }
}

#pragma mark RecordViewDelegate

-(void)didTakePhotoFinish:(UIImage *)photo{
    //保存照片
    [IISettingViewController saveImageWithSetting:photo];
    UIImage *markImg = [_recordView addlogoImage:photo];
    if (!_isActiveShare) {
        [AppUtils CaptureAnimationWithImageView:photo startPoint:_recordView.center endPoint:self.currentPoint runView:self.view];
        //发送消息
        IMPackageSessionData* sessionData = self.clickSessionData;
        NSString* text = self.recordView.inputText;
        
        //获取需要发送的text后置空输入框
        [self.recordView resetInputText];
        [self.recordView resetGraffitView];
        
        [RootViewController sendImageWithUserinfo:sessionData firstFrameImage:markImg text:text msgType:sessionData.eType completion:^{
            
        }];
    }else{
        [self OpenPlatformShareWithImg:markImg WithVideoPath:nil];
    }
}

-(void)didRecordFinish:(NSURL*)videoUrl videoTime:(CGFloat)time{
    
    //视频拼接
//    NSArray *array = [NSArray arrayWithObjects:videoUrl,videoUrl, nil];
//    [SBVideoRecorder mergeAndExportVideosAtFileURLs:array withScale:0 withCompleteBlock:^(NSURL *path, CGFloat length) {
//       
//        NSLog(@"aaa");
//    }];
//    return;

    
//    PostViewController *con = [[PostViewController alloc] initWithVideoPath:videoUrl];
//    [self.navigationController pushViewController:con animated:YES];
//    
//    return;
    
    
    //保存视频
    [IISettingViewController saveVideoWithSetting:videoUrl];

    if (self.isEdit) {
        //进入视频编辑界面
        SelectFilterViewController* con = [[SelectFilterViewController alloc] initWithVideoUrl:videoUrl andTime:time];
        con.delegate = self;
        con.sessionData = self.clickSessionData;
        [self.navigationController pushViewController:con animated:YES];
        
        AIAI_LOG(@"录制视频结束并进入编辑界面");
    }else{
        //做动画
        [AppUtils CaptureAnimationWithImageView:[AppUtils getImage:videoUrl] startPoint:_recordView.center endPoint:self.currentPoint runView:self.view];
        //发送消息
        [self sendVideoWithPath:videoUrl AndLength:time];
    }
    
    //需要把输入框设置成空
    [self.recordView resetInputText];
    [self.recordView resetGraffitView];
}

//发送视频
-(void)sendVideoWithPath:(NSURL*)videoUrl AndLength:(CGFloat)time{
    
    if ([_recordView needAddMark]) {
        
        UIImage *markImg = [_recordView getMarkImg];
        
        //创建新的data 防止只复制指针 数据被修改
        IMPackageSessionData *data = [[IMPackageSessionData alloc] init];
        data.pSessionId = self.clickSessionData.pSessionId;
        [self blendVideoAndImagesWithImg:markImg srcVedioPath:videoUrl targetData:data prepare:^{
            
        } complate:^(NSURL *outputPath, IMPackageSessionData *data) {
            dispatch_async(dispatch_get_main_queue(), ^{
                
                [self simpleSendVideoWithPath:outputPath AndLength:time AndTargetData:data];
            });
        }];
        
    }else{
        [self simpleSendVideoWithPath:videoUrl AndLength:time AndTargetData:nil];
    }
}

//如果发视频调用sendVideoWithPath:AndLength:
-(void)simpleSendVideoWithPath:(NSURL*)videoPath AndLength:(CGFloat)time AndTargetData:(IMPackageSessionData*)targetData{
    
    if (!_isActiveShare) {
        IMPackageSessionData* sessionData = targetData?: self.clickSessionData;
        CGFloat vedioTime = time;
        NSString* videoTitle = [NSString stringWithFormat:@"%@发送",[GlobalUserInfo myselfInfo].userName];
        NSString* text = self.recordView.inputText;
        [RootViewController sendVedioWithUserinfo:sessionData vedioUrl:videoPath vedioTime:vedioTime videoTitle:videoTitle text:text msgType:sessionData.eType completion:nil];
    }else{
        [self OpenPlatformShareWithImg:nil WithVideoPath:videoPath];
    }
}

#pragma mark SelectFilterViewControllerDelegate

-(void)SelectFilterViewControllerDidEditMovieUrl:(NSURL*)editMovieUrl movieTime:(CGFloat)time sessionData:(IMPackageSessionData*)sessionData{
    
    [self sendVideoWithPath:editMovieUrl AndLength:time];
}

-(void)SelectFilterViewControllerWillGetUserInfo:(NSDictionary*)userInfo{
    
    
    NSURL * movieUrl = [userInfo objectForKey:kMoviePathKey];
    CGFloat time = [[userInfo objectForKey:kMovieTimeKey] floatValue];
    NSInteger index = [[userInfo objectForKey:kSelectFilterIndex] integerValue];
    NSArray* selectSubviews = [userInfo objectForKey:kSubViewImagePathArray];
    SelectFilterView_SelectBtn btnType = (SelectFilterView_SelectBtn)[[userInfo objectForKey:kBtnType] integerValue];
    IMPackageSessionData * sessionData = [userInfo objectForKey:kUserSessionData];

    IMPackageMessageData* msgModel = [[IMPackageMessageData alloc] init];
    msgModel.pSessionId = sessionData.pSessionId;
    [self.contactView willSendingMsgUpdateCollectionCellWithIMPackageMessageData:msgModel];
    
    void (^CompletionHandler)(NSURL *outputPath) = ^(NSURL *outputPath){

        [self SelectFilterViewControllerDidEditMovieUrl:outputPath movieTime:time sessionData:sessionData];
    };
    if (btnType == Select_TeXiaoBtn){
        //视频叠加之后的路径
        [FilterManager videoSuperpositionWithMoviePath:movieUrl movieTime:time index:index prepare:^{} complate:CompletionHandler];
        
    }else if (btnType == Select_LvJingBtn){
        //视频叠加之后的路径
        [FilterManager videoAddFilterWithMoviePath:movieUrl movieTime:time index:index prepare:^{} complate:CompletionHandler];
        
    }else if (btnType == Select_TeXiaoTimeBtn){
        
    }else if (btnType == Select_KaDunBtn) {
        //进行播放播放
        [FilterManager catonFilterPathWithScrollImageViewArray:selectSubviews srcVedioPath:movieUrl prepare:^{} complate:CompletionHandler];
    }
}

@end
