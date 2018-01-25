//
//  FeedBackViewController.m
//  aiai
//
//  Created by wangnan on 15/5/20.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "FeedBackViewController.h"

#import "UITextView+Placeholder.h"

#import "UpdateBackPOJO.pb.h"
#import "OutputResultData.pb.h"

@interface FeedBackViewController ()<UITextViewDelegate>

@property(nonatomic, strong)UITextView *textView;

@end

@implementation FeedBackViewController

-(void)viewDidLoad
{
    
    [super viewDidLoad];
    
    self.leftBtnImageName = @"取消";
    self.centerBtnImageName = @"意见反馈";
    self.rightBtnImageName = @"发送";
    
    CGFloat bg_y = 64;
    UIView *textBgView = [[UIView alloc] initWithFrame:CGRectMake(0, bg_y, [UIScreen mainScreen].bounds.size.width, ([UIScreen mainScreen].bounds.size.height - bg_y)/3)];
    textBgView.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.05];
    [self.view addSubview:textBgView];
    
    UIImageView *lineView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
    lineView.frame = CGRectMake(0, 0, textBgView.frame.size.width, 1);
    [textBgView addSubview:lineView];
    
    CGFloat origin_x = 13;
    CGFloat origin_y = 5;
    self.textView = [[UITextView alloc] initWithFrame:CGRectMake(origin_x, origin_y, textBgView.frame.size.width - 13*2, textBgView.frame.size.height - origin_x)];
    self.textView.backgroundColor = [UIColor clearColor];
    self.textView.font = [UIFont systemFontOfSize:14];
    self.textView.textColor = [UIColor whiteColor];
    [self.textView setPlaceholder:@"快来向我们吐槽吧"];
    self.textView.delegate = self;
    [textBgView addSubview:self.textView];
}

- (void)rightItemClicked:(id)sender {
    
    com::mm::pb::po::UpdateBackPOJO builder;
    builder.set_uid((int)[GlobalUserInfo myselfInfo].userID);
    builder.set_content([self.textView.text UTF8String]);
    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
    NSString *appCurVersion = [infoDictionary objectForKey:@"CFBundleShortVersionString"];   // 当前应用软件版本号
    builder.set_version([appCurVersion UTF8String]);
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    __weak FeedBackViewController *weakSelf = self;
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"getUserFeedBack.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        if (code >= 0) {
            [weakSelf.navigationController popViewControllerAnimated:YES];
        }
        else{
            
        }
        
    } failed:^(NSError *error) {

    }];
    
}

#pragma mark textviewDelegate
- (void)textViewDidChange:(UITextView *)textView
{
    [self.textView changePlaceholder];
}

@end
