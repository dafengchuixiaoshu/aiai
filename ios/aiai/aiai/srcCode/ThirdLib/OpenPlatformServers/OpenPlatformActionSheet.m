//
//  OpenPlatformActionSheet.m
//  LXActivityDemo
//
//  Created by lixiang on 14-3-17.
//  Copyright (c) 2014年 lcolco. All rights reserved.
//

#import "OpenPlatformActionSheet.h"

#define WINDOW_COLOR                            [UIColor colorWithRed:0 green:0 blue:0 alpha:0.5]
#define ACTIONSHEET_BACKGROUNDCOLOR             [UIColor colorWithRed:106/255.00f green:106/255.00f blue:106/255.00f alpha:0.8]
#define ANIMATE_DURATION                        0.25f

#define CORNER_RADIUS                           5
#define SHAREBUTTON_BORDER_WIDTH                0.5f
#define SHAREBUTTON_BORDER_COLOR                [UIColor colorWithRed:0.0f green:0.0f blue:0.0f alpha:0.8].CGColor
#define SHAREBUTTONTITLE_FONT                   [UIFont fontWithName:@"HelveticaNeue-Bold" size:18]

#define CANCEL_BUTTON_COLOR                     [UIColor colorWithRed:53/255.00f green:53/255.00f blue:53/255.00f alpha:1]

#define SHAREBUTTON_WIDTH                       70
#define SHAREBUTTON_HEIGHT                      70
#define SHAREBUTTON_INTERVAL_HEIGHT             30

#define SHARETITLE_WIDTH                        70
#define SHARETITLE_HEIGHT                       20

#define SHARETITLE_INTERVAL_HEIGHT              SHAREBUTTON_WIDTH+SHAREBUTTON_INTERVAL_HEIGHT
#define SHARETITLE_FONT                         [UIFont fontWithName:@"Helvetica-Bold" size:14]

#define TITLE_INTERVAL_HEIGHT                   15
#define TITLE_HEIGHT                            35
#define TITLE_INTERVAL_WIDTH                    30
#define TITLE_WIDTH                             260
#define TITLE_FONT                              [UIFont fontWithName:@"Helvetica-Bold" size:10]
#define SHADOW_OFFSET                           CGSizeMake(0, 0.8f)
#define TITLE_NUMBER_LINES                      2

#define BUTTON_INTERVAL_HEIGHT                  20
#define BUTTON_HEIGHT                           60
#define BUTTON_INTERVAL_WIDTH                   40
#define BUTTON_WIDTH                            240
#define BUTTONTITLE_FONT                        [UIFont fontWithName:@"HelveticaNeue-Bold" size:18]
#define BUTTON_BORDER_WIDTH                     0.5f
#define BUTTON_BORDER_COLOR                     [UIColor colorWithRed:0.0f green:0.0f blue:0.0f alpha:0.8].CGColor


#define LINECOUNT                               3


@interface UIImage (custom)

+ (UIImage *)imageWithColor:(UIColor *)color;

@end


@implementation UIImage (custom)

+ (UIImage *)imageWithColor:(UIColor *)color
{
    CGRect rect = CGRectMake(0.0f, 0.0f, 1.0f, 1.0f);
    UIGraphicsBeginImageContext(rect.size);
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGContextSetFillColorWithColor(context, [color CGColor]);
    CGContextFillRect(context, rect);
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image;
}
@end

@interface OpenPlatformActionSheet ()

@property (nonatomic,assign) CGFloat SHAREBUTTON_INTERVAL_WIDTH;
@property (nonatomic,assign) CGFloat SHARETITLE_INTERVAL_WIDTH;

@property (nonatomic,strong) UIView *backGroundView;
@property (nonatomic,strong) NSString *actionTitle;
@property (nonatomic,assign) NSInteger postionIndexNumber;
@property (nonatomic,assign) BOOL isHadTitle;
@property (nonatomic,assign) BOOL isHadShareButton;
@property (nonatomic,assign) BOOL isHadCancelButton;
@property (nonatomic,assign) CGFloat LXActivityHeight;
@property (nonatomic,assign) id<OpenPlatformActionSheetDelegate>delegate;

@end

@implementation OpenPlatformActionSheet

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

#pragma mark - Public method

