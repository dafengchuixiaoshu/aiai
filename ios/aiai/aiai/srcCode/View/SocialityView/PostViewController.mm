//
//  PostViewController.m
//  aiai
//
//  Created by wangnan on 15/11/3.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "PostViewController.h"
#import "InitiateGroupChatViewController.h"
#import "SocialityListViewController.h"

#import "requestParam.pb.h"
#import "sendStatus.pb.h"

@interface PostViewController ()<UITextViewDelegate,InitiateGroupChatDelegate>

@property(nonatomic, copy)NSURL *videoPath;
@property(nonatomic, strong)NSMutableArray *atNameArray;            //所有at的人昵称的数组
@property(nonatomic, strong)NSMutableArray *atIDArray;            //所有at的人id的数组

@property(nonatomic, strong)UITextView *messageTextView;        //消息输入框
@property(nonatomic, strong)UIImageView *thumbnailView;        //缩略图

@end

@implementation PostViewController

-(id)initWithVideoPath:(NSURL*)videoPath{
    if (self = [super init]) {
        
        _videoPath = videoPath;
        _atNameArray = [[NSMutableArray alloc] init];
        _atIDArray = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    //title
    self.centerBtnImageName = @"发布";
    self.rightBtnImageName = @"发送";
    
    //输入框
    CGFloat padding = 13;
    _messageTextView = [[UITextView alloc] init];
    _messageTextView.frame = CGRectMake(padding, 64, [UIScreen mainScreen].bounds.size.width - padding*2, 100);
    _messageTextView.font = [UIFont systemFontOfSize:14];
    _messageTextView.textColor = Every_ContentTextColor;
    _messageTextView.delegate = self;
    [self.view addSubview:_messageTextView];
    
    //@人
    
    //视频缩略图
    _thumbnailView = [[UIImageView alloc] init];
    _thumbnailView.frame = CGRectMake(padding, _messageTextView.frame.origin.y + _messageTextView.frame.size.height + 10, 60, 60);
    _thumbnailView.backgroundColor = [UIColor clearColor];
    _thumbnailView.image = [AppUtils getImage:_videoPath];
    [self.view addSubview:_thumbnailView];
    
    //添加手势
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] init];
    [tap addTarget:self action:@selector(resign)];
    [self.view addGestureRecognizer:tap];
    
    UIButton *btn = [UIButton buttonWithType:UIButtonTypeCustom];
    btn.backgroundColor = [UIColor redColor];
    btn.frame = CGRectMake(100, 100, 100, 100);
    [btn addTarget:self action:@selector(aa) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btn];
}

-(void)aa{
    SocialityListViewController *con = [[SocialityListViewController alloc] init];
    [self.navigationController pushViewController:con animated:YES];
}

#pragma mark -- titleBtnAction
-(void)rightItemClicked:(id)sender{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"sendContent";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    //set发帖人ID
    NSString *value1 = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    tutorial::RequestParam_Param *param1 = builder.add_param();
    [self setParams:param1 AndKey:@"uid" Value:value1];
    
    //set视频标签
    tutorial::RequestParam_Param *param2 = builder.add_param();
    [self setParams:param2 AndKey:@"label" Value:@"我是标签"];
    
    //set特效 没用就传空
    tutorial::RequestParam_Param *param3 = builder.add_param();
    [self setParams:param3 AndKey:@"effect" Value:@""];
    
    //set发帖人说的话
    tutorial::RequestParam_Param *param6 = builder.add_param();
    [self setParams:param6 AndKey:@"content" Value:_messageTextView.text];
    
    //set at的人列表
    tutorial::RequestParam_Param *param7 = builder.add_param();
    [self setParams:param7 AndKey:@"refer" Value:@"88"];
    
    __weak PostViewController *weakSelf = self;
    [self upLoadVideoComplete:^(NSString *imgKey) {
        
        tutorial::RequestParam_Param *param4 = builder.add_param();
        [self setParams:param4 AndKey:@"firstKey" Value:imgKey];
        
    } :^(NSString *videoKey) {
        
        //获得Key之后 set params
        tutorial::RequestParam_Param *param5 = builder.add_param();
        [self setParams:param5 AndKey:@"key" Value:videoKey];
        
        //申请内存对数据进行序列化
        ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
        builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
        
        [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
            
        } successed:^(NSDictionary *dic) {
            
            NSData* data = [dic objectForKey:@"data"];
            tutorial::SendStatus outPutObj;
            outPutObj.ParseFromArray([data bytes], (int)[data length]);
            ::google::protobuf::int32 code = outPutObj.status();
            if (code == 1) {
                //发送成功;
                [weakSelf.navigationController popToRootViewControllerAnimated:YES];
            }else if (code <= 0){
                NSLog(@"发送失败");
            }
            
        } failed:^(NSError *error) {
            
        }];
        
    }];
}

