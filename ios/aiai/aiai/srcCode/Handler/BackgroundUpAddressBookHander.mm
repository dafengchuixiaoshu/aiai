//
//  BackgroundUpAddressBookHander.m
//  aiai
//
//  Created by wangnan on 15/4/21.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "BackgroundUpAddressBookHander.h"
#import "ContactHandler.h"

#import "UpContactsParamsPOJO.pb.h"
#import "IMPackageEngine.h"

#import <AddressBook/AddressBook.h>

/*
 后台上传通讯录逻辑:首先通过UID获取服务器联系人版本号（sVersion）, 如果没有版本号那么全量上传本地通讯录,如果有版本号 那么获取本地联系人版本号(lVersion)如果本地没有版本号(即联系人数据)那么先从服务器获取联系人,然后增量上传联系人。如果有版本号，那么用sVersion和lVersion进行对比 如果两个不相等 那么全量上传通讯录，如果两个相当那么增量上传通讯录
 */

/*
 数据格式，为了节省流量用了简写 { "uId": 15960, "phones": [ { "id": 123, -- 客户端的id "n":"张三", -- 名称 "t": 1, -- 操作类型 1为删除，2为新增，3为修改 
 * "p": [ { "pn": 18612345678,--电话号码 "pt": 2 --电话类型 }, { "pn": 15800000000, "pt": 1 } ] } ] }
 */

typedef void (^CompleteIncrementalUp)(NSDictionary *resultDic);


@implementation BackgroundUpAddressBookHander

+(void)upAddressBook{
    
    [BackgroundUpAddressBookHander getContactVersionComplete:^(int version) {
        
        NSString *versionStr = [NSString stringWithFormat:@"%zi",version];
        
        if (version>0) {//是否有标示
            NSString *path = [SaveManager libraryPath:AddressBookPath];
            NSDictionary *userAddressBook = [SaveManager readToDictionaryWithFilePath:path];
            if (userAddressBook) {//本地是否有缓存
                if (version==[[userAddressBook objectForKey:@"version"] intValue]) {//对比标示
                    
                    [BackgroundUpAddressBookHander incrementalUpDateContactWithComparBook:userAddressBook andVersion:versionStr complete:^(NSDictionary *resultDic) {

                        if (resultDic) {
                            NSString *path = [SaveManager libraryPath:AddressBookPath];
                            [SaveManager saveToFileWithDictionary:resultDic filePath:path];
                        }
                    }];
                    
                }else{
                    [BackgroundUpAddressBookHander fullUpDateContact];
                }
            }else{//没有缓存
                [BackgroundUpAddressBookHander getContactFromSeriesComplete:^(NSDictionary *resultDic) {
                    [BackgroundUpAddressBookHander incrementalUpDateContactWithComparBook:resultDic andVersion:versionStr complete:^(NSDictionary *resultDic) {
                        
                    }];
                }];
            }
        }else{//没有标示
            [BackgroundUpAddressBookHander fullUpDateContact];
        }
        
    }];
    
}

#pragma mark --- 获取服务器联系人版本号
+(void)getContactVersionComplete:(void (^)(int version))complete{

    com::mm::pb::po::UpContactsParamsPOJO builder;
    builder.set_uid((int)[GlobalUserInfo myselfInfo].userID);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"getMobileVersion.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::UpContactsParamsPOJO outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        
        ::google::protobuf::int32 code = outPutObj.resultcode();
        ::google::protobuf::int32 version;
        if (code == 0) {
            version = outPutObj.version();
        }else{
            version = 0;
        }
        
        complete(version);
        
    } failed:^(NSError *error) {
        complete(0);
    }];
}


