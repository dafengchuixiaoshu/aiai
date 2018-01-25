//
//  FilesDownManage.h
//  Created by yu on 13-1-21.


#import <Foundation/Foundation.h>
#import "CommonHelper.h"
#import "LoadFilterModel.h"

#import <AVFoundation/AVAudioPlayer.h>

typedef void (^RequestBlock)(LoadFilterModel *model);

@interface FilesDownManage : NSObject
//单例创建
+(FilesDownManage *) sharedFilesDownManage;

//清掉请求
-(void)clearManage;

//开始请求
-(void)requestDownLoadWithfilterModel:(LoadFilterModel*)model receiveBlock:(RequestBlock)receiveBlock successed:(RequestBlock)successed failed:(RequestBlock)failed;

@end


