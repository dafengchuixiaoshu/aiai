//
//  SocialityViewController.m
//  aiai
//
//  Created by wangnan on 15/11/16.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "SocialityDetailViewController.h"
#import "SocialityDetailHeaderView.h"
#import "UserPageViewController.h"

//View
#import "ReplyTableViewCell.h"
#import "MyTextView.h"
#import "UITextView+Placeholder.h"

//data
#import "requestParam.pb.h"
#import "sendStatus.pb.h"
#import "ReplyData.h"

@interface SocialityDetailViewController ()<UITableViewDataSource,UITableViewDelegate, UITextViewDelegate>

@property(nonatomic, strong)SocialityDetailHeaderView *headerView;
@property(nonatomic, strong)UITableView *replyTable;
@property(nonatomic, strong)MyTextView *commentText;

@property(nonatomic, copy)NSString *sid;
@property(nonatomic, strong)NSMutableArray *listArray;
@end

@implementation SocialityDetailViewController

-(id)initWithSid:(NSString*)sid{
    if (self = [super init]) {
        _sid = sid;
        _listArray = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    CGRect headerRect = CGRectMake(0, 64, self.view.frame.size.width, 500);
    _headerView = [[SocialityDetailHeaderView alloc] initWithFrame:headerRect];
    
    //输入框
    _commentText = [[MyTextView alloc] initWithFrame:CGRectMake(0, [UIScreen mainScreen].bounds.size.height - MessageTextViewHeight, [UIScreen mainScreen].bounds.size.width, MessageTextViewHeight)];
    _commentText.autoresizingMask = UIViewAutoresizingFlexibleWidth;
    _commentText.font = [UIFont systemFontOfSize:14];
    _commentText.delegate = self;
    _commentText.returnKeyType = UIReturnKeySend;
    [_commentText setPlaceholder:@"评论"];
    [self.view addSubview:_commentText];
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height = rect.size.height - rect.origin.y - _commentText.frame.size.height;
    _replyTable = [[UITableView alloc] init];
    _replyTable.frame = rect;
    _replyTable.backgroundColor = EveryView_BackGroundColor;
    [_replyTable setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    _replyTable.backgroundColor = [UIColor clearColor];
    _replyTable.delegate = self;
    _replyTable.dataSource = self;
    [_replyTable registerClass:[ReplyTableViewCell class] forCellReuseIdentifier:@"ReplyTableViewCell"];
    _replyTable.tableHeaderView = _headerView;
    [self.view addSubview:_replyTable];
    [self.view bringSubviewToFront:_commentText];
    
    //获取数据
    [self getDetailData];
    
}

-(void)viewWillAppear:(BOOL)animated{
    [self addNotification];
}

-(void)viewWillDisappear:(BOOL)animated{
    [self releaseNotification];
}

#pragma mark -- httpRequest
-(void)getDetailData{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"contentInfo";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    //set发帖人ID
    tutorial::RequestParam_Param *param = builder.add_param();
    [self setParams:param AndKey:@"sid" Value:_sid];
    
    __weak SocialityDetailViewController *weakSelf = self;
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::ContentInfo outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        NSLog(@"11");
        //如果存在表示结构体有值
        if (outPutObj.sid()) {
            
            //set view数据
            dispatch_async(dispatch_get_main_queue(), ^{
                //set数据
                weakSelf.headerView.nickName = [NSString stringWithUTF8String:outPutObj.nickname().c_str()];
                weakSelf.headerView.praiseCount = [NSString stringWithFormat:@"%d",outPutObj.praisecount()];
                weakSelf.headerView.commentCount = [NSString stringWithFormat:@"%d",outPutObj.commentcount()];
                weakSelf.headerView.perText = [NSString stringWithUTF8String:outPutObj.content().c_str()];
                weakSelf.headerView.sendTime = [AppUtils getTime:[NSNumber numberWithLongLong:outPutObj.sendtime()]];
                weakSelf.headerView.firstImgUrl = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:outPutObj.firstkey().c_str()]];
                weakSelf.headerView.videoUrl = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:outPutObj.videokey().c_str()]];
                
                weakSelf.headerView.perHeader.headerVideoPath = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:outPutObj.facevideo().c_str()]];
                NSString *headerUrl = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[NSString stringWithUTF8String:outPutObj.face().c_str()]];
                [weakSelf.headerView.perHeader loadImageWithPath:headerUrl andPlaceHolderName:DefalutHeadImage];
                weakSelf.headerView.perHeader.singleTapBlock = ^(UIGestureRecognizer* recognizer){
                    
                    UserPageViewController *con = [[UserPageViewController alloc] initWithUid:[NSString stringWithFormat:@"%d",outPutObj.uid()]];
                    con.titleStr = [NSString stringWithUTF8String:outPutObj.nickname().c_str()];
                    [weakSelf.navigationController pushViewController:con animated:YES];
//                    [weakSelf.headerView.perHeader playUserHeaderVideo];
                };
                //点击赞按钮
                weakSelf.headerView.praiseBlock = ^(){
                    [self addPraise];
                };
                
                //评论列表
                int replyCount = outPutObj.reply_size();
                for (int i = 0 ; i < replyCount; ++i) {
                    tutorial::ContentInfo_Reply reply = outPutObj.reply(i);
                    ReplyData *data = [[ReplyData alloc] init];
                    [data setReplyDataWith:reply];
                    [weakSelf.listArray addObject:data];
                }
                [_replyTable reloadData];
            });
        }
        
    } failed:^(NSError *error) {
        
    }];
}

