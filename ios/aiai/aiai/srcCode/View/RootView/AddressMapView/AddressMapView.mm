//
//  AddressMapView.m
//  aiai
//
//  Created by zhangyang on 15/1/12.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//
/*
 在地图收起状态 默认将距离最近的那个人置为选中状态 只能给选中状态的人发消息
 地图展开状态 点击用户头像播放 点击的用户置于选中状态 地图收起后将选中的用户置于下方区域最中央
 选中的用户头像需要放大
 */

#import "AddressMapView.h"
#import "AddressMapHeadView.h"

#import "IMPackageEngine.h"
#import "IMPackageData.h"

#define MapRegionDistance (3000)

@class BMKUserLocation;
@interface AddressMapView ()<BMKMapViewDelegate,BMKLocationServiceDelegate>

@property(nonatomic,assign) BOOL isOpen;

@property(nonatomic,strong) BMKLocationService* locationService;
@property(nonatomic,strong) BMKMapView* mapView;
@property(nonatomic, assign) CLLocationCoordinate2D centerPoint;

@property(nonatomic,strong) NSMutableArray* annomationArray;//必须使用成员变量来保存annomation，如果直接使用map.annomaiton则会引发崩溃问题

@property(nonatomic, strong)NSMutableArray *headAnnotationArray;//存放地图上所有标志 以便查找
@property(nonatomic, strong)AddressMapHeadView* recentAnnotationView;//   距离自己最近的好友
@end

@implementation AddressMapView
@synthesize mapView = _mapView;
@synthesize sendImgBlick,sendVideoBlock,changeStateBlock;
@synthesize centerPoint;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        
        //创建mapview
        CGFloat mapview_W = frame.size.width;
        CGFloat mapview_H = frame.size.height;
        _mapView = [[BMKMapView alloc]initWithFrame:CGRectMake(0, 0, mapview_W, mapview_H)];
        _mapView.delegate = self;
        _mapView.userTrackingMode = BMKUserTrackingModeNone;
        _mapView.overlookEnabled = NO;
        _mapView.rotateEnabled = NO;
        _mapView.showsUserLocation = YES;
        [self openOrClose:NO];//关闭状态
        [self addSubview:_mapView];
        
        //设置我的位置样式
        BMKLocationViewDisplayParam *locationViewDisplayParam = [[BMKLocationViewDisplayParam alloc] init];
        locationViewDisplayParam.isAccuracyCircleShow = NO;
        [_mapView updateLocationViewWithParam:locationViewDisplayParam];
        //启动定位服务
        self.locationService = [[BMKLocationService alloc]init];
        self.locationService.delegate = (id)self;
        [self.locationService startUserLocationService];
        
        //添加双击手势，作用与展开关闭按钮一样的
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(doubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        _headAnnotationArray = [[NSMutableArray alloc] init];
        
    }
    return self;
}

#pragma mark 双击展开或者关闭地图
-(void)doubleTap:(UITapGestureRecognizer*)gesture{
    [self openOrCloseBtnPress];
}

-(void)openOrCloseBtnPress{
    if (self.openOrCloseMapBtnAction) {
        self.openOrCloseMapBtnAction();
    }
}

#pragma mark overwrite property
-(void)setFrame:(CGRect)frame{
    [super setFrame:frame];
    
    CGFloat mapview_Y = _mapView.frame.origin.y;
    _mapView.frame = CGRectMake(0, 0, frame.size.width, frame.size.height - mapview_Y);
}

- (void)setNearFriends:(NSArray *)nearFriends
{
    if ([nearFriends count] > 0) {
        
        _nearFriends = nearFriends;
        
        if (_annomationArray == nil) {
            _annomationArray = [NSMutableArray array];
        }else{
            [_mapView removeAnnotations:_annomationArray];
            [_annomationArray removeAllObjects];
        }
        
        for (GlobalUserInfo* oneFriend in nearFriends) {
            //不显示自己的头像
            if ([oneFriend.userName isEqualToString:[GlobalUserInfo myselfInfo].userName]) {
                break;
            }
            AddressMapHeadAnnotation* annotation = [[AddressMapHeadAnnotation alloc] initWithUserInfo:oneFriend];
            annotation.title = @" ";//百度地图的bug，必须设置title，哪怕一个空格
            [_annomationArray addObject:annotation];
        }
        [_mapView addAnnotations:_annomationArray];
    }
}

#pragma mark implement BMKMapViewDelegate

