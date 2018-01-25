//
//  ContactParentView.m
//  aiai
//
//  Created by zhangyang on 14/12/14.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "ContactParentView.h"
#import "ContactHeadLogoConllectionCell.h"
#import "ContactHeadLogoConllectionHeaderView.h"
#import "IMPackageData.h"

@interface ContactParentView ()<UIGestureRecognizerDelegate,UICollectionViewDataSource,UICollectionViewDelegate,UICollectionViewDelegateFlowLayout>


@property(nonatomic,strong)ContactHeadLogoConllectionHeaderView* headerView;

@property(nonatomic)BOOL isEditStatus;
@property (nonatomic, assign) BOOL isOpen;
@property(nonatomic)BOOL isShake;//是否抖动
@property(nonatomic,strong)NSMutableDictionary *sendingSessionDic;

@end

@implementation ContactParentView
@synthesize myCollectionView;

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        
        CGFloat headerHeight = 25;
        _headerView = [[ContactHeadLogoConllectionHeaderView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, headerHeight)];
        _headerView.backgroundColor = EveryView_BackGroundColor;
        __weak ContactParentView* weakself = self;
        _headerView.openCloseBtnBlock = ^{
            [weakself bgImageButtonPress:nil];
        };
        [self addSubview:_headerView];

        //创建联系人列表
        UICollectionViewFlowLayout* layout = [[UICollectionViewFlowLayout alloc] init];
        
        CGFloat toLeft = 20;
        CGFloat w = CONTACTHEADLOGO_W;
        CGFloat space = ([UIScreen mainScreen].bounds.size.width - toLeft * 2 - w * 4)/3;
        layout.itemSize = CGSizeMake(w, CONTACTHEADLOGO_H);
        layout.minimumLineSpacing = 16;
        layout.minimumInteritemSpacing = space;
        layout.sectionInset = UIEdgeInsetsMake(6, toLeft, 0, toLeft);//上 左 下 右

        myCollectionView = [[UICollectionView alloc] initWithFrame:CGRectMake(0, headerHeight, self.frame.size.width, self.frame.size.height) collectionViewLayout:layout];
        myCollectionView.backgroundColor = EveryView_BackGroundColor;
        myCollectionView.delegate = self;
        myCollectionView.dataSource = self;
        [myCollectionView registerClass:[ContactHeadLogoConllectionCell class] forCellWithReuseIdentifier:@"ContactHeadLogoConllectionCell"];
        [self addSubview:myCollectionView];
        
        UITapGestureRecognizer *doubleTapGesture = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleDoubleTap:)];
        doubleTapGesture.numberOfTapsRequired = 2;
        doubleTapGesture.numberOfTouchesRequired = 1;
        [self addGestureRecognizer:doubleTapGesture];
        
        self.sendingSessionDic = [[NSMutableDictionary alloc] init];
        
        [self addObserver:self forKeyPath:@"frame" options:NSKeyValueObservingOptionNew context:nil];
        
        _isOpen = NO;
    }
    return self;
}

#pragma mark KVO监听frame的变化来确定是否允许联系人进入编辑状态
-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context{
    if([keyPath isEqualToString:@"frame"]){
        CGRect newRect = [[change objectForKey:@"new"] CGRectValue];
        //如果newrect的y是0，则证明该进入编辑状态了
        if (newRect.size.width < newRect.size.height) {
            if (self.isEditStatus == NO) {
                self.isEditStatus = YES;
            }
        }else{
            if (self.isEditStatus == YES) {
                self.isEditStatus = NO;
            }
        }
    }
}

#pragma mark overwrite setFrame
-(void)setFrame:(CGRect)frame{
    [super setFrame:frame];

    //需要把表格也重置frame
    CGRect rect = myCollectionView.frame;
    rect.size.height = frame.size.height - rect.origin.y;
    myCollectionView.frame = rect;
}

-(void)setContactArray:(NSArray *)contactArray{
    _contactArray = [NSMutableArray arrayWithArray:contactArray];
    
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [myCollectionView reloadData];
//    });
}

