//
//  HomeViewController.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/10/28.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "HomeViewController.h"
#import "BestSellersViewController.h"
#import "TransferCompanyViewController.h"
#import "STSRaidersViewController.h"

#import "SkinModel.h"
#import "HomeModel.h"
#import "TransferCompanyModel.h"
#import "BaseHandler.h"
#import "TransferCompanyCommentModel.h"
#import "STSRaidersModel.h"
#import "GlobalUserInfo.h"
#import "SearchGoodModel.h"
#import "GoodsDetailModel.h"

#define animationLabel_H 2

@interface HomeViewController ()<UIScrollViewDelegate>

@property (nonatomic ,strong) BestSellersViewController *firstVC;
@property (nonatomic ,strong) TransferCompanyViewController *secondVC;
@property (nonatomic ,strong) STSRaidersViewController *thirdVC;

@property (nonatomic ,strong) UIViewController *currentVC;
@property (nonatomic,strong)UILabel* animationLabel;

//保存titlebtn用，主要是用来变换按钮颜色的
@property (nonatomic,strong)NSMutableArray* titleBtnArray;

@end

@implementation HomeViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        _titleBtnArray = [NSMutableArray array];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    //生成标题scrollview和对应的button视图
    self.automaticallyAdjustsScrollViewInsets = NO;
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height = 40;
    UIScrollView* headScrollView = [[UIScrollView alloc] initWithFrame:rect];
    headScrollView.backgroundColor = [UIColor colorWithHexString:STS_GRAY_BACKGOUND_COLOR];
    headScrollView.contentSize = CGSizeMake(rect.size.width, rect.size.height);
    headScrollView.bounces = NO;
    headScrollView.pagingEnabled = YES;
    [self.view addSubview:headScrollView];
    
    NSArray* titleArray = @[@"热卖",@"转运公司",@"海淘攻略"];
    for (int i = 0; i < [titleArray count]; i++) {
        UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
        CGFloat buttonWidth = headScrollView.contentSize.width/[titleArray count];
        CGFloat buttonHeight = headScrollView.contentSize.height;
        button.frame = CGRectMake(0 + i*buttonWidth, 0, buttonWidth, buttonHeight);
        [button setTitle:[titleArray objectAtIndex:i] forState:UIControlStateNormal];
        if (i==0) {
            [button setTitleColor:[UIColor colorWithHexString:STS_BLACK_COLOR] forState:UIControlStateNormal];
        }else{
           [button setTitleColor:[UIColor colorWithHexString:STS_GRAY_TITLE_COLOR] forState:UIControlStateNormal];
        }
        
        button.tag = i + 100;
        [button addTarget:self action:@selector(didClickHeadButtonAction:) forControlEvents:UIControlEventTouchUpInside];
        [headScrollView addSubview:button];
        [_titleBtnArray addObject:button];
        
        if (self.animationLabel == nil) {
            self.animationLabel = [[UILabel alloc] initWithFrame:CGRectMake(button.frame.origin.x, button.frame.size.height-animationLabel_H, button.frame.size.width, animationLabel_H)];
            self.animationLabel.backgroundColor = [button titleColorForState:UIControlStateNormal];
            [headScrollView addSubview:self.animationLabel];
        }
    }
    //确认和计算subController的视图frame
    CGFloat vc_Y = headScrollView.frame.size.height+headScrollView.frame.origin.y;
    [self createChildViewControllerWithStartPoint:CGPointMake(0, vc_Y)];
    
    //请求数据
    HomeModel* cacheGoodsData = [SaveManager readDataUnArchiverWithFilePath:[SaveManager tmpPath:@"homeCache"]];
    if (cacheGoodsData == nil) {
        NSString* urlString = [RequestUrl httpMothed_HomePageUrl];
        [BaseHandler requestJsonDataWithGETUrl:urlString ready:^{
            STS_Log(@" 首页获取相关数据 ");
        } success:^(id responseObject) {
            HomeModel* homeModel = [HomeModel objectWithKeyValues:responseObject];
            [SaveManager saveDataArchiverWithModel:homeModel filePath:[SaveManager tmpPath:@"homeCache"]];
            STS_Log(@"homeModel = %@",homeModel);
        } failed:^(NSError *error) {
            STS_Log(@"error: %@", error);
        }];
    }else{
        [self reloadChildViewControllerWithHomeModel:cacheGoodsData];
        //加载缓存之后进行网络请求最新数据
        NSString* urlString = [RequestUrl httpMothed_HomePageUrl];
        [BaseHandler requestJsonDataWithGETUrl:urlString ready:^{
            STS_Log(@" 首页获取相关数据 ");
        } success:^(id responseObject) {
            HomeModel* homeModel = [HomeModel objectWithKeyValues:responseObject];
            [SaveManager saveDataArchiverWithModel:homeModel filePath:[SaveManager tmpPath:@"homeCache"]];
            STS_Log(@"homeModel = %@",homeModel);
            NSArray* goodskinArray = [SearchGoodSkinModel objectArrayWithKeyValuesArray:homeModel.goodList];
            SearchGoodSkinModel* skinModel = [goodskinArray lastObject];
            SearchGoodModel* goodsInfo = [SearchGoodModel objectWithKeyValues:skinModel._source];
            NSString* goodsInfoUrl = [RequestUrl httpMothed_GoodsInfoUrlWithGoodsId:goodsInfo.prod_id];
            NSString* commentListUrl = [RequestUrl httpMothed_GoodsCommentListUrlWithGoodsId:goodsInfo.prod_id];
            NSString* postContentUrl = [RequestUrl httpMothed_GoodsPostCommentUrlWithGoodsId:goodsInfo.prod_id score:5 content:@"测试中文" userName:@"zhangyang"];
            [BaseHandler requestJsonDataWithGETUrl:postContentUrl ready:^{
                
            } success:^(id responseObject) {
//                GoodsDetailModel* detailModel = [GoodsDetailModel objectWithKeyValues:responseObject];
                STS_Log(@"%@",responseObject);
            } failed:^(NSError *error) {
                STS_Log(@"error:%@",error);
            }];
//            NSString* searchListUrl = [RequestUrl httpMothed_SearchListUrl];
//            NSDictionary*parmarDic = [RequestUrl httpMothed_SearchListUrlWithBrand:nil website:nil query:@"que" sort:SORT_MODE_DEFAULT sortType:SORT_TYPE_POSITIVESEQUENCE page:0 size:10];
//            [BaseHandler requestJsonDataWithPostUrl:searchListUrl     ready:^{
//                
//            } success:^(id responseObject) {
//                STS_Log(@"%@",responseObject);
//            } failed:^(NSError *error) {
//                STS_Log(@" %@ ",error);
//            }];
            
        } failed:^(NSError *error) {
            STS_Log(@"error: %@", error);
        }];
    }
    
    
    
