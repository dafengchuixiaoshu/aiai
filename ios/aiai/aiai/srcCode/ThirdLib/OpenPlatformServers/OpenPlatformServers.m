//
//  OpenPlatformServers.m
//  Test
//
//  Created by Circle Liu on 14-5-23.
//  Copyright (c) 2014年 Circle Liu. All rights reserved.
//

#import "OpenPlatformServers.h"

static NSString *sinaWeiboToken = @"";

@interface OpenPlatformServers () {
    TencentOAuth *oauth;
}

@end

@implementation OpenPlatformServers

#pragma maek Public Method

+ (OpenPlatformServers*)share {
    static OpenPlatformServers *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (id)init {
    if (self = [super init]) {
        oauth = [[TencentOAuth alloc] initWithAppId:kQQAppKey andDelegate:nil];
    }
    return self;
}

- (void)registerServers {
    
    [WeiboSDK registerApp:kSinaWeiboAppKey];
    [WeiboSDK enableDebugMode:YES];
    
    [WXApi registerApp:kWeixinAppKey withDescription:@"weixin"];
}

- (BOOL)handleOpenURL:(NSURL *)url {
    return [QQApiInterface handleOpenURL:url delegate:self] || [TencentOAuth HandleOpenURL:url];
}

#pragma mark QQ 回调接口 （本来应该放到AppDelegate里的，但是QQ和微信的回调函数同名，无法在一个文件里处理，所以暂时把QQ的回调放在这里处理）

- (void)onReq:(QQBaseReq *)req {
    
}

/**
 处理来至QQ的响应
 */
- (void)onResp:(QQBaseResp *)resp {
    int result = [resp.result intValue];
    if (0 == result) {


        AIAI_LOG(@"已成功分享至QQ空间");
    }
}

- (void)isOnlineResponse:(NSDictionary *)response {

}

#pragma mark Sina Weibo

+ (void)setSinaWeiboToken:(NSString*)token {
    sinaWeiboToken = token;
}

+ (NSString*)sinaWeiboToken {
    return sinaWeiboToken;
}

+ (void)enableDebugMode:(BOOL)enabled {
    [WeiboSDK enableDebugMode:enabled];
}

+ (BOOL)sinaWeiboAppIsInstalled {
    return [WeiboSDK isWeiboAppInstalled];
}

+ (void)sinaWeiboLogin {
    WBAuthorizeRequest *request = [WBAuthorizeRequest request];
    request.redirectURI = kSinaWeiboRedirectURI;
    [WeiboSDK sendRequest:request];
}

+ (BOOL)shareToWeiBoWithMessage:(NSString*)message {
    WBMessageObject *mo = [WBMessageObject message];
    
    if (nil != message && message.length)
    {
        mo.text = [AppUtils replaceExpression:message];
    }
    else {
        mo.text = @"[ii]";
    }
    
    WBSendMessageToWeiboRequest *request = [WBSendMessageToWeiboRequest requestWithMessage:mo];
    
    return [WeiboSDK sendRequest:request];
}

+ (BOOL)shareToWeiBoWithMessage:(NSString*)message image:(UIImage*)image mediaURL:(NSString*)url {
    
    WBMessageObject *mo = [WBMessageObject message];
    
    NSString* webUrl;
    if (nil != url && url.length) {
        webUrl = [url stringByReplacingOccurrencesOfString:@"#" withString:@"POUND_SIGN"];
    }
    else {
        webUrl = @"http://www.xmatrix.com.cn";
    }
    
    
    if (nil != message && message.length)
    {
        mo.text = [OpenPlatformServers subString:message url:webUrl];
    }
    else {
        mo.text = [NSString stringWithFormat:@"[ii] %@", webUrl];
    }
    
    if (nil != image) {
        WBImageObject *io = [WBImageObject object];
        io.imageData = UIImageJPEGRepresentation(image, 1.0);
        mo.imageObject = io;
    }
    else {
        WBImageObject *io = [WBImageObject object];
        io.imageData = UIImageJPEGRepresentation([UIImage imageNamed:@"aiaiLogo.png"], 1.0);
        mo.imageObject = io;
    }
    
    WBSendMessageToWeiboRequest *request = [WBSendMessageToWeiboRequest requestWithMessage:mo];
    
    return [WeiboSDK sendRequest:request];
}

+ (NSString*)subString:(NSString*)content url:(NSString*)url {
    NSString *string = [NSString stringWithFormat:@"%@ %@", content, url];
    if (140 > string.length) {
        return string;
    }
    
    NSInteger length = 140 - url.length - 3 - 1; //3是文本内容后面的... ，1是之间的空格
    
    NSString *resultString = [content substringWithRange:NSMakeRange(0, length)];
    
    return [NSString stringWithFormat:@"%@... %@", resultString, url];
}

#pragma mark WeiXin

+ (BOOL)weiXinAppIsInstalled {
    return [WXApi isWXAppInstalled];
}

//微信登录
+(void)WXLogin{
    
    SendAuthReq* req =[[SendAuthReq alloc ] init];
    req.scope = @"snsapi_userinfo,snsapi_base";
    req.state = @"8080";
    [WXApi sendReq:req];
}

+(void)getAccess_tokenWithCode:(NSString*)code{
    
    NSString *urlStr =[NSString stringWithFormat:@"https://api.weixin.qq.com/sns/oauth2/access_token?appid=%@&secret=%@&code=%@&grant_type=authorization_code",kWeixinAppKey,kWeixinAppSecret,code];
    
    [[RequestServers share] requestWithURL:urlStr objclass:[self class] prepare:^{
        
    } successed:^(NSDictionary *dic) {
        
        NSString *openid = [dic objectForKey:@"openid"];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:USEROPENPLATFORMLOGINNOTIFICATION object:nil userInfo:[NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:openid, @"2", nil] forKeys:[NSArray arrayWithObjects:SourceID, SourceCode, nil]]];
        
    } failed:^(NSError *error) {
        
    }];
}