-(void)addSpecialBuddyData{
    
//    微信朋友圈", @"微信好友", @"QQ空间", @"微博"
    IMPackageSessionData* sessionData0 = [[IMPackageSessionData alloc] init];
    sessionData0.pSessionName = ContactEntity_Text_WXCircleShare;
    sessionData0.pPortraitKey = @"weixin_circle_able.png";
    [_contactArray addObject:sessionData0];
    
    IMPackageSessionData* sessionData1 = [[IMPackageSessionData alloc] init];
    sessionData1.pSessionName = ContactEntity_Text_WXFriendShare;
    sessionData1.pPortraitKey = @"weixin_friend_able.png";
    [_contactArray addObject:sessionData1];
    
    IMPackageSessionData* sessionData2 = [[IMPackageSessionData alloc] init];
    sessionData2.pSessionName = ContactEntity_Text_QQZoneShare;
    sessionData2.pPortraitKey = @"qq_zone_able.png";
    [_contactArray addObject:sessionData2];
    
    IMPackageSessionData* sessionData3 = [[IMPackageSessionData alloc] init];
    sessionData3.pSessionName = ContactEntity_Text_WBShare;
    sessionData3.pPortraitKey = @"sina_weibo_able.png";
    [_contactArray addObject:sessionData3];
    
    IMPackageSessionData* sessionData4 = [[IMPackageSessionData alloc] init];
    sessionData4.pSessionName = ContactEntity_Text_NewBuildChat;
    sessionData4.pPortraitKey = @"添加好友.png";
    [_contactArray addObject:sessionData4];
    
}

-(void)setIsScroll:(BOOL)isScroll{
    _isScroll = isScroll;
    
    myCollectionView.scrollEnabled = isScroll;
}

#pragma mark 按钮响应事件
-(void)tap1GestureRecognizer:(UIGestureRecognizer*)gesture entity:(IMPackageSessionData*)sessionData{
    
    [self EndWobble];
    if (self.tap1Block) {
        self.tap1Block(gesture,sessionData,nil);
    }
}

-(void)tap2GestureRecognizer:(UIGestureRecognizer*)gesture entity:(IMPackageSessionData*)sessionData indexPath:(NSIndexPath*)indexPath {
    if (self.tap2Block) {
        self.tap2Block(gesture,sessionData,indexPath);
    }
}

-(void)topSwipingGestureRecognizer:(UIGestureRecognizer*)gesture entity:(IMPackageSessionData*)sessionData{
    if (self.topSwipingBlock) {
        self.topSwipingBlock(gesture,sessionData,nil);
    }
}

-(void)bottomSwipingGestureRecognizer:(UIGestureRecognizer*)gesture entity:(IMPackageSessionData*)sessionData{
    if (self.bottomSwipingBtnBlock) {
        self.bottomSwipingBtnBlock(gesture,sessionData,nil);
    }
}

-(void)longPressGestureRecognizer:(UIGestureRecognizer*)gesture entity:(IMPackageSessionData*)sessionData{
    if (gesture.state == UIGestureRecognizerStateBegan) {
        if (self.isEditStatus) {
            [self BeginWobble];
        }else{
            if (self.longPressBtnBlock) {
                self.longPressBtnBlock(gesture,sessionData,nil);
            }
        }
    }else{
        if (self.longPressBtnBlock) {
            self.longPressBtnBlock(gesture,sessionData,nil);
        }
        
        IMPackageMessageData *data = [[IMPackageMessageData alloc] init];
        data.pSessionId = sessionData.pSessionId;
        [self willSendingMsgUpdateCollectionCellWithIMPackageMessageData:data];
    }
}

-(void)handleDoubleTap:(UIGestureRecognizer *)sender{
    [self bgImageButtonPress:nil];
    
    _isOpen = !_isOpen;
}

-(void)deleteButtonPress:(IMPackageSessionData*)sessionData{
    if (self.deleteBlock) {
        self.deleteBlock(sessionData);
    }
    int i = 0;
    for (i = 0; i<[self.contactArray count];i++) {
        IMPackageSessionData* one = [self.contactArray objectAtIndex:i];
        if ([one.pSessionId isEqualToString:sessionData.pSessionId]) {
            [self.contactArray removeObjectAtIndex:i];
            break;
        }
    }
    [myCollectionView deleteItemsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:i inSection:0]]];
    //删除之后系统不做动画了，需要重新做一下
    [self BeginWobble];
}

-(void)unReadMsgButtonPress:(IMPackageSessionData*)sessionData unReadMsgArray:(NSArray*)unReadMsgArray{
    if (self.unReadMsgBlock) {
        self.unReadMsgBlock(sessionData,unReadMsgArray);
    }
}

-(void)changeWithState:(EditState)state{
    if (self.stateChangeBlock) {
        self.stateChangeBlock(state);
    }
}

-(void)bgImageButtonPress:(id)sender{
    //转动button
    [_headerView changeButtonTransForm];
    //停止抖动
    [self EndWobble];
    //进行发送事件
    if (self.bgImageBtnBlock) {
        self.bgImageBtnBlock();
    }
}

#pragma mark UICollectionViewDataSource

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView{
    return 1;
}