- (id)initWithTitle:(NSString *)title delegate:(id<OpenPlatformActionSheetDelegate>)delegate {
    self = [self initWithTitle:title delegate:delegate cancelButtonTitle:@"取消" ShareButtonTitles:[NSArray arrayWithObjects:@"微信朋友圈", @"微信好友", @"QQ空间", @"微博", /*@"圈儿", @"好友",*/ nil] withShareButtonImagesName:[NSArray arrayWithObjects:@"weixin_circle_able.png", @"weixin_friend_able.png",@"qq_zone_able.png", @"sina_weibo_able.png", /* @"circle_icon.png", @"friend_icon.png",*/ nil]];
    if (self) {
        
    }
    return self;
}

- (id)initWithTitle:(NSString *)title delegate:(id<OpenPlatformActionSheetDelegate>)delegate cancelButtonTitle:(NSString *)cancelButtonTitle ShareButtonTitles:(NSArray *)shareButtonTitlesArray withShareButtonImagesName:(NSArray *)shareButtonImagesNameArray;
{
    self = [super init];
    if (self) {
        //初始化背景视图，添加手势
        self.frame = CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
        
        self.SHAREBUTTON_INTERVAL_WIDTH = (self.frame.size.width-SHAREBUTTON_WIDTH*LINECOUNT)/(LINECOUNT+1);
        self.SHARETITLE_INTERVAL_WIDTH = self.SHAREBUTTON_INTERVAL_WIDTH;
        
        self.backgroundColor = WINDOW_COLOR;
        self.userInteractionEnabled = YES;
        UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tappedCancel)];
        [self addGestureRecognizer:tapGesture];
        
        if (delegate) {
            self.delegate = delegate;
        }
        
        [self creatButtonsWithTitle:title cancelButtonTitle:cancelButtonTitle shareButtonTitles:shareButtonTitlesArray withShareButtonImagesName:shareButtonImagesNameArray];
        
    }
    return self;
}

- (void)showInView:(UIView *)view
{
    [[UIApplication sharedApplication].delegate.window.rootViewController.view addSubview:self];
}

#pragma mark - Praviate method

