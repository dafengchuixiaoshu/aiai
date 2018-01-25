//
//  TSAssetsViewController.m
//  TSAssetsPickerController
//
//  Created by Tomasz Szulc on 05.01.2014.
//  Copyright (c) 2014 Tomasz Szulc. All rights reserved.
//

#import "TSAssetsViewController.h"

#import "AssetCell.h"
#import "AssetSupplementaryCollectionReusableView.h"
#import "AlbumCell.h"
#import "AssetsCollectionView.h"
#import "SystemVersionMacros.h"
#import "TSAssetsLoader.h"
#import "TSAssetsManager.h"
#import "TSAssetsPickerController.h"
#import "TSAssetsPickerController+Internals.h"
#import "AssetsCollectionViewLayout.h"
#import "PDKTStickySectionHeadersCollectionViewLayout.h"


@interface TSAssetsViewController () <UICollectionViewDelegate, UICollectionViewDataSource> {
    TSAssetsManager *_assetsManager;
    NSIndexPath *_selectedIndexPath;
    NSString *_albumName;

    NSOperationQueue *_thumbnailQueue;
    
    UIActivityIndicatorView *_indicatorView;
    UIBarButtonItem *_selectBarButtonItem;
    
    NSMutableDictionary* _sectionDic;//用时间戳拆开的数据
    NSArray* _sortKeyArray;//时间戳排序之后的key数组
}

@end

@implementation TSAssetsViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self _setupAssetsManager];
    
    _thumbnailQueue = [NSOperationQueue new];
    _thumbnailQueue.maxConcurrentOperationCount = 3;
    
    [self _setupViews];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fetchAssets) name:UIApplicationDidBecomeActiveNotification object:nil];
}

-(void)_setSectionDic{
    //初始化数据源，需要时显示时间戳
    _sectionDic = [NSMutableDictionary dictionary];
    for (ALAsset *asset in _assetsManager.fetchedAssets) {
        NSString *timestr = [NSString stringWithFormat:@"%@",[asset valueForProperty:ALAssetPropertyDate]];
        NSRange rang;
        rang.location = 0;
        rang.length = 10;
        NSString *needSetTime = [timestr substringWithRange:rang];
        
        NSMutableArray* sameTimeArray = [_sectionDic objectForKey:needSetTime];
        if (sameTimeArray==nil) {
            sameTimeArray = [NSMutableArray array];
            [_sectionDic setObject:sameTimeArray forKey:needSetTime];
        }
        [sameTimeArray addObject:asset];
    }
    _sortKeyArray = [[_sectionDic allKeys] sortedArrayUsingComparator:^NSComparisonResult(id obj1, id obj2) {
        NSComparisonResult result = [obj1 compare:obj2];
//        return result == NSOrderedDescending; // 升序
        return result == NSOrderedAscending;  // 降序
    }];
}

- (void)_setupViews {
    _collectionView = [self newCollectionView];

    [self.view addSubview:_collectionView];
    
    _selectButton = nil;//[self newSelectButton];
    self.navigationItem.rightBarButtonItem = _selectButton;
}

- (void)_setupAssetsManager {
    TSFilter *filter = [_picker.dataSource filterOfAssetsPickerController:_picker];

    TSAssetsLoader *assetsLoader =
    [[TSAssetsLoader alloc] initWithLibrary:[ALAssetsLibrary new]
                                     filter:filter];
    assetsLoader.shouldReverseOrder = _picker.shouldReverseAssetsOrder;
    
    _assetsManager = [TSAssetsManager managerWithLoader:assetsLoader];
}

- (void)configureWithAlbumName:(NSString *)name {
    _albumName = name;
    self.navigationItem.title = name;
    [_assetsManager deselectAllAssets];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self _fetchAssets];
}


#pragma mark - Actions
- (void)onSelectPressed {
    [self.navigationItem setHidesBackButton:YES];
    [self.navigationItem.rightBarButtonItem setEnabled:NO];
    self.collectionView.userInteractionEnabled = NO;
    
    [self _addActivityIndicatorToNavigationBar];
    
    [_delegate assetsViewController:self didFinishPickingAssets:_assetsManager.selectedAssets];
}

