//
//  CreateGroupMembersView.m
//  F2CPlatform
//
//  Created by Circle Liu on 14-7-20.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "CreateGroupMembersView.h"

#import "CreateGroupMembers_AsyImageView.h"

#define CreateGroupMembersView_LeftToBounds     15
#define CreateGroupMembersView_FaceViewWide     40
#define CreateGroupMembersView_FaceViewSPACE    6

@interface CreateGroupMembersView () {
    UIScrollView *_containerView;
    UIButton     *_comfirmButton;
    UILabel      *_msgCountLabel;
    
    dispatch_block_t comfirm;
    deleteBlock deleteAction;
}

@end

@implementation CreateGroupMembersView

- (id)initWithFrame:(CGRect)frame comfirm:(dispatch_block_t)block deleteAction:(deleteBlock)delBlock
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        
        
//        UIImageView *lineImageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, 1)];
//        lineImageView.image = [UIImage imageNamed:@"我的订单分割线"];
//        [self addSubview:lineImageView];
        
        //左右间距 + btn的宽
        CGFloat btnHeight = 33;
        CGFloat btnWidth = 70;
        _comfirmButton = [[UIButton alloc] initWithFrame:CGRectMake([UIScreen mainScreen].bounds.size.width-CreateGroupMembersView_LeftToBounds-btnWidth, (frame.size.height - btnHeight)/2, btnWidth, btnHeight)];
        [_comfirmButton setTitle:@"确定" forState:UIControlStateNormal];
        [_comfirmButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [_comfirmButton setBackgroundImage:[UIImage imageNamed:@"register_btn_able.png"] forState:UIControlStateNormal];
        [_comfirmButton addTarget:self action:@selector(comfirmButtonClicked) forControlEvents:UIControlEventTouchUpInside];
        _comfirmButton.layer.borderColor = [UIColor whiteColor].CGColor;
        _comfirmButton.layer.borderWidth = 1.0;
        _comfirmButton.layer.cornerRadius = btnHeight/2;
        _comfirmButton.layer.masksToBounds = YES;
        _comfirmButton.titleLabel.font = [UIFont systemFontOfSize:15];
        [self addSubview:_comfirmButton];
        
        CGFloat msgLabelHeight = 20;
        CGFloat msgLabelWide = 20;
        _msgCountLabel = [[UILabel alloc] initWithFrame:CGRectMake(_comfirmButton.frame.origin.x + _comfirmButton.frame.size.width - msgLabelHeight*2/3, _comfirmButton.frame.origin.y - msgLabelHeight/3, msgLabelWide, msgLabelHeight)];
        _msgCountLabel.text = @"0";
        _msgCountLabel.backgroundColor = [UIColor clearColor];
        _msgCountLabel.font = [UIFont boldSystemFontOfSize:14];
        _msgCountLabel.textColor = [UIColor whiteColor];
        _msgCountLabel.backgroundColor = Every_YellowColor;
        _msgCountLabel.layer.cornerRadius = _msgCountLabel.frame.size.width/2;
        _msgCountLabel.layer.masksToBounds = YES;
        _msgCountLabel.textAlignment = NSTextAlignmentCenter;
        _msgCountLabel.hidden = YES;
        [self addSubview:_msgCountLabel];
        
        comfirm = block;
        deleteAction = delBlock;
        
        _containerView = [[UIScrollView alloc] initWithFrame:CGRectMake(CreateGroupMembersView_LeftToBounds, 0, [UIScreen mainScreen].bounds.size.width-_comfirmButton.frame.size.width - CreateGroupMembersView_LeftToBounds*2, frame.size.height)];
        _containerView.backgroundColor = [UIColor clearColor];
        _containerView.showsHorizontalScrollIndicator = NO;
        _containerView.showsVerticalScrollIndicator = NO;
        [self addSubview:_containerView];
        
        self.backgroundColor = EveryView_BackGroundColor;
    }
    return self;
}

- (void)insertPerson:(NSString*)memberName userURL:(NSString*)userURL {
    NSArray *array = [_containerView subviews];
    
    CreateGroupMembers_AsyImageView *avatar = [[CreateGroupMembers_AsyImageView alloc] initWithAction:^(NSString *memberID) {
        [self deletePerson:memberID];
        //通知界面删除
        deleteAction(memberID);
    }];
    avatar.memberID = memberName;
    
    avatar.frame = CGRectMake(0, (self.frame.size.height-CreateGroupMembersView_FaceViewWide)/2, CreateGroupMembersView_FaceViewWide, CreateGroupMembersView_FaceViewWide);
    avatar.layer.cornerRadius = avatar.frame.size.width/2;
    avatar.layer.masksToBounds = YES;
    [avatar loadImageWithPath:userURL andPlaceHolderName:nil];
    [_containerView insertSubview:avatar atIndex:0];

    for (NSInteger i=[array count]-1; i>=0; i--) {
        CreateGroupMembers_AsyImageView *avatar = [array objectAtIndex:i];
        CGRect rect = avatar.frame;
        avatar.frame = CGRectMake(rect.origin.x + CreateGroupMembersView_FaceViewWide + CreateGroupMembersView_FaceViewSPACE, rect.origin.y, rect.size.width, rect.size.height);
    }
    
    CGSize size = _containerView.contentSize;
    _containerView.contentSize = CGSizeMake([array count] * (CreateGroupMembersView_FaceViewWide + CreateGroupMembersView_FaceViewSPACE), size.height);
    
    _msgCountLabel.text = [NSString stringWithFormat:@"%zi", [array count] + 1];
    _msgCountLabel.hidden = NO;
}

- (void)deletePerson:(NSString*)memberName {
    NSArray *array = [_containerView subviews];
    
    BOOL move = NO;
    
    for (int i=0; i<[array count]; i++) {
        CreateGroupMembers_AsyImageView *avatar = [array objectAtIndex:i];
        if (memberName == avatar.memberID) {
            [avatar removeFromSuperview];
            move = YES;
        }
        else if (move) {
            CGRect rect = avatar.frame;
            avatar.frame = CGRectMake(rect.origin.x - (CreateGroupMembersView_FaceViewWide + CreateGroupMembersView_FaceViewSPACE), rect.origin.y, rect.size.width, rect.size.height);
        }
    }
    
    CGSize size = _containerView.contentSize;
    _containerView.contentSize = CGSizeMake(([array count]-1) * (CreateGroupMembersView_FaceViewWide + CreateGroupMembersView_FaceViewSPACE), size.height);
    
    _msgCountLabel.text = [NSString stringWithFormat:@"%zi", [array count] - 1];
    if ([array count] - 1 == 0) {
        _msgCountLabel.hidden = YES;
    }
}

- (void)comfirmButtonClicked {
    [self createActionAble:NO];
    comfirm();
}

- (void)createActionAble:(BOOL)able {
    _comfirmButton.enabled = able;
}

@end
