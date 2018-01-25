//
//  AddMemberView.m
//  F2CPlatform
//
//  Created by Circle Liu on 14-8-12.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "AddMemberView.h"
#import "AsyImageView.h"

#import "IMPackageData.h"

#import "sys/utsname.h"

#define DurationTime 0.3
#define AddMemberView_FaceViewWide          60
#define AddMemberView_AvatarViewHeigh       80
#define AddMemberView_AvatarViewWide        60
#define AddMemberView_ORIGIN_X              20
#define AddMemberView_TPSPACE               23

@interface AddMemberView () {
    NSString *owener;
    
    NSMutableArray   *membersArray;
    
    dispatch_block_t addBlock;
    dispatch_block_t cutDownBlock;
    AvatarAction     avatarBlock;
    
    UIButton         *addButton;
    UIButton         *cutButton;
    UIImageView      *lineImageView;
}

@end

@implementation AddMemberView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

- (id)initWithOwenerID:(NSString*)owenerID members:(NSArray*)members addAction:(dispatch_block_t)addAction avatarAction:(AvatarAction)avatarAction cutDownAction:(dispatch_block_t)cutDownAction{
    if (self = [super init]) {
        
        owener = owenerID;
        
        membersArray = [[NSMutableArray alloc] initWithArray:members];
        
        addBlock = addAction;
        cutDownBlock = cutDownAction;
        
        avatarBlock = avatarAction;
        
        [self createElement];
    }
    return self;
}

