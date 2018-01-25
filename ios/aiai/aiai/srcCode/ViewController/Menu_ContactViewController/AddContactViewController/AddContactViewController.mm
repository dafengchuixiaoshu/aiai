//
//  AddContactViewController.m
//  aiai
//
//  Created by zhangyang on 15/2/7.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "AddContactViewController.h"
#import "SearchResultViewController.h"
#import "AddNewFriendViewController.h"
#import "AddressBookViewController.h"

#import "ContactTableViewCell.h"

#import "UIButton+FillColor.h"

@interface AddContactViewController ()<UIScrollViewDelegate,UISearchBarDelegate>
@property(nonatomic, strong)UISearchBar* searchBar;

@property(nonatomic, strong)UITextField* searchField;
@property(nonatomic, strong)UILabel *countLabel;

@property(nonatomic, strong)UIScrollView *bgScroll;

@end

@implementation AddContactViewController
@synthesize bgScroll;

-(id)init{
    if (self = [super init]) {
        
    }
    return self;
}

- (void)searchBarInit {
    
    CGFloat origin_x = 13;
    CGFloat origin_y = 10;
    _searchBar = [[UISearchBar alloc] init];
    _searchBar.frame = CGRectMake(origin_x, origin_y, self.view.frame.size.width - origin_x*2, 44);
    UITextField *searchField = [_searchBar valueForKey:@"_searchField"];
    searchField.textColor = [UIColor whiteColor];
    searchField.backgroundColor = [UIColor clearColor];
    _searchBar.delegate = (id)self;
    _searchBar.placeholder = [NSString stringWithCString:"通过用户名/手机号添加好友" encoding: NSUTF8StringEncoding];
    _searchBar.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.05];
    _searchBar.backgroundImage = [AppUtils imageWithColor:[UIColor clearColor] rect:_searchBar.frame];
}

-(void)viewWillAppear:(BOOL)animated{
    
    if (_countLabel) {
        _countLabel.text = [NSString stringWithFormat:@"%d",[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetNewFansCount]];
        _countLabel.hidden = [_countLabel.text isEqualToString:@"0"]? YES:NO;
    }
    
}

