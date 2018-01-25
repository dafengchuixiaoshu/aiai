//
//  ContactHandler.h
//  aiai
//
//  Created by wangnan on 15/4/19.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BaseHandler.h"
#import "BaseModel.h"

typedef void (^CompleteUp)(NSArray *resultArray);

@interface ContactHandler : BaseHandler

+(void)getMyContactsStatuWithFriendList:(NSArray*)friendList prepare:(void (^)(void))prepare complete:(CompleteUp)block;

@end

typedef NS_ENUM(NSInteger, Buddystatus) {
    //status = 0:未注册未添加好友
    //status = 1:已注册未添加好友
    //status = 2:已经为好友
    noRegistered = 0,
    notFriend,
    isFriend
};
@interface ContactsStatuResultModel : BaseModel

@property(nonatomic, strong)NSArray *phoneArray;        //存放PhoneModel
@property(nonatomic, copy)NSString *firstName;          //通讯录中的firstname
@property(nonatomic, copy)NSString *lastName;           //通讯录中的lastname
@property(nonatomic, copy)NSString *face_url;
@property(nonatomic, copy)NSString *nickName;
@property(nonatomic, assign)Buddystatus status;
@property(nonatomic, assign)NSInteger otherId;          //对方的ID
@property(nonatomic , copy)NSString *recordID;          //联系人在本地通讯录中的id

@end

@interface PhoneModel : BaseModel
@property(nonatomic, copy)NSString *phoneStr;
@property(nonatomic, assign)NSInteger phoneType;

@end