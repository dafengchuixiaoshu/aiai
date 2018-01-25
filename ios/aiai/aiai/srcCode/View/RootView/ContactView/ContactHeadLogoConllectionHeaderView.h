//
//  ContactHeadLogoConllectionHeaderView.h
//  aiai
//
//  Created by zhangyang on 15/1/14.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void (^ContactHeadLogoConllectionHeaderViewBtnAction)(void);

@interface ContactHeadLogoConllectionHeaderView : UICollectionReusableView

@property(nonatomic,copy)ContactHeadLogoConllectionHeaderViewBtnAction openCloseBtnBlock;

//旋转button
-(void)changeButtonTransForm;
@end
