//
//  GroupSettingCell.m
//  aiai
//
//  Created by wangnan on 15/4/16.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "GroupSettingCell.h"

@interface GroupSettingCell ()<UITextFieldDelegate>

@property(nonatomic, strong)UILabel *titleLabel;
@property(nonatomic, strong)UITextField *textField;

@property(nonatomic, strong)UISwitch *msgSwitch;

@end

@implementation GroupSettingCell
@synthesize msgActionBlock;
@synthesize changeNameBlock;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        //title
        CGFloat titleLabel_X = 13;
        CGFloat titleLabel_Y = 0;
        CGFloat titleLabel_W = 100;
        CGFloat titleLabel_H = GROUPSETTINGCELLHEIGHT;
        self.titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(titleLabel_X, titleLabel_Y, titleLabel_W, titleLabel_H)];
        self.titleLabel.textAlignment = NSTextAlignmentLeft;
        self.titleLabel.backgroundColor = [UIColor clearColor];
        self.titleLabel.font = [UIFont systemFontOfSize:16];
        self.titleLabel.textColor = [UIColor whiteColor];
        [self addSubview:self.titleLabel];
        
        //群组名
        CGFloat field_W = 200;
        CGFloat field_H = GROUPSETTINGCELLHEIGHT;
        CGFloat field_X = [UIScreen mainScreen].bounds.size.width - field_W - titleLabel_X;
        CGFloat field_Y = 0;
        self.textField = [[UITextField alloc] initWithFrame:CGRectMake(field_X, field_Y, field_W, field_H)];
        self.textField.textAlignment = NSTextAlignmentRight;
        self.textField.backgroundColor = [UIColor clearColor];
        self.textField.font = [UIFont systemFontOfSize:16];
        self.textField.textColor = [UIColor whiteColor];
        self.textField.delegate = self;
        self.textField.returnKeyType = UIReturnKeyDone;
        [self addSubview:self.textField];
        
        self.msgSwitch = [[UISwitch alloc] init];
        self.msgSwitch.frame = CGRectMake([UIScreen mainScreen].bounds.size.width-titleLabel_X-self.msgSwitch.frame.size.width, (GROUPSETTINGCELLHEIGHT-self.msgSwitch.frame.size.height)/2, self.msgSwitch.frame.size.width, self.msgSwitch.frame.size.height);
        self.msgSwitch.hidden = YES;
        self.msgSwitch.onTintColor = Every_YellowColor;
        [self.msgSwitch addTarget:self action:@selector(newMsgAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:self.msgSwitch];
        
        self.selectionStyle = UITableViewCellSelectionStyleNone;
        self.backgroundColor = EveryView_BackGroundColor;
    }
    
    return self;
}

#pragma mark setCell
-(void)setTitleText:(NSString*)title subText:(NSString*)subTitle needSwitch:(BOOL)need andSwitchOn:(BOOL)on{
    
    self.titleLabel.text = title? :@"";
    self.textField.text = subTitle? :@"";
    
    if (need) {
        self.msgSwitch.hidden = NO;
        self.msgSwitch.on = on;
    }else{
        self.msgSwitch.hidden = YES;
    }
}

-(void)newMsgAction:(id)sender{
    
    if (msgActionBlock) {
        msgActionBlock(self.msgSwitch.on);
    }
}

#pragma mark ---- textFieldDelegate
- (BOOL)textFieldShouldReturn:(UITextField *)textField{
    
    [textField resignFirstResponder];
    if (changeNameBlock) {
        changeNameBlock(textField.text);
    }
    
    return YES;
}

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
