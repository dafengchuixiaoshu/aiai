//
//  ContactParentView.h
//  aiai
//
//  Created by zhangyang on 14/12/14.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import "ContactHeadLogoConllectionCell.h"

@class IMPackageSessionData;
@class IMPackageMessageData;

typedef void (^ContactViewOtherBtnAction)(void);
typedef void (^ContactViewGestureAction)(UIGestureRecognizer *gesture,IMPackageSessionData* sessionData,NSIndexPath* indexPath);
typedef void (^ContactViewDeleteAction)(IMPackageSessionData* sessionData);
typedef void (^ContactViewUnReadMsgAction)(IMPackageSessionData* sessionData,NSArray* unReadMsgArray);
typedef void (^ContactViewStateChange)(EditState state);

@interface ContactParentView : UIView
@property(nonatomic,strong)UICollectionView* myCollectionView;
@property(nonatomic,strong)NSMutableArray* contactArray;

@property (nonatomic, assign) BOOL isScroll;                //设置列表是否可滑动
@property (nonatomic, assign, readonly)BOOL isEditStatus;   //判断列表是否在编辑状态
@property (nonatomic, assign, readonly) BOOL isOpen;        //判断列表是否为全屏

-(void)addSpecialBuddyData;    //添加第三方分享和添加按钮数据

@property(nonatomic,copy)ContactViewGestureAction tap1Block;
@property(nonatomic,copy)ContactViewGestureAction tap2Block;//双击没有响应，被userheader截获了
@property(nonatomic,copy)ContactViewGestureAction topSwipingBlock;
@property(nonatomic,copy)ContactViewGestureAction bottomSwipingBtnBlock;
@property(nonatomic,copy)ContactViewGestureAction longPressBtnBlock;
@property(nonatomic,copy)ContactViewDeleteAction deleteBlock;
@property(nonatomic,copy)ContactViewUnReadMsgAction unReadMsgBlock;
@property(nonatomic,copy)ContactViewOtherBtnAction bgImageBtnBlock;
@property(nonatomic,copy)ContactViewStateChange stateChangeBlock;

-(void)readFinishCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel;
-(void)receiveMsgUpdateCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel;
-(void)willSendingMsgUpdateCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel;
-(void)didSendMsgSuccessUpdateCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel;
-(void)didSendMsgFaildUpdateCollectionCellWithIMPackageMessageData:(IMPackageMessageData*)msgModel;

@end
