//
//  ShowMsgPlayView.m
//  aiai
//
//  Created by zhangyang on 15/4/14.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ShowMsgPlayView.h"
#import "ShowMsgPlayView+GestureRecognizer.h"
#import "PlayMovieView.h"
#import "AsyImageView.h"

@interface ShowMsgPlayView ()
@property(nonatomic,strong)AsyImageView* imageview;
@property(nonatomic,strong)PlayMovieView* playView;
@property(nonatomic,strong)UILabel* contentLabel;
@property(nonatomic,strong)UIButton* unReadMsgButton;

//分享和保存
@property(nonatomic,strong)UIButton* shareBtn;
@property(nonatomic,strong)UIButton* saveBtn;
@property(nonatomic, readwrite, strong)UIImage *showImg;
@end

@implementation ShowMsgPlayView

-(id)initWithFrame:(CGRect)frame{
    if (self = [super initWithFrame:frame]) {
        
        _playView = [[PlayMovieView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, self.frame.size.width)];
        _playView.complateBlock = ^{
            
        };
        __weak ShowMsgPlayView* weakself = self;
        _playView.loadFinishBlock = ^(UIImage *finishImg){
            weakself.showImg = finishImg;
        };
        [self addSubview:_playView];
        
        //初始化
        _imageview = [[AsyImageView alloc] init];
        [self addSubview:_imageview];
        
        _contentLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, (frame.size.width-40)/2, frame.size.width, 40)];
        _contentLabel.textAlignment = NSTextAlignmentCenter;
        _contentLabel.textColor = [UIColor whiteColor];
        _contentLabel.font = [UIFont systemFontOfSize:28];
        _contentLabel.numberOfLines = 5;
        [self addSubview:_contentLabel];
        
        CGFloat unReadMsgLabel_W = 40;
        CGFloat unReadMsgLabel_H = unReadMsgLabel_W;
        CGFloat unReadMsgLabel_X = (self.frame.size.width - unReadMsgLabel_W)/2;
        CGFloat unReadMsgLabel_Y = self.frame.size.width - unReadMsgLabel_W;
        _unReadMsgButton = [[UIButton alloc] initWithFrame:CGRectMake(unReadMsgLabel_X, unReadMsgLabel_Y, unReadMsgLabel_W, unReadMsgLabel_H)];
        [_unReadMsgButton setBackgroundColor:Every_YellowColor];
        _unReadMsgButton.titleLabel.font = Every_SystemFontSize_12;
        [_unReadMsgButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [_unReadMsgButton setTitle:@"0" forState:UIControlStateNormal];
        _unReadMsgButton.layer.cornerRadius = _unReadMsgButton.frame.size.width / 2;
        _unReadMsgButton.layer.masksToBounds = YES;
        [_unReadMsgButton addTarget:self action:@selector(unReadMsgButtonAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_unReadMsgButton];
        
        //分享按钮
        CGFloat shareBtn_W = 40;
        CGFloat shareBtn_H = 25;
        CGFloat shareBtn_X = [UIScreen mainScreen].bounds.size.width - shareBtn_W - 15;
        CGFloat shareBtn_Y = self.frame.size.width - shareBtn_H - 5;
        _shareBtn = [[UIButton alloc] initWithFrame:CGRectMake(shareBtn_X, shareBtn_Y, shareBtn_W, shareBtn_H)];
        _shareBtn.titleLabel.font = Every_SystemFontSize_12;
        [_shareBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [_shareBtn setTitle:@"分享" forState:UIControlStateNormal];
        _shareBtn.layer.cornerRadius = _shareBtn.frame.size.height / 2;
        _shareBtn.layer.masksToBounds = YES;
        _shareBtn.layer.borderColor = [UIColor whiteColor].CGColor;
        _shareBtn.layer.borderWidth = 1.0;
        [_shareBtn addTarget:self action:@selector(shareBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_shareBtn];
        
        //保存按钮
        CGFloat saveBtn_W = 40;
        CGFloat saveBtn_H = 25;
        CGFloat saveBtn_X = _shareBtn.frame.origin.x - saveBtn_W - 15;
        CGFloat saveBtn_Y = _shareBtn.frame.origin.y;
        _saveBtn = [[UIButton alloc] initWithFrame:CGRectMake(saveBtn_X, saveBtn_Y, saveBtn_W, saveBtn_H)];
        _saveBtn.titleLabel.font = Every_SystemFontSize_12;
        [_saveBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [_saveBtn setTitle:@"保存" forState:UIControlStateNormal];
        _saveBtn.layer.cornerRadius = _saveBtn.frame.size.height / 2;
        _saveBtn.layer.masksToBounds = YES;
        _saveBtn.layer.borderColor = [UIColor whiteColor].CGColor;
        _saveBtn.layer.borderWidth = 1.0;
        [_saveBtn addTarget:self action:@selector(saveBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_saveBtn];
        
        
        
        //添加手势
        [self addUIGestureRecognizer];
    }
    return self;
}

-(void)releaseResource{
    if (_playView) {
        [_playView releaseResource];
    }
}

-(void)dealloc{
    AIAI_LOG(@"showView dealloc");
}

-(void)setImageUrl:(NSString *)imageUrl{
    if (_playView) {
        [_playView stopMovie];
        [self sendSubviewToBack:_playView];
    }
    NSString* tmpUrl = [imageUrl copy];
    _imageUrl = [[AppUtils getImageNewUrlWithSrcImageUrl:tmpUrl width:self.frame.size.width height:self.frame.size.height] copy];
    [_imageview loadImageWithPath:imageUrl andPlaceHolderName:nil];
    
    __weak ShowMsgPlayView* weakself = self;
    _imageview.finishLoadBlock = ^(AsyImageView *obj){
        weakself.showImg = obj.image;
    };
    _imageview.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.height);
}

-(void)setVedioUrl:(NSString *)vedioUrl{
    _vedioUrl = [vedioUrl copy];
    
    if (_playView) {
        [_playView stopMovie];
        
        [ShowMsgPlayView videoPathWitSourceUrl:vedioUrl ready:^{
            [WaitingAlertView showWaitingViewAtCenterView:_playView];
            _saveBtn.userInteractionEnabled = NO;
        } complate:^(NSString *videoPath) {
            [WaitingAlertView hideWaitingView];
            _saveBtn.userInteractionEnabled = YES;
            //添加播放器
            _playView.moviePath = videoPath;
            [_playView playMovie];
            [self sendSubviewToBack:_imageview];
            
        } failed:^(NSError *error) {
            [WaitingAlertView hideWaitingView];
            AIAI_LOG(@"ShowMsgPlayView.error = %@",error);
        }];
    }
}

-(void)setContentText:(NSString *)contentText{
    if (contentText) {
        _contentText = [contentText copy];
    }else{
        _contentText = @"";
    }
    _contentLabel.text = _contentText;
}

-(void)setUnReadMsgCount:(NSInteger)unReadMsgCount{
    _unReadMsgCount = unReadMsgCount;
    [_unReadMsgButton setTitle:[[NSNumber numberWithInteger:unReadMsgCount] stringValue] forState:UIControlStateNormal];
}

#pragma mark ButtonAction
-(void)unReadMsgButtonAction:(id)sender{
    if (self.unReadMsgButtonBlock) {
        self.unReadMsgButtonBlock();
    }
}

-(void)shareBtnAction:(id)sender{
    if (_shareBlock) {
        _shareBlock();
    }
}

-(void)saveBtnAction:(id)sender{
    if (_saveBlock) {
        _saveBlock(_playView.moviePath);
    }
}

-(void)shareBtnHidden:(BOOL)hidden{
    _shareBtn.hidden = hidden;
    _saveBtn.hidden = hidden;
}

#pragma mark 视频缓存
+ (NSString *)cachePathForKey:(NSString *)key
{
    const char *str = [key UTF8String];
    unsigned char r[CC_MD5_DIGEST_LENGTH];
    CC_MD5(str, (CC_LONG)strlen(str), r);
    NSString *filename = [NSString stringWithFormat:@"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                          r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15]];
    
    return [VEDIOCACHEPATH stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.%@",filename,@"mp4"]];
}

+(void)videoPathWitSourceUrl:(NSString*)sourceUrl ready:(void (^)(void))ready complate:(void (^)(NSString* videoPath))complate failed:(void (^)(NSError *error))failed{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error;
    BOOL isDir;
    
    BOOL exist = [fileManager fileExistsAtPath:VEDIOCACHEPATH isDirectory:&isDir];
    if(exist == NO) {
        [fileManager createDirectoryAtPath:VEDIOCACHEPATH withIntermediateDirectories:NO attributes:nil error:&error];
    }else{
        NSString* videoPath = [self cachePathForKey:sourceUrl];
        if ([fileManager fileExistsAtPath:videoPath]) {
            complate(videoPath);
        }else{
            [[RequestServers share] requestDownLoadWithURL:sourceUrl savePath:videoPath prepare:^{
                ready();
            } successed:^(NSDictionary *dic) {
                complate(videoPath);
            } failed:^(NSError *error) {
                failed(error);
            }];
        }
    }
}

@end
