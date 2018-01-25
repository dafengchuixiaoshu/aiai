//
//  ChangeFaceImgViewController.m
//  aiai
//
//  Created by wangnan on 15/6/15.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ChangeFaceImgViewController.h"
#import "RECHeaderViewController.h"

#import "LoginMsgCenter.h"

@interface ChangeFaceImgViewController ()<RECHeaderViewControllerDelegate>

@property(nonatomic, strong)AsyImageView *headerView;

@property(nonatomic, strong)NSURL *uploadFaceVedio;

@property(nonatomic, assign)BOOL needUpData;    //只有进行修改后再进行上传新头像

@end

@implementation ChangeFaceImgViewController

-(id)init{
    if (self = [super init]) {
        
        _needUpData = NO;
    }
    return self;
}

-(void)viewDidLoad{
    
    [super viewDidLoad];
    
    self.centerBtnImageName = @"修改头像";
    self.rightBtnImageName = @"完成";
    
    _headerView = [[AsyImageView alloc] init];
    _headerView.frame = CGRectMake(0, 0, 60, 60);
    _headerView.layer.cornerRadius = _headerView.frame.size.width/2;
    _headerView.layer.masksToBounds = YES;
    _headerView.center = self.view.center;
    [_headerView loadImageWithPath:[AppUtils getImageNewUrlWithSrcImageUrl:[GlobalUserInfo myselfInfo].userFaceUrl width:0 height:0] andPlaceHolderName:DefalutHeadImage];
    [self.view addSubview:_headerView];
    
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(captchvideo)];
    [_headerView addGestureRecognizer:tap];
}

#pragma mark -- buttonAction
-(void)captchvideo{
    RECHeaderViewController *con = [[RECHeaderViewController alloc] init];
    con.delegate = self;
    [self.navigationController pushViewController:con animated:YES];
}

-(void)rightItemClicked:(id)sender{
    
    if (!_needUpData) {
        return;
    }
    
    __weak ChangeFaceImgViewController *weakSelf = self;
    __block GlobalUserInfo *info = [[GlobalUserInfo alloc] init];
    info.userID = [GlobalUserInfo myselfInfo].userID;
    
    UIImage *faceImage = _headerView.image;
    [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[self class] data:UIImagePNGRepresentation(faceImage)  prepare:^{
        [WaitingAlertView showWaitingViewAtView:self.view];
    } successed:^(NSDictionary *dic) {
        
        if ([[dic objectForKey:@"result"] isEqualToString:@"error"]) {
            
            [WaitingAlertView hideWaitingView];
            return ;
        }
        
        info.userFaceUrl = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[dic objectForKey:@"fileName"]];;
        //先上传头像获取key
        NSData* uploadData = [NSData dataWithContentsOfURL:_uploadFaceVedio];
        [[RequestServers share] uploadSrouceDataRequestWithURL:IMAGE_SERVICE_URL objclass:[self class] data:uploadData prepare:^{
            
        } successed:^(NSDictionary *dic) {
            if ([[dic objectForKey:@"result"] isEqualToString:@"error"] || !dic) {
                
                [WaitingAlertView hideWaitingView];
                [[AiAiAlertView share] showAlertViewWithTitle:nil message:@"头像修改失败" confirmTitle:@"确认" cancelTitle:nil confirm:nil cancel:nil];
                return ;
            }
            //成功之后进行注册
            info.userFaceVedio = [NSString stringWithFormat:@"%@%@",IMAGE_SERVICE_URL,[dic objectForKey:@"fileName"]];
            
            [LoginMsgCenter changeUserInfoRequestWithUserInfo:info comlate:^{
                
                [GlobalUserInfo myselfInfo].userFaceUrl = info.userFaceUrl;
                [GlobalUserInfo myselfInfo].userFaceVedio = info.userFaceVedio;
                if (_changeFaceImgBlock) {
                    _changeFaceImgBlock();
                }
                [WaitingAlertView hideWaitingView];
                [weakSelf.navigationController popViewControllerAnimated:YES];
                
            } fail:^{
                
                [WaitingAlertView hideWaitingView];
            }];
        } failed:^(NSError *error) {
            [WaitingAlertView hideWaitingView];
        }];
    } failed:^(NSError *error) {
        [WaitingAlertView hideWaitingView];
    }];
    
}

#pragma mark -- controller delegate
-(void)REC_VideoPath:(NSURL*)path videoTime:(CGFloat)time{
    
    UIImage *firstFrameImage = [AppUtils getImage:path];
    _headerView.image = firstFrameImage;
    
    _uploadFaceVedio = path;
    _needUpData = YES;
}

@end
