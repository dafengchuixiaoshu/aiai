//
//  ContactTableHeaderView.m
//  aiai
//
//  Created by zhangyang on 15/2/5.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "ContactTableHeaderView.h"
//#import "ContactsStatuModel.h"
#import "ContactHandler.h"
#import "BackgroundUpAddressBookHander.h"
#import "UIButton+FillColor.h"

@interface ContactTableHeaderView ()
@property(nonatomic,strong)UIImageView *addFriendImageView;
@property(nonatomic,strong)UILabel *addFriendLabel;
@property(nonatomic,strong)UILabel *msgCountLabel;

@property(nonatomic,strong)UIImageView *lurenImageView;
@property(nonatomic,strong)UILabel *lurenLabel;

@property(nonatomic,strong)UIImageView *groupImageView;
@property(nonatomic,strong)UILabel *grouLabel;

@end

@implementation ContactTableHeaderView

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        self.backgroundColor = EveryView_BackGroundColor;
        
        CGFloat top_bottom_padding = 13;
        
        CGFloat contrl_X = 13;
        CGFloat contrl_Y = top_bottom_padding;
        CGFloat contrl_W = 40;
        CGFloat contrl_H = contrl_W;
        _addFriendImageView = [[UIImageView alloc] initWithFrame:CGRectMake(contrl_X, contrl_Y, contrl_W, contrl_H)];
        _addFriendImageView.image = [UIImage imageNamed:ContactEntity_ImageName_AddFriend];
        [self addSubview:_addFriendImageView];
        
        contrl_X = _addFriendImageView.frame.origin.x+_addFriendImageView.frame.size.width + top_bottom_padding;
        contrl_Y = _addFriendImageView.frame.origin.y;
        contrl_W = 160;
        contrl_H = _addFriendImageView.frame.size.height;
        _addFriendLabel = [[UILabel alloc] initWithFrame:CGRectMake(contrl_X, contrl_Y, contrl_W, contrl_H)];
        _addFriendLabel.backgroundColor = [UIColor clearColor];
        _addFriendLabel.font = [UIFont systemFontOfSize:16];
        _addFriendLabel.textColor = [UIColor whiteColor];
        [self addSubview:_addFriendLabel];
        _addFriendLabel.text = @"添加好友";
        
        contrl_W = 20;
        contrl_H = 20;
        contrl_X = frame.size.width - top_bottom_padding - contrl_W;
        contrl_Y = _addFriendImageView.frame.origin.y + (_addFriendImageView.frame.size.height - contrl_H)/2;
        _msgCountLabel = [[UILabel alloc] initWithFrame:CGRectMake(contrl_X, contrl_Y, contrl_W, contrl_H)];
        _msgCountLabel.hidden = YES;
        _msgCountLabel.backgroundColor = [UIColor clearColor];
        _msgCountLabel.font = [UIFont boldSystemFontOfSize:16];
        _msgCountLabel.textColor = [UIColor whiteColor];
        _msgCountLabel.backgroundColor = [UIColor colorWithRed:253.0/255 green:149.0/255 blue:35.0/255 alpha:1.0];
        _msgCountLabel.layer.cornerRadius = _msgCountLabel.frame.size.width/2;
        _msgCountLabel.layer.masksToBounds = YES;
        _msgCountLabel.textAlignment = NSTextAlignmentCenter;
        [self addSubview:_msgCountLabel];
        _msgCountLabel.text = @"0";
        
        UIImageView* cutline1 = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        cutline1.frame = CGRectMake(top_bottom_padding, top_bottom_padding*2 + _addFriendImageView.frame.size.height, frame.size.width - top_bottom_padding*2, 1);
        cutline1.alpha = 0.5;
        [self addSubview:cutline1];
        
        //添加button
        UIButton* btn1 = [UIButton buttonWithType:UIButtonTypeCustom];
        btn1.tag = 1;
        btn1.frame = CGRectMake(0, 0, frame.size.width, frame.size.height/2);
        [btn1 setBackgroundColor:Every_YellowColor forState:UIControlStateHighlighted];
        [btn1 addTarget:self action:@selector(buttonClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:btn1];
        [self sendSubviewToBack:btn1];
        
        /*
        contrl_X = _addFriendImageView.frame.origin.x;
        contrl_Y = cutline1.frame.origin.y+cutline1.frame.size.height + top_bottom_padding;
        contrl_W = _addFriendImageView.frame.size.width;
        contrl_H = _addFriendImageView.frame.size.height;
        _lurenImageView = [[UIImageView alloc] initWithFrame:CGRectMake(contrl_X, contrl_Y, contrl_W, contrl_H)];
        _lurenImageView.image = [UIImage imageNamed:ContactEntity_ImageName_STRANGER];
        [self addSubview:_lurenImageView];
        
        contrl_X = _addFriendLabel.frame.origin.x;
        contrl_Y = _lurenImageView.frame.origin.y;
        contrl_W = _addFriendLabel.frame.size.width;
        contrl_H = _addFriendLabel.frame.size.height;
        _lurenLabel = [[UILabel alloc] initWithFrame:CGRectMake(contrl_X, contrl_Y, contrl_W, contrl_H)];
        _lurenLabel.backgroundColor = [UIColor clearColor];
        _lurenLabel.font = [UIFont systemFontOfSize:16];
        _lurenLabel.textColor = [UIColor whiteColor];
        [self addSubview:_lurenLabel];
        _lurenLabel.text = @"路人";
        
        UIImageView* cutline2 = [[UIImageView alloc] initWithImage:[UIImage imageNamed:COMMON_CUTLINE]];
        cutline2.frame = CGRectMake(0, _lurenImageView.frame.origin.y+_lurenImageView.frame.size.height + top_bottom_padding, frame.size.width, 1);
        [self addSubview:cutline2];
        
        //添加button
        UIButton* btn2 = [UIButton buttonWithType:UIButtonTypeCustom];
        btn2.tag = 2;
        btn2.frame = CGRectMake(0, btn1.frame.origin.y+btn1.frame.size.height, frame.size.width, frame.size.height/3);
        [btn2 addTarget:self action:@selector(buttonClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:btn2];
        */
        
        contrl_X = _addFriendImageView.frame.origin.x;
        contrl_Y = cutline1.frame.origin.y+cutline1.frame.size.height+top_bottom_padding;
        contrl_W = _addFriendImageView.frame.size.width;
        contrl_H = _addFriendImageView.frame.size.height;
        _groupImageView = [[UIImageView alloc] initWithFrame:CGRectMake(contrl_X, contrl_Y, contrl_W, contrl_H)];
        _groupImageView.image = [UIImage imageNamed:ContactEntity_ImageName_GROUP];
        [self addSubview:_groupImageView];
        
        contrl_X = _addFriendLabel.frame.origin.x;
        contrl_Y = _groupImageView.frame.origin.y;
        contrl_W = _addFriendLabel.frame.size.width;
        contrl_H = _addFriendLabel.frame.size.height;
        _grouLabel = [[UILabel alloc] initWithFrame:CGRectMake(contrl_X, contrl_Y, contrl_W, contrl_H)];
        _grouLabel.backgroundColor = [UIColor clearColor];
        _grouLabel.font = [UIFont systemFontOfSize:16];
        _grouLabel.textColor = [UIColor whiteColor];
        [self addSubview:_grouLabel];
        _grouLabel.text = @"群聊";
        
        //添加button
        UIButton* btn3 = [UIButton buttonWithType:UIButtonTypeCustom];
        btn3.tag = 3;
        [btn3 setBackgroundColor:Every_YellowColor forState:UIControlStateHighlighted];
        btn3.frame = CGRectMake(0, btn1.frame.origin.y+btn1.frame.size.height, frame.size.width, frame.size.height/2);
        [btn3 addTarget:self action:@selector(buttonClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:btn3];
        [self sendSubviewToBack:btn3];
    }

    return self;
}

