//
//  ContactDetailViewController.m
//  aiai
//
//  Created by wangnan on 15/7/1.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ContactDetailViewController.h"
#import "ContactDetail_SubNameViewController.h"

#import "UserHeaderView.h"
#import "InputTextView.h"

#import "IMPackageData.h"
#import "UsersFriendProPOJO.pb.h"
#import "OutputResultData.pb.h"

@interface ContactDetailViewController ()<IMPackageEngineDelegate>

@property(nonatomic, strong)UISwitch *shareSwitch;  //分享开关
@property(nonatomic, strong)UserHeaderView *headImageView;     //用户头像
@property(nonatomic, strong)UILabel *nickNameLabel;     //昵称label

@property(nonatomic, copy)NSString *buddyUserName;

@end

@implementation ContactDetailViewController

-(id)initWithBuddyUserName:(NSString*)userName{
    if (self = [super init]) {
        _buddyUserName = userName;
    }
    return self;
}

-(void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] addDelegate:self];
}

-(void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    
    [[IMPackageEngine sharedInstanceIMPackageEngine] removeDelegate:self];
}

-(void)viewDidLoad{
    [super viewDidLoad];
    
    self.centerBtnImageName = @"详细信息";
    
    IMPackageBuddyData *buddy = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetContactWithUserId:_buddyUserName isFromServer:YES];
    
    //添加头像
    _headImageView = [[UserHeaderView alloc] init];
    _headImageView.frame = CGRectMake(15, 64 + 15, 60, 60);
    [_headImageView loadImageWithPath:[AppUtils getImageNewUrlWithSrcImageUrl:buddy.buddyPortraitKey width:0 height:0] andPlaceHolderName:DefalutHeadImage];
    [self.view addSubview:_headImageView];
    
    //修改昵称按钮
    CGFloat editBtn_w = 100;
    CGFloat editBtn_x = [UIScreen mainScreen].bounds.size.width - editBtn_w - 15;
    UIButton *editBtn = [[UIButton alloc] initWithFrame:CGRectMake(editBtn_x, _headImageView.frame.origin.y, editBtn_w, 28)];
