//
//  DownLoadFilterCell.m
//  aiai
//
//  Created by wangnan on 15/5/28.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "DownLoadFilterCell.h"
#import "LoadFilterModel.h"

@interface DownLoadFilterCell ()

@property(nonatomic, strong)AsyImageView *headImageView;
@property(nonatomic, strong)UILabel *firterNameLabel;
@property(nonatomic, strong)UILabel *upLoadNameLabel;
@property(nonatomic, strong)UIButton *downLoadBtn;
@property(nonatomic, strong)UIProgressView *perProgress;

@property(nonatomic, strong)LoadFilterModel *filterModel;

@end

@implementation DownLoadFilterCell
@synthesize downLoadBlock;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        //头像
        _headImageView = [[AsyImageView alloc] init];
        CGFloat padding_TB = 15;
        CGFloat padding_LR = 8;
        
        CGFloat headImageView_H = 50;
        _headImageView.frame = CGRectMake(padding_LR, padding_TB, headImageView_H, headImageView_H);
        _headImageView.backgroundColor = [UIColor clearColor];
        [self addSubview:_headImageView];
        
        //用户名称
        CGFloat nameLabel_X = _headImageView.frame.origin.x+_headImageView.frame.size.width+padding_LR;
        CGFloat nameLabel_Y = 10;
        CGFloat nameLabel_W = self.frame.size.width - _headImageView.frame.size.width - padding_LR*2;
        CGFloat nameLabel_H = 30;
        _firterNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(nameLabel_X, nameLabel_Y, nameLabel_W, nameLabel_H)];
        _firterNameLabel.textAlignment = NSTextAlignmentLeft;
        _firterNameLabel.textColor = [UIColor whiteColor];
        _firterNameLabel.font = [UIFont systemFontOfSize:15];
        _firterNameLabel.text = @"";
        [self addSubview:_firterNameLabel];
        
        //上传者名称
        CGFloat upLoadLabel_X = _firterNameLabel.frame.origin.x;
        CGFloat upLoadLabel_Y = _firterNameLabel.frame.origin.y + _firterNameLabel.frame.size.height;
        CGFloat upLoadLabel_W = self.frame.size.width - _headImageView.frame.size.width - padding_LR*2;
        CGFloat upLoadLabel_H = 30;
        _upLoadNameLabel = [[UILabel alloc] initWithFrame:CGRectMake(upLoadLabel_X, upLoadLabel_Y, upLoadLabel_W, upLoadLabel_H)];
        _upLoadNameLabel.textAlignment = NSTextAlignmentLeft;
        _upLoadNameLabel.textColor = [[UIColor whiteColor] colorWithAlphaComponent:0.5];
        _upLoadNameLabel.font = [UIFont systemFontOfSize:11];
        _upLoadNameLabel.text = @"上传者:屁屁";
        [self addSubview:_upLoadNameLabel];
        
        CGFloat btnWide = 60;
        CGFloat btnHeight = 25;
        _downLoadBtn = [[UIButton alloc] init];
        _downLoadBtn.frame = CGRectMake([UIScreen mainScreen].bounds.size.width - btnWide - padding_LR, (DOWNLOADFILTER_CELLHEIGHT - btnHeight)/2, btnWide, btnHeight);
        [_downLoadBtn setTitle:@"下载" forState:UIControlStateNormal];
        [_downLoadBtn setTitle:@"下载" forState:UIControlStateHighlighted];
        [_downLoadBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [_downLoadBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
        [_downLoadBtn setBackgroundColor:[UIColor clearColor]];
        _downLoadBtn.layer.borderColor = [UIColor whiteColor].CGColor;
        _downLoadBtn.layer.borderWidth = 1.0;
        _downLoadBtn.layer.cornerRadius = _downLoadBtn.frame.size.height/2;
        _downLoadBtn.layer.masksToBounds = YES;
        _downLoadBtn.titleLabel.font = [UIFont systemFontOfSize:14];
        [_downLoadBtn addTarget:self action:@selector(downLoadAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_downLoadBtn];
        
        CGFloat perWide = btnWide;
        CGFloat perHeight = 3;
        _perProgress = [[UIProgressView alloc] initWithFrame:CGRectMake([UIScreen mainScreen].bounds.size.width - perWide - padding_LR, (DOWNLOADFILTER_CELLHEIGHT - perHeight)/2, perWide, perHeight)];
        _perProgress.trackTintColor = [[UIColor whiteColor] colorWithAlphaComponent:0.3];
        _perProgress.progressTintColor = Every_YellowColor;
        _perProgress.progress = 0.0;
        _perProgress.hidden = YES;
        [self addSubview:_perProgress];
        
        UIImageView *lineView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        lineView.frame = CGRectMake(padding_LR, DOWNLOADFILTER_CELLHEIGHT - 1, [UIScreen mainScreen].bounds.size.width - padding_LR*2, 1);
        lineView.alpha = 0.5;
        [self addSubview:lineView];
    }
    
    self.backgroundColor = EveryView_BackGroundColor;
    self.selectionStyle = UITableViewCellSelectionStyleNone;
    return self;
}

-(void)setFilterCellWithModel:(LoadFilterModel*)model{
    
    _filterModel = model;
    
    _firterNameLabel.text = model.loadName;
    _upLoadNameLabel.text = [NSString stringWithFormat:@"上传者:%@",model.upLoadAuth];
    [_headImageView loadImageWithPath:model.loadimagePath andPlaceHolderName:DefalutHeadImage];
    
    [self setUIWithState:_filterModel.loadState];
}

-(void)layoutSubviews{
    
    //cell重用 如果是下载字体 重新码下UI
    if (_filterModel.loadType == downLoad_font) {
        
        _upLoadNameLabel.hidden = YES;
        _firterNameLabel.hidden = YES;
        _headImageView.frame = CGRectMake(16, _headImageView.frame.origin.y, _headImageView.frame.size.width, _headImageView.frame.size.height);
//        _firterNameLabel.frame = CGRectMake(_firterNameLabel.frame.origin.x, 0, 150, self.frame.size.height);
    }
    
}

#pragma mark -- setLoadStateUI
-(void)setUIWithState:(DownLoadState)type{
    switch (type) {
        case NODownLoad:
        case FailDownLoad:
        {
            _perProgress.hidden = YES;
            _downLoadBtn.hidden = NO;
            
            [_downLoadBtn setTitle:@"下载" forState:UIControlStateNormal];
            [_downLoadBtn setTitle:@"下载" forState:UIControlStateHighlighted];
            [_downLoadBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
            [_downLoadBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
            _downLoadBtn.layer.borderColor = [UIColor whiteColor].CGColor;
            _downLoadBtn.userInteractionEnabled = YES;
        }
            break;
        case DownLoading:
        {
            _perProgress.hidden = NO;
            _downLoadBtn.hidden = YES;
            CGFloat progress = _filterModel.loadTotalSize?[_filterModel.loadReceivedSize doubleValue]/[_filterModel.loadTotalSize doubleValue]:0.0;
            _perProgress.progress = progress;
        }
            break;
        case FinishDownLoad:
        {
            _perProgress.hidden = YES;
            _downLoadBtn.hidden = NO;
            
            [_downLoadBtn setTitle:@"已下载" forState:UIControlStateNormal];
            [_downLoadBtn setTitle:@"已下载" forState:UIControlStateHighlighted];
            [_downLoadBtn setTitleColor:[[UIColor whiteColor] colorWithAlphaComponent:0.5] forState:UIControlStateNormal];
            [_downLoadBtn setTitleColor:[[UIColor whiteColor] colorWithAlphaComponent:0.5] forState:UIControlStateHighlighted];
            _downLoadBtn.layer.borderColor = [[UIColor whiteColor] colorWithAlphaComponent:0.5].CGColor;
            _downLoadBtn.userInteractionEnabled = NO;
        }
            break;
        default:
            break;
    }
}

#pragma mark -- action
-(void)downLoadAction:(id)sender{
    
    _perProgress.hidden = NO;
    _downLoadBtn.hidden = YES;
    _perProgress.progress = 0.0;
    _filterModel.loadState = DownLoading;
    
    if (downLoadBlock) {
        downLoadBlock();
    }
}

#pragma mark -- 回调函数
-(void)changeProgress{
    
    if (_filterModel.loadState == DownLoading) {
        if (_filterModel.loadTotalSize) {
            [self setUIWithState:DownLoading];
        }
    }
}

-(void)downLoadFinish{
    
    if (_filterModel.loadState == FinishDownLoad) {
        [self setUIWithState:FinishDownLoad];
    }
}

@end