-(void)viewDidLoad{
    [super viewDidLoad];
    self.centerBtnImageName = @"添加朋友";
    
    CGRect rect = self.view.frame;
    rect.origin.y = 64;
    rect.size.height -= rect.origin.y;
    bgScroll = [[UIScrollView alloc] initWithFrame:rect];
    bgScroll.backgroundColor = [UIColor clearColor];
    bgScroll.delegate = self;
    [self.view addSubview:bgScroll];
    
    CGFloat origin_x = 13;
    CGFloat space = 13;
    [self searchBarInit];
    
    [bgScroll addSubview:_searchBar];
    
    //新朋友
    CGFloat viewHeight = 68;
    CGFloat img_origin_y = 20;
    CGFloat img_wide = 40;
    UIView *newFriendView = [[UIView alloc] initWithFrame:CGRectMake(0, _searchBar.frame.origin.y+_searchBar.frame.size.height + space, [UIScreen mainScreen].bounds.size.width, viewHeight)];
    newFriendView.backgroundColor = [UIColor clearColor];
    [bgScroll addSubview:newFriendView];
    
    UIImageView *friendImg = [[UIImageView alloc] initWithImage:[UIImage imageNamed:ContactEntity_ImageName_AddFriend]];
    friendImg.frame = CGRectMake(origin_x, img_origin_y, img_wide, img_wide);
    [newFriendView addSubview:friendImg];
    
    UILabel *friendLabel = [[UILabel alloc] initWithFrame:CGRectMake(friendImg.frame.origin.x + friendImg.frame.size.width + space, 0, 200, newFriendView.frame.size.height)];
    friendLabel.text = @"新朋友";
    friendLabel.textColor = [UIColor whiteColor];
    friendLabel.font = [UIFont systemFontOfSize:16];
    [newFriendView addSubview:friendLabel];
    
    UIImageView *sub1View = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"系统导航右箭头.png"]];
    sub1View.frame = CGRectMake([UIScreen mainScreen].bounds.size.width - origin_x - sub1View.image.size.width, (newFriendView.frame.size.height-sub1View.image.size.height)/2,sub1View.image.size.width , sub1View.image.size.height);
    [newFriendView addSubview:sub1View];
    
    CGFloat contrl_W = 20;
    CGFloat contrl_H = 20;
    CGFloat contrl_X = sub1View.frame.origin.x - contrl_W - space;
    CGFloat contrl_Y = (newFriendView.frame.size.height-contrl_H)/2;
    _countLabel = [[UILabel alloc] initWithFrame:CGRectMake(contrl_X, contrl_Y, contrl_W, contrl_H)];
    _countLabel.font = [UIFont boldSystemFontOfSize:16];
    _countLabel.textColor = [UIColor whiteColor];
    _countLabel.backgroundColor = [UIColor colorWithRed:253.0/255 green:149.0/255 blue:35.0/255 alpha:1.0];
    _countLabel.layer.cornerRadius = _countLabel.frame.size.width/2;
    _countLabel.layer.masksToBounds = YES;
    _countLabel.textAlignment = NSTextAlignmentCenter;
    _countLabel.text = [NSString stringWithFormat:@"%d",[[IMPackageEngine sharedInstanceIMPackageEngine] IMPackage_GetNewFansCount]];
    _countLabel.hidden = [_countLabel.text isEqualToString:@"0"]? YES:NO;
    [newFriendView addSubview:_countLabel];
    
    UIImageView *line1View = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
    line1View.frame = CGRectMake(origin_x, newFriendView.frame.size.height - 1,newFriendView.frame.size.width - origin_x*2 , 1);
    line1View.alpha = 0.5;
    [newFriendView addSubview:line1View];
    
    UIButton* btn1 = [UIButton buttonWithType:UIButtonTypeCustom];
    btn1.frame = CGRectMake(0, 0, newFriendView.frame.size.width, newFriendView.frame.size.height);
    [btn1 setBackgroundColor:Every_YellowColor forState:UIControlStateHighlighted];
    [btn1 addTarget:self action:@selector(gotoFriend) forControlEvents:UIControlEventTouchUpInside];
    [newFriendView addSubview:btn1];
    [newFriendView sendSubviewToBack:btn1];
    
    //通讯录
    UIView *addressBookFriendView = [[UIView alloc] initWithFrame:CGRectMake(0, newFriendView.frame.origin.y+newFriendView.frame.size.height, [UIScreen mainScreen].bounds.size.width, viewHeight)];
    addressBookFriendView.backgroundColor = [UIColor clearColor];
    bgScroll.contentSize = CGSizeMake([UIScreen mainScreen].bounds.size.width, bgScroll.frame.size.height+1);
    [bgScroll addSubview:addressBookFriendView];
    
    UIImageView *addressBookImg = [[UIImageView alloc] initWithImage:[UIImage imageNamed:ContactEntity_ImageName_AddAddressBook]];
    addressBookImg.frame = CGRectMake(origin_x, img_origin_y, img_wide, img_wide);
    [addressBookFriendView addSubview:addressBookImg];
    
    UILabel *addressBookLabel = [[UILabel alloc] initWithFrame:CGRectMake(friendImg.frame.origin.x + friendImg.frame.size.width + space, 0, 200, newFriendView.frame.size.height)];
    addressBookLabel.text = @"通讯录";
    addressBookLabel.textColor = [UIColor whiteColor];
    addressBookLabel.font = [UIFont systemFontOfSize:16];
    [addressBookFriendView addSubview:addressBookLabel];
    
    UIImageView *sub2View = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"系统导航右箭头.png"]];
    sub2View.frame = CGRectMake([UIScreen mainScreen].bounds.size.width - origin_x - sub2View.image.size.width, (addressBookLabel.frame.size.height-sub2View.image.size.height)/2,sub2View.image.size.width , sub2View.image.size.height);
    [addressBookFriendView addSubview:sub2View];
    
    UIImageView *line2View = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
    line2View.frame = CGRectMake(origin_x, addressBookFriendView.frame.size.height - 1,addressBookFriendView.frame.size.width - origin_x*2, 1);
    line2View.alpha = 0.5;
    [addressBookFriendView addSubview:line2View];
    
    UIButton* btn2 = [UIButton buttonWithType:UIButtonTypeCustom];
    [btn2 setBackgroundColor:Every_YellowColor forState:UIControlStateHighlighted];
    btn2.frame = CGRectMake(0, 0, addressBookFriendView.frame.size.width, addressBookFriendView.frame.size.height);
    [btn2 addTarget:self action:@selector(gotoAddressBook) forControlEvents:UIControlEventTouchUpInside];
    [addressBookFriendView addSubview:btn2];
    [addressBookFriendView sendSubviewToBack:btn2];
}

#pragma mark -- action
-(void)gotoFriend{
    //新的好友
    AddNewFriendViewController* con = [[AddNewFriendViewController alloc] init];
    [self.navigationController pushViewController:con animated:YES];
}

-(void)gotoAddressBook{
    //通讯录
    AddressBookViewController *con = [[AddressBookViewController alloc] init];
    [self.navigationController pushViewController:con animated:YES];
}

#pragma mark UISearchBarDelegate
- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar{
    SearchResultViewController* con = [[SearchResultViewController alloc] initWithSearchText:searchBar.text];
    [self.navigationController pushViewController:con animated:YES];
}

#pragma mark UIScrollerViewDelegate
- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView{
    
    [_searchBar resignFirstResponder];
    
}
@end