- (NSInteger)collectionView:(UICollectionView *)view numberOfItemsInSection:(NSInteger)section;{
    return [_contactArray count];
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath{
    
    __weak ContactHeadLogoConllectionCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"ContactHeadLogoConllectionCell" forIndexPath:indexPath];

    //dic不懂,看cell头文件的注释
    [cell setIMPackageSessionData:[self.contactArray objectAtIndex:indexPath.row] withSendingDic:self.sendingSessionDic];
//    cell.currentIndexPath = indexPath;
    
    __weak ContactParentView* weakself = self;
    cell.headImageView1TapAction = ^(UIGestureRecognizer *gesture,IMPackageSessionData*sessionData){
        [weakself tap1GestureRecognizer:gesture entity:sessionData];
        [cell makeAnimation:!_isOpen];
    };
    cell.headImageView2TapAction = ^(UIGestureRecognizer *gesture,IMPackageSessionData*sessionData){
        [weakself tap2GestureRecognizer:gesture entity:sessionData indexPath:indexPath];
    };
    cell.topSwipingAction = ^(UIGestureRecognizer *gesture,IMPackageSessionData*sessionData){
        [weakself topSwipingGestureRecognizer:gesture entity:sessionData];
    };
    cell.bottomSwipingAction = ^(UIGestureRecognizer *gesture,IMPackageSessionData*sessionData){
        [weakself bottomSwipingGestureRecognizer:gesture entity:sessionData];
    };
    cell.longPressAction = ^(UIGestureRecognizer *gesture,IMPackageSessionData*sessionData){
        [weakself longPressGestureRecognizer:gesture entity:sessionData];
    };
    cell.bgImageViewAction = ^(UIGestureRecognizer *gesture,IMPackageSessionData*sessionData){
        [weakself bgImageButtonPress:nil];
    };
    cell.deleteBtnAction = ^(IMPackageSessionData*sessionData){
        [weakself deleteButtonPress:sessionData];
    };
    cell.unReadMsgBtnAction = ^(IMPackageSessionData*sessionData,NSArray* unReadMsgArray){
        [weakself unReadMsgButtonPress:sessionData unReadMsgArray:unReadMsgArray];
    };
    cell.stateChangeAction = ^(EditState state){
        [weakself changeWithState:state];
    };
    return cell;
}

#pragma mark UICollectionViewDelegate
-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    
}
#pragma mark 抖动动画
-(void)BeginWobble
{
    for (ContactHeadLogoConllectionCell *view in self.myCollectionView.subviews)
    {
        if ([view isKindOfClass:[ContactHeadLogoConllectionCell class]]) {
            ((ContactHeadLogoConllectionCell*)view).isEditStatus = YES;
        }

        srand([[NSDate date] timeIntervalSince1970]);
        float rand=(float)random();
        CFTimeInterval t=rand*0.0000000001;
        [UIView animateWithDuration:0.1 delay:t options:0  animations:^
         {
             view.transform=CGAffineTransformMakeRotation(-0.05);
         } completion:^(BOOL finished)
         {
             [UIView animateWithDuration:0.1 delay:0 options:UIViewAnimationOptionRepeat|UIViewAnimationOptionAutoreverse|UIViewAnimationOptionAllowUserInteraction  animations:^
              {
                  view.transform=CGAffineTransformMakeRotation(0.05);
              } completion:^(BOOL finished) {}];
         }];
    }
}

-(void)EndWobble
{
    for (UIView *view in self.myCollectionView.subviews)
    {
        if ([view isKindOfClass:[ContactHeadLogoConllectionCell class]]) {
            ((ContactHeadLogoConllectionCell*)view).isEditStatus = NO;
        }
        
        [UIView animateWithDuration:0.1 delay:0 options:UIViewAnimationOptionAllowUserInteraction|UIViewAnimationOptionBeginFromCurrentState animations:^
         {
             view.transform=CGAffineTransformIdentity;
             for (UIView *v in view.subviews)
             {
                 if ([v isMemberOfClass:[UIImageView class]])
                     [v setHidden:YES];
             }
         } completion:^(BOOL finished) {}];
    }
}

#pragma mark mothed public
-(void)readFinishCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel{
    
    for (int i = 0; i < [_contactArray count]; ++i) {
        IMPackageSessionData *tempData = [_contactArray objectAtIndex:i];
        if ([tempData.pSessionId isEqualToString:msgModel.pSessionId]) {
            //手动置为0
            tempData.nUnreadNum = 0;
            NSIndexPath *indexPath = [NSIndexPath indexPathForRow:i inSection:0];
            
            [myCollectionView reloadItemsAtIndexPaths:[NSArray arrayWithObject:indexPath]];
            return;
        }
    }
}

-(void)receiveMsgUpdateCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel{
    NSArray* sessionlist = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSessionList];
    IMPackageSessionData *sessionData = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetSession:msgModel.pSessionId];
    
    int i = 0;
    int topCount = 0;
    NSIndexPath *indexPath = nil;
