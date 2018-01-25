#ifndef _NDK_INTERFACE_H
#define _NDK_INTERFACE_H

#include <jni.h>
#include "BizCommon.h"
#include "IObserver.h"

#ifdef __cplusplus
extern "C" {
#endif

class JNILoginObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify);
};

class JNIBuddyObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify);
};

class JNIMessageObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify);
};

class JNIGroupObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify);
};

class JNIFansObserver : public IObserver
{
	virtual HRESULT OnNotify (VOID* pNotify);
};

/**
* 对JNI全局环境进行保存，以备回调时使用
* 唯一的全局对象，必须通过接口访问
*/
struct JNI_GLOBAL
{
	JNIEnv* current_env;		///< 当前前台env,必须只能在前台线程才可以调用
	JavaVM* jvm;				///< jvm虚拟机
	jobject obj;				///< java 环境中的接口对象
	int current_pid;			///< 当前线程id

	jclass    clsRspParams;
	jclass    clsMessageParams;
	jclass    clsGroupParams;
	jclass    clsBuddyParams;
	jclass    clsIqParams;
	jclass    clsOfflineMessageParams;

	// login ui observer & jni observer
	jobject           uiLoginObserver;
	JNILoginObserver* pJNILoginObserver;

	// message ui observer & jni observer
	jobject             uiMessageObserver;
//	jclass              clsUiMessageObserver;
	JNIMessageObserver* pJNIMessageObserver;

	// buddy ui observer & jni observer
	jobject           uiBuddyObserver;
	JNIBuddyObserver* pJNIBuddyObserver;

	// group ui observer & jni observer
	jobject           uiGroupObserver;
	JNIGroupObserver* pJNIGroupObserver;

	// fans ui observer & jni observer
	jobject           uiFansObserver;
	JNIFansObserver* pJNIFansObserver;
};

extern struct JNI_GLOBAL g_jni_global;

/**
* 初始化全局对象
* @param env	环境
* @param obj	绑定的接口对象
*/
void init_jni_global (JNIEnv* env, jobject obj);

/**
* 释放全局对象
*/
void free_jni_global ();

jint JNI_OnNotify_Login   (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify);
jint JNI_OnNotify_Message (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify);
jint JNI_OnNotify_Buddy   (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify);
jint JNI_OnNotify_Group   (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify);
jint JNI_OnNotify_Fans    (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify);

/*
 * JNI初始化
 *
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved);

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Init (JNIEnv *, jobject);
JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Release (JNIEnv *, jobject);

JNIEXPORT jint JNICALL    Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetRootDir (JNIEnv *, jobject, jstring);
JNIEXPORT jint JNICALL    Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetServerAddr (JNIEnv *, jobject, jstring, jstring);
JNIEXPORT jint JNICALL    Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetUserId (JNIEnv *, jobject, jstring);
JNIEXPORT jstring JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetUserId (JNIEnv *, jobject);
JNIEXPORT jint JNICALL    Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetObserver (JNIEnv *, jobject, jint, jobject);
JNIEXPORT jint JNICALL    Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_NotifyNetworkStatus (JNIEnv *, jobject, jint);
JNIEXPORT jint JNICALL    Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetConnectionStatus (JNIEnv *, jobject);
JNIEXPORT jint JNICALL    Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_CloseConnection (JNIEnv *, jobject);
JNIEXPORT jint JNICALL    Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_OpenLog (JNIEnv *, jobject, jint);

// 登录相关
JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Login (JNIEnv *, jobject, jobject);
JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Logout (JNIEnv *, jobject, jobject);

// 消息相关
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSessionList  (JNIEnv *, jobject);
JNIEXPORT jobject      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSession (JNIEnv *, jobject, jstring);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteSession (JNIEnv *, jobject, jstring, jint);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteSessionList (JNIEnv *, jobject, jobjectArray, jint);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ClearSession (JNIEnv *, jobject, jstring);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetSessionOption (JNIEnv *, jobject, jint, jobject);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetSessionListReaded (JNIEnv *, jobject, jobjectArray);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetTotalUnreadNum (JNIEnv *, jobject);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_UpdateDraft (JNIEnv *, jobject, jstring, jstring);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetMessageList (JNIEnv *, jobject, jstring, jstring, jint);
JNIEXPORT jobject      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetMessage (JNIEnv *, jobject, jstring, jstring, jint);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteMessage (JNIEnv *, jobject, jstring, jstring, jstring, jstring);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetAudioPlayed (JNIEnv *, jobject, jstring, jstring);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGivenFieldList (JNIEnv *, jobject, jstring, jint, jint);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetOfflineMessageList (JNIEnv *, jobject, jstring);

//群组相关接口
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_OperateGroup (JNIEnv *, jobject, jobject);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGroupList(JNIEnv *, jobject, jobject);
JNIEXPORT jobject      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGroupInfo(JNIEnv *, jobject ,jstring);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGroupMemberList(JNIEnv *, jobject, jstring);
JNIEXPORT jobject      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGroupMemberInfo(JNIEnv *, jobject, jstring, jstring,jint);

// 好友相关
JNIEXPORT jobject      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetBuddy (JNIEnv *, jobject, jstring, jint);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetBuddyList (JNIEnv *, jobject);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_AddBuddy (JNIEnv *, jobject, jstring);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ModifyBuddy (JNIEnv *, jobject, jobject);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteBuddy (JNIEnv *, jobject, jstring);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_IsBuddyExist (JNIEnv *, jobject, jstring);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SearchContact (JNIEnv *, jobject, jstring);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSearchContact (JNIEnv *, jobject);
JNIEXPORT jobject      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetContact (JNIEnv *, jobject, jstring, BOOL);

// 粉丝
JNIEXPORT jobject      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetFans (JNIEnv *, jobject, jstring, jint);
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetFansList (JNIEnv *, jobject);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetNewFansCount(JNIEnv *, jobject);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ClearNewFansCount(JNIEnv *, jobject);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSnsCount(JNIEnv *, jobject,jint);
JNIEXPORT jint         JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ClearSnsCount(JNIEnv *, jobject,jint);

JNIEXPORT jint      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetSavePath(JNIEnv *, jobject,jstring);
JNIEXPORT jint      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetMapPoint(JNIEnv *, jobject,jstring,jstring);
JNIEXPORT jint      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetFileServer(JNIEnv *, jobject,jstring,jint);

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SendImage(JNIEnv * env, jobject thiz,
		jstring fromuser, jstring touser, jstring imagepath, jstring txtbuffer, jint imagewidth, jint imageheight, jint msgtype);

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SendVideo(JNIEnv * env, jobject thiz,
		jstring fromuser, jstring touser, jstring videopath, jstring faceimagepath, jstring txtbuffer, jstring videotitle, jint videolength, jint msgtype);

JNIEXPORT jint      JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetBlackListFlag(JNIEnv *, jobject,jstring,jint);

#ifdef __cplusplus
}
#endif

#endif
