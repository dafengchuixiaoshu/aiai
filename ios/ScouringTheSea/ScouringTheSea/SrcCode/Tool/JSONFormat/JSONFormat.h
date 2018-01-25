//
//  NSString+JSON.h
//  F2CPlatform
//
//  Created by zhangyang on 13-12-14.
//  Copyright (c) 2013年 zhangyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (JSON)
-(id)myObjectFromJSONString;
-(NSData*)myJSONData;
@end

@interface NSData (JSON)
-(id)myObjectFromJSONData;
@end

@interface NSArray (JSON)
-(NSData*)myJSONData;
@end

@interface NSDictionary (JSON)
-(NSData*)myJSONData;
@end