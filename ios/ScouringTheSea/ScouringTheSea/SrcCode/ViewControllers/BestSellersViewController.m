//
//  BestSellersViewController.m
//  ScouringTheSea
//
//  Created by zhangyang on 15/11/19.
//  Copyright © 2015年 zhangyang. All rights reserved.
//

#import "BestSellersViewController.h"
#import "SDCycleScrollView.h"

#import "BestSellersModel.h"

@interface BestSellersViewController ()<SDCycleScrollViewDelegate,UICollectionViewDataSource,UICollectionViewDelegate,UICollectionViewDelegateFlowLayout>

@property(nonatomic,strong)UICollectionView* brandsCollectionView;
@property(nonatomic,strong)UICollectionView* goodsCollectionView;

@end

@implementation BestSellersViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    CGRect frame = CGRectMake(0, 0, self.view.frame.size.width, 200);
    NSArray *imageURLStringsGroup = [NSArray array];
    SDCycleScrollView *cycleScrollView = [SDCycleScrollView cycleScrollViewWithFrame:frame imageURLStringsGroup:imageURLStringsGroup];
    cycleScrollView.backgroundColor = [UIColor redColor];
    cycleScrollView.delegate = self; // 如需监听图片点击，请设置代理，实现代理方法
    cycleScrollView.placeholderImage = nil;//默认图
    [self.view addSubview:cycleScrollView];
    
    //创建品牌列表
    UICollectionViewFlowLayout* brandsLayout = [[UICollectionViewFlowLayout alloc] init];
    CGFloat brandCell_W = 80;
    CGFloat brandCell_H = 50;
    CGFloat brandToLeft = 10;
    CGFloat brandSpace = brandToLeft;
    brandsLayout.itemSize = CGSizeMake(brandCell_W, brandCell_H);
    brandsLayout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    brandsLayout.minimumLineSpacing = 16;
    brandsLayout.minimumInteritemSpacing = brandSpace;
    brandsLayout.sectionInset = UIEdgeInsetsMake(6, brandToLeft, 0, brandToLeft);//上 左 下 右
    
    frame.origin.y = frame.origin.y+frame.size.height;
    frame.size.height = brandCell_H*2+brandToLeft*3;
    _brandsCollectionView = [[UICollectionView alloc] initWithFrame:frame collectionViewLayout:brandsLayout];
    _brandsCollectionView.backgroundColor = [UIColor colorWithHexString:STS_BLACK_COLOR];
    _brandsCollectionView.delegate = self;
    _brandsCollectionView.dataSource = self;
    [_brandsCollectionView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:@"BrandsCell"];
    [self.view addSubview:_brandsCollectionView];
    
    //创建商品列表
    UICollectionViewFlowLayout* goodsLayout = [[UICollectionViewFlowLayout alloc] init];
    CGFloat goodsCell_W = 50;
    CGFloat goodsCell_H = 50;
    CGFloat goodsToLeft = 0;
    CGFloat space = 0;
    goodsLayout.itemSize = CGSizeMake(goodsCell_W, goodsCell_H);
    goodsLayout.minimumLineSpacing = 16;
    goodsLayout.minimumInteritemSpacing = space;
    goodsLayout.sectionInset = UIEdgeInsetsMake(6, goodsToLeft, 0, goodsToLeft);//上 左 下 右
    
    frame.origin.y = frame.origin.y+frame.size.height;
    frame.size.height = self.view.frame.size.height - frame.origin.y;
    _goodsCollectionView = [[UICollectionView alloc] initWithFrame:frame collectionViewLayout:goodsLayout];
    _goodsCollectionView.backgroundColor = [UIColor colorWithHexString:STS_YELLOW_COLOR];
    _goodsCollectionView.delegate = self;
    _goodsCollectionView.dataSource = self;
    [_goodsCollectionView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:@"GoodsCell"];
    [self.view addSubview:_goodsCollectionView];
    
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark SDCycleScrollViewDelegate
- (void)cycleScrollView:(SDCycleScrollView *)cycleScrollView didSelectItemAtIndex:(NSInteger)index{
    //轮播图点击
}

#pragma mark -- UICollectionViewDataSource
//定义展示的UICollectionViewCell的个数
-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    if (_brandsCollectionView == collectionView) {
        return 20;
    }
    return 100;//[_goodsData.goodsList count];
}

-(NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 1;
}

-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString * goodCellIdentifier = @"GoodsCell";
    static NSString * brandCellIdentifier = @"BrandsCell";
    UICollectionViewCell * cell = nil;
    
    if (_brandsCollectionView == collectionView) {
        cell = [collectionView dequeueReusableCellWithReuseIdentifier:brandCellIdentifier forIndexPath:indexPath];
    }else{
        cell = [collectionView dequeueReusableCellWithReuseIdentifier:goodCellIdentifier forIndexPath:indexPath];
    }
    
    cell.backgroundColor = [UIColor colorWithRed:((10 * indexPath.row) / 255.0) green:((20 * indexPath.row)/255.0) blue:((30 * indexPath.row)/255.0) alpha:1.0f];
    return cell;
}

#pragma mark --UICollectionViewDelegate
-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewCell * cell = (UICollectionViewCell *)[collectionView cellForItemAtIndexPath:indexPath];
    cell.backgroundColor = [UIColor whiteColor];
}

-(BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    return YES;
}

@end
