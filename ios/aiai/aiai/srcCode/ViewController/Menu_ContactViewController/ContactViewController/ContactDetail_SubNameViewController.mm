//
//  ContactDetail_SubNameViewController.m
//  aiai
//
//  Created by wangnan on 15/8/4.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ContactDetail_SubNameViewController.h"

#import "IMPackageData.h"
#import "UsersFriendProPOJO.pb.h"
#import "OutputResultData.pb.h"

@interface ContactDetail_SubNameViewController ()<UITextFieldDelegate>

@property(nonatomic, strong)UITextField *subNameField;

@property(nonatomic, copy)NSString *subName;
@property(nonatomic, copy)NSString *buddyName;

@end

@implementation ContactDetail_SubNameViewController

-(id)initWithBuddyUserName:(NSString*)buddyName andSubName:(NSString *)subName{
    if (self = [super init]) {
        
        _buddyName = buddyName;
        _subName = subName;
    }
    return self;
}

-(void)viewDidLoad{
    
    [super viewDidLoad];
    
    self.centerBtnImageName = @"详细信息";
    
    CGFloat field_x = 15;
    CGFloat field_y = 64 + 40;
    CGFloat field_w = [UIScreen mainScreen].bounds.size.width - field_x *2;
    CGFloat field_h = 40;
    _subNameField = [[UITextField alloc] initWithFrame:CGRectMake(field_x, field_y, field_w, field_h)];
    _subNameField.backgroundColor = [UIColor clearColor];
    _subNameField.text = _subName? :@"";
    _subNameField.font = Every_SystemFontSize_14;
    _subNameField.textColor = [UIColor whiteColor];
//    _subNameField.placeholder = @"请输入昵称";
    _subNameField.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.05];
    _subNameField.layer.cornerRadius = field_h/2;
    _subNameField.layer.masksToBounds = YES;
    _subNameField.clearButtonMode = UITextFieldViewModeWhileEditing;
    //设置左间距
    UIView *leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 15, _subNameField.frame.size.height)];
    _subNameField.leftView = leftView;
    _subNameField.leftViewMode = UITextFieldViewModeAlways;
    _subNameField.delegate = self;
    [self.view addSubview:_subNameField];
    
    CGFloat confirmBtn_W = 120;
    CGFloat confirmBtn_H = 44;
    CGFloat confirmBtn_X = ([UIScreen mainScreen].bounds.size.width) / 2  - confirmBtn_W - 20;
    CGFloat confirmBtn_Y = _subNameField.frame.origin.y+_subNameField.frame.size.height + 50;
    UIButton *confirmBtn = [[UIButton alloc] initWithFrame:CGRectMake(confirmBtn_X, confirmBtn_Y, confirmBtn_W, confirmBtn_H)];
    [confirmBtn setBackgroundColor:[UIColor clearColor]];
    [confirmBtn.titleLabel setFont:Every_SystemFontSize_12];
    [confirmBtn setTitle:@"确定" forState:UIControlStateNormal];
    [confirmBtn setTitle:@"确定" forState:UIControlStateHighlighted];
    confirmBtn.layer.borderColor = [UIColor whiteColor].CGColor;
    confirmBtn.layer.borderWidth = 1.0;
    confirmBtn.layer.cornerRadius = confirmBtn_H/2;
    confirmBtn.layer.masksToBounds = YES;
    [confirmBtn setTitleColor:Every_YellowColor forState:UIControlStateNormal];
    [confirmBtn setTitleColor:Every_YellowColor forState:UIControlStateHighlighted];
    [self.view addSubview:confirmBtn];
    [confirmBtn addTarget:self action:@selector(confirmBtnAction:) forControlEvents:UIControlEventTouchUpInside];
    
    CGFloat cancelBtn_W = 120;
    CGFloat cancelBtn_H = 44;
    CGFloat cancelBtn_X = ([UIScreen mainScreen].bounds.size.width) / 2 + 20;
    CGFloat cancelBtn_Y = confirmBtn_Y;
    UIButton *cancelBtn = [[UIButton alloc] initWithFrame:CGRectMake(cancelBtn_X, cancelBtn_Y, cancelBtn_W, cancelBtn_H)];
    [cancelBtn setBackgroundColor:[UIColor clearColor]];
    [cancelBtn.titleLabel setFont:Every_SystemFontSize_12];
    [cancelBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [cancelBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
    [cancelBtn setTitle:@"取消" forState:UIControlStateNormal];
    [cancelBtn setTitle:@"取消" forState:UIControlStateHighlighted];
    [cancelBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [cancelBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
    cancelBtn.layer.borderColor = [UIColor whiteColor].CGColor;
    cancelBtn.layer.borderWidth = 1.0;
    cancelBtn.layer.cornerRadius = cancelBtn_H/2;
    cancelBtn.layer.masksToBounds = YES;
    [self.view addSubview:cancelBtn];
    [cancelBtn addTarget:self action:@selector(cancelBtnAction:) forControlEvents:UIControlEventTouchUpInside];
    
    //收起键盘
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(resignFirstAction)];
    [self.view addGestureRecognizer:tap];
}

#pragma mark button action
-(void)cancelBtnAction:(id)sender{
    [self leftItemClicked:sender];
}

-(void)confirmBtnAction:(id)sender{
    
    [self httpRequestWithSubname:_subNameField.text completion:^{
        if (_modifyBlock) {
            _modifyBlock(_subNameField.text);
        }
        [self.navigationController popViewControllerAnimated:YES];
    }];
}

-(void)resignFirstAction{
    [_subNameField resignFirstResponder];
}

#pragma mark http
-(void)httpRequestWithSubname:(NSString*)subName completion:(void (^)(void))completion{
    
    com::mm::pb::po::UsersFriendProPOJO usersObj;
    
    IMPackageBuddyData *buddy = [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetContactWithUserId:_buddyName isFromServer:NO];
    NSString *uid = [NSString stringWithFormat:@"%zi",[GlobalUserInfo myselfInfo].userID];
    usersObj.set_uid(uid.intValue);
    
    ::com::mm::pb::po::UsersFriendsPOJO *friendObj = usersObj.add_list();
    friendObj->set_friendid(buddy.buddyUserName.UTF8String);
//    friendObj->set_issave(_shareSwitch.on?1:0);
//    friendObj->set_isshare(_shareSwitch.on?1:0);
    if (subName) {
        friendObj->set_subname(subName.UTF8String);
    }
    
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

@end
