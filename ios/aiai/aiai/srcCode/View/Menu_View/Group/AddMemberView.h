//
//  AddMemberView.h
//  F2CPlatform
//
//  Created by Circle Liu on 14-8-12.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import <UIKit/UIKit.h>
@class AddMemberView;
@class AddMemberAvatarView;

typedef void (^AvatarAction)(NSDictionary* userInfo);
typedef void (^CutDownAction) (AddMemberAvatarView* moveView);

@interface AddMemberView : UIView

- (void)refreshElement:(NSArray*)members;            //增减群成员后重新布局
- (void)resetGroupNickName:(NSString*)nickName;      //修改群昵称

- (id)initWithOwenerID:(NSString*)owenerID members:(NSArray*)members addAction:(dispatch_block_t)addAction avatarAction:(AvatarAction)avatarAction cutDownAction:(dispatch_block_t)cutDownAction;

@end

@interface AddMemberAvatarView : UIButton

- (id)initWithFrame:(CGRect)frame WithCutDownAction:(CutDownAction)cutDownBlock;

- (void)fillDataWithURL:(NSString*)avatarURL nameStr:(NSString*)nameStr userName:(NSString*)userName;

- (void)resetNickName:(NSString*)nickName;

@property(nonatomic, strong)UIButton* cutDownBtn;

@end