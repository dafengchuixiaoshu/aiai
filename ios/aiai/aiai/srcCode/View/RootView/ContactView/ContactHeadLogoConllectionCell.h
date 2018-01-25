//
//  ContactConllectionCell.h
//  aiai
//
//  Created by zhangyang on 14/12/14.
//  Copyright (c) 2014年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, SendMsgStatus){
    SendMsgStatus_willSend = 0,
    SendMsgStatus_didSendSuccess,
    SendMsgStatus_didSendFaild
};

typedef NS_ENUM(NSInteger, EditState) {
    
    normalState = 0  ,
    upState          ,
    downState
    
};

#define CONTACTHEADLOGO_W       60
#define CONTACTHEADLOGO_H       82

@class IMPackageSessionData;

typedef void (^ContactConllectionCellGestureAction)(UIGestureRecognizer *gesture,IMPackageSessionData* sessionData);
typedef void (^ContactConllectionCellDeleteBtnAction)(IMPackageSessionData* sessionData);
typedef void (^ContactConllectionCellUnReadMsgBtnAction)(IMPackageSessionData* sessionData,NSArray* unReadMsgArray);
typedef void (^ContactConllectionCellStateChange)(EditState state);

@interface ContactHeadLogoConllectionCell : UICollectionViewCell

//dic里存放着正在发送消息的sessionid key为sessionid value为自定义的字符串 以便reload的时候ui贴乱
-(void)setIMPackageSessionData:(IMPackageSessionData* )sessionData withSendingDic:(NSDictionary*)sendingDic;

-(void)setSendMsgImageWithStatus:(SendMsgStatus)status;

-(void)makeAnimation:(BOOL)make;

@property(nonatomic,readonly,strong)IMPackageSessionData* currentSessionData;

@property(nonatomic)BOOL isEditStatus;

@property(nonatomic,copy)ContactConllectionCellGestureAction headImageView1TapAction;//单击
@property(nonatomic,copy)ContactConllectionCellGestureAction headImageView2TapAction;//双击
@property(nonatomic,copy)ContactConllectionCellGestureAction topSwipingAction;
@property(nonatomic,copy)ContactConllectionCellGestureAction bottomSwipingAction;
@property(nonatomic,copy)ContactConllectionCellStateChange stateChangeAction;
@property(nonatomic,copy)ContactConllectionCellGestureAction longPressAction;
@property(nonatomic,copy)ContactConllectionCellGestureAction bgImageViewAction;
@property(nonatomic,copy)ContactConllectionCellDeleteBtnAction deleteBtnAction;
@property(nonatomic,copy)ContactConllectionCellUnReadMsgBtnAction unReadMsgBtnAction;

@end
