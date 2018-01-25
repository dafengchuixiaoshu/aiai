//
//  ContactEntity.m
//  aiai
//
//  Created by zhangyang on 15/1/12.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ContactEntity.h"
#import "PinyinHelper.h"
#import "HanyuPinyinOutputFormat.h"

@implementation ContactEntity

-(id)init{
    if (self = [super init]) {
        self.buddyData = [[IMPackageBuddyData alloc] init];
//        self.sessionData = [[IMPackageSessionData alloc] init];
    }
    return self;
}

-(id)initWithContactEntity:(ContactEntity*)entity{
    if (self = [super init]) {
        self.localID = [entity.localID copy];
        self.buddyUserName = [entity.buddyUserName copy];

        self.buddyData = [[IMPackageBuddyData alloc] init];
        self.buddyData.buddyCommunityUrl = [entity.buddyData.buddyCommunityUrl copy];
        self.buddyData.buddyEmotionMood = [entity.buddyData.buddyEmotionMood copy];
        self.buddyData.buddyIsBuddy = entity.buddyData.buddyIsBuddy;
        self.buddyData.buddyPortraitKey = [entity.buddyData.buddyPortraitKey copy];
        self.buddyData.buddyQrerUrl = [entity.buddyData.buddyQrerUrl copy];
        self.buddyData.buddyCMSID = [entity.buddyData.buddyCMSID copy];
        self.buddyData.buddyUserName = [entity.buddyData.buddyUserName copy];
        
//        self.sessionData = [[IMPackageSessionData alloc] init];
//        self.sessionData.pPortraitKey = [entity.sessionData.pPortraitKey copy];
//        self.sessionData.pMsgId = [entity.sessionData.pMsgId copy];
//        self.sessionData.pSessionId = entity.sessionData.pSessionId;
//        self.sessionData.pSessionName = [entity.sessionData.pSessionName copy];
//        self.sessionData.pDraftContent = [entity.sessionData.pDraftContent copy];
//        self.sessionData.nUnreadNum = entity.sessionData.nUnreadNum;
//        self.sessionData.eType = entity.sessionData.eType;
//        self.sessionData.eTop = entity.sessionData.eTop;
//        self.sessionData.eRemind = entity.sessionData.eRemind;
//        self.sessionData.eDraftChanged = entity.sessionData.eDraftChanged;
    }
    return self;
}

+(NSDictionary*)firstLetterContactDictionaryWithBuddyList:(NSArray*)buddyList{
    NSMutableDictionary* tempfirstLetterToContactDataDic = [[NSMutableDictionary alloc] init];

    HanyuPinyinOutputFormat *outputFormat=[[HanyuPinyinOutputFormat alloc] init];
    [outputFormat setToneType:ToneTypeWithoutTone];
    [outputFormat setVCharType:VCharTypeWithV];
    [outputFormat setCaseType:CaseTypeUppercase];
    
    for (int i = 0;i<[buddyList count];i++) {
        
        IMPackageBuddyData *buddyData = [buddyList objectAtIndex:i];
        
        //抽取首字母,ChineseToPinyin不支持繁体字，废弃。
        //NSString* pinyin = [ChineseToPinyin pinyinFromChiniseString:data.name];
        NSString* pinyin = [PinyinHelper toHanyuPinyinStringWithNSString:buddyData.buddyUserName withHanyuPinyinOutputFormat:outputFormat withNSString:@""];
        NSString* pinyinNoSpace = [pinyin stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];//去掉空格
        //去除错误数据
        if ([pinyinNoSpace isEmpty]) {
            continue;
        }
        NSString *firstLetter = [[pinyinNoSpace substringToIndex:1] uppercaseString];//并转成大写
        firstLetter = [ContactEntity SpecialCharacterConversion:firstLetter];
        
        //把相同首字母的对象收到一个数组里，用首字母当key保存到字典里
        NSMutableArray* firstLetterDataArray = [tempfirstLetterToContactDataDic objectForKey:firstLetter];
        if (firstLetterDataArray == nil) {
            firstLetterDataArray = [[NSMutableArray alloc] init];
            [tempfirstLetterToContactDataDic setObject:firstLetterDataArray forKey:firstLetter];
        }
        [firstLetterDataArray addObject:buddyData];
    }

    NSArray* firstLetterArray = [ContactEntity firstLetterArray:tempfirstLetterToContactDataDic];
    //获取结束把数组按升序重新排列
    for (NSString *eachNameIndex in firstLetterArray) {
        NSSortDescriptor *nameDescriptor = [[NSSortDescriptor alloc] initWithKey:@"buddyUserName" ascending:NO];
        NSArray *descriptors = [NSArray arrayWithObject:nameDescriptor];
        NSMutableArray* someFirstLetterArray = [tempfirstLetterToContactDataDic objectForKey:eachNameIndex];
        [someFirstLetterArray sortUsingDescriptors:descriptors];
    }
    
    return tempfirstLetterToContactDataDic;
}


+(NSArray*)firstLetterArray:(NSDictionary*)contactBuddyDic{
    return [[contactBuddyDic allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
}

#pragma mark private method
+(NSString*)SpecialCharacterConversion:(NSString*)firstLetter{
    NSString* pinyinInitials = @"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    NSRange range = [pinyinInitials rangeOfString:firstLetter];
    if (range.length == 0) {
        firstLetter = @"#";
    }
    return firstLetter;
}

@end
