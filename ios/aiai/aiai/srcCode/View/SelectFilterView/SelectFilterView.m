//
//  SelectFilterView.m
//  aiai
//
//  Created by zhangyang on 15/3/27.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "SelectFilterView.h"
#import "ScrollSelectView.h"
#import "SelectFilterPlayView.h"
#import "VedioInsertEditView.h"
#import "FilterManager.h"


#import "DownLoadFilterViewController.h"

#define firstBtnTitle   @"视频特效"
#define secondBtnTitle  @"视频滤镜"
#define threedBtnTitle  @"修改时间"
#define fourthBtnTitle  @"定格动画"
#define fiveBtnTitle    @"添加标签"

@interface SelectFilterView ()<ScrollSelectViewDelegate,DownLoadFilterViewControllerDelegate>
@property(nonatomic,strong)NSURL* moviePath;
@property(nonatomic,assign)CGFloat movieTime;

//播放器
@property(nonatomic,strong)SelectFilterPlayView* selectFilterPlayView;

//图片展示
@property(nonatomic,strong)UIImageView* selectImageView;

//滤镜选择视图
@property(nonatomic,strong)ScrollSelectView* scrollSelectView;

//滤镜时间选择视图
@property(nonatomic,strong)VedioInsertEditView* vedioInsertEditView;
@property(nonatomic,assign)CGFloat effectInsertTime;

@property(nonatomic,assign)SelectFilterView_SelectBtn selectBtn;
@property(nonatomic,strong)NSMutableArray* btnArray;

@end

@implementation SelectFilterView

