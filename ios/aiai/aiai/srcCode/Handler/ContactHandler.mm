//
//  ContactHandler.m
//  aiai
//
//  Created by wangnan on 15/4/19.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ContactHandler.h"
#import "IMPackageEngine.h"
#import "ContactsParamsPOJO.pb.h"

#import <AddressBook/AddressBook.h>
#import <objc/runtime.h>

@implementation ContactHandler

+(void)getMyContactsStatuWithFriendList:(NSArray*)friendList prepare:(void (^)(void))prepare complete:(CompleteUp)block{
    
    ABAddressBookRef tmpAddressBook = nil;
    tmpAddressBook=ABAddressBookCreateWithOptions(NULL, NULL);
    
    if (tmpAddressBook == nil) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:@"请在iPhone的“设置-隐私-通讯录-ii”选项中，允许ii访问您的通讯录" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil];
        [alert show];
        return ;
    }
    
    if (prepare) {
        prepare();
    }
    
    com::mm::pb::po::ContactsParamsPOJO builder;
    builder.set_uid((int)[GlobalUserInfo myselfInfo].userID);
    
    for (int i = 0; i<[friendList count]; ++i) {
        
        ContactsStatuResultModel *model = [friendList objectAtIndex:i];
        ::com::mm::pb::po::Contacts *contactObj = builder.add_contactslist();
        
        ::com::mm::pb::po::ConName *name = new ::com::mm::pb::po::ConName();
        name->set_firstname([model.firstName? :model.nickName UTF8String]);
        name->set_lastname([model.lastName? :@"" UTF8String]);
        contactObj->set_allocated_name(name);
        
        for (int j = 0; j<[model.phoneArray count]; ++j) {
            
            ::com::mm::pb::po::ConPhone* phone = contactObj->add_phone();
            
            PhoneModel *pModel = [model.phoneArray objectAtIndex:j];
            phone->set_value([pModel.phoneStr? :@"" UTF8String]);
            
            NSString *type = [NSString stringWithFormat:@"%zi",pModel.phoneType];
            phone->set_type([type UTF8String]);
        }
        
        
    }
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"getMyContactsStatus.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::ContactsParamsPOJO outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        
        ::google::protobuf::int32 contactCount = outPutObj.resultcontacts_size();
        
        NSMutableArray *resultArray = [[NSMutableArray alloc] init];
        for (int i = 0; i<contactCount; ++i) {
            
            ContactsStatuResultModel *model = [[ContactsStatuResultModel alloc] init];
            
            ::com::mm::pb::po::ResultContacts *tempModel = outPutObj.mutable_resultcontacts(i);
            ::com::mm::pb::po::ConName name = tempModel->ResultContacts::name();
            model.firstName = [NSString stringWithUTF8String:name.firstname().c_str()];
            model.lastName = [NSString stringWithUTF8String:name.lastname().c_str()];
            model.face_url = [NSString stringWithUTF8String:tempModel->face_url().c_str()? :[@"" UTF8String]];
            model.nickName = [NSString stringWithUTF8String:tempModel->nickname().c_str()? :[@"" UTF8String]];
            model.status = (Buddystatus)tempModel->ResultContacts::status();
            
            PhoneModel *pModel = [[PhoneModel alloc] init];
            pModel.phoneStr = [NSString stringWithUTF8String:tempModel->ResultContacts::phone().c_str()];
            pModel.phoneType = 1;
            model.phoneArray = [NSArray arrayWithObject:pModel];
            
            model.otherId = tempModel->ResultContacts::otherid();
            [resultArray addObject:model];
        }
        if (block) {
            block(resultArray);
        }
        
    } failed:^(NSError *error) {
        block(nil);
    }];
    
}

@end

@implementation ContactsStatuResultModel

-(id)init{
    if (self = [super init]) {
        self.phoneArray = [[NSArray alloc] init];
    }
    return self;
}

@end

@implementation PhoneModel

@end