- (void)mapView:(BMKMapView *)mapView didAddAnnotationViews:(NSArray *)views{
    
    for (int i = 0; i<[views count]; ++i) {
        AddressMapHeadView *temp = [views objectAtIndex:i];
        AddressMapHeadAnnotation* headAnnotation = temp.annotation;
        AddressMapHeadAnnotation* recentAnnotation = _recentAnnotationView.annotation;
        if ([headAnnotation.nearByName isEqualToString:recentAnnotation.nearByName]) {
            //用mapview还不管用
            _recentAnnotationView.userInteractionEnabled = YES;
            [_recentAnnotationView.superview bringSubviewToFront:_recentAnnotationView];
            return;
        }
    }
}

// 根据anntation生成对应的View
- (BMKAnnotationView *)mapView:(BMKMapView *)mapView viewForAnnotation:(id <BMKAnnotation>)annotation
{
    NSString *AnnotationViewID = @"AnnotationViewID";
    AddressMapHeadView* annotationView = (AddressMapHeadView *)[_mapView dequeueReusableAnnotationViewWithIdentifier:AnnotationViewID];
    if (annotationView == nil) {
        annotationView = [[AddressMapHeadView alloc] initWithAnnotation:annotation reuseIdentifier:AnnotationViewID];
        annotationView.canShowCallout = NO;
    }
    //设置数据
    if ([annotation isKindOfClass:[AddressMapHeadAnnotation class]]) {
        AddressMapHeadAnnotation* headAnnotation = (AddressMapHeadAnnotation *)annotation;
        annotationView.distance = [self getLengthWithOffset:headAnnotation.coordinate];
        annotationView.userInteractionEnabled = NO;
        [_headAnnotationArray addObject:annotationView];
        
        //只有最近的一个view可以点击,找到离自己最近的view
        if ([_headAnnotationArray count] == [_annomationArray count]) {
            _recentAnnotationView = [_headAnnotationArray objectAtIndex:0];
            for (int i = 0; i<[_headAnnotationArray count]; ++i) {
                AddressMapHeadView* annotationView = [_headAnnotationArray objectAtIndex:i];
                if (_recentAnnotationView.distance > annotationView.distance) {
                    _recentAnnotationView = annotationView;
                }
            }
        }
    }
    __weak AddressMapHeadView* headAnnotationView = annotationView;
    __weak AddressMapHeadAnnotation* headAnnotation = (AddressMapHeadAnnotation *)annotation;
    
    annotationView.tap2Block = ^(UIGestureRecognizer* recognizer){
        if (self.isOpen == NO) {
            [headAnnotationView playHeaderAudio];
        }else{
            _recentAnnotationView = annotationView;
        }
        centerPoint = annotation.coordinate;
    };
    //不适用百度地图的单击事件，有bug
    annotationView.tap1Block = ^(UIGestureRecognizer* recognizer){
        if(self.isOpen){
            dispatch_async(dispatch_get_main_queue(), ^{
                //用mapview还不管用
                [headAnnotationView.superview bringSubviewToFront:headAnnotationView];
                [headAnnotationView playHeaderAudio];
                _recentAnnotationView = annotationView;
            });
        }else{
            if (sendImgBlick) {
                //添加逻辑在此界面发消息 的话需要先添加好友
                sendImgBlick(headAnnotation.nearByName, recognizer);
                //判断如果此人不在sessionlist中 说明为第一次聊天 则进行添加好友操作
                if ([self needAddBuddy:headAnnotation.nearByName]) {
                    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_AddBuddyWithUserID:headAnnotation.nearByName];
                }
            }
        }
        centerPoint = annotation.coordinate;
    };
    annotationView.longPressBlock = ^(UIGestureRecognizer* recognizer, EditState state){
        if (self.isOpen) {
            //啥都不干
        }else{
            if (sendVideoBlock) {
                //添加逻辑在此界面发消息 的话需要先添加好友
                sendVideoBlock(headAnnotation.nearByName, recognizer, state);
                if ([self needAddBuddy:headAnnotation.nearByName]) {
                    
                    [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_AddBuddyWithUserID:headAnnotation.nearByName];
                }
            }
        }
        centerPoint = annotation.coordinate;
    };
    annotationView.changeStateBlock = ^(EditState state){
        if (self.isOpen) {
            //啥都不干
        }else{
            if (changeStateBlock) {
                changeStateBlock(state);
            }
        }
    };
    
    return annotationView;
}

#pragma mark -- 判断是否需要添加好友
-(BOOL)needAddBuddy:(NSString*)userName{
    
    NSArray *sessionList = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSessionList];
    for (IMPackageSessionData* sessionData in sessionList) {
        if ([sessionData.pSessionId isEqualToString:userName]) {
            return NO;
        }
    }
    
    return YES;
}

#pragma mark 定位纠正算法
- (BOOL)isValidLocationCoordinate:(CLLocationCoordinate2D)locationCoordinate
{
    return CLLocationCoordinate2DIsValid(locationCoordinate) && locationCoordinate.longitude != 0 && locationCoordinate.latitude != 0;
}