- (void)_addActivityIndicatorToNavigationBar {
    if (!_indicatorView) {
        _indicatorView = [_picker activityIndicatorViewForPlace:AssetsView];
    }
    
    _selectBarButtonItem = self.navigationItem.rightBarButtonItem;
    UIBarButtonItem *itemIndicator = [[UIBarButtonItem alloc] initWithCustomView:_indicatorView];
    [self.navigationItem setRightBarButtonItem:itemIndicator];
    [_indicatorView startAnimating];
}

- (void)_removeActivityIndicatorFromNavigationBar {
    [_indicatorView stopAnimating];
    [self.navigationItem setRightBarButtonItem:_selectBarButtonItem];
}


#pragma mark - Fetch
- (void)_fetchAssets {
    [self _addActivityIndicatorToNavigationBar];
    [_assetsManager fetchAssetsWithAlbumName:_albumName block:^(NSUInteger numberOfAssets, NSError *error) {
        [self _removeActivityIndicatorFromNavigationBar];
        if (!error) {
            if (numberOfAssets > 0 || _picker.shouldShowEmptyAlbums){
                //set数据
                [self _setSectionDic];
                [_collectionView reloadData];
            } else {
                [self.navigationController popViewControllerAnimated:YES];
            }
        } else {
            [_delegate assetsViewController:self failedWithError:error];
        }
    }];
}


#pragma mark - Setup View
static NSString *cellIdentifier = nil;
- (UICollectionView *)newCollectionView {
    Class subclassOfAssetCellClass = [_picker subclassForClass:[AssetCell class]];
    cellIdentifier = NSStringFromClass(subclassOfAssetCellClass);
    
    Class supplementaryViewClass = [_picker subclassForClass:[AssetSupplementaryCollectionReusableView class]];
    NSString* SupplementaryIdentifier = NSStringFromClass(supplementaryViewClass);

    CGRect frame = self.view.bounds;
    
    Class collectionViewClass = [_picker subclassForClass:[AssetsCollectionView class]];
    
    UICollectionViewLayout *layout = [[PDKTStickySectionHeadersCollectionViewLayout alloc] init];

    UICollectionView *collectionView =
    [[collectionViewClass alloc] initWithFrame:frame
                          collectionViewLayout:layout];
    
    [collectionView setAutoresizingMask:(UIViewAutoresizingFlexibleWidth |
                                         UIViewAutoresizingFlexibleHeight)];

    [collectionView registerClass:subclassOfAssetCellClass
       forCellWithReuseIdentifier:cellIdentifier];
    
    [collectionView registerClass:supplementaryViewClass
       forSupplementaryViewOfKind:UICollectionElementKindSectionHeader
              withReuseIdentifier:SupplementaryIdentifier];

    [collectionView setDelegate:self];
    [collectionView setDataSource:self];
    
    return collectionView;
}

- (UIBarButtonItem *)newSelectButton {
    UIBarButtonItem *selectButton = [[UIBarButtonItem alloc] initWithTitle:_picker.selectButtonTitle style:UIBarButtonItemStyleDone target:self action:@selector(onSelectPressed)];
    [selectButton setEnabled:NO];
    return selectButton;
}