#pragma mark ---set PB Params
-(void)setParams:(tutorial::RequestParam_Param *)params AndKey:(NSString*)key Value:(NSString*)value{
    
    params->set_paramkey([key UTF8String]);
    params->set_paramvalue([value UTF8String]);
}

#pragma mark -- upLoadVideo Key
-(void)upLoadVideoComplete:(void (^)(NSString *imgKey))successedImg :(void (^)(NSString *videoKey))successedVideo{
    
    UIImage *firstFrameImage = [AppUtils getImage:_videoPath];;
    [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[self class] data:UIImagePNGRepresentation(firstFrameImage)  prepare:^{
        [WaitingAlertView showWaitingViewAtView:self.view];
    } successed:^(NSDictionary *dic) {
        
        if ([[dic objectForKey:@"result"] isEqualToString:@"error"]) {
            
            return ;
        }
        
        NSString *imgKey = [dic objectForKey:@"fileName"];
        successedImg(imgKey);
        
        //先上传头像获取key
        NSData* uploadData = [NSData dataWithContentsOfURL:_videoPath];
        [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[self class] data:uploadData prepare:^{
            
        } successed:^(NSDictionary *dic) {
            if ([[dic objectForKey:@"result"] isEqualToString:@"error"] || !dic) {
                
                [WaitingAlertView hideWaitingView];
                return ;
            }
            
            NSString *videoKey = [dic objectForKey:@"fileName"];
            successedVideo(videoKey);
            
        } failed:^(NSError *error) {
            
        }];
    } failed:^(NSError *error) {
        
    }];
}

-(void)resign{
    [_messageTextView resignFirstResponder];
}

#pragma mark -- textViewDelegate
- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text{
    
    if ([text isEqualToString:@"@"]) {
        
        InitiateGroupChatViewController *con = [[InitiateGroupChatViewController alloc] initWithGroupID:nil existingMembers:nil];
        con.delegate = self;
        con.conType = AT;
        [self.navigationController presentViewController:con animated:YES completion:nil];
        
    }else if ([text isEqualToString:@""]){
        //删除字符操作
        NSString *atStr = [_atNameArray lastObject];
        if (atStr && range.location != 0) {
            NSString *atName = [textView.text substringWithRange:NSMakeRange(range.location - [atStr length], [atStr length]+1)];
            if ([[NSString stringWithFormat:@"@%@",atStr] isEqualToString:atName]) {
                
                //从光标位置删除at的人 再重新设置光标位置
                NSString *text = [NSString stringWithFormat:@"%@%@",[textView.text substringToIndex:(range.location - [atStr length])],[_messageTextView.text substringFromIndex:range.location+1]];
                textView.text = text;
                _messageTextView.selectedRange = NSMakeRange(range.location - [atStr length], 0);
                
                [_atNameArray removeLastObject];
                [_atIDArray removeLastObject];
                return NO;
            }
        }
        
    }
    return YES;
}

-(void)ATNewPeople:(NSArray *)atArray{
    
    [self dismissViewControllerAnimated:YES completion:^{
        
        NSMutableString *atNickName = [[NSMutableString alloc] init];
        for (IMPackageBuddyData *data in atArray) {
            
            [_atNameArray addObject:data.buddyNickName];
            [_atIDArray addObject:data.buddyCMSID];
            [atNickName appendString:@"@"];
            [atNickName appendString:data.buddyNickName];
        }
        
        //弹出键盘
        [_messageTextView becomeFirstResponder];
        
        // 获得光标所在的位置
        NSInteger location = _messageTextView.selectedRange.location;
        NSString *result = [NSString stringWithFormat:@"%@%@%@",[_messageTextView.text substringToIndex:location],atNickName,[_messageTextView.text substringFromIndex:location]];
        // 将调整后的字符串
        _messageTextView.text = result;
        
        _messageTextView.selectedRange = NSMakeRange(location + [atNickName length], 0);
        
    }];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
