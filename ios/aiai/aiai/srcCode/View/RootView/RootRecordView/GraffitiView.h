//
//  MyView.h
//  UI-9-11-Tuyax


#import <UIKit/UIKit.h>

#define BOOMPADDING     42//80//
#define LEFTPADDING     23
#define ANIMATETIME     0.3

typedef NS_ENUM(NSInteger, FontWidth) {
    
    font_low = 1,
    font_normal = 3,
    font_high = 5,
    
};

@interface GraffitiView : UIView

@property(nonatomic, readonly, assign)FontWidth width;

-(void)setCurrentColorWith:(UIColor*)currentColor;

-(void)resetGraffitiView;
//撤销
-(void)revokeAction;
//改变字体粗细
-(void)strokeAction;

//判断涂鸦板是否被编辑了
-(BOOL)hasEdit;
@end