-(void)sendComment{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"contentReply";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    //set发帖人ID
    tutorial::RequestParam_Param *param1 = builder.add_param();
    NSString *value = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    [self setParams:param1 AndKey:@"uid" Value:value];
    
    tutorial::RequestParam_Param *param2 = builder.add_param();
    [self setParams:param2 AndKey:@"sid" Value:_sid];
    
    tutorial::RequestParam_Param *param3 = builder.add_param();
    [self setParams:param3 AndKey:@"content" Value:_commentText.text];
    //提到的人 at的人
    tutorial::RequestParam_Param *param4 = builder.add_param();
    [self setParams:param4 AndKey:@"refer" Value:@""];
    //回复人的id
    tutorial::RequestParam_Param *param5 = builder.add_param();
    [self setParams:param5 AndKey:@"rid" Value:@""];
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    __weak SocialityDetailViewController *weakSelf = self;
    [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::SendStatus outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        //如果存在表示结构体有值
        if (outPutObj.status() == 1) {
            NSLog(@"%@",[NSString stringWithUTF8String:outPutObj.desc().c_str()]);
            [weakSelf resetTextView];
        }
        
    } failed:^(NSError *error) {
        
    }];
    
}

#pragma mark --- block
-(void)addPraise{
    
    //设置接口参数
    NSString *queue = @"mobileSNSCommunityService";
    NSString *method = @"addPraise";
    
    __block tutorial::RequestParam builder;
    builder.set_queuename([queue UTF8String]);
    builder.set_methodname([method UTF8String]);
    
    //set参数
    //set发帖人ID
    tutorial::RequestParam_Param *param1 = builder.add_param();
    NSString *value = [NSString stringWithFormat:@"%ld",[GlobalUserInfo myselfInfo].userID? :0];
    [self setParams:param1 AndKey:@"uid" Value:value];
    
    tutorial::RequestParam_Param *param2 = builder.add_param();
    [self setParams:param2 AndKey:@"sid" Value:_sid];
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8 *buffer = new ::google::protobuf::uint8[builder.ByteSize()]();
    builder.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    [[RequestServers share] requestProtobufWithURL:SNS_SERVICE_URL objclass:[self class] data:[NSData dataWithBytes:buffer length:builder.ByteSize()] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSData* data = [dic objectForKey:@"data"];
        tutorial::SendStatus outPutObj;
        outPutObj.ParseFromArray([data bytes], (int)[data length]);
        //如果存在表示结构体有值
        if (outPutObj.status() == 1) {
            NSLog(@"%@",[NSString stringWithUTF8String:outPutObj.desc().c_str()]);
        }
        
    } failed:^(NSError *error) {
        
    }];
}

#pragma mark ---set PB Params
-(void)setParams:(tutorial::RequestParam_Param *)params AndKey:(NSString*)key Value:(NSString*)value{
    
    params->set_paramkey([key UTF8String]);
    params->set_paramvalue([value UTF8String]);
}

#pragma mark -- tableDelegate
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [_listArray count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    ReplyTableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"ReplyTableViewCell" forIndexPath:indexPath];
    
    ReplyData *model = [_listArray objectAtIndex:indexPath.row];
    [cell setCellWithModel:model];
    
    return cell;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 100;
}

-(void)scrollViewWillBeginDragging:(UIScrollView *)scrollView{
    //只有滑动table的时候键盘才收取 滑动键盘
    if ([scrollView isKindOfClass:[UITableView class]]) {
        [_commentText resignFirstResponder];
    }
}

#pragma mark -- 键盘变化的监听
- (void)addNotification {
    //监听键盘高度的变换
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillChangeFrame:) name:UIKeyboardWillChangeFrameNotification object:nil];
}

-(void)releaseNotification{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillChangeFrameNotification object:nil];
}

#pragma mark Responding to keyboard events
- (void)keyboardWillChangeFrame:(NSNotification *)notification{
    NSDictionary *userInfo = [notification userInfo];
    NSValue* aValue = [userInfo objectForKey:UIKeyboardFrameEndUserInfoKey];
    CGRect keyboardRect = [aValue CGRectValue];
    if (keyboardRect.origin.y) {
        [UIView animateWithDuration:0.3 animations:^{
            _commentText.frame = CGRectMake(_commentText.frame.origin.x,  keyboardRect.origin.y - _commentText.frame.size.height , _commentText.frame.size.width, _commentText.frame.size.height);
        }];
    }
}

-(void)textViewDidChange:(UITextView *)textView{
    [_commentText changePlaceholder];
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text{
    if ([text isEqualToString:@"\n"]){ //判断输入的字是否是回车，即按下return
        //发送评论
        [self sendComment];
        return NO; //这里返回NO，就代表return键值失效，即页面上按下return，不会出现换行，如果为yes，则输入页面会换行
    }
    return YES;
}

-(void)resetTextView{
    [_commentText resignFirstResponder];
    _commentText.text = @"";
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