#pragma mark -- 从服务器获取本UID的联系人信息
+(void)getContactFromSeriesComplete:(void (^)(NSDictionary *resultDic))complete{
    
    com::mm::pb::po::UpContactsParamsPOJO builder;
    builder.set_uid((int)[GlobalUserInfo myselfInfo].userID);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"queryContacts.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::UpContactsParamsPOJO outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        
        ::google::protobuf::int32 resultSize = outPutObj.phones_size();
        ::google::protobuf::int32 code = outPutObj.resultcode();
        if (code == 0) {
            
            NSMutableDictionary *resultDic = [[NSMutableDictionary alloc] init];
            for (int i = 0; i<resultSize; ++i) {
                ContactsStatuResultModel *model = [[ContactsStatuResultModel alloc] init];
                
                ::com::mm::pb::po::UpContacts tempModel = outPutObj.phones(i);
                
                model.firstName = [NSString stringWithUTF8String:tempModel.n().c_str()];
                
                ::google::protobuf::int32 phoneSize = tempModel.p_size();
                
                NSMutableArray *phoneArr = [[NSMutableArray alloc] init];
                for (int j = 0 ; j<phoneSize; ++j) {
                    ::com::mm::pb::po::Phone phone = tempModel.p(j);
                    
                    PhoneModel *pModel = [[PhoneModel alloc] init];
                    pModel.phoneStr = [NSString stringWithFormat:@"%zi",phone.pn()];
                    pModel.phoneType = phone.pt();
                    [phoneArr addObject:pModel];
                }
                model.phoneArray = phoneArr;
                model.recordID = [NSString stringWithFormat:@"%zi",tempModel.id()];
                
                [resultDic setObject:model forKey:model.recordID];
            }
            complete(resultDic);
        }
        
    } failed:^(NSError *error) {
        complete(nil);
    }];
}

#pragma mark --- 增量上传
+(void)incrementalUpDateContactWithComparBook:(NSDictionary *)serverDic andVersion:(NSString *)version complete:(CompleteIncrementalUp)block{
    
    NSDictionary *bookDic = [BackgroundUpAddressBookHander getAddressBooks];
    if (bookDic == nil) {
        block(nil);
        return ;
    }
    
    
    com::mm::pb::po::UpContactsParamsPOJO builder;
    builder.set_uid((int)[GlobalUserInfo myselfInfo].userID);
    
    NSArray *keyArray = [bookDic allKeys];
    for (int i = 0; i<[keyArray count]; ++i) {
        
        ::com::mm::pb::po::UpContacts *contactObj = builder.add_phones();
        
        
        NSString *recordID = [keyArray objectAtIndex:i];
        ContactsStatuResultModel *bookModel = [bookDic objectForKey:recordID];
        
        int action = 0;
        if (bookModel) {
            ContactsStatuResultModel *serverModel = [serverDic objectForKey:recordID];
            if (![BackgroundUpAddressBookHander isEqual:serverModel toModel:bookModel]) {//修改
                action = 3;
            }
        }else{//删除
            action = 1;
        }
        if (action > 0) {
            NSString *name = bookModel.nickName;
            contactObj->set_id([recordID intValue]);
            contactObj->set_n([name UTF8String]);
            contactObj->set_t(action);
            
            NSArray *phones = bookModel.phoneArray;
            for (PhoneModel *pModel in phones){
                ::com::mm::pb::po::Phone* phoneList = contactObj->add_p();
                
                NSString *pn = pModel.phoneStr;
                if (pn) {
                    phoneList->set_pn([pn integerValue]);
                }
                NSInteger pt = pModel.phoneType;
                if (pt) {
                    phoneList->set_pt(pt);
                }
            }
        }
    }
    //新增的联系人
    for (NSString *recordID in keyArray) {
        
        ContactsStatuResultModel *sModel = [serverDic objectForKey:recordID];
        if (!sModel) {
            
            ContactsStatuResultModel *lModel = [bookDic objectForKey:recordID];
            ::com::mm::pb::po::UpContacts *contactObj = builder.add_phones();
            
            
            NSString *name = lModel.nickName;
            contactObj->set_id([recordID intValue]);
            contactObj->set_n([name UTF8String]);
            contactObj->set_t(2);
            
            NSArray *phones = lModel.phoneArray;
            for (PhoneModel *pModel in phones){
                ::com::mm::pb::po::Phone* phoneList = contactObj->add_p();
                
                NSString *pn = pModel.phoneStr;
                if (pn) {
                    phoneList->set_pn([pn integerValue]);
                }
                NSInteger pt = pModel.phoneType;
                if (pt) {
                    phoneList->set_pt(pt);
                }
            }
        }
    }
    
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"incrementalUpdateContacts.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::UpContactsParamsPOJO outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        
        ::google::protobuf::int32 code = outPutObj.resultcode();
        if (code == 0) {
            
        }
        
        block(nil);
        
    } failed:^(NSError *error) {
        block(nil);
    }];
    
}