- (void)createElement {
    for (int i=0; i<[membersArray count]+1; i++) {
        
        //60是两边的间距
        NSInteger space = ([UIApplication sharedApplication].keyWindow.frame.size.width - 40 - AddMemberView_FaceViewWide*4) / 3;
        
        if (i == [membersArray count]) {
            
            UIImage *btnImg = [UIImage imageNamed:@"添加好友.png"];
            addButton = [[UIButton alloc] initWithFrame:CGRectMake(AddMemberView_ORIGIN_X+(space+AddMemberView_AvatarViewWide) * (i % 4), AddMemberView_ORIGIN_X+(AddMemberView_AvatarViewHeigh+AddMemberView_TPSPACE) * (i / 4), btnImg.size.width, btnImg.size.height)];
            [addButton setImage:btnImg forState:UIControlStateNormal];
            [self addSubview:addButton];
            [addButton addTarget:self action:@selector(addButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
            
            //需求变更 去掉踢人功能
//            if ([[IMPackageEngine sharedInstanceIMPackageEngine].cmsId isEqualToString:owener]) {
//                cutButton = [[UIButton alloc] initWithFrame:CGRectMake(20 + 77 * ((i+1) % 4), 20 + 100 * ((i+1) / 4), 50, 50)];
//                [cutButton setImage:[UIImage imageNamed:@"contact_cut"] forState:UIControlStateNormal];
//                [self addSubview:cutButton];
//                [cutButton addTarget:self action:@selector(cutDownButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
//
//            }

        }
        else {
            IMPackageGroupMemberData *data = [membersArray objectAtIndex:i];
            
            CGRect rect = CGRectMake(AddMemberView_ORIGIN_X+(space+AddMemberView_AvatarViewWide) * (i % 4), AddMemberView_ORIGIN_X+(AddMemberView_AvatarViewHeigh+AddMemberView_TPSPACE) * (i / 4), AddMemberView_AvatarViewWide, AddMemberView_AvatarViewHeigh);
            AddMemberAvatarView *avatar = [[AddMemberAvatarView alloc] initWithFrame:rect WithCutDownAction:^(AddMemberAvatarView *moveView) {

                [self reloadViewWithDeleteView:moveView];
                if (cutDownBlock) {
                    cutDownBlock();
                }
            }];
            
            avatar.tag = i;
            [self addSubview:avatar];
            [avatar fillDataWithURL:data.portraitKey nameStr:[data.GroupCardName length]>0?data.GroupCardName:data.memberNickName userName:data.memberUserName];
            [avatar addTarget:self action:@selector(avatarButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
        }
    }
    
    // 因为最后要加一个添加的按钮，所以([membersArray count] + 1)。一排是4个，所以/4。小于4的数/4是0，但是最少要有1排，所以最后+1
    int lineNum = 0;
    int btnCount = 1;
    if ([[IMPackageEngine sharedInstanceIMPackageEngine].userName isEqualToString:owener]){
//        btnCount += 1; //如果是群主 还会有一个删除按钮      //需求变更 去掉踢人功能
    }
    if (0 == (([membersArray count] + btnCount) % 4)) {
        lineNum += (([membersArray count] + btnCount) / 4);
    }
    else {
        lineNum += (([membersArray count] + btnCount) / 4) + 1;
    }
    //15是间距  20是里上面的距离
    self.frame = CGRectMake(0, 0, [UIApplication sharedApplication].keyWindow.frame.size.width, AddMemberView_ORIGIN_X + (AddMemberView_AvatarViewHeigh+AddMemberView_TPSPACE) * lineNum + 5);
    self.backgroundColor = [UIColor clearColor];
    
    lineImageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, self.frame.size.height - 1, [UIApplication sharedApplication].keyWindow.frame.size.width, 1)];
    lineImageView.image = [UIImage imageNamed:COMMON_CUTLINE];
    lineImageView.tag = -1;     //区分上面AddMemberAvatarView的tag
    [self addSubview:lineImageView];
}

-(void)reloadViewWithDeleteView:(AddMemberAvatarView*)moveView{
    
    [moveView removeFromSuperview];
    [membersArray removeObjectAtIndex:moveView.tag];
    //60是两边的间距
    NSInteger space = ([UIApplication sharedApplication].keyWindow.frame.size.width - 40 - AddMemberView_FaceViewWide*4) / 3;
    
    for (int i = 0; i<[self.subviews count]; i++) {
        UIView* tempMoveView = [self.subviews objectAtIndex:i];
        if ([tempMoveView isKindOfClass:[AddMemberAvatarView class]]) {
            if (i >= moveView.tag) {
                [UIView animateWithDuration:DurationTime animations:^{
                    CGRect rect = CGRectMake(AddMemberView_ORIGIN_X+(space+AddMemberView_AvatarViewWide) * (i % 4), AddMemberView_ORIGIN_X+(AddMemberView_AvatarViewHeigh+AddMemberView_TPSPACE) * (i / 4), AddMemberView_AvatarViewWide, AddMemberView_AvatarViewHeigh);
                    tempMoveView.frame = rect;
                    tempMoveView.tag = i;
                }];
            }
        }
    }
    
    [UIView animateWithDuration:DurationTime animations:^{
        
        NSInteger totalCount = [membersArray count];
        
        addButton.frame = CGRectMake(AddMemberView_ORIGIN_X+(space+AddMemberView_AvatarViewWide) * (totalCount % 4), AddMemberView_ORIGIN_X+(AddMemberView_AvatarViewHeigh+AddMemberView_TPSPACE) * (totalCount / 4), addButton.imageView.image.size.width, addButton.imageView.image.size.height);
        cutButton.frame = CGRectMake(AddMemberView_ORIGIN_X+(space+AddMemberView_AvatarViewWide) * ((totalCount+1) % 4), AddMemberView_ORIGIN_X+(AddMemberView_AvatarViewHeigh+AddMemberView_TPSPACE) * ((totalCount+1) / 4), addButton.imageView.image.size.width, addButton.imageView.image.size.height);;
    }];
    
    // 因为最后要加一个添加的按钮，所以([membersArray count] + 1)。一排是4个，所以/4。小于4的数/4是0，但是最少要有1排，所以最后+1
    int lineNum = 0;
    int btnCount = 1;
    if ([[IMPackageEngine sharedInstanceIMPackageEngine].userName isEqualToString:owener]){
        btnCount += 1; //如果是群主 还会有一个删除按钮
    }
    if (0 == (([membersArray count] + btnCount) % 4)) {
        lineNum += (([membersArray count] + btnCount) / 4);
    }
    else {
        lineNum += (([membersArray count] + btnCount) / 4) + 1;
    }
    self.frame = CGRectMake(0, 0, [UIApplication sharedApplication].keyWindow.frame.size.width, AddMemberView_ORIGIN_X + (AddMemberView_AvatarViewHeigh+AddMemberView_TPSPACE) * lineNum + 5);
    
    lineImageView.frame = CGRectMake(0, self.frame.size.height - 1, [UIApplication sharedApplication].keyWindow.frame.size.width, 1);
}

- (void)refreshElement:(NSArray*)members {
    NSArray *array = [self subviews];
    for (UIView *view in array) {
        [view removeFromSuperview];
    }
    
    [membersArray removeAllObjects];
    [membersArray addObjectsFromArray:members];
    
    [self createElement];
}

//修改群名片时的回调
-(void)resetGroupNickName:(NSString*)nickName{
    for (int i = 0 ; i<[membersArray count];i++) {
        IMPackageGroupMemberData* data = [membersArray objectAtIndex:i];
        if ([data.memberUserName isEqualToString:[IMPackageEngine sharedInstanceIMPackageEngine].userName]) {
            AddMemberAvatarView* avatarView = [self.subviews objectAtIndex:i];
            [avatarView resetNickName:nickName];
        }
    }
}

- (void)addButtonClicked:(id)sender {
    addBlock();
}

-(void)cutDownButtonClicked:(id)sender{

    for (AddMemberAvatarView *avatar in self.subviews) {
        if ([avatar isKindOfClass:[AddMemberAvatarView class]]) {
            avatar.cutDownBtn.hidden = NO;
        }
    }
}

- (void)avatarButtonClicked:(id)sender {
    AddMemberAvatarView *avatar = (AddMemberAvatarView*)sender;
    IMPackageGroupMemberData *data = [membersArray objectAtIndex:avatar.tag];
    
    NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
    [dic setObject:data.memberNickName forKey:@"nickname"];
    [dic setObject:data.portraitKey?[AppUtils getImageNewUrlWithSrcImageUrl:data.portraitKey width:0 height:0]:@"" forKey:@"faceUrl"];
    [dic setObject:data.memberCMSID forKey:@"id"];
    [dic setObject:data.memberUserName forKey:@"userName"];
    
    avatarBlock(dic);
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
    for (AddMemberAvatarView *avatar in self.subviews) {
        if ([avatar isKindOfClass:[AddMemberAvatarView class]]) {
            avatar.cutDownBtn.hidden = YES;
        }
    }
}

@end

@interface AddMemberAvatarView () {
    AsyImageView *avatarImageView;
    UILabel      *nameLabel;
    
    CutDownAction cutBlock;
}

@end

@implementation AddMemberAvatarView
@synthesize cutDownBtn;

- (id)initWithFrame:(CGRect)frame WithCutDownAction:(CutDownAction)cutDownBlock{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        
        avatarImageView = [[AsyImageView alloc] init];
        avatarImageView.frame = CGRectMake(0, 0, AddMemberView_FaceViewWide, AddMemberView_FaceViewWide);
//        avatarImageView.layer.cornerRadius = avatarImageView.frame.size.width/2;
//        avatarImageView.layer.masksToBounds = YES;
        [self addSubview:avatarImageView];
        avatarImageView.userInteractionEnabled = NO;
        
        UIImage *maskImg = [UIImage imageNamed:@"avatar_mask.png"];
        UIImageView *imageView = [[UIImageView alloc] initWithImage:maskImg];
        imageView.frame = CGRectMake(0, 0, maskImg.size.width, maskImg.size.height);
        [self addSubview:imageView];
        
        cutDownBtn = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 28, 28)];
        cutDownBtn.center = CGPointMake(70, 20);
        [cutDownBtn setImage:[UIImage imageNamed:@"我的活动编辑.png"] forState:UIControlStateNormal];
        [cutDownBtn addTarget:self action:@selector(cutDownAction:) forControlEvents:UIControlEventTouchUpInside];
        cutDownBtn.hidden = YES;
        [self addSubview:cutDownBtn];
        
        nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, avatarImageView.frame.origin.y+avatarImageView.frame.size.height+7, AddMemberView_AvatarViewWide, 13)];
        nameLabel.font = [UIFont systemFontOfSize:14];
        nameLabel.backgroundColor = [UIColor clearColor];
        nameLabel.textColor = [UIColor colorWithRed:153.0/255 green:153.0/255 blue:153.0/255 alpha:1.0];
        nameLabel.textAlignment = NSTextAlignmentCenter;
        [self addSubview:nameLabel];
        nameLabel.userInteractionEnabled = NO;
        
        cutBlock = cutDownBlock;
    }
    return self;
}

- (void)fillDataWithURL:(NSString*)avatarURL nameStr:(NSString*)nameStr userName:(NSString*)userName{
    [avatarImageView loadImageWithPath:[AppUtils getImageNewUrlWithSrcImageUrl:avatarURL width:50 height:50] andPlaceHolderName:DefalutHeadImage];
    
    nameLabel.text = nameStr;
    
    if ([[IMPackageEngine sharedInstanceIMPackageEngine].userName isEqualToString:userName]) {
        [cutDownBtn removeFromSuperview];
    }
}

-(void)resetNickName:(NSString *)nickName{
    if (nickName) {
        nameLabel.text = nickName;
    }
}

-(void)cutDownAction:(id)sender{
    if (cutBlock) {
        cutBlock(self);
    }
}

@end