+ (void)shareToWeiXinLinkContent:(NSString*)content title:(NSString*)title thumbImage:(UIImage*)thumbImage webpageUrl:(NSString*)webpageUrl scene:(int)scene {
    
    if (![OpenPlatformServers weiXinAppIsInstalled]) {
        AIAI_LOG(@"未安装微信手机客户端，分享失败");
        return;
    }
    
    WXMediaMessage *message = [WXMediaMessage message];
    
    if (nil != title && title.length){
        NSString* tmpTitle = [AppUtils replaceExpression:title];
        message.title = [OpenPlatformServers subString:tmpTitle url:@""];
    }else{
        message.title = @"[ii]";
    }

    if (nil != thumbImage && [thumbImage isKindOfClass:[UIImage class]]) {
        NSData *data = UIImageJPEGRepresentation([OpenPlatformServers reSizeImage:thumbImage toSize:CGSizeMake(80, 80)], 1.0);
        if (data.length < 32000) {
            [message setThumbImage:[OpenPlatformServers reSizeImage:thumbImage toSize:CGSizeMake(80, 80)]];
        }
        else {
            [message setThumbImage:thumbImage];
        }
    }
    else {
        [message setThumbImage:[UIImage imageNamed:@"aiaiLogo.png"]];
    }
    
    if (nil != webpageUrl && webpageUrl.length) {
        NSString* url = webpageUrl;//[webpageUrl stringByReplacingOccurrencesOfString:@"#" withString:@"POUND_SIGN"];
        WXWebpageObject *ext = [WXWebpageObject object];
        ext.webpageUrl = url;
        message.mediaObject = ext;
    }
    else {
        WXWebpageObject *ext = [WXWebpageObject object];
        ext.webpageUrl = @"http://www.xmatrix.com.cn";
        message.mediaObject = ext;
    }
    SendMessageToWXReq* req = [[SendMessageToWXReq alloc] init];
    req.bText = NO;
    req.message = message;
    req.scene = scene;
    
    [WXApi sendReq:req];
}