- (void)creatButtonsWithTitle:(NSString *)title cancelButtonTitle:(NSString *)cancelButtonTitle shareButtonTitles:(NSArray *)shareButtonTitlesArray withShareButtonImagesName:(NSArray *)shareButtonImagesNameArray
{
    //初始化
    self.isHadTitle = NO;
    self.isHadShareButton = NO;
    self.isHadCancelButton = NO;
    
    //初始化LXACtionView的高度为0
    self.LXActivityHeight = 0;
    
    //初始化IndexNumber为0;
    self.postionIndexNumber = 0;
    
    //生成LXActionSheetView
    self.backGroundView = [[UIView alloc] initWithFrame:CGRectMake(0, [UIScreen mainScreen].bounds.size.height, [UIScreen mainScreen].bounds.size.width, 0)];
    self.backGroundView.backgroundColor = EveryView_BackGroundColor;//[UIColor colorWithRed:248/255.0 green:249/255.0 blue:251/255.0 alpha:1];
    
    //给LXActionSheetView添加响应事件
    UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tappedBackGroundView)];
    [self.backGroundView addGestureRecognizer:tapGesture];
    
    [self addSubview:self.backGroundView];
    
    if (title) {
        self.isHadTitle = YES;
        UILabel *titleLabel = [self creatTitleLabelWith:title];
        self.LXActivityHeight = self.LXActivityHeight + 2*TITLE_INTERVAL_HEIGHT+TITLE_HEIGHT;
        [self.backGroundView addSubview:titleLabel];
    }
    
    if (shareButtonImagesNameArray) {
        if (shareButtonImagesNameArray.count > 0) {
            self.isHadShareButton = YES;
            for (int i = 1; i < shareButtonImagesNameArray.count+1; i++) {
                //计算出行数，与列数
                int column = (int)ceil((float)(i)/LINECOUNT); //行
                int line = (i)%LINECOUNT; //列
                if (line == 0) {
                    line = LINECOUNT;
                }
                UIButton *shareButton = [self creatShareButtonWithColumn:column andLine:line];
                shareButton.tag = self.postionIndexNumber;
                
                UIImage *image = [UIImage imageNamed:[shareButtonImagesNameArray objectAtIndex:i-1]];
                
                if (nil != image) {
                    [shareButton setBackgroundImage:image forState:UIControlStateNormal];
                    
                    [shareButton addTarget:self action:@selector(didClickOnImageIndex:) forControlEvents:UIControlEventTouchUpInside];
                }
                
                /*
                if ([@"qq_zone_able.png" isEqualToString:[shareButtonImagesNameArray objectAtIndex:i-1]] && ![OpenPlatformServers qqAppIsInstalled]) {
                    [shareButton setBackgroundImage:[UIImage imageNamed:@"qq_zone_unable.png"] forState:UIControlStateNormal];
                    shareButton.enabled = NO;
                }
                else if ([@"sina_weibo_able.png" isEqualToString:[shareButtonImagesNameArray objectAtIndex:i-1]] && ![OpenPlatformServers sinaWeiboAppIsInstalled]) {
                    [shareButton setBackgroundImage:[UIImage imageNamed:@"sina_weibo_unable.png"] forState:UIControlStateNormal];
                    shareButton.enabled = NO;
                }
                else if ([@"weixin_circle_able.png" isEqualToString:[shareButtonImagesNameArray objectAtIndex:i-1]] && ![OpenPlatformServers weiXinAppIsInstalled]) {
                    [shareButton setBackgroundImage:[UIImage imageNamed:@"weixin_circle_unable.png"] forState:UIControlStateNormal];
                    shareButton.enabled = NO;
                }
                else if ([@"weixin_friend_able.png" isEqualToString:[shareButtonImagesNameArray objectAtIndex:i-1]] && ![OpenPlatformServers weiXinAppIsInstalled]) {
                    [shareButton setBackgroundImage:[UIImage imageNamed:@"weixin_friend_unable.png"] forState:UIControlStateNormal];
                    shareButton.enabled = NO;
                }
                 */

                //有Title的时候
                if (self.isHadTitle == YES) {
                    [shareButton setFrame:CGRectMake(self.SHAREBUTTON_INTERVAL_WIDTH+((line-1)*(self.SHAREBUTTON_INTERVAL_WIDTH+SHAREBUTTON_WIDTH)), self.LXActivityHeight+((column-1)*(SHAREBUTTON_INTERVAL_HEIGHT+SHAREBUTTON_HEIGHT + BUTTON_INTERVAL_HEIGHT)), SHAREBUTTON_WIDTH, SHAREBUTTON_HEIGHT)];
                }
                else{
                    [shareButton setFrame:CGRectMake(self.SHAREBUTTON_INTERVAL_WIDTH+((line-1)*(self.SHAREBUTTON_INTERVAL_WIDTH+SHAREBUTTON_WIDTH)), SHAREBUTTON_INTERVAL_HEIGHT+((column-1)*(SHAREBUTTON_INTERVAL_HEIGHT+SHAREBUTTON_HEIGHT + BUTTON_INTERVAL_HEIGHT)), SHAREBUTTON_WIDTH, SHAREBUTTON_HEIGHT)];
                }
                [self.backGroundView addSubview:shareButton];
                
                self.postionIndexNumber++;
            }
        }
    }
    
    if (shareButtonTitlesArray) {
        if (shareButtonTitlesArray.count > 0 && shareButtonImagesNameArray.count > 0) {
            for (int j = 1; j < shareButtonTitlesArray.count+1; j++) {
                //计算出行数，与列数
                int column = (int)ceil((float)(j)/LINECOUNT); //行
                int line = (j)%LINECOUNT; //列
                if (line == 0) {
                    line = LINECOUNT;
                }
                UILabel *shareLabel = [self creatShareLabelWithColumn:column andLine:line];
                shareLabel.textColor = [UIColor whiteColor];
                shareLabel.text = [shareButtonTitlesArray objectAtIndex:j-1];
                //有Title的时候
                if (self.isHadTitle == YES) {
                    [shareLabel setFrame:CGRectMake(self.SHARETITLE_INTERVAL_WIDTH+((line-1)*(self.SHARETITLE_INTERVAL_WIDTH+SHARETITLE_WIDTH)), self.LXActivityHeight+SHAREBUTTON_HEIGHT+((column-1)*(SHARETITLE_INTERVAL_HEIGHT + BUTTON_INTERVAL_HEIGHT)), SHARETITLE_WIDTH, SHARETITLE_HEIGHT)];
                }
                [self.backGroundView addSubview:shareLabel];
            }
        }
    }
    
    UIImageView *imageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 120, [UIScreen mainScreen].bounds.size.width, 1)];
    imageView.image = [UIImage imageNamed:@"我的订单分割线.png"];
    [self.backGroundView addSubview:imageView];
    
    //再次计算加入shareButtons后LXActivity的高度
    if (shareButtonImagesNameArray && shareButtonImagesNameArray.count > 0) {
        int totalColumns = (int)ceil((float)(shareButtonImagesNameArray.count)/LINECOUNT);
        if (self.isHadTitle  == YES) {
            self.LXActivityHeight = self.LXActivityHeight + totalColumns*(SHAREBUTTON_INTERVAL_HEIGHT+SHAREBUTTON_HEIGHT + 10);
        }
        else{
            self.LXActivityHeight = SHAREBUTTON_INTERVAL_HEIGHT + totalColumns*(SHAREBUTTON_INTERVAL_HEIGHT+SHAREBUTTON_HEIGHT + 10);
        }
    }
    
    if (LINECOUNT < [shareButtonTitlesArray count]) {
        self.LXActivityHeight += 30;
    }
    