- (void)buttonClicked:(id)sender {
    UIButton *button = (UIButton*)sender;
    
    
    
    if (button.tag == 1) {
        if (self.addContactBlock) {
            self.addContactBlock();
        }
    }else if (button.tag == 2) {
        if (self.strangerBlock) {
            self.strangerBlock();
        }
    }else if (button.tag == 3){
        if (self.groupBlock) {
            self.groupBlock();
        }
    }
}

-(void)setMsgCountLabelText:(NSString*)count{
    if (count && [count integerValue] > 0) {
        _msgCountLabel.text = count;
        _msgCountLabel.hidden = NO;
    }else{
        _msgCountLabel.hidden = YES;
    }
}

- (void)requestNewBuddyNotificationNumber {
//    LoginViewController* con = [LoginViewController sharedInstanceLoginViewController];
//    NSString* infoString = [[NSString alloc]initWithFormat:@"{\"methodName\":\"getMyIndexNum\",\"data\":{\"x-uid\":%@,\"f2cId\":%@}}",con.cmsid,con.f2cid];
//    NSString* baseInfoString = [AppUtils base64StringEncode:infoString];
//    
//#ifdef NEW_REST_SERVER_QUEUE
//    NSString* requestString = [[NSString alloc]initWithFormat:@"%@%@",REST_SERVER_CIRCLE_URL,baseInfoString];
//#else
//    NSString *requestString = [NSString stringWithFormat:@"%@%@",REST_SERVER_LETTER_URL,baseInfoString];
//#endif
//    
//    
//    [[NewRequestServers share] requestWithURL:requestString objclass:[self class] prepare:^{
//        
//    }successed:^(NSDictionary *dic) {
//        if (0 == [[[dic objectForKey:@"status"] objectForKey:@"code"] intValue]) {
//            if (0 < [[[dic objectForKey:@"returndata"] objectForKey:@"friendNum"] intValue]) {
//                newBadgeLabel.hidden = NO;
//                newBadgeLabel.text = [NSString stringWithFormat:@"%@", [[dic objectForKey:@"returndata"] objectForKey:@"friendNum"]];
//            }
//            else {
//                newBadgeLabel.hidden = YES;
//                newBadgeLabel.text = @"";
//            }
//        };
//    } failed:^(NSError *error) {
//        ;
//    }];
}

@end