-(id)initWithFrame:(CGRect)frame moviePath:(NSURL*)moviePath movieTime:(CGFloat)time{
    if (self = [super initWithFrame:frame]) {
        self.moviePath = moviePath;
        self.movieTime = time;
        //播放视图
        _selectFilterPlayView = [[SelectFilterPlayView alloc] initWithFrame:CGRectMake(0, 0, frame.size.width, frame.size.width) moviePath:moviePath movieTime:time];
        [self addSubview:_selectFilterPlayView];
        //图片展示
        _selectImageView = [[UIImageView alloc] initWithFrame:_selectFilterPlayView.frame];
        _selectImageView.hidden = YES;
        [self addSubview:_selectImageView];
        
        //下半部分子视图的起点和终点
        CGFloat subViewStart_Y = _selectFilterPlayView.frame.size.height+_selectFilterPlayView.frame.origin.y;
        CGFloat subViewEnd_Y = self.frame.origin.y+self.frame.size.height;
        //各个子视图的高度
        CGFloat buttonBackGroundView_H = 24;
        UIFont* btnFont = [UIFont systemFontOfSize:18];
        CGFloat scrollSelectView_H = 130;
        CGFloat sureCancleBtnBgView_H = 24;
        if (iPhone5) {
            scrollSelectView_H = 140;
        }else if (iPhone6) {
            scrollSelectView_H = 160;
        }else if (iPhone6_Plus) {
            scrollSelectView_H = 190;
        }
        CGFloat subViewPadding = (subViewEnd_Y - subViewStart_Y - buttonBackGroundView_H - scrollSelectView_H - sureCancleBtnBgView_H)/4;
        
        //进行子视图的布局
        CGFloat buttonBackGroundView_Y = _selectFilterPlayView.frame.size.height + _selectFilterPlayView.frame.origin.y + subViewPadding;
        UIView* buttonBackGroundView = [[UIView alloc] initWithFrame:CGRectMake(0, buttonBackGroundView_Y, frame.size.width, buttonBackGroundView_H)];
        buttonBackGroundView.backgroundColor = [UIColor clearColor];
        
        CGFloat btn_padding = 10;
        CGFloat btn_W = 75;
        CGFloat btn_X = (frame.size.width - btn_W*4 - btn_padding*3)/2;
        CGFloat btn_Y = 0;
        CGFloat btn_H = buttonBackGroundView.frame.size.height;

        //视频特效
        UIButton* TeXiaoBtn = [UIButton buttonWithType:UIButtonTypeSystem];
        TeXiaoBtn.titleLabel.font = btnFont;
        TeXiaoBtn.frame = CGRectMake(btn_X, btn_Y, btn_W, btn_H);
        [TeXiaoBtn setTitle:firstBtnTitle forState:UIControlStateNormal];
        [TeXiaoBtn addTarget:self action:@selector(TeXiaoBtnBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [buttonBackGroundView addSubview:TeXiaoBtn];
        
        //视频滤镜
        btn_X = TeXiaoBtn.frame.origin.x+TeXiaoBtn.frame.size.width + btn_padding;
        UIButton* LvJingBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        LvJingBtn.titleLabel.font = btnFont;
        LvJingBtn.frame = CGRectMake(btn_X, btn_Y, btn_W, btn_H);
        [LvJingBtn setTitle:secondBtnTitle forState:UIControlStateNormal];
        [LvJingBtn addTarget:self action:@selector(LvJingBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [buttonBackGroundView addSubview:LvJingBtn];
        
        //定格动画
        btn_X = LvJingBtn.frame.origin.x+LvJingBtn.frame.size.width + btn_padding;
        UIButton* KaDunBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        KaDunBtn.titleLabel.font = btnFont;
        KaDunBtn.frame = CGRectMake(btn_X, btn_Y, btn_W, btn_H);
        [KaDunBtn setTitle:fourthBtnTitle forState:UIControlStateNormal];
        [KaDunBtn addTarget:self action:@selector(KaDunBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [buttonBackGroundView addSubview:KaDunBtn];
        
        //标签
        btn_X = KaDunBtn.frame.origin.x+KaDunBtn.frame.size.width + btn_padding;
        UIButton *logoBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        logoBtn.titleLabel.font = btnFont;
        logoBtn.frame = CGRectMake(btn_X, btn_Y, btn_W, btn_H);
        [logoBtn setTitle:fiveBtnTitle forState:UIControlStateNormal];
        [logoBtn addTarget:self action:@selector(logoAction:) forControlEvents:UIControlEventTouchUpInside];
        [buttonBackGroundView addSubview:logoBtn];
        [self addSubview:buttonBackGroundView];
        
        //滚动控件
        CGFloat scrollSelectView_Y = buttonBackGroundView.frame.origin.y + buttonBackGroundView.frame.size.height + subViewPadding;
        _scrollSelectView = [[ScrollSelectView alloc] initWithFrame:CGRectMake(0, scrollSelectView_Y, self.frame.size.width, scrollSelectView_H)];
        _scrollSelectView.delegate = self;
        _scrollSelectView.backgroundColor = [UIColor clearColor];
        [self addSubview:_scrollSelectView];
        
        //确认取消按钮的背景图
        CGFloat sureCancleBtnBgView_Y = _scrollSelectView.frame.origin.y+_scrollSelectView.frame.size.height + subViewPadding;
        UIView* sureCancleBtnBgView = [[UIView alloc] initWithFrame:CGRectMake(0, sureCancleBtnBgView_Y, _scrollSelectView.frame.size.width, sureCancleBtnBgView_H)];
        sureCancleBtnBgView.backgroundColor = [UIColor clearColor];
        [self addSubview:sureCancleBtnBgView];
        //确认按钮
        CGFloat Btn_X = 20;
        CGFloat Btn_W = sureCancleBtnBgView_H;
        CGFloat Btn_H = Btn_W;
        
        UIButton* cancleBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        cancleBtn.frame = CGRectMake(Btn_X, 0, Btn_W, Btn_H);
        [cancleBtn setBackgroundImage:[UIImage imageNamed:@"selectFilter_cancle.png"] forState:UIControlStateNormal];
        [cancleBtn addTarget:self action:@selector(cancleBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [sureCancleBtnBgView addSubview:cancleBtn];
        
        Btn_X = sureCancleBtnBgView.frame.size.width - Btn_W - Btn_X;
        UIButton* sureBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        sureBtn.frame = CGRectMake(Btn_X, 0, Btn_W, Btn_H);
        [sureBtn setBackgroundImage:[UIImage imageNamed:@"selectFilter_sure.png"] forState:UIControlStateNormal];
        [sureBtn addTarget:self action:@selector(sureBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [sureCancleBtnBgView addSubview:sureBtn];
        
        //修改时间
        UIButton* TeXiaoTimeBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        TeXiaoTimeBtn.titleLabel.font = btnFont;
        TeXiaoTimeBtn.frame = CGRectMake(0, 0, btn_W, Btn_H);
        [TeXiaoTimeBtn setTitle:threedBtnTitle forState:UIControlStateNormal];
        [TeXiaoTimeBtn addTarget:self action:@selector(TeXiaoTimeBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        TeXiaoTimeBtn.center = sureCancleBtnBgView.center;
        CGRect rect = TeXiaoTimeBtn.frame;
        rect.origin.x = self.center.x - rect.size.width/2;
        rect.origin.y = 0;
        TeXiaoTimeBtn.frame = rect;
        [sureCancleBtnBgView addSubview:TeXiaoTimeBtn];
        
        //按钮数组用来进行按钮的逻辑
        _btnArray = [NSMutableArray arrayWithObjects:TeXiaoBtn,LvJingBtn,TeXiaoTimeBtn,KaDunBtn,logoBtn,nil];
        
        [self TeXiaoBtnBtnAction:TeXiaoBtn];
    }
    return self;
}

-(void)releaseAllSource{
    [_selectFilterPlayView releaseSystemPlayer];
    [_selectFilterPlayView releaseFilterPlayer];
}

-(void)dealloc{
    AIAI_LOG(@"SelectFilterView dealloc");
}

#pragma mark BtnAction
//处理btn的逻辑和ui展示
-(void)getFouceBtnColorWithBtn:(UIButton*)btn{
    NSString * btnTitle = [btn titleForState:UIControlStateNormal];
    if ([btnTitle isEqualToString:firstBtnTitle]) {
        self.selectBtn = Select_TeXiaoBtn;
        _selectImageView.hidden = YES;
    }else if ([btnTitle isEqualToString:secondBtnTitle]) {
        self.selectBtn = Select_LvJingBtn;
        _selectImageView.hidden = YES;
    }else if ([btnTitle isEqualToString:threedBtnTitle]){
        self.selectBtn = Select_TeXiaoTimeBtn;
        _selectImageView.hidden = YES;
    }else if ([btnTitle isEqualToString:fourthBtnTitle]){
        self.selectBtn = Select_KaDunBtn;
        _selectImageView.hidden = NO;
    }else if ([btnTitle isEqualToString:fiveBtnTitle]){
        self.selectBtn = Select_MovieBtn;
        _selectImageView.hidden = NO;
    }
    
    for (UIButton* one in _btnArray) {
        if (btn == one) {
            [one setTitleColor:Every_YellowColor forState:UIControlStateNormal];
        }else{
            [one setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        }
    }
}

-(void)TeXiaoBtnBtnAction:(id)sender{
    [self getFouceBtnColorWithBtn:sender];
    
    NSArray* imagePathArray = [FilterManager share].effectImagePathArray;
    NSArray* textArray = [FilterManager share].effectTitleArray;
    
    _vedioInsertEditView.hidden = YES;
    _scrollSelectView.hidden = NO;
    [self bringSubviewToFront:_scrollSelectView];

    _scrollSelectView.isShowSelectView = NO;
    _scrollSelectView.isSwapeDeleteEnable = NO;
    [_scrollSelectView setImagePathArray:imagePathArray textArray:textArray];
}

-(void)LvJingBtnAction:(id)sender{
    [self getFouceBtnColorWithBtn:sender];
    
    NSArray* imagePathArray = [FilterManager share].filterImagePathArray;
    NSArray* textArray = [FilterManager share].filterTitleArray;
    
    _vedioInsertEditView.hidden = YES;
    _scrollSelectView.hidden = NO;
    [self bringSubviewToFront:_scrollSelectView];

    _scrollSelectView.isShowSelectView = NO;
    _scrollSelectView.isSwapeDeleteEnable = NO;
    [_scrollSelectView setImagePathArray:imagePathArray textArray:textArray];
}

-(void)TeXiaoTimeBtnAction:(id)sender{
    [self getFouceBtnColorWithBtn:sender];
    
    if (_vedioInsertEditView) {
        _vedioInsertEditView.hidden = NO;
        _scrollSelectView.hidden = YES;
        [self bringSubviewToFront:_vedioInsertEditView];
    }else{
        __weak SelectFilterView* weakself = self;
        _vedioInsertEditView = [[VedioInsertEditView alloc] initWithFrame:_scrollSelectView.frame andVedioLength:self.movieTime];
        _vedioInsertEditView.completeBlock = ^(CGFloat value){
            weakself.effectInsertTime = value;
             [weakself.selectFilterPlayView selectEffectsWithIndex:weakself.scrollSelectView.selectIndex insetFilterTime:value];
        };
        _vedioInsertEditView.cancelBlock = ^{
            for (UIButton* btn in weakself.btnArray) {
                if ([[btn titleForState:UIControlStateNormal] isEqualToString:firstBtnTitle]) {
                    [weakself TeXiaoBtnBtnAction:btn];
                    break;
                }
            }
        };
        [self addSubview:_vedioInsertEditView];
    }
}

-(void)KaDunBtnAction:(id)sender{
    [self getFouceBtnColorWithBtn:sender];
    
    __weak SelectFilterView* weakself = self;
    
    [SelectFilterView slowResultOutputDicPathWithVideoPath:self.moviePath.resourceSpecifier prepare:^{
        [WaitingAlertView showWaitingViewAtView:weakself.selectFilterPlayView];
    } complate:^(NSString *outputPathDic) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [WaitingAlertView hideWaitingView];
        });
        
        NSMutableArray* imagePathArray = [NSMutableArray array];
        
        NSFileManager *temFM = [NSFileManager defaultManager];//创建文件管理器
        if ([temFM fileExistsAtPath:outputPathDic]){
            NSArray* tmpPhotoArray = [temFM subpathsAtPath:outputPathDic];
            for (NSString* onePhotoPath in tmpPhotoArray) {
                NSString* imagePath = [outputPathDic stringByAppendingPathComponent:onePhotoPath];
                [imagePathArray addObject:imagePath];
            }
        }
        
        NSMutableArray* textArray = [NSMutableArray array];
        for (int i = 1; i<=[imagePathArray count]; i++) {
            [textArray addObject:[NSString stringWithFormat:@"%d.jpg",i]];
        }
        
        //从视频分割图片的函数为了避免阻塞使用线程，当回调的时候需要合并到主线程
        dispatch_async(dispatch_get_main_queue(), ^{
            _vedioInsertEditView.hidden = YES;
            _scrollSelectView.hidden = NO;
            [self bringSubviewToFront:_scrollSelectView];

            _scrollSelectView.isShowSelectView = NO;
            _scrollSelectView.isSwapeDeleteEnable = YES;
            [_scrollSelectView setImagePathArray:imagePathArray textArray:textArray];
        });
    }];
}

-(void)logoAction:(id)sender{
    
    [self getFouceBtnColorWithBtn:sender];
    
    NSArray* imagePathArray = [FilterManager share].movieImagePathArray;
    NSArray* textArray = [FilterManager share].movieTitleArray;
    
    _vedioInsertEditView.hidden = YES;
    _scrollSelectView.hidden = NO;
    [self bringSubviewToFront:_scrollSelectView];
    
    _scrollSelectView.isShowSelectView = NO;
    _scrollSelectView.isSwapeDeleteEnable = NO;
    [_scrollSelectView setImagePathArray:imagePathArray textArray:textArray];
    
}

-(void)sureBtnAction:(id)sender{
 
    if (self.sureBtnBlock) {
        self.sureBtnBlock(self.selectBtn);
    }
}

-(void)cancleBtnAction:(id)sender{
    if (self.cancleBtnBlock) {
        self.cancleBtnBlock(self.selectBtn);
    }
}

#pragma mark pravite Mothed

+(void)slowResultOutputDicPathWithVideoPath:(NSString*)videoPath prepare:(void (^)(void))prepare complate:(void (^)(NSString *outputPathDic))complate{
    
    prepare();
    
    dispatch_queue_t getAllFrameQueue = dispatch_queue_create("GetAllFrame Queue", NULL);
    dispatch_async(getAllFrameQueue, ^{
        NSString* outPutImageDic = [NSTemporaryDirectory() stringByAppendingPathComponent:@"tmpVideoImageDic"];
        NSFileManager *temFM = [NSFileManager defaultManager];//创建文件管理器
        if ([temFM fileExistsAtPath:outPutImageDic]){
            [temFM removeItemAtPath:outPutImageDic error:nil];
        }
        [temFM createDirectoryAtPath:outPutImageDic withIntermediateDirectories:YES attributes:nil error:nil];
        
        int size = 0;
        char** commandLinestr = [CommandLine GetAllFrameWithStringSize:&size inputSrc:videoPath outputSrc:outPutImageDic];
        ffmpegMain(size,commandLinestr);
        [CommandLine releaseCommandLineStr:commandLinestr size:size];
        
        complate(outPutImageDic);
    });
}

#pragma mark
-(void)notifyDelegateReloadData{
    for (UIButton* btn in self.btnArray) {
        if ([[btn titleForState:UIControlStateNormal] isEqualToString:firstBtnTitle]) {
            [self TeXiaoBtnBtnAction:btn];
            break;
        }
    }
}

#pragma mark ScrollSelectViewDelegate

-(void)scrollSelectViewTapActionWithSelectIndex:(NSInteger)selectIndex{

    NSArray* effectArray = [FilterManager share].effectImagePathArray;
    if (selectIndex == [effectArray count]-1) {
        
        [_selectFilterPlayView stopFilterPlayer];
        
        DownLoadFilterViewController* con = [[DownLoadFilterViewController alloc] init];
        con.delegate = self;
        [[AppDelegate rootViewController] pushViewController:con animated:YES];
    }else{
        [self scrollSelectViewDidEndDraggingWithSelectIndex:selectIndex];
    }
}

-(void)scrollSelectViewDidScrollWithSelectImagePath:(NSString*)selectImagePath{
    if (self.selectBtn == Select_TeXiaoBtn){
        return;
    }else if (self.selectBtn == Select_LvJingBtn){
        return;
    }else if (self.selectBtn == Select_TeXiaoTimeBtn){
        return;
    }else if (self.selectBtn == Select_KaDunBtn) {
        _selectImageView.image = [UIImage imageWithContentsOfFile:selectImagePath];
    }else if (self.selectBtn == Select_MovieBtn){
        return;
    }
}

-(void)scrollSelectViewDidEndDraggingWithSelectIndex:(NSInteger)selectIndex{
    if (self.selectBtn == Select_TeXiaoBtn){
        //选择特效
        [self.selectFilterPlayView selectEffectsWithIndex:_scrollSelectView.selectIndex insetFilterTime:self.effectInsertTime];
        
    }else if (self.selectBtn == Select_LvJingBtn){
        [self.selectFilterPlayView selectFilterWithIndex:_scrollSelectView.selectIndex insetFilterTime:self.effectInsertTime];
    }else if (self.selectBtn == Select_TeXiaoTimeBtn){
        return;
    }else if (self.selectBtn == Select_KaDunBtn) {
        return;
    }else if (self.selectBtn == Select_MovieBtn){
        [self.selectFilterPlayView selectMovieWithIndex:_scrollSelectView.selectIndex insetFilterTime:self.effectInsertTime];
    }
}
-(void)scrollSelectViewDidEndDeceleratingWithSelectIndex:(NSInteger)selectIndex{
    [self scrollSelectViewDidEndDraggingWithSelectIndex:selectIndex];
}

-(NSInteger)getSelectIndex{
    return _scrollSelectView.selectIndex;
}

-(NSArray*)getSelectSubViewImagePathArray{
    NSMutableArray* returnarray = [NSMutableArray array];
    NSArray* sub = [_scrollSelectView selectSubViews];
    for (ScrollImageView* imageview in sub) {
        [returnarray addObject:imageview.imagePath];
    }
    return returnarray;
}

@end