//对比两个联系人数据
+(BOOL)isEqual:(ContactsStatuResultModel *)model1 toModel:(ContactsStatuResultModel *)model2{
    NSString *n1 = model1.nickName;
    NSString *n2 = model2.nickName;
    if (![n1 isEqualToString:n2])
        return NO;
    NSArray *phones1 = model1.phoneArray;
    NSArray *phones2 = model2.phoneArray;
    if(phones1.count != phones2.count)
        return NO;
    for (int i = 0; i<phones1.count; i++) {
        PhoneModel *temp1Model = [phones1 objectAtIndex:i];
        PhoneModel *temp2Model = [phones2 objectAtIndex:i];
        NSString *pn1 = temp1Model.phoneStr;
        NSInteger pt1 = temp1Model.phoneType;
        NSString *pn2 = temp2Model.phoneStr;
        NSInteger pt2 = temp2Model.phoneType;
        if (![pn1 isEqualToString:pn2])
            return NO;
        if (pt1!=pt2)
            return NO;
    }
    return YES;
}

#pragma mark --- 全量上传通讯录
+(void)fullUpDateContact{
    
    com::mm::pb::po::UpContactsParamsPOJO builder;
    builder.set_uid((int)[GlobalUserInfo myselfInfo].userID);
    
    NSDictionary* bookDic = [BackgroundUpAddressBookHander getAddressBooks];
    
    NSArray *bookArray = [bookDic allValues];
    for (int i = 0; i<[bookArray count]; ++i) {
        
        ::com::mm::pb::po::UpContacts *contactObj = builder.add_phones();
        
        ContactsStatuResultModel *model = [bookArray objectAtIndex:i];
        
        NSString *recordID = model.recordID;
        contactObj->set_id([recordID intValue]);
        
        NSString *strName = model.nickName;
        if (strName) {
            contactObj->set_n([strName UTF8String]);
        }
        NSArray *phones = model.phoneArray;
        for (PhoneModel *pModel in phones){
            ::com::mm::pb::po::Phone* phoneList = contactObj->add_p();
            
            NSString *pn = pModel.phoneStr;
            if (pn) {
                phoneList->set_pn([pn integerValue]);
            }
            NSInteger pt = pModel.phoneType;
            if (pt) {
                phoneList->set_pt(pt);
            }
        }
    }
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"fullUpCont.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::UpContactsParamsPOJO outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        
        ::google::protobuf::int32 code = outPutObj.resultcode();
        if (code == 0) {

            ::google::protobuf::int32 version = outPutObj.version();
            
            NSMutableDictionary *contactDic = [[NSMutableDictionary alloc] init];
            [contactDic setObject:[NSNumber numberWithInt:version] forKey:@"version"];
            [contactDic setObject:bookArray forKey:@"addressBook"];
            NSDictionary *dic = [NSDictionary dictionaryWithObject:contactDic forKey:[NSNumber numberWithInteger:[GlobalUserInfo myselfInfo].userID]];
            
            NSString *path = [SaveManager libraryPath:AddressBookPath];
            [SaveManager saveToFileWithDictionary:dic filePath:path];
        }
        
    } failed:^(NSError *error) {
        
    }];
    
}