- (void)updateUserLocation:(BMKUserLocation *)userLocation
{
    if (![self isValidLocationCoordinate:userLocation.location.coordinate]) {
        return;
    }
    //显示定位点
    [_mapView updateLocationData:userLocation];
    //点击不弹气泡
    userLocation.title = nil;
    userLocation.subtitle = nil;
    
    [GlobalUserInfo myselfInfo].userLongitude = userLocation.location.coordinate.longitude;
    [GlobalUserInfo myselfInfo].userLatitude = userLocation.location.coordinate.latitude;
    //如果没有初始值则使用我的位置
    if (0.0000001 > centerPoint.longitude && centerPoint.longitude > -0.0000001 ) {
        centerPoint = CLLocationCoordinate2DMake([GlobalUserInfo myselfInfo].userLatitude, [GlobalUserInfo myselfInfo].userLongitude);
        _mapView.region = BMKCoordinateRegionMakeWithDistance(self.locationService.userLocation.location.coordinate, MapRegionDistance, MapRegionDistance);
        
        NSString* lonString = [[NSNumber numberWithDouble:userLocation.location.coordinate.longitude] stringValue];
        NSString* latString = [[NSNumber numberWithDouble:userLocation.location.coordinate.latitude] stringValue];
        [SaveManager saveValue:lonString forKey:USER_LONGITUDE];
        [SaveManager saveValue:latString forKey:USER_LATITUDE];
        
        _mapView.zoomLevel = MAPZOOMLEVEL;
    }
}

#pragma mark 用户方向位置更新
- (void)didUpdateUserHeading:(BMKUserLocation *)userLocation
{
    [self updateUserLocation:userLocation];
}

- (void)didUpdateBMKUserLocation:(BMKUserLocation *)userLocation
{
    [self updateUserLocation:userLocation];
}


#pragma mark 定位失败后，会调用此函数
- (void)mapView:(BMKMapView *)mapView didFailToLocateUserWithError:(NSError *)error
{
    AIAI_LOG(@"location error");
}

#pragma mark 获取到物理地址信息
-(void)onGetReverseGeoCodeResult:(BMKGeoCodeSearch *)searcher result:(BMKReverseGeoCodeResult *)result errorCode:(BMKSearchErrorCode)error
{
    AIAI_LOG(@"%@",result.address);
}


#pragma mark pulic mothed
-(void)startLocation{
    [self.locationService startUserLocationService];
    
    _mapView.showsUserLocation = YES;
    //    _mapView.userTrackingMode = BMKUserTrackingModeFollow;
}

-(void)stopLocation{
    _mapView.showsUserLocation = NO;
    [self.locationService stopUserLocationService];
}

-(void)openOrClose:(BOOL)isOpen{
    self.isOpen = isOpen;
    if (isOpen) {
        _mapView.scrollEnabled = YES;
        _mapView.zoomEnabled = YES;
    } else {
        _mapView.scrollEnabled = NO;
        _mapView.zoomEnabled = NO;
    }
    [_mapView setCenterCoordinate:centerPoint animated:YES];
    [self SetAnnotationView:isOpen];
    //切换状态停掉头像播放
    [[NSNotificationCenter defaultCenter] postNotificationName:MPMoviePlayerPlaybackDidFinishNotification object:nil];
}

-(void)SetAnnotationView:(BOOL)userActive{
    
    for (int i = 0; i < [_headAnnotationArray count]; ++i) {
        AddressMapHeadView* addressMapHeadView = [_headAnnotationArray objectAtIndex:i];
        addressMapHeadView.userInteractionEnabled = userActive;
        
        AddressMapHeadAnnotation* headAnnotation = addressMapHeadView.annotation;
        AddressMapHeadAnnotation* recentAnnotation = _recentAnnotationView.annotation;
        
        if ([headAnnotation.nearByName isEqualToString:recentAnnotation.nearByName]) {
            addressMapHeadView.userInteractionEnabled = YES;
        }
    }
}

//计算地图上两点间的距离
-(double)getLengthWithOffset:(CLLocationCoordinate2D)location{
    
    double myLatitude = [GlobalUserInfo myselfInfo].userLatitude;
    double myLogitude = [GlobalUserInfo myselfInfo].userLongitude;
    
    CLLocationCoordinate2D coors[2] = {0};
    //我所在的点
    coors[0].latitude = myLatitude;
    coors[0].longitude = myLogitude;
    //好友所在的点
    coors[1].latitude = location.latitude;
    coors[1].longitude = location.longitude;
    
    BMKMapPoint pmine = BMKMapPointForCoordinate(coors[0]);
    BMKMapPoint pbuddy = BMKMapPointForCoordinate(coors[1]);
    
    CLLocationDistance dis = BMKMetersBetweenMapPoints(pmine, pbuddy);
    
    return dis;
    
}
@end