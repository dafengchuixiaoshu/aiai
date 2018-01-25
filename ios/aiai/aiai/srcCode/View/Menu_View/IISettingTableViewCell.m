//
//  IISettingTableViewCell.m
//  aiai
//
//  Created by wangnan on 15/4/30.
//  Copyright (c) 2015年 zhangyang. All rights reserved.
//

#import "IISettingTableViewCell.h"

@interface IISettingTableViewCell ()

@property(nonatomic, strong)UILabel *titleLabel;

@property(nonatomic, strong)UISwitch *cSwitch;

@end

@implementation IISettingTableViewCell
@synthesize actionBlock;

-(id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier{
    
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        
        //title
        CGFloat titleLabel_X = 13;
        CGFloat titleLabel_Y = 0;
        CGFloat titleLabel_W = 160;
        CGFloat titleLabel_H = IISETTINGCELLHEIGHT;
        self.titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(titleLabel_X, titleLabel_Y, titleLabel_W, titleLabel_H)];
        self.titleLabel.textAlignment = NSTextAlignmentLeft;
        self.titleLabel.backgroundColor = [UIColor clearColor];
        self.titleLabel.font = [UIFont systemFontOfSize:16];
        self.titleLabel.textColor = [UIColor whiteColor];
        [self addSubview:self.titleLabel];
        
        self.cSwitch = [[UISwitch alloc] init];
        self.cSwitch.frame = CGRectMake([UIScreen mainScreen].bounds.size.width-titleLabel_X-self.cSwitch.frame.size.width, (IISETTINGCELLHEIGHT-self.cSwitch.frame.size.height)/2, self.cSwitch.frame.size.width, self.cSwitch.frame.size.height);
        self.cSwitch.hidden = YES;
        self.cSwitch.onTintColor = Every_YellowColor;
        [self.cSwitch addTarget:self action:@selector(switchAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:self.cSwitch];
        
        self.selectionStyle = UITableViewCellSelectionStyleNone;
        self.backgroundColor = EveryView_BackGroundColor;
    }
    
    return self;
}

-(void)setTitleText:(NSString*)title subText:(NSString*)subTitle needSwitch:(BOOL)need andSwitchOn:(BOOL)on{
    self.titleLabel.text = title? :@"";
    
    if (need) {
        self.cSwitch.hidden = NO;
        self.cSwitch.on = on;
    }else{
        self.cSwitch.hidden = YES;
    }
}

-(void)switchAction:(id)sender{
    
    if (actionBlock) {
        actionBlock(self.cSwitch.on);
    }
}

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