//    if (!IOS7_OR_LATER) {
//        self.LXActivityHeight += 30;
//    }
//    else if (IOS7_OR_LATER && 4 < [shareButtonImagesNameArray count]) {
//        self.LXActivityHeight += 30;
//    }
    
    if (cancelButtonTitle) {
        self.isHadCancelButton = YES;
        UIButton *cancelButton = [self creatCancelButtonWith:cancelButtonTitle];
        cancelButton.tag = self.postionIndexNumber;
        [cancelButton addTarget:self action:@selector(didClickOnImageIndex:) forControlEvents:UIControlEventTouchUpInside];
        
        //当没title destructionButton otherbuttons时
        if (self.isHadTitle == NO && self.isHadShareButton == NO) {
            self.LXActivityHeight = self.LXActivityHeight + cancelButton.frame.size.height+(2*BUTTON_INTERVAL_HEIGHT);
        }
        //当有title或destructionButton或otherbuttons时
        if (self.isHadTitle == YES || self.isHadShareButton == YES) {
            [cancelButton setFrame:CGRectMake(cancelButton.frame.origin.x, IOS7_OR_LATER ? self.LXActivityHeight : self.LXActivityHeight, cancelButton.frame.size.width, cancelButton.frame.size.height)];
            self.LXActivityHeight = self.LXActivityHeight + cancelButton.frame.size.height+BUTTON_INTERVAL_HEIGHT;
        }
        [self.backGroundView addSubview:cancelButton];
        
        self.postionIndexNumber++;
    }
    
//    if (cancelButtonTitle) {
//        self.isHadCancelButton = YES;
//        UIButton *cancelButton = [self creatCancelButtonWith:cancelButtonTitle];
//        cancelButton.tag = self.postionIndexNumber;
//        [cancelButton addTarget:self action:@selector(didClickOnImageIndex:) forControlEvents:UIControlEventTouchUpInside];
//        
//        //当没title destructionButton otherbuttons时
//        if (self.isHadTitle == NO && self.isHadShareButton == NO) {
//            self.LXActivityHeight = self.LXActivityHeight + cancelButton.frame.size.height+(2*BUTTON_INTERVAL_HEIGHT);
//        }
//        //当有title或destructionButton或otherbuttons时
//        if (self.isHadTitle == YES || self.isHadShareButton == YES) {
//            [cancelButton setFrame:CGRectMake(cancelButton.frame.origin.x, 120, cancelButton.frame.size.width, cancelButton.frame.size.height)];
//            self.LXActivityHeight = self.LXActivityHeight + cancelButton.frame.size.height+BUTTON_INTERVAL_HEIGHT;
//        }
//        [self.backGroundView addSubview:cancelButton];
//        
//        self.postionIndexNumber++;
//    }
//    
//    [UIView animateWithDuration:ANIMATE_DURATION animations:^{
//        if (IOS7_OR_LATER) {
//            [self.backGroundView setFrame:CGRectMake(0, [UIScreen mainScreen].bounds.size.height-180, [UIScreen mainScreen].bounds.size.width, 180)];
//        }
//        else {
//            [self.backGroundView setFrame:CGRectMake(0, [UIScreen mainScreen].bounds.size.height-180, [UIScreen mainScreen].bounds.size.width, 180)];
//        }
//    } completion:^(BOOL finished) {
//    }];
    
    [UIView animateWithDuration:ANIMATE_DURATION animations:^{
        if (IOS7_OR_LATER) {
            [self.backGroundView setFrame:CGRectMake(0, [UIScreen mainScreen].bounds.size.height-self.LXActivityHeight + 20, [UIScreen mainScreen].bounds.size.width, self.LXActivityHeight+30)];
        }
        else {
            [self.backGroundView setFrame:CGRectMake(0, [UIScreen mainScreen].bounds.size.height-self.LXActivityHeight, [UIScreen mainScreen].bounds.size.width, self.LXActivityHeight+30)];
        }
    } completion:^(BOOL finished) {
    }];
}


