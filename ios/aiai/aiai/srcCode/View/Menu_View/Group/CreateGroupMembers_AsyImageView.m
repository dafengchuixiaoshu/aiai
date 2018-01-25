//
//  CreateGroupMembers_AsyImageView.m
//  F2CPlatform
//
//  Created by zhangyang on 14-8-14.
//  Copyright (c) 2014年 xiehaiduo. All rights reserved.
//

#import "CreateGroupMembers_AsyImageView.h"

@interface CreateGroupMembers_AsyImageView ()
@end

@implementation CreateGroupMembers_AsyImageView{
    tapBlock tapAction;
}

- (id)initWithAction:(tapBlock)block{
    self = [super init];
    if (self) {

        tapAction = block;
        
        _imageType = AsyImageViewTypeFilled;
        UITapGestureRecognizer* viewswipRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(CreateGroupMembersView_TapAction:)];
        viewswipRecognizer.delegate = (id)self;
        [self addGestureRecognizer:viewswipRecognizer];
    }
    return self;
}

-(void)CreateGroupMembersView_TapAction:(UIGestureRecognizer*)gestureRecognizer{
    tapAction(self.memberID);
}

@end