+ (UIImage *)reSizeImage:(UIImage *)image toSize:(CGSize)reSize

{
    UIGraphicsBeginImageContext(CGSizeMake(reSize.width, reSize.height));
    [image drawInRect:CGRectMake(0, 0, reSize.width, reSize.height)];
    UIImage *reSizeImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return reSizeImage;
    
}

#pragma mark QQ
- (NSString*)qqUserID {
    return oauth.openId;
}

- (void)qqLogin:(id<TencentSessionDelegate>)delegate {
    oauth.sessionDelegate = delegate;
    
    [oauth authorize:[NSArray arrayWithObjects:
                      kOPEN_PERMISSION_GET_USER_INFO,
                      kOPEN_PERMISSION_GET_SIMPLE_USER_INFO,
                      kOPEN_PERMISSION_ADD_ALBUM,
                      kOPEN_PERMISSION_ADD_IDOL,
                      kOPEN_PERMISSION_ADD_ONE_BLOG,
                      kOPEN_PERMISSION_ADD_PIC_T,
                      kOPEN_PERMISSION_ADD_SHARE,
                      kOPEN_PERMISSION_ADD_TOPIC,
                      kOPEN_PERMISSION_CHECK_PAGE_FANS,
                      kOPEN_PERMISSION_DEL_IDOL,
                      kOPEN_PERMISSION_DEL_T,
                      kOPEN_PERMISSION_GET_FANSLIST,
                      kOPEN_PERMISSION_GET_IDOLLIST,
                      kOPEN_PERMISSION_GET_INFO,
                      kOPEN_PERMISSION_GET_OTHER_INFO,
                      kOPEN_PERMISSION_GET_REPOST_LIST,
                      kOPEN_PERMISSION_LIST_ALBUM,
                      kOPEN_PERMISSION_UPLOAD_PIC,
                      kOPEN_PERMISSION_GET_VIP_INFO,
                      kOPEN_PERMISSION_GET_VIP_RICH_INFO,
                      kOPEN_PERMISSION_GET_INTIMATE_FRIENDS_WEIBO,
                      kOPEN_PERMISSION_MATCH_NICK_TIPS_WEIBO,
                      nil] inSafari:NO];
}

+ (BOOL)qqAppIsInstalled {
    return [QQApiInterface isQQInstalled];
}

+ (void)shareToQQLinkContent:(NSString*)content title:(NSString*)title thumbImage:(UIImage*)thumbImage webpageUrl:(NSString*)webpageUrl type:(int)type {
    
    if (![OpenPlatformServers qqAppIsInstalled]) {
        AIAI_LOG(@"未安装QQ手机客户端，分享失败");
        return;
    }
    
    NSString* tmpcontent = [AppUtils replaceExpression:content];
    NSString* subcontent = [OpenPlatformServers subString:tmpcontent url:@""];
    
    NSString* url;
    if (nil != webpageUrl && webpageUrl.length) {
        url = webpageUrl;//[webpageUrl stringByReplacingOccurrencesOfString:@"#" withString:@"POUND_SIGN"];
    }
    else {
        url = @"http://www.xmatrix.com.cn";
    }
    
    QQApiNewsObject *newsObj = [QQApiNewsObject objectWithURL:[NSURL URLWithString:url]
                                                        title:title.length ? title : @"[ii]"
                                                  description:subcontent ? : @""
                                             previewImageData:UIImageJPEGRepresentation(thumbImage, 0.1) ? : UIImageJPEGRepresentation([UIImage imageNamed:@"aiaiLogo.png"], 0.1)];
    
    SendMessageToQQReq *req = [SendMessageToQQReq reqWithContent:newsObj];
    
    QQApiSendResultCode sent = 0;
    
    if (0 == type)
    {
        //分享到QZone
        sent = [QQApiInterface SendReqToQZone:req];
    }
    else
    {
        //分享到QQ
        sent = [QQApiInterface sendReq:req];
    }
}