//    editBtn.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
    editBtn.titleLabel.font = Every_SystemFontSize_12;
    [editBtn setTitle:@"修改备注名" forState:UIControlStateNormal];
    [editBtn setTitle:@"修改备注名" forState:UIControlStateHighlighted];
    [editBtn setBackgroundColor:[UIColor clearColor]];
    editBtn.layer.borderColor = [UIColor whiteColor].CGColor;
    editBtn.layer.borderWidth = 1.0;
    editBtn.layer.cornerRadius = editBtn.frame.size.height/2;
    editBtn.layer.masksToBounds = YES;
    [editBtn addTarget:self action:@selector(editAction:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:editBtn];
    
    //昵称
    CGFloat nickName_x = _headImageView.frame.origin.x + _headImageView.frame.size.width + 10;
    _nickNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(nickName_x, _headImageView.frame.origin.y, [UIScreen mainScreen].bounds.size.width - nickName_x - editBtn.frame.size.width - 15, 20)];
    _nickNameLabel.textColor = [UIColor whiteColor];
    _nickNameLabel.textAlignment = NSTextAlignmentLeft;
    _nickNameLabel.font = Every_SystemFontSize_14;
    if (buddy.buddySubname && [buddy.buddySubname length] > 0) {
        _nickNameLabel.text = buddy.buddySubname;
    }
    else{
        _nickNameLabel.text = buddy.buddyNickName;
    }
    [self.view addSubview:_nickNameLabel];
    
    
    //分割线
    CGFloat line1_y = _headImageView.frame.origin.y + _headImageView.frame.size.height + 20;
    UIImageView* cutline1 = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
    cutline1.frame = CGRectMake(15, line1_y, [UIScreen mainScreen].bounds.size.width - 15*2, 1);
    cutline1.alpha = 0.5;
    [self.view addSubview:cutline1];

    //label
    UILabel *shareLabel = [[UILabel alloc] initWithFrame:CGRectMake(_headImageView.frame.origin.x, cutline1.frame.origin.y+cutline1.frame.size.height+20, 200, 15)];
    shareLabel.text = @"允许对方保存及分享";
    shareLabel.textColor = [UIColor whiteColor];
    shareLabel.textAlignment = NSTextAlignmentLeft;
    shareLabel.font = Every_SystemFontSize_12;
    [self.view addSubview:shareLabel];
    
    //保存分享
    _shareSwitch = [[UISwitch alloc] init];
    _shareSwitch.frame = CGRectMake([UIScreen mainScreen].bounds.size.width- 15 - _shareSwitch.frame.size.width, shareLabel.center.y - _shareSwitch.frame.size.height/2, _shareSwitch.frame.size.width, _shareSwitch.frame.size.height);
    _shareSwitch.on = buddy.buddyIsShare.intValue == 1 ? YES:NO;
    _shareSwitch.onTintColor = Every_YellowColor;
    [_shareSwitch addTarget:self action:@selector(switchAction:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:_shareSwitch];
    
    //分割线
    CGFloat line2_y = shareLabel.frame.origin.y + shareLabel.frame.size.height + 20;
    UIImageView* cutline2 = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
    cutline2.frame = CGRectMake(15, line2_y, [UIScreen mainScreen].bounds.size.width - 15*2, 1);
    cutline2.alpha = 0.5;
    [self.view addSubview:cutline2];
    
    CGFloat sendMsgBtn_W = 120;
    CGFloat sendMsgBtn_H = 44;
    CGFloat sendMsgBtn_X = ([UIScreen mainScreen].bounds.size.width) / 2  - sendMsgBtn_W - 20;
    CGFloat sendMsgBtn_Y = cutline2.frame.origin.y+cutline2.frame.size.height + 50;
    UIButton *sendMsgBtn = [[UIButton alloc] initWithFrame:CGRectMake(sendMsgBtn_X, sendMsgBtn_Y, sendMsgBtn_W, sendMsgBtn_H)];
    [sendMsgBtn setBackgroundColor:[UIColor clearColor]];
    [sendMsgBtn.titleLabel setFont:Every_SystemFontSize_12];
    [sendMsgBtn setTitle:@"发消息" forState:UIControlStateNormal];
    [sendMsgBtn setTitle:@"发消息" forState:UIControlStateHighlighted];
    sendMsgBtn.layer.borderColor = [UIColor whiteColor].CGColor;
    sendMsgBtn.layer.borderWidth = 1.0;
    sendMsgBtn.layer.cornerRadius = sendMsgBtn_H/2;
    sendMsgBtn.layer.masksToBounds = YES;
    [sendMsgBtn setTitleColor:Every_YellowColor forState:UIControlStateNormal];
    [sendMsgBtn setTitleColor:Every_YellowColor forState:UIControlStateHighlighted];
    [self.view addSubview:sendMsgBtn];
    [sendMsgBtn addTarget:self action:@selector(sendMsgBtnAction:) forControlEvents:UIControlEventTouchUpInside];
    
    CGFloat deleteBtn_W = 120;
    CGFloat deleteBtn_H = 44;
    CGFloat deleteBtn_X = ([UIScreen mainScreen].bounds.size.width) / 2 + 20;
    CGFloat deleteBtn_Y = sendMsgBtn_Y;
    UIButton *deleteBtn = [[UIButton alloc] initWithFrame:CGRectMake(deleteBtn_X, deleteBtn_Y, deleteBtn_W, deleteBtn_H)];
    [deleteBtn setBackgroundColor:[UIColor clearColor]];
    [deleteBtn.titleLabel setFont:Every_SystemFontSize_12];
    [deleteBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [deleteBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
    [deleteBtn setTitle:@"删除好友" forState:UIControlStateNormal];
    [deleteBtn setTitle:@"删除好友" forState:UIControlStateHighlighted];
    [deleteBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [deleteBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
    deleteBtn.layer.borderColor = [UIColor whiteColor].CGColor;
    deleteBtn.layer.borderWidth = 1.0;
    deleteBtn.layer.cornerRadius = sendMsgBtn_H/2;
    deleteBtn.layer.masksToBounds = YES;
    [self.view addSubview:deleteBtn];
    [deleteBtn addTarget:self action:@selector(deleteBtnAction:) forControlEvents:UIControlEventTouchUpInside];
    
}

#pragma mark button action
-(void)switchAction:(id)sender{
    
    [self httpRequestWithSubname:nil completion:nil];
}

-(void)editAction:(id)sender{
    
    /*
    InputTextView *inputView = [[InputTextView alloc] initWithContent:_nickNameLabel.text cancelButtonTitle:@"取消" confirmButtonTitle:@"确定" placeholder:@""];
    inputView.cancelBlock = ^(){
        
    };
    inputView.confirmBlock = ^(NSString *inputText){
        [self httpRequestWithSubname:inputText completion:^{
            _nickNameLabel.text = inputText;
            if (_modifyBlock) {
                _modifyBlock(inputText);
            }
        }];
    };
    [inputView show];
     */
    
    //新需求
    ContactDetail_SubNameViewController *con = [[ContactDetail_SubNameViewController alloc] initWithBuddyUserName:_buddyUserName andSubName:_nickNameLabel.text];
    con.modifyBlock = ^(NSString *modifyName){
        
        _nickNameLabel.text = modifyName;
        if (_modifyBlock) {
            _modifyBlock(modifyName);
        }
        
    };
    [self.navigationController pushViewController:con animated:YES];
    
}

-(void)sendMsgBtnAction:(id)sender{
    
    
    IMPackageBuddyData *buddy = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetContactWithUserId:_buddyUserName isFromServer:NO];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:SENDMSG_NOTIFY object:buddy];
    [self.navigationController popToRootViewControllerAnimated:YES];
    
}

-(void)deleteBtnAction:(id)sender{
    [[AiAiAlertView share] showAlertViewWithTitle:@"提示" message:@"确认删除" confirmTitle:@"确定" cancelTitle:@"取消" confirm:^{
        
        if (_deleteBlock) {
            _deleteBlock(_buddyUserName);
        }
        [self.navigationController popViewControllerAnimated:YES];
    } cancel:^{
        
    }];
}

#pragma mark http
-(void)httpRequestWithSubname:(NSString*)subName completion:(void (^)(void))completion{
    
    com::mm::pb::po::UsersFriendProPOJO usersObj;
    
    IMPackageBuddyData *buddy = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetContactWithUserId:_buddyUserName isFromServer:NO];
    NSString *uid = [NSString stringWithFormat:@"%zi",[GlobalUserInfo myselfInfo].userID];
    usersObj.set_uid(uid.intValue);
    
    ::com::mm::pb::po::UsersFriendsPOJO *friendObj = usersObj.add_list();
    friendObj->set_friendid(buddy.buddyUserName.UTF8String);
    friendObj->set_issave(_shareSwitch.on?1:0);
    friendObj->set_isshare(_shareSwitch.on?1:0);
//    if (subName) {
//        friendObj->set_subname(subName.UTF8String);
//    }
    
    //申请内存对数据进行序列化
    ::google::protobuf::uint8* buffer = new ::google::protobuf::uint8[usersObj.ByteSize()]();
    usersObj.google::protobuf::MessageLite::SerializeWithCachedSizesToArray(buffer);
    
    //序列化之后进行网络请求
    NSString* requestUrlString = [NSString stringWithFormat:@"%@%@",LOGIN_MODEL_SERVICE_URL,@"setFriendPro.pb"];
    [[RequestServers share] requestProtobufWithURL:requestUrlString objclass:[self class] data:[NSData dataWithBytes:buffer length:usersObj.ByteSize()] prepare:^{
        
        [WaitingAlertView showWaitingViewAtView:self.view];
        
    } successed:^(NSDictionary *dic) {
        
        [WaitingAlertView hideWaitingView];
        NSData* data = [dic objectForKey:@"data"];
        
        com::mm::pb::po::OutputResultData outputOjb;
        outputOjb.ParseFromArray([data bytes], (int)[data length]);
        ::google::protobuf::int32 code = outputOjb.resultcode();
        AIAI_LOG(@"%zi",code);
        if (code == 0) {
            if (completion) {
                completion();
            }
        }
        
    } failed:^(NSError *error) {
        AIAI_LOG(@"%@",error);
        [WaitingAlertView hideWaitingView];
    }];
}

#pragma mark im回调
//在回调中获取数据
-(void)IMPackageGetBuddyDataSuccess:(IMPackageBuddyData*)buddyData{
    
    _shareSwitch.on = buddyData.buddyIsShare.intValue == 1 ? YES:NO;
    if (buddyData.buddySubname && [buddyData.buddySubname length] > 0) {
        _nickNameLabel.text = buddyData.buddySubname;
    }
    else{
        _nickNameLabel.text = buddyData.buddyNickName;
    }
}

-(void)dealloc{
    NSLog(@"dealloc");
}

@end