//    for (IMPackageSessionData *data in sessionList) {
//        //判断置顶个数,新进来的数据应该是插在置顶session的下面
//        if (data.eTop == YES) {
//            topCount++;
//        }else{
//            break;
//        }
//        if ([data.pSessionId isEqualToString:sessionData.pSessionId]) {
//            
//        }
//    }
    //找到需要刷新的session,更新到置顶消息下面
    IMPackageSessionData *tempData;
    for (i = 0; i < [_contactArray count]; ++i) {
        tempData = [_contactArray objectAtIndex:i];
        if (!tempData) {
            tempData = sessionData;
        }
        if ([tempData.pSessionId isEqualToString:sessionData.pSessionId]) {
            indexPath = [NSIndexPath indexPathForRow:i inSection:0];
            [_contactArray removeObjectAtIndex:i];
            //如果新来的session为置顶,那么重置topCount 将数据插在第0个
            if (sessionData.eTop == YES) {
                topCount = 0;
            }
            [_contactArray insertObject:sessionData atIndex:topCount];
            break;
        }
    }
    //如果是新进来的数据直接插在置顶帖下面
    if (!indexPath) {
        //如果之前没有session那么直接插入新数据刷新
        if ([_contactArray count] == 0) {
            [_contactArray insertObject:sessionData atIndex:0];
            dispatch_async(dispatch_get_main_queue(), ^{
                self.contactArray = (NSMutableArray*)sessionlist;
                 [self.myCollectionView reloadData];
            });
            return;
        }
        else{
            [_contactArray insertObject:sessionData atIndex:topCount];
        }
        indexPath = [NSIndexPath indexPathForRow:topCount inSection:0];
        [myCollectionView insertItemsAtIndexPaths:[NSArray arrayWithObject:indexPath]];
        return;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if (indexPath.row != topCount) {
            [myCollectionView moveItemAtIndexPath:indexPath toIndexPath:[NSIndexPath indexPathForRow:topCount inSection:0]];
            
            [myCollectionView reloadItemsAtIndexPaths:[NSArray arrayWithObjects:indexPath, [NSIndexPath indexPathForRow:topCount inSection:0],nil]];
        }else{
            [myCollectionView reloadItemsAtIndexPaths:[NSArray arrayWithObject:indexPath]];
        }
        //刷新新来数据插入前后的session位置
//        if (indexPath.row != topCount)
//            [myCollectionView reloadItemsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:topCount inSection:0]]];
    });
    
    
    
}

-(void)willSendingMsgUpdateCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel{
    
    //如果为空则为第三方主动分享,
    if (msgModel.pSessionId == nil) {
        return;
    }
    [self.sendingSessionDic setObject:@"sending" forKey:msgModel.pSessionId];
    
    ContactHeadLogoConllectionCell *cell = nil;

    for (int i = 0;i<[self.contactArray count];i++) {
        IMPackageSessionData* sessionData = [self.contactArray objectAtIndex:i];
        if ([sessionData.pSessionId isEqualToString:msgModel.pSessionId] == YES) {
            NSArray* visibaleArray = [self.myCollectionView visibleCells];
            for (cell in visibaleArray) {
                if([cell.currentSessionData.pSessionId isEqualToString:msgModel.pSessionId]){
                    [cell setSendMsgImageWithStatus:SendMsgStatus_willSend];
                    break;
                }
            }
            break;
        }
    }
}

-(void)didSendMsgSuccessUpdateCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel{
    ContactHeadLogoConllectionCell *cell = nil;
    
    [self.sendingSessionDic removeObjectForKey:msgModel.pSessionId];
    for (int i = 0;i<[self.contactArray count];i++) {
        IMPackageSessionData* sessionData = [self.contactArray objectAtIndex:i];
        if ([sessionData.pSessionId isEqualToString:msgModel.pSessionId] == YES) {
            NSArray* visibaleArray = [self.myCollectionView visibleCells];
            for (cell in visibaleArray) {
                if([cell.currentSessionData.pSessionId isEqualToString:msgModel.pSessionId]){
                    [cell setSendMsgImageWithStatus:SendMsgStatus_didSendSuccess];
                    break;
                }
            }
            break;
        }
    }
}

-(void)didSendMsgFaildUpdateCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel{
    
    ContactHeadLogoConllectionCell *cell = nil;
    
    [self.sendingSessionDic removeObjectForKey:msgModel.pSessionId];
    for (int i = 0;i<[self.contactArray count];i++) {
        IMPackageSessionData* sessionData = [self.contactArray objectAtIndex:i];
        if ([sessionData.pSessionId isEqualToString:msgModel.pSessionId] == YES) {
            NSArray* visibaleArray = [self.myCollectionView visibleCells];
            for (cell in visibaleArray) {
                if([cell.currentSessionData.pSessionId isEqualToString:msgModel.pSessionId]){
                    [cell setSendMsgImageWithStatus:SendMsgStatus_didSendFaild];
                    break;
                }
            }
            break;
        }
    }
}

@end
