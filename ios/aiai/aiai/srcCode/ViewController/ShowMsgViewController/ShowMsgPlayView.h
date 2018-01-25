//
//  ShowMsgPlayView.h
//  aiai
//
//  Created by zhangyang on 15/4/14.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^ShowMsgPlayViewUnReadMsgButtonBlock)(void);
typedef void (^ShowMsgPlayViewShareButtonBlock)(void);
typedef void (^ShowMsgPlayViewSaveButtonBlock)(NSString *videoPath);
typedef void (^ShowMsgPlayViewDidMoveBottomBlock)(BOOL isBottom);

@interface ShowMsgPlayView : UIView

@property(nonatomic,copy)NSString* imageUrl;
@property(nonatomic,copy)NSString* vedioUrl;
@property(nonatomic,copy)NSString* contentText;

@property(nonatomic,assign)NSInteger unReadMsgCount;
@property(nonatomic,strong)ShowMsgPlayViewUnReadMsgButtonBlock unReadMsgButtonBlock;
@property(nonatomic,strong)ShowMsgPlayViewDidMoveBottomBlock moveBottomBlock;
@property(nonatomic,strong)ShowMsgPlayViewShareButtonBlock shareBlock;
@property(nonatomic,strong)ShowMsgPlayViewSaveButtonBlock saveBlock;

//showImg
@property(nonatomic, readonly, strong)UIImage *showImg;

-(void)shareBtnHidden:(BOOL)hidden;

-(void)releaseResource;

@end
