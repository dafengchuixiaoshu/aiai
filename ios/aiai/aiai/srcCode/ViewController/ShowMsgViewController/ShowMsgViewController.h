//
//  ShowMsgViewController.h
//  aiai
//
//  Created by zhangyang on 15/4/13.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseViewController.h"
#import "IMPackageData.h"

@class ShowMsgViewController;
@protocol ShowMsgViewControllerDelegate <NSObject>

-(void)willDisappear:(ShowMsgViewController*)vc;

@end


typedef void (^SendVideoAction)(NSURL *videoUrl, CGFloat time, NSString *videoTitle, NSString *text, IM_MESSAGE_TYPE type);
typedef void (^SendImgAction)(UIImage *sendImg, NSString *text, IM_MESSAGE_TYPE type);

typedef void (^LookFinishBlock)(IMPackageMessageData* currentMsgData);

@interface ShowMsgViewController : BaseViewController

-(id)initWithIMPackageSessionData:(IMPackageSessionData*)sessionData;

@property(nonatomic, strong)LookFinishBlock finishBlock;

@property(nonatomic,weak)id<ShowMsgViewControllerDelegate>delegate;

@property(nonatomic, strong)SendVideoAction sendVideoBlock;
@property(nonatomic, strong)SendImgAction sendImgBlock;

@end