#pragma mark 获取本地通讯录
+(NSDictionary *)getAddressBooks{
    //新建一个通讯录类
    ABAddressBookRef addressBooks =  ABAddressBookCreateWithOptions(NULL, NULL);

    if (addressBooks == nil) {
//        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:@"请在iPhone的“设置-隐私-通讯录-ii”选项中，允许ii访问您的通讯录" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil];
//        [alert show];
        return nil;
    }
    
    //获取通讯录权限
    dispatch_semaphore_t sema = dispatch_semaphore_create(0);
    ABAddressBookRequestAccessWithCompletion(addressBooks, ^(bool granted, CFErrorRef error){dispatch_semaphore_signal(sema);});
    dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
    
    //获取通讯录中的所有人
    CFArrayRef allPeople = ABAddressBookCopyArrayOfAllPeople(addressBooks);
    //通讯录中人数
    CFIndex nPeople = ABAddressBookGetPersonCount(addressBooks);
    
    NSMutableDictionary *phoneList = [[NSMutableDictionary alloc] init];
    //循环，获取每个人的个人信息
    for (NSInteger i = 0; i < nPeople; i++)
    {
        NSString *recordID;
        ContactsStatuResultModel *phoneListModel = [[ContactsStatuResultModel alloc] init];
        //获取个人
        ABRecordRef person = CFArrayGetValueAtIndex(allPeople, i);
        //获取个人名字
        CFTypeRef abName = ABRecordCopyValue(person, kABPersonFirstNameProperty);
        CFTypeRef abLastName = ABRecordCopyValue(person, kABPersonLastNameProperty);
        CFStringRef abFullName = ABRecordCopyCompositeName(person);
        NSString *nameString = (__bridge NSString *)abName;
        NSString *lastNameString = (__bridge NSString *)abLastName;
        
        if ((__bridge id)abFullName != nil) {
            nameString = (__bridge NSString *)abFullName;
        } else {
            if ((__bridge id)abLastName != nil||(__bridge id)abName != nil)
            {
                nameString = [NSString stringWithFormat:@"%@ %@", nameString, lastNameString];
            }
        }
        recordID = [NSString stringWithFormat:@"%d",(int)ABRecordGetRecordID(person)];
//        [phoneListDic setObject:[NSString stringWithFormat:@"%d",(int)ABRecordGetRecordID(person)] forKey:@"recordID"];
        if (nameString) {
            phoneListModel.nickName = nameString;
        }else{
            phoneListModel.nickName = @"";
        }
        NSMutableArray *phonesArr = [[NSMutableArray alloc] init];
        
        ABPropertyID multiProperties[] = {
            kABPersonPhoneProperty,
//            kABPersonEmailProperty
        };
        NSInteger multiPropertiesTotal = sizeof(multiProperties) / sizeof(ABPropertyID);
        for (NSInteger j = 0; j < multiPropertiesTotal; j++) {
            ABPropertyID property = multiProperties[j];
            
            ABMultiValueRef valuesRef = ABRecordCopyValue(person, property);
            NSInteger valuesCount = 0;
            if (valuesRef != nil) valuesCount = ABMultiValueGetCount(valuesRef);
            
            if (valuesCount == 0) {
                CFRelease(valuesRef);
                continue;
            }
            
            PhoneModel *phoneModel = [[PhoneModel alloc] init];
            //获取电话号码和email
            for (NSInteger k = 0; k < valuesCount; k++) {
                NSString *phonenum =  (__bridge NSString*)ABMultiValueCopyValueAtIndex(valuesRef, k);
                NSString *strphonetype = (__bridge NSString *)ABMultiValueCopyLabelAtIndex(valuesRef, k);
                int phoneType = 2;
                if ([strphonetype isEqualToString:@"_$!<Home>!$_"]) {
                    phoneType = 1;
                }
                if ([strphonetype isEqualToString:@"_$!<Work>!$_"]) {
                    phoneType = 3;
                }
                phoneModel.phoneStr = phonenum;
                phoneModel.phoneType = phoneType;
                [phonesArr addObject:phoneModel];
                
//                CFRelease(phonenum);
            }
            CFRelease(valuesRef);
        }
        phoneListModel.phoneArray = phonesArr;
        //将个人信息添加到数组中，循环完成后addressBookTemp中包含所有联系人的信息
        phoneListModel.recordID = recordID;
        
        [phoneList setValue:phoneListModel forKey:recordID];
        
        if (abName) CFRelease(abName);
        if (abLastName) CFRelease(abLastName);
        if (abFullName) CFRelease(abFullName);
    }
    return phoneList;
}

@end