//    NSString* registerUrl = [RequestUrl httpMothed_UserRegisterUrlWithUsername:@"zhangyang" pwd:@"zhangt1985" tel:@"18600326932"];
//    NSString* loginUrl = [RequestUrl httpMothed_UserLoginUrlWithUsername:@"zhangyang" pwd:@"123"];
//    NSString* changePwdUrl = [RequestUrl httpMothed_UserChangePassWordUrlWithPwd:@"123" tel:@"18600326932"];
//    NSString* myCommentUrl = [RequestUrl httpMothed_MyselfCommentUrlWithUserID:@"45" from:1 size:10];
//    NSString* searchPageUrl = [RequestUrl httpMothed_SearchPageUrl];

//    [BaseHandler requestJsonDataWithGETUrl:searchPageUrl ready:^{
//        
//    } success:^(id responseObject) {
//        
//        STS_Log(@"%@", responseObject);
//        
//    } failed:^(NSError *error) {
//        STS_Log(@"error: %@", error);
//    }];
    
//    //转运公司接口测试
//    NSString* urlString = [RequestUrl httpMothed_TransferCompanyListUrlFrom:0 size:20];
//    [BaseHandler requestJsonDataWithGETUrl:urlString ready:^{
//        STS_Log(@" 转运公司发起请求 ");
//    } success:^(id responseObject) {
//        NSArray* list = [TransferCompanyModel objectArrayWithKeyValuesArray:responseObject];
//        TransferCompanyModel* model = [list objectAtIndex:0];
//
//        //获取评论列表
//        NSString* TransferCompanyCommentListUrlString = [RequestUrl httpMothed_TransferCompanyCommentListUrlWithId:model.id from:0 size:10];
//        [BaseHandler requestJsonDataWithGETUrl:TransferCompanyCommentListUrlString ready:^{
//            
//        } success:^(id responseObject) {
//            NSArray* transferCompanyCommentList = [TransferCompanyCommentModel objectArrayWithKeyValuesArray:responseObject];
//            TransferCompanyCommentModel* transferCompanyCommentModel = [transferCompanyCommentList objectAtIndex:0];
//            STS_Log(@"%@",transferCompanyCommentModel.seller);
//            
//            //发表评论
//            NSDictionary* parmarDic = [RequestUrl httpMothed_TransferCompanyCommentUrlWithId:model.id cpName:model.name cpTime:@"2015-12-10" allscore:transferCompanyCommentModel.allscore orderTime:@"2015-12-01" seller:@"张阳"];
//            NSString* TransferCompanyTalkCommentUrlString = [RequestUrl httpMothed_TransferCompanyCommentUrl];
//            [BaseHandler requestJsonDataWithPostUrl:TransferCompanyTalkCommentUrlString parame:parmarDic ready:^{
//                
//            } success:^(id responseObject) {
//                STS_Log(@" 发表评论成功 ");
//            } failed:^(NSError *error) {
//                STS_Log(@" %@ ",error);
//            }];
//        } failed:^(NSError *error) {
//            
//        }];
//        
//        
//    } failed:^(NSError *error) {
//        STS_Log(@"error: %@", error);
//    }];
    
