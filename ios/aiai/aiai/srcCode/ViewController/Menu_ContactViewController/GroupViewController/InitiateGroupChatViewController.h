//
//  CreateGroupChatViewController.h
//  F2CPlatform
//
//  Created by Circle Liu on 14-7-20.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "IMPackageData.h"

@protocol InitiateGroupChatDelegate <NSObject>

@optional
-(void)InitiateGroupChat_NewGroupID:(NSString*)groupID isCreatGroup:(BOOL)isCreat;

-(void)ATNewPeople:(NSArray *)atArray;
@end

typedef NS_ENUM(NSInteger, ConType){
    
    NORMAL  = 0,
    AT      = 1,
};

@protocol GroupViewControllerDelegate;
@interface InitiateGroupChatViewController : BaseViewController

@property (nonatomic, assign) id <InitiateGroupChatDelegate> delegate;
@property (nonatomic, assign) id<GroupViewControllerDelegate>groupDel;

- (id)initWithGroupID:(NSString*)groupID existingMembers:(NSArray*)existingMembers;

@property(nonatomic, assign)ConType conType;        //默认为普通的邀请成员模式

@end