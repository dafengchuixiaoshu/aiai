//
//  RegisterNextView.h
//  aiai
//
//  Created by zhangyang on 15/4/2.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LoginTextField.h"
#import "PlayMovieView.h"

typedef void (^registerNextBtnAction)(void);

@interface RegisterNextView : UIView

@property (nonatomic, retain) LoginTextField* nickNameField;
@property (nonatomic, retain) LoginTextField* passwordField;
@property (nonatomic, retain) UIButton* passwordShowBtn;
@property (nonatomic, retain) UIButton* finishBtn;
@property (nonatomic, retain) UIButton* camerBtn;

@property(nonatomic,strong) PlayMovieView* moveView;

@property(nonatomic,strong)registerNextBtnAction camerBtnBtnBlock;
@property(nonatomic,strong)registerNextBtnAction finishBtnBlock;

-(void)replaceMovieViewWithMoviePath:(NSString*)path;

@end