//    //攻略接口，列表和详情
//    NSString* gonglue = [RequestUrl httpMothed_StsRaidersListUrlFrom:0 size:10];
//    [BaseHandler requestJsonDataWithGETUrl:gonglue ready:^{
//        
//    } success:^(id responseObject) {
//        
//        NSArray* raidersList = [STSRaidersModel objectArrayWithKeyValuesArray:responseObject];
//        STSRaidersModel* one = [raidersList lastObject];
//        
//        STS_Log(@"%@",one);
//        dispatch_async(dispatch_get_main_queue(), ^{
//            UIWebView* webview = [[UIWebView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height)];
//            [webview loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:one.url]]];
//            [self.view addSubview:webview];
//        });
//        
//        
//    } failed:^(NSError *error) {
//        STS_Log(@"%@",error);
//    }];
}

- (void)didClickHeadButtonAction:(UIButton *)button
{
    //给按钮变换颜色
    for (UIButton* btn in _titleBtnArray) {
        if (btn == button) {
            [btn setTitleColor:[UIColor colorWithHexString:STS_BLACK_COLOR] forState:UIControlStateNormal];
        }else{
            [btn setTitleColor:[UIColor colorWithHexString:STS_GRAY_TITLE_COLOR] forState:UIControlStateNormal];
        }
    }
    //进行界面替换
    switch (button.tag) {
        case 100:
            [button setTitleColor:[UIColor colorWithHexString:STS_BLACK_COLOR] forState:UIControlStateNormal];
            [self replaceController:self.currentVC newController:self.firstVC currentBtn:button];
            break;
        case 101:
            [self replaceController:self.currentVC newController:self.secondVC currentBtn:button];
            break;
        case 102:
            [self replaceController:self.currentVC newController:self.thirdVC currentBtn:button];
            break;
        default:
            break;
    }
    
}

-(void)createChildViewControllerWithStartPoint:(CGPoint)startPoint{
    CGRect rect = self.view.frame;
    rect.origin.y = startPoint.y;
    rect.size.height = rect.size.height - rect.origin.y;
    
    self.firstVC = [[BestSellersViewController alloc] init];
    [self.firstVC setPresetViewFrame:rect];
    [self addChildViewController:_firstVC];
    [self.view addSubview:self.firstVC.view];
    self.currentVC = self.firstVC;
    
    
    self.secondVC = [[TransferCompanyViewController alloc] init];
    [self.secondVC setPresetViewFrame:rect];
    self.thirdVC = [[STSRaidersViewController alloc] init];
    [self.thirdVC setPresetViewFrame:rect];
}

//  切换各个标签内容
- (void)replaceController:(UIViewController *)oldController newController:(UIViewController *)newController currentBtn:(UIButton*)button
{
    [self addChildViewController:newController];
    [self transitionFromViewController:oldController toViewController:newController duration:0.3 options:UIViewAnimationOptionTransitionNone animations:^{
        self.animationLabel.frame = CGRectMake(button.frame.origin.x, button.frame.size.height-animationLabel_H, button.frame.size.width, animationLabel_H);
    }completion:^(BOOL finished) {
        if (finished) {
            [newController didMoveToParentViewController:self];
            [oldController willMoveToParentViewController:nil];
            [oldController removeFromParentViewController];
            self.currentVC = newController;
        }else{
            self.currentVC = oldController;
        }
    }];
}

#pragma mark 请求数据接口

-(void)reloadChildViewControllerWithHomeModel:(HomeModel*) homeModel{
//    homeModel.brandList
}


@end