#pragma mark - UICollectionViewDataSource
- (UICollectionReusableView *)collectionView:(UICollectionView *)collectionView viewForSupplementaryElementOfKind:(NSString *)kind atIndexPath:(NSIndexPath *)indexPath{
    AssetSupplementaryCollectionReusableView *header = nil;
    if([kind isEqual:UICollectionElementKindSectionHeader]){
        header = [collectionView dequeueReusableSupplementaryViewOfKind:kind withReuseIdentifier:NSStringFromClass([AssetSupplementaryCollectionReusableView class]) forIndexPath:indexPath];
        
        NSString* title = [_sortKeyArray objectAtIndex:indexPath.section];
        [header setTitleText:title];
    }
    return header;
}

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return [_sectionDic count];
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    NSString* timeKey = [_sortKeyArray objectAtIndex:section];
    return [[_sectionDic objectForKey:timeKey] count];
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    id cell = [collectionView dequeueReusableCellWithReuseIdentifier:cellIdentifier forIndexPath:indexPath];
    if (!cell) {
        Class class = [_picker subclassForClass:[AlbumCell class]];
        cell = [class new];
    }
    NSString* timeKey = [_sortKeyArray objectAtIndex:indexPath.section];
    ALAsset *asset = [[_sectionDic objectForKey:timeKey] objectAtIndex:indexPath.row];
    
    [cell configure:asset];

    BOOL isSelected = [_assetsManager isAssetSelected:asset];
    [cell markAsSelected:isSelected];

    __weak TSAssetsViewController *weakSelf = self;
    NSBlockOperation *operation = [NSBlockOperation blockOperationWithBlock:^{
        CGImageRef thumbnailRef = [asset thumbnail];
        UIImage *thumbnail = [UIImage imageWithCGImage:thumbnailRef];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([weakSelf.collectionView.indexPathsForVisibleItems containsObject:indexPath]) {
                id aCell = [weakSelf.collectionView cellForItemAtIndexPath:indexPath];
                [[(AssetCell *)aCell thumbnailImageView] setImage:thumbnail];
                NSString *type = [asset valueForProperty:ALAssetPropertyType];
                [[(AssetCell *)aCell movieMarkImageView] setHidden:(![type isEqualToString:ALAssetTypeVideo])];
            }
        });
    }];
    
    [_thumbnailQueue addOperation:operation];

    return cell;
}


#pragma mark - UICollectionViewDelegate
/*
- (BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    BOOL shouldSelect = (_assetsManager.selectedAssets.count < [_picker.dataSource numberOfItemsToSelectInAssetsPickerController:_picker]);

    AssetCell *cell = (AssetCell *)[collectionView cellForItemAtIndexPath:indexPath];
    if (cell.isCellSelected) {
        shouldSelect = NO;
    }
    
    ALAsset *asset = _assetsManager.fetchedAssets[indexPath.row];
    if (shouldSelect) {
        [_assetsManager selectAsset:asset];
        _selectedIndexPath = indexPath;
    } else {
        [_assetsManager deselectAsset:asset];
    }
    
    [cell markAsSelected:shouldSelect];
    [_selectButton setEnabled:(_assetsManager.selectedAssets.count > 0)];
    
    return shouldSelect;
}
*/
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    
    NSString* timeKey = [_sortKeyArray objectAtIndex:indexPath.section];
    ALAsset *asset = [[_sectionDic objectForKey:timeKey] objectAtIndex:indexPath.row];
    NSNumber* videoTime = [asset valueForProperty:ALAssetPropertyDuration];
    
    //根据asset取出数据写入临时文件
    NSData* data = [self getALAssetData:asset];
    NSString *tmpDir =  NSTemporaryDirectory();
	NSString* savePath = [tmpDir stringByAppendingPathComponent:@"tmp.mp4"];
    [data writeToFile:savePath atomically:YES];
    
//    //把临时文件路径通过代理传入界面
    if (self.delegate && [self.delegate respondsToSelector:@selector(assetsViewController:didSelectItemFilePath:VideoTime:)]) {
        [self.delegate assetsViewController:self didSelectItemFilePath:savePath VideoTime:videoTime];
    }
}

#pragma mark - View Rotation
- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
    _collectionView.collectionViewLayout = [_picker assetsCollectionViewLayoutForOrientation:toInterfaceOrientation];
    [_collectionView reloadData];
}

#pragma mark - UICollectionViewDelegateFlowLayout
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath{
    return CGSizeMake(80, 80);
}
- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section{
    return 0.0;
}
- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section{
    return 0.0;
}
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout referenceSizeForHeaderInSection:(NSInteger)section{
    return CGSizeMake(self.collectionView.bounds.size.width, 20);
}

#pragma mark 通过asset获取视频数据
- (NSData *)getALAssetData:(ALAsset*)asset {
    ALAssetRepresentation *assetRep = [asset defaultRepresentation];
    
    NSUInteger size = (NSUInteger)[assetRep size];
    uint8_t *buff = malloc(size);
    
    NSError *err = nil;
    NSUInteger gotByteCount = [assetRep getBytes:buff fromOffset:0 length:size error:&err];
    
    if (gotByteCount) {
        if (err) {
            free(buff);
            return nil;
        }
    }
    
    return [NSData dataWithBytesNoCopy:buff length:size freeWhenDone:YES];
}

@end