- (UIButton *)creatCancelButtonWith:(NSString *)cancelButtonTitle
{
    UIButton *cancelButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, BUTTON_HEIGHT)];

    cancelButton.backgroundColor = [UIColor whiteColor];

    [cancelButton setTitle:cancelButtonTitle forState:UIControlStateNormal];
    cancelButton.titleLabel.font = BUTTONTITLE_FONT;
    [cancelButton setTitleColor:[UIColor colorWithRed:0/255.0 green:90/255.0 blue:254/255.0 alpha:1] forState:UIControlStateNormal];

    return cancelButton;
}

- (UIButton *)creatShareButtonWithColumn:(int)column andLine:(int)line
{
    UIButton *shareButton = [[UIButton alloc] initWithFrame:CGRectMake(self.SHAREBUTTON_INTERVAL_WIDTH+((line-1)*(self.SHAREBUTTON_INTERVAL_WIDTH+SHAREBUTTON_WIDTH)), SHAREBUTTON_INTERVAL_HEIGHT+((column-1)*(SHAREBUTTON_INTERVAL_HEIGHT+SHAREBUTTON_HEIGHT + 10)), SHAREBUTTON_WIDTH, SHAREBUTTON_HEIGHT)];
    return shareButton;
}

- (UILabel *)creatShareLabelWithColumn:(int)column andLine:(int)line
{
    UILabel *shareLabel = [[UILabel alloc] initWithFrame:CGRectMake(self.SHARETITLE_INTERVAL_WIDTH+((line-1)*(self.SHARETITLE_INTERVAL_WIDTH+SHARETITLE_WIDTH)), SHARETITLE_INTERVAL_HEIGHT+((column-1)*(SHARETITLE_INTERVAL_HEIGHT + BUTTON_INTERVAL_HEIGHT)), SHARETITLE_WIDTH, SHARETITLE_HEIGHT)];
    
    shareLabel.backgroundColor = [UIColor clearColor];
    shareLabel.textAlignment = NSTextAlignmentCenter;
    shareLabel.font = TITLE_FONT;
    shareLabel.textColor = [UIColor whiteColor];
    return shareLabel;
}

- (UILabel *)creatTitleLabelWith:(NSString *)title
{
    UILabel *titlelabel = [[UILabel alloc] initWithFrame:CGRectMake(TITLE_INTERVAL_WIDTH, TITLE_INTERVAL_HEIGHT, TITLE_WIDTH, TITLE_HEIGHT)];
    titlelabel.backgroundColor = [UIColor clearColor];
    titlelabel.textAlignment = NSTextAlignmentCenter;
    titlelabel.shadowColor = [UIColor blackColor];
    titlelabel.shadowOffset = SHADOW_OFFSET;
    titlelabel.font = SHARETITLE_FONT;
    titlelabel.text = title;
    titlelabel.textColor = [UIColor whiteColor];
    titlelabel.numberOfLines = TITLE_NUMBER_LINES;
    return titlelabel;
}

- (void)didClickOnImageIndex:(UIButton *)button
{
    if (self.delegate) {
        if (button.tag != self.postionIndexNumber) {
            if ([self.delegate respondsToSelector:@selector(didClickOnImageIndex:)] == YES) {
                [self.delegate didClickOnImageIndex:button.tag];
            }
        }
        else{
            if ([self.delegate respondsToSelector:@selector(didClickOnCancelButton)] == YES){
                [self.delegate didClickOnCancelButton];
            }
        }
    }
    [self tappedCancel];
}

- (void)tappedCancel
{
    [UIView animateWithDuration:ANIMATE_DURATION animations:^{
        [self.backGroundView setFrame:CGRectMake(0, [UIScreen mainScreen].bounds.size.height, [UIScreen mainScreen].bounds.size.width, 0)];
        self.alpha = 0;
    } completion:^(BOOL finished) {
        if (finished) {
            [self removeFromSuperview];
        }
    }];
}

- (void)tappedBackGroundView
{
    //
    
}

@end