+ (void)didReceiveWeiboResponse:(WBBaseResponse *)response {
    if ([response isKindOfClass:WBSendMessageToWeiboResponse.class])
    {
        switch ((int)response.statusCode) {
            case 0:{
                //分享成功后积分增加
                
                
                AIAI_LOG(@"已成功分享至新浪微博");
            }
                break;
            case -1:
                break;
            case -2:
                AIAI_LOG(@"新浪微博分享失败");
                break;
            case -3:
                AIAI_LOG(@"新浪微博授权失败");
                break;
            case -4:
                AIAI_LOG(@"已取消安装新浪微博客户端");
                break;
            case -99:
                AIAI_LOG(@"不支持的请求");
                break;
            default:
                return ;
                break;
        }
    }
    else if ([response isKindOfClass:WBAuthorizeResponse.class])
    {
        // 如果新浪微博授权成功了
        if (WeiboSDKResponseStatusCodeSuccess == response.statusCode) {
            [[NSNotificationCenter defaultCenter] postNotificationName:USEROPENPLATFORMLOGINNOTIFICATION object:nil userInfo:[NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:[(WBAuthorizeResponse*)response userID], @"3", nil] forKeys:[NSArray arrayWithObjects:SourceID, SourceCode, nil]]];
        }
    }
}

+ (void)onReq:(BaseReq*)req
{
    if([req isKindOfClass:[GetMessageFromWXReq class]])
    {
        // 微信请求App提供内容， 需要app提供内容后使用sendRsp返回
        NSString *strTitle = [NSString stringWithFormat:@"微信请求App提供内容"];
        NSString *strMsg = @"微信请求App提供内容，App要调用sendResp:GetMessageFromWXResp返回给微信";
        
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:strTitle message:strMsg delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        alert.tag = 1000;
        [alert show];
    }
    else if([req isKindOfClass:[ShowMessageFromWXReq class]])
    {
        ShowMessageFromWXReq* temp = (ShowMessageFromWXReq*)req;
        WXMediaMessage *msg = temp.message;
        
        //显示微信传过来的内容
        WXAppExtendObject *obj = msg.mediaObject;
        
        NSString *strTitle = [NSString stringWithFormat:@"微信请求App显示内容"];
        NSString *strMsg = [NSString stringWithFormat:@"标题：%@ \n内容：%@ \n附带信息：%@ \n缩略图:%zi bytes\n\n", msg.title, msg.description, obj.extInfo, msg.thumbData.length];
        
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:strTitle message:strMsg delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alert show];
    }
    else if([req isKindOfClass:[LaunchFromWXReq class]])
    {
        //从微信启动App
        NSString *strTitle = [NSString stringWithFormat:@"从微信启动"];
        NSString *strMsg = @"这是从微信启动的消息";
        
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:strTitle message:strMsg delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alert show];
    }
}

+ (void)onResp:(BaseResp*)resp
{
    if([resp isKindOfClass:[SendMessageToWXResp class]]){
        if (0 == resp.errCode){
            
            AIAI_LOG(@"微信分享成功");
        }
        else if (-2 == resp.errCode)
            ;
        else
            AIAI_LOG(@"微信分享失败");
    }
    else if ([resp isKindOfClass:[SendAuthResp class]]){
        if (0 == resp.errCode) {
            AIAI_LOG(@"登录授权成功");
            
            SendAuthResp *tempResp = (SendAuthResp *)resp;
            NSString *wxRespCode = tempResp.code;
            [self getAccess_tokenWithCode:wxRespCode];
        }
        else {
            AIAI_LOG(@"登录授权失败");
        }
    }
}

@end