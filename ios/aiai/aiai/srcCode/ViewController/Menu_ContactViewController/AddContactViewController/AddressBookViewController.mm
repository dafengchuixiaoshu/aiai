//
//  AddressBookViewController.m
//  aiai
//
//  Created by zhangyang on 15/2/10.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AddressBookViewController.h"
#import "AddressBookTableViewCell.h"

#import "PinyinHelper.h"
#import "HanyuPinyinOutputFormat.h"
#import "BackgroundUpAddressBookHander.h"
#import "ContactHandler.h"
#import "IMPackageEngine.h"
#import "AiAiAddressBook.h"

@interface AddressBookViewController ()<UITableViewDataSource, UITableViewDelegate, MFMessageComposeViewControllerDelegate>

@property(nonatomic, strong)NSMutableArray *addressBookArray;

@property(nonatomic, strong)UITableView *addressBookTable;

@end

@implementation AddressBookViewController

-(id)init{
    if (self = [super init]) {
        
        self.addressBookArray = [[NSMutableArray alloc] init];
    }
    return self;
}

-(void)viewDidLoad{
    
    [super viewDidLoad];
    
    self.centerBtnImageName = @"联系人";
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    self.addressBookTable = [[UITableView alloc] initWithFrame:rect];
    self.addressBookTable.backgroundColor = EveryView_BackGroundColor;
    [self.addressBookTable setSeparatorStyle:UITableViewCellSeparatorStyleNone];
    self.addressBookTable.backgroundColor = [UIColor clearColor];//必须clear，否则下边的模糊看不见
    self.addressBookTable.sectionIndexBackgroundColor = [UIColor clearColor];
    self.addressBookTable.delegate = self;
    self.addressBookTable.dataSource = self;
    [self.addressBookTable registerClass:[AddressBookTableViewCell class] forCellReuseIdentifier:@"AddressBookTableViewCell"];
    [self.view addSubview:self.addressBookTable];
    
    //先读取本地缓存通讯录
    self.addressBookArray = [SaveManager readDataUnArchiverWithFilePath:[SaveManager libraryPath:@"cacheContact.txt"]];
    if (self.addressBookArray && [self.addressBookArray count]>0) {
        [self.addressBookTable reloadData];
    }
    
    //再进行网络请求通讯录
    __weak AddressBookViewController *weakSelf = self;
    NSArray *bookArray = [[BackgroundUpAddressBookHander getAddressBooks] allValues];
    [ContactHandler getMyContactsStatuWithFriendList:bookArray prepare:^{
        //如果有缓存则不需要显示菊花
        if (!self.addressBookArray || [self.addressBookArray count]==0) {
            [WaitingAlertView showWaitingViewAtView:weakSelf.addressBookTable];
        }
        
    } complete:^(NSArray *resultArray) {
        
        //同步完成时候 隐藏alert,更新table;
        [WaitingAlertView hideWaitingView];
        if (!resultArray) {
            return ;
        }
        
        NSArray *sortDescriptors = [NSArray arrayWithObject:[NSSortDescriptor sortDescriptorWithKey:@"status" ascending:YES]];
        weakSelf.addressBookArray = (NSMutableArray*)[resultArray sortedArrayUsingDescriptors:sortDescriptors];
        
        [weakSelf.addressBookTable reloadData];
        
        //缓存网络通讯录
        [SaveManager saveDataArchiverWithModel:weakSelf.addressBookArray filePath:[SaveManager libraryPath:@"cacheContact.txt"]];
    }];
}

#pragma mark -- tableDelegate

//cell

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return ADDRESSBOOKCELLHEIGHT;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    return [self.addressBookArray count];
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    static NSString *identifier = @"AddressBookTableViewCell";
    
    AddressBookTableViewCell *cell = [[AddressBookTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifier];
    ContactsStatuResultModel *model = [self.addressBookArray objectAtIndex:indexPath.row];
    
    NSString *phoneNick = model.firstName? :model.lastName;
    
    [cell setCellWithFaceString:model.face_url buddyNickName:model.nickName phoneBookNick:phoneNick contaceType:model.status];
    
    __weak AddressBookViewController *weakSelf = self;
    //添加好友
    cell.addBuddyBlock = ^(){
        NSString *phoneNum;
        if ([model.phoneArray count]>0) {
            PhoneModel *pModel = [model.phoneArray objectAtIndex:0];
            phoneNum = pModel.phoneStr? :@"";
        }
        else{
            phoneNum = @"";
        }
        
        [[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_AddBuddyWithUserID:phoneNum];
        //手动刷新状态 以便刷新
        model.status = isFriend;
    };
    //邀请注册
    cell.invitationBlock = ^(){
        [weakSelf showSMSPicker:model];
    };
    return cell;
}

#pragma mark 调用系统短信
- (void)showSMSPicker:(ContactsStatuResultModel*)data{
    Class messageClass = (NSClassFromString(@"MFMessageComposeViewController"));
    if (messageClass != nil) {
        if ([messageClass canSendText]) {
            [self displaySMSComposerSheet:data];
        }
        else {
            [[AiAiAlertView share] showAlertViewWithTitle:nil message:@"设备没有短信功能" confirmTitle:nil cancelTitle:@"取消" confirm:nil cancel:^(){
                return ;
            }];
        }
    }
    else {
        [[AiAiAlertView share] showAlertViewWithTitle:nil message:@"iOS版本过低,不支持程序内发送短信" confirmTitle:nil cancelTitle:@"取消" confirm:nil cancel:^(){
            return ;
        }];
    }
}

- (void)displaySMSComposerSheet:(ContactsStatuResultModel*)data {
    MFMessageComposeViewController *picker = [[MFMessageComposeViewController alloc] init];
    picker.messageComposeDelegate = self;
    
    NSString* msg = [NSString stringWithFormat:@"邀请你使用ii——最潮流的聊天软件，马上来下载吧:%@",MESSAGE_DOWNLOAD_URL];
    picker.body = [[NSString alloc] initWithString:msg];
    
    if (data.phoneArray && [data.phoneArray count] > 0) {
        PhoneModel *model = [data.phoneArray objectAtIndex:0];
        picker.recipients = [NSArray arrayWithObject:model.phoneStr];
    }
    [self presentViewController:picker animated:YES completion:nil];
}

- (void)messageComposeViewController:(MFMessageComposeViewController *)controller didFinishWithResult:(MessageComposeResult)result {
    switch (result)
    {
        case MessageComposeResultCancelled:
            //LOG_EXPR(@"Result: SMS sending canceled");
            break;
        case MessageComposeResultSent:
        {
//            [self showMessage:@"发送成功" andStyle:TxtCenter andHideTime:2 andLookMsg:nil];
        }
            break;
        case MessageComposeResultFailed:
        {
//            [self showMessage:@"发送失败" andStyle:TxtCenter andHideTime:2 andLookMsg:nil];
        }
            break;
        default:
            //LOG_EXPR(@"Result: SMS not sent");
            break;
    }
    [self dismissViewControllerAnimated:YES completion:nil];
}

@end
