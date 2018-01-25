#include "Defines.h"
#include "CIMI.h"
#include "thread_env_manager.h"
#include "IMSDKJni.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JNI_GLOBAL g_jni_global;

// jstring to std::string
const string JString2String(JNIEnv* env, jstring jstr)
{
	string rtn = "";
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF("utf-8");
	jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
	if(jstr)
	{
		jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
		jsize alen = env->GetArrayLength(barr);
		jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
		if (alen > 0)
		{
			char* tmpstr = (char*)malloc(alen + 1);
			memcpy(tmpstr, ba, alen);
			tmpstr[alen] = 0;
			rtn = tmpstr;
			free(tmpstr);
		}
		env->ReleaseByteArrayElements(barr, ba, 0);
		env->DeleteLocalRef (barr);
	}

	env->DeleteLocalRef (strencode);
	env->DeleteLocalRef (clsstring);

	return rtn;
}

// std::string to jstring
jstring String2JString(JNIEnv* env, const string pat)
{
	if (pat.empty() == TRUE)
	{
		return env->NewStringUTF("");
	}

	// FindClass /NewString/ NewStringUTF/NewObject/ GetObjectField等产生的都是LocalRef，LocalRef有三种方式被VM 的GC清理。
	jclass strClass = env->FindClass("java/lang/String");
	jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
	int n = pat.length();
	jbyteArray bytes = env->NewByteArray(n);
	env->SetByteArrayRegion(bytes, 0, n, (jbyte*)pat.c_str());
    jchar encoding_name[] = { 'U', 'T', 'F', '-', '8', '\0' };
    jstring encoding = env->NewString(encoding_name, 5);
	jstring ret = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);

	env->DeleteLocalRef (encoding);
	env->DeleteLocalRef (bytes);
	env->DeleteLocalRef (strClass);

	return ret;
}


HRESULT JNILoginObserver::OnNotify (VOID* pNotify)
{
	int pid = pthread_self ();
	int nRetCode;

	if (pid == g_jni_global.current_pid)
	{
//		LOGD ("JNILoginObserver Current Thread......\r\n");
	}
	// 从后台线程调的
	else
	{
//		LOGD ("JNILoginObserver Attach Thread......\r\n");
		JNIEnv* env = g_env_man.get_env ();
		nRetCode = JNI_OnNotify_Login (env, g_jni_global.obj, g_jni_global.uiLoginObserver, pNotify);
		g_env_man.release_env (env);
	}

	return nRetCode;
}

HRESULT JNIMessageObserver::OnNotify (VOID* pNotify)
{
	int pid = pthread_self ();
	int nRetCode;

	if (pid == g_jni_global.current_pid)
	{
//		LOGD ("JNIMessageObserver Current Thread\r\n");
	}
	// 从后台线程调的
	else
	{
//		LOGD ("JNIMessageObserver Attach Thread\r\n");
		JNIEnv* env = g_env_man.get_env ();
		nRetCode = JNI_OnNotify_Message (env, g_jni_global.obj, g_jni_global.uiMessageObserver, pNotify);
		g_env_man.release_env (env);
	}

	return nRetCode;
}

HRESULT JNIBuddyObserver::OnNotify (VOID* pNotify)
{
	int pid = pthread_self ();
	int nRetCode;

	if (pid == g_jni_global.current_pid)
	{
//		LOGD ("JNIBuddyObserver Current Thread......\r\n");
	}
	// 从后台线程调的
	else
	{
//		LOGD ("JNIBuddyObserver Attach Thread......\r\n");
		JNIEnv* env = g_env_man.get_env ();
		nRetCode = JNI_OnNotify_Buddy (env, g_jni_global.obj, g_jni_global.uiBuddyObserver, pNotify);
		g_env_man.release_env (env);
	}

	return nRetCode;
}

HRESULT JNIGroupObserver::OnNotify (VOID* pNotify)
{
	int pid = pthread_self ();
	int nRetCode;

	if (pid == g_jni_global.current_pid)
	{
	}
	// 从后台线程调的
	else
	{
//		LOGD ("JNIGroupObserver::OnNotify() Attach Thread......\r\n");
		JNIEnv* env = g_env_man.get_env ();
		nRetCode = JNI_OnNotify_Group (env, g_jni_global.obj, g_jni_global.uiGroupObserver, pNotify);
		g_env_man.release_env (env);
	}

	return nRetCode;
}

HRESULT JNIFansObserver::OnNotify (VOID* pNotify)
{
	int pid = pthread_self ();
	int nRetCode;

	if (pid == g_jni_global.current_pid)
	{
	}
	// 从后台线程调的
	else
	{
//		LOGD ("JNIFansObserver::OnNotify() Attach Thread......\r\n");
		JNIEnv* env = g_env_man.get_env ();
		nRetCode = JNI_OnNotify_Fans (env, g_jni_global.obj, g_jni_global.uiFansObserver, pNotify);
		g_env_man.release_env (env);
	}

	return nRetCode;
}

void init_jni_global (JNIEnv* env, jobject thiz)
{
	///< 保存全局环境的线程id
	g_jni_global.current_pid = pthread_self ();
//	LOGD ("java ui thread id --- %s\r\n", FromInt (g_jni_global.current_pid).c_str());

	///< 设置全局环境
	env->GetJavaVM (&g_jni_global.jvm);
	g_jni_global.obj = env->NewGlobalRef (thiz);
	g_jni_global.current_env = env;

	g_jni_global.clsRspParams     = env->NewGlobalRef (env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$RspParams"));
	g_jni_global.clsMessageParams = env->NewGlobalRef (env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MessageParams"));
	g_jni_global.clsGroupParams   = env->NewGlobalRef (env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$GroupParams"));
	g_jni_global.clsBuddyParams   = env->NewGlobalRef (env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$BuddyParams"));
	g_jni_global.clsIqParams      = env->NewGlobalRef (env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$IqParams"));
	g_jni_global.clsOfflineMessageParams = env->NewGlobalRef (env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$OfflineMessageParams"));

	g_jni_global.uiLoginObserver   = NULL;
	g_jni_global.uiMessageObserver = NULL;
	g_jni_global.uiBuddyObserver   = NULL;
	g_jni_global.uiGroupObserver   = NULL;
	g_jni_global.uiFansObserver    = NULL;

	g_jni_global.pJNILoginObserver   = new JNILoginObserver();
	g_jni_global.pJNIMessageObserver = new JNIMessageObserver();
	g_jni_global.pJNIBuddyObserver   = new JNIBuddyObserver();
	g_jni_global.pJNIGroupObserver   = new JNIGroupObserver();
	g_jni_global.pJNIFansObserver    = new JNIFansObserver();
}

void free_jni_global ()
{
	if (g_jni_global.clsRspParams)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.clsRspParams);
		g_jni_global.clsRspParams = NULL;
	}
	if (g_jni_global.clsMessageParams)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.clsMessageParams);
		g_jni_global.clsMessageParams = NULL;
	}
	if (g_jni_global.clsGroupParams)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.clsGroupParams);
		g_jni_global.clsGroupParams = NULL;
	}
	if (g_jni_global.clsBuddyParams)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.clsBuddyParams);
		g_jni_global.clsBuddyParams = NULL;
	}
	if (g_jni_global.clsIqParams)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.clsIqParams);
		g_jni_global.clsIqParams = NULL;
	}
	if (g_jni_global.clsOfflineMessageParams)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.clsOfflineMessageParams);
		g_jni_global.clsOfflineMessageParams = NULL;
	}

	if (g_jni_global.uiLoginObserver)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.uiLoginObserver);
		g_jni_global.uiLoginObserver = NULL;
	}
	if (g_jni_global.uiMessageObserver)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.uiMessageObserver);
		g_jni_global.uiMessageObserver = NULL;
	}
	if (g_jni_global.uiBuddyObserver)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.uiBuddyObserver);
		g_jni_global.uiBuddyObserver = NULL;
	}
	if (g_jni_global.uiGroupObserver)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.uiGroupObserver);
		g_jni_global.uiGroupObserver = NULL;
	}
	if (g_jni_global.uiFansObserver)
	{
		g_jni_global.current_env->DeleteGlobalRef (g_jni_global.uiFansObserver);
		g_jni_global.uiFansObserver = NULL;
	}

	if (g_jni_global.pJNILoginObserver)
	{
		delete g_jni_global.pJNILoginObserver;
		g_jni_global.pJNILoginObserver = NULL;
	}
	if (g_jni_global.pJNIMessageObserver)
	{
		delete g_jni_global.pJNIMessageObserver;
		g_jni_global.pJNIMessageObserver = NULL;
	}
	if (g_jni_global.pJNIBuddyObserver)
	{
		delete g_jni_global.pJNIBuddyObserver;
		g_jni_global.pJNIBuddyObserver = NULL;
	}
	if (g_jni_global.pJNIGroupObserver)
	{
		delete g_jni_global.pJNIGroupObserver;
		g_jni_global.pJNIGroupObserver = NULL;
	}
	if (g_jni_global.pJNIFansObserver)
	{
		delete g_jni_global.pJNIFansObserver;
		g_jni_global.pJNIFansObserver = NULL;
	}

	g_jni_global.current_env->DeleteGlobalRef (g_jni_global.obj);
	g_jni_global.current_env = NULL;
	g_jni_global.obj = NULL;
	g_jni_global.jvm = NULL;
}


/**
* Table of methods associated with a single class.
*/
static JNINativeMethod gMethods [] =
{
	{ "Init",               "()I",                                       (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Init },
	{ "Release",            "()I",                                       (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Release },

	{ "SetRootDir",         "(Ljava/lang/String;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetRootDir },
	{ "SetServerAddr",      "(Ljava/lang/String;Ljava/lang/String;)I",   (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetServerAddr },
	{ "SetUserId",          "(Ljava/lang/String;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetUserId },
	{ "GetUserId",          "()Ljava/lang/String;",                      (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetUserId },
	{ "SetObserver",        "(ILjava/lang/Object;)I",                    (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetObserver },
	{ "NotifyNetworkStatus","(I)I",                                      (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_NotifyNetworkStatus },
	{ "GetConnectionStatus","()I",                                       (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetConnectionStatus },
	{ "CloseConnection",    "()I",                                       (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_CloseConnection },
	{ "OpenLog",            "(I)I",                                      (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_OpenLog },

	{ "Login",              "(Ljava/lang/Object;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Login },
	{ "Logout",             "(Ljava/lang/Object;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Logout },

	{ "GetSessionList",     "()[Ljava/lang/Object;",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSessionList },
	{ "GetSession",         "(Ljava/lang/String;)Ljava/lang/Object;",    (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSession },
	{ "DeleteSession",      "(Ljava/lang/String;I)I",                    (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteSession },
	{ "DeleteSessionList",  "(Ljava/lang/Object;I)I",                    (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteSessionList },
	{ "ClearSession",       "(Ljava/lang/String;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ClearSession },
	{ "SetSessionOption",   "(ILjava/lang/Object;)I",                    (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetSessionOption },
	{ "SetSessionListReaded", "(Ljava/lang/Object;)I",                   (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetSessionListReaded },
	{ "GetTotalUnreadNum",  "()I",                                       (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetTotalUnreadNum },
	{ "UpdateDraft",        "(Ljava/lang/String;Ljava/lang/String;)I",   (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_UpdateDraft },
	{ "GetMessageList",     "(Ljava/lang/String;Ljava/lang/String;I)[Ljava/lang/Object;", (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetMessageList },
	{ "GetMessage",         "(Ljava/lang/String;Ljava/lang/String;I)Ljava/lang/Object;",  (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetMessage },
	{ "DeleteMessage",      "(Ljava/lang/String;Ljava/lang/String;)I",   (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteMessage },
	{ "SetAudioPlayed",     "(Ljava/lang/String;Ljava/lang/String;)I",   (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetAudioPlayed },
	{ "GetGivenFieldList",  "(Ljava/lang/String;II)[Ljava/lang/Object;", (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGivenFieldList },
	{ "GetOfflineMessageList", "(Ljava/lang/String;)[Ljava/lang/Object;", (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetOfflineMessageList },

	{ "GetBuddy",           "(Ljava/lang/String;I)Ljava/lang/Object;",   (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetBuddy },
	{ "GetBuddyList",       "()[Ljava/lang/Object;",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetBuddyList },
	{ "AddBuddy",           "(Ljava/lang/String;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_AddBuddy },
	{ "ModifyBuddy",        "(Ljava/lang/Object;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ModifyBuddy },
	{ "DeleteBuddy",        "(Ljava/lang/String;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteBuddy },
	{ "IsBuddyExist",       "(Ljava/lang/String;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_IsBuddyExist },
	{ "SearchContact",      "(Ljava/lang/String;)I",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SearchContact },
	{ "GetSearchContact",   "()[Ljava/lang/Object;",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSearchContact },
	{ "GetContact",         "(Ljava/lang/String;I)Ljava/lang/Object;",   (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetContact },

	{ "GetFans",            "(Ljava/lang/String;I)Ljava/lang/Object;",   (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetFans },
	{ "GetFansList",        "()[Ljava/lang/Object;",                     (void*) Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetFansList },

};

#define JNIREG_CLASS "cn/com/xmatrix/ii/imsdkjni/IMSDKJni"//指定要注册的类

/*
* Register several native methods for one class.
*/
int registerNativeMethods(JNIEnv* env, const char* className,
        JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;
	clazz = env->FindClass(className);
	if (clazz == NULL) {
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		return JNI_FALSE;
	}

	env->DeleteLocalRef (clazz);
	return JNI_TRUE;
}

/*
* Register native methods for all classes we know about.
*/
int registerNatives(JNIEnv* env)
{
	if (!registerNativeMethods(env, JNIREG_CLASS, gMethods,
		sizeof(gMethods) / sizeof(gMethods[0])))
	{
		return JNI_FALSE;
	}
	return JNI_TRUE;
}

void ClearTreadJNIEnv(JavaVM* vm)
{
	vm->DetachCurrentThread();
}

/*
* Set some test stuff up.
*
* Returns the JNI version on success, -1 on failure.
*/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv( (void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		return -1;
	}

	if (!registerNatives(env)) {//注册
		return -1;
	}
	/* success -- return valid version number */
	result = JNI_VERSION_1_6;
	return result;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Init (JNIEnv* env, jobject thiz)
{
	init_jni_global (env, thiz);

	CIMI* pimi = CIMI::GetInstance();
	if (pimi == NULL)
	{
		return E_FAIL;
	}
	const string strRootDir = pimi->GetRootDir();
	INIT_CRASH_REPORT_FOR_ANDROID (strRootDir);
	pimi->Init();
	return S_OK;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Release (JNIEnv* env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();
	if (pimi == NULL)
	{
		return E_FAIL;
	}

	pimi->Release();
	free_jni_global ();
	return S_OK;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetRootDir (JNIEnv* env, jobject thiz, jstring strRootDir)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pRootDir = JString2String(env, strRootDir);
	return pimi->SetRootDir(pRootDir);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetServerAddr (JNIEnv* env, jobject thiz,
		jstring strServerAddr, jstring strServerPort)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pServerAddr = JString2String(env, strServerAddr);
	const string pServerPort = JString2String(env, strServerPort);
	return pimi->SetServerAddr(pServerAddr, pServerPort);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetUserId (JNIEnv* env, jobject thiz, jstring strUserId)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pUserId = JString2String(env, strUserId);
	return pimi->SetUserId(pUserId);
}

JNIEXPORT jstring JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetUserId (JNIEnv* env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pUserId = pimi->GetUserId();
	return String2JString(env, pUserId);
}

jint JNI_OnNotify_Login (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify)
{
	RspParams * pRspParams = (RspParams*)pNotify;

	LOGD ("Login uiObserver %p\r\n",uiObserver);
	if(!uiObserver)
	{
		return -1;
	}

	jclass cls = env->GetObjectClass (uiObserver);
	jmethodID mid = env->GetMethodID (cls, "OnNotify", "(Ljava/lang/Object;)I");

//	jclass    cls2   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$RspParams");
	jclass    cls2   = (jclass) env->NewGlobalRef (g_jni_global.clsRspParams);
	jmethodID mid2   = env->GetMethodID(cls2, "<init>","()V");
	jfieldID  fid2_1 = env->GetFieldID(cls2, "eRspType", "I");
//	jfieldID  fid2_2 = env->GetFieldID(cls2, "objRsp",   "Ljava/lang/Object;");
	jobject   obj2   = env->NewObject(cls2, mid2);
	env->SetIntField(obj2, fid2_1, pRspParams->eRspType);
	jint jresp = env->CallIntMethod (uiObserver, mid, obj2);

	// free ref
	env->DeleteLocalRef (obj2);
	env->DeleteGlobalRef (cls2);
	env->DeleteLocalRef (cls);

	return jresp;
}

jint JNI_OnNotify_Message (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify)
{
	RspParams * pRspParams = (RspParams*)pNotify;

	LOGD ("Message uiObserver %p\r\n",uiObserver);
	if(!uiObserver)
	{
		return -1;
	}

	jclass cls = env->GetObjectClass (uiObserver);
	jmethodID mid = env->GetMethodID (cls, "OnNotify", "(Ljava/lang/Object;)I");

//	jclass    cls2   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$RspParams");
	jclass    cls2   = (jclass) env->NewGlobalRef (g_jni_global.clsRspParams);
	jmethodID mid2   = env->GetMethodID(cls2, "<init>",   "()V");
	jfieldID  fid2_1 = env->GetFieldID (cls2, "eRspType", "I");
	jfieldID  fid2_2 = env->GetFieldID (cls2, "objRsp",   "Ljava/lang/Object;");
	jobject   obj2   = env->NewObject(cls2, mid2);
	env->SetIntField (obj2, fid2_1, pRspParams->eRspType);

//	LOGD ("eRspType=%d\n", pRspParams->eRspType);

	switch (pRspParams->eRspType)
	{
	case ST_RECV_MESSAGE:
		{
			MessageParams * pMessageParams = pRspParams->pMessageParams;

	//		jclass    cls3    = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MessageParams");
			jclass    cls3    = (jclass) env->NewGlobalRef (g_jni_global.clsMessageParams);
			jmethodID mid3    = env->GetMethodID(cls3, "<init>","()V");
			jfieldID  fid3_1  = env->GetFieldID(cls3, "pMsgId",      "Ljava/lang/String;");
			jfieldID  fid3_2  = env->GetFieldID(cls3, "pSessionId",  "Ljava/lang/String;");
			jfieldID  fid3_3  = env->GetFieldID(cls3, "pSender",     "Ljava/lang/String;");
			jfieldID  fid3_4  = env->GetFieldID(cls3, "pSenderName", "Ljava/lang/String;");
			jfieldID  fid3_5  = env->GetFieldID(cls3, "pSenderKey",  "Ljava/lang/String;");
			jfieldID  fid3_6  = env->GetFieldID(cls3, "pReceiver",   "Ljava/lang/String;");
			jfieldID  fid3_7  = env->GetFieldID(cls3, "pContent",    "Ljava/lang/String;");
			jfieldID  fid3_8  = env->GetFieldID(cls3, "pTime",       "Ljava/lang/String;");
			jfieldID  fid3_9  = env->GetFieldID(cls3, "eType",       "I");
			jfieldID  fid3_10 = env->GetFieldID(cls3, "eKind",       "I");
			jfieldID  fid3_11 = env->GetFieldID(cls3, "eCategory",   "I");

			jobject   obj3    = env->NewObject(cls3, mid3);
			jstring str_1 = String2JString(env, pMessageParams->pMsgId);
			jstring str_2 = String2JString(env, pMessageParams->pSessionId);
			jstring str_3 = String2JString(env, pMessageParams->pSender);
			jstring str_4 = String2JString(env, pMessageParams->pSenderName);
			jstring str_5 = String2JString(env, pMessageParams->pSenderKey);
			jstring str_6 = String2JString(env, pMessageParams->pReceiver);
			jstring str_7 = String2JString(env, pMessageParams->pContent);
			jstring str_8 = String2JString(env, pMessageParams->pTime);
			if (str_1)
			{
				env->SetObjectField (obj3, fid3_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField (obj3, fid3_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField (obj3, fid3_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			if (str_4)
			{
				env->SetObjectField (obj3, fid3_4, str_4);
				env->DeleteLocalRef (str_4);
			}
			if (str_5)
			{
				env->SetObjectField (obj3, fid3_5, str_5);
				env->DeleteLocalRef (str_5);
			}
			if (str_6)
			{
				env->SetObjectField (obj3, fid3_6, str_6);
				env->DeleteLocalRef (str_6);
			}
			if (str_7)
			{
				env->SetObjectField (obj3, fid3_7, str_7);
				env->DeleteLocalRef (str_7);
			}
			if (str_8)
			{
				env->SetObjectField (obj3, fid3_8, str_8);
				env->DeleteLocalRef (str_8);
			}
			env->SetIntField    (obj3, fid3_9,  pMessageParams->eType);
			env->SetIntField    (obj3, fid3_10, pMessageParams->eKind);
			env->SetIntField    (obj3, fid3_11, pMessageParams->eCategory);

			env->SetObjectField (obj2, fid2_2,  obj3);
			env->DeleteLocalRef (obj3);
			env->DeleteGlobalRef (cls3);
		}
		break;

	case ST_RECV_OFFLINE_MESSAGE:
		{
			OfflineMessageParams * pOfflineMessageParams = pRspParams->pOfflineMessageParams;

			LOGD ("JNI: nOfflineTotal=%d, nSessionCount=%d\r\n",
					pOfflineMessageParams->nOfflineTotal, pOfflineMessageParams->nSessionCount);

			// ST_RECV_OFFLINE_MESSAGE
	//		jclass    cls3    = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$OfflineMessageParams");
			jclass    cls3    = (jclass) env->NewGlobalRef (g_jni_global.clsOfflineMessageParams);
			jmethodID mid3    = env->GetMethodID(cls3, "<init>","()V");
			jfieldID  fid3_1  = env->GetFieldID(cls3, "nOfflineTotal", "I");
			jfieldID  fid3_2  = env->GetFieldID(cls3, "nSessionCount", "I");
			jfieldID  fid3_3  = env->GetFieldID(cls3, "ppSessionId",   "Ljava/lang/Object;");

			jobject   obj3    = env->NewObject(cls3, mid3);
			env->SetIntField (obj3, fid3_1,  pOfflineMessageParams->nOfflineTotal);
			env->SetIntField (obj3, fid3_2,  pOfflineMessageParams->nSessionCount);

			jobjectArray obj_arr = NULL;
			if (pOfflineMessageParams->nSessionCount > 0)
			{
				jclass clsstring = env->FindClass("java/lang/String");
				obj_arr = env->NewObjectArray (pOfflineMessageParams->nSessionCount, clsstring, NULL);
				env->DeleteLocalRef (clsstring);

				for (int i=0; i<pOfflineMessageParams->nSessionCount; i++)
				{
					jstring str_1 = String2JString(env, pOfflineMessageParams->ppSessionId[i]);
					if (str_1)
					{
			            env->SetObjectArrayElement(obj_arr, i, str_1);
						env->DeleteLocalRef (str_1);
					}
				}
			}
			env->SetObjectField (obj3, fid3_3,  obj_arr);
			env->DeleteLocalRef (obj_arr);

			env->SetObjectField (obj2, fid2_2, obj3);
			env->DeleteLocalRef (obj3);
			env->DeleteGlobalRef (cls3);
		}
		break;

	case ST_RECV_CUSTOM_SMALLNUM:
		{
			IqParams * pIqParams = pRspParams->pIqParams;

			// ST_RECV_CUSTOM_SMALLNUM
	//		jclass    cls3    = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$IqParams");
			jclass    cls3    = (jclass) env->NewGlobalRef (g_jni_global.clsIqParams);
			jmethodID mid3    = env->GetMethodID(cls3, "<init>","()V");
			jfieldID  fid3_1  = env->GetFieldID(cls3, "eType",     "I");
			jfieldID  fid3_2  = env->GetFieldID(cls3, "pTalkId",   "Ljava/lang/String;");
			jfieldID  fid3_3  = env->GetFieldID(cls3, "pSmallNum", "Ljava/lang/String;");

			jobject   obj3    = env->NewObject(cls3, mid3);
			jstring str_2 = String2JString(env, pIqParams->pTalkId);
			jstring str_3 = String2JString(env, pIqParams->pSmallNum);
			env->SetIntField (obj3, fid3_1,  pIqParams->eType);
			if (str_2)
			{
				env->SetObjectField (obj3, fid3_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField (obj3, fid3_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			env->SetObjectField (obj2, fid2_2, obj3);
			env->DeleteLocalRef (obj3);
			env->DeleteGlobalRef (cls3);
		}
		break;

	default:
		{
			MessageParams * pMessageParams = pRspParams->pMessageParams;

			// ST_SEND_MESSAGE_SUCCESS
			// ST_SEND_MESSAGE_FAILURE
			// ST_SEND_MESSAGE_PROCESS
			// ST_WRITE_MESSAGE_SUCCESS
			// ST_WRITE_MESSAGE_FAILURE
	//		jclass    cls3    = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MessageParams");
			jclass    cls3    = (jclass) env->NewGlobalRef (g_jni_global.clsMessageParams);
			jmethodID mid3    = env->GetMethodID(cls3, "<init>",     "()V");
			jfieldID  fid3_1  = env->GetFieldID (cls3, "pMsgId",     "Ljava/lang/String;");
			jfieldID  fid3_2  = env->GetFieldID (cls3, "pSessionId", "Ljava/lang/String;");

			jobject   obj3    = env->NewObject(cls3, mid3);
			jstring   str_1   = String2JString(env, pMessageParams->pMsgId);
			jstring   str_2   = String2JString(env, pMessageParams->pSessionId);
			if (str_1)
			{
				env->SetObjectField (obj3, fid3_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField (obj3, fid3_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			env->SetObjectField (obj2, fid2_2, obj3);
			env->DeleteLocalRef (obj3);
			env->DeleteGlobalRef (cls3);
		}
		break;
	}

//	LOGD ("uiObserver, mid, obj2=%p, %p, %p\r\n", uiObserver, mid, obj2);
	jint jresp = env->CallIntMethod (uiObserver, mid, obj2);

	// free ref
	env->DeleteLocalRef (obj2);
	env->DeleteGlobalRef (cls2);
	env->DeleteLocalRef (cls);

	return jresp;
}

jint JNI_OnNotify_Buddy (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify)
{
	RspParams * pRspParams = (RspParams*)pNotify;

	LOGD ("Buddy uiObserver %p\r\n",uiObserver);
	if(!uiObserver)
	{
		return -1;
	}

	jclass cls = env->GetObjectClass (uiObserver);
	jmethodID mid = env->GetMethodID (cls, "OnNotify", "(Ljava/lang/Object;)I");

//	jclass    cls2   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$RspParams");
	jclass    cls2   = (jclass) env->NewGlobalRef (g_jni_global.clsRspParams);
	jmethodID mid2   = env->GetMethodID(cls2, "<init>","()V");
	jfieldID  fid2_1 = env->GetFieldID(cls2, "eRspType", "I");
	jfieldID  fid2_2 = env->GetFieldID(cls2, "objRsp",   "Ljava/lang/Object;");
	jobject   obj2   = env->NewObject(cls2, mid2);
	env->SetIntField(obj2, fid2_1, pRspParams->eRspType);

	LOGD ("Buddy pBuddyParams %p\r\n",pRspParams->pBuddyParams);
	if(pRspParams->pBuddyParams)
	{
		jclass    cls3    = env->NewGlobalRef (g_jni_global.clsBuddyParams);
		jmethodID mid3    = env->GetMethodID(cls3, "<init>","()V");
		jobject   obj3   = env->NewObject(cls3, mid3);

		jfieldID  fid3_1  = env->GetFieldID(cls3, "pUserId", "Ljava/lang/String;");
		jstring userid = String2JString(env,pRspParams->pBuddyParams->pUserId);

		if (userid)
		{
			env->SetObjectField (obj3, fid3_1, userid);
			env->DeleteLocalRef (userid);
		}

		env->SetObjectField (obj2, fid2_2, obj3);

	}

	jint jresp = env->CallIntMethod (uiObserver, mid, obj2);

	// free ref
	env->DeleteLocalRef (obj2);
	env->DeleteGlobalRef (cls2);
	env->DeleteLocalRef (cls);

	return jresp;
}

jint JNI_OnNotify_Group (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify)
{
	RspParams * pRspParams = (RspParams*)pNotify;
	GroupParams * pMessageParams = pRspParams->pGroupParams;

	LOGD ("Group uiObserver %p\r\n",uiObserver);
	if(!uiObserver)
	{
		return -1;
	}

	jclass cls = env->GetObjectClass (uiObserver);
	LOGD ("cls %p\r\n",cls);
	jmethodID mid = env->GetMethodID (cls, "OnNotify", "(Ljava/lang/Object;)I");

	jclass    cls2   = env->NewGlobalRef (g_jni_global.clsRspParams);
	jmethodID mid2   = env->GetMethodID(cls2, "<init>","()V");
	jfieldID  fid2_1 = env->GetFieldID(cls2, "eRspType", "I");
	jfieldID  fid2_2 = env->GetFieldID(cls2, "objRsp", "Ljava/lang/Object;");
	jfieldID  fid2_3 = env->GetFieldID(cls2, "eRspErrCode", "I");
	jfieldID  fid2_4 = env->GetFieldID(cls2, "eRspExtendCode", "I");
	jobject   obj2   = env->NewObject(cls2, mid2);
	env->SetIntField (obj2, fid2_1, pRspParams->eRspType);
	env->SetIntField (obj2, fid2_3, pRspParams->errcode);
	env->SetIntField (obj2, fid2_4, pRspParams->extendcode);

	if (pRspParams->eRspType == ST_ADD_GROUP_SUCCESS || pRspParams->eRspType == ST_NOTIFY_CREATE_GROUP || pRspParams->eRspType == ST_JOINGRUOP_SUCCESS || pRspParams->eRspType == ST_NOTIFY_REFRESH_GROUP)
	{
		jclass    cls3    = env->NewGlobalRef (g_jni_global.clsGroupParams);
		jmethodID mid3    = env->GetMethodID(cls3, "<init>","()V");
		jfieldID  fid3_1  = env->GetFieldID(cls3, "GroupID", "Ljava/lang/String;");
		jfieldID  fid3_2  = env->GetFieldID(cls3, "GroupName", "Ljava/lang/String;");
		jfieldID  fid3_3  = env->GetFieldID(cls3, "GroupPortraitKey", "Ljava/lang/String;");
		//jfieldID  fid3_3  = env->GetFieldID(cls3, "Userlist", "Ljava/lang/Object;");
		//jfieldID  fid3_4  = env->GetFieldID(cls3, "UserID", "Ljava/lang/String;");
		jobject   obj3    = env->NewObject(cls3, mid3);

		jstring groupid = String2JString(env,pRspParams->pGroupParams->GroupID.c_str());

		if (groupid)
		{
			env->SetObjectField (obj3, fid3_1, groupid);
			env->DeleteLocalRef (groupid);
		}

		jstring groupname = String2JString(env,pRspParams->pGroupParams->GroupName.c_str());

		if (groupname)
		{
			env->SetObjectField (obj3, fid3_2, groupname);
			env->DeleteLocalRef (groupname);
		}

		jstring groupkey = String2JString(env,pRspParams->pGroupParams->GroupPortraitKey.c_str());

		if (groupkey)
		{
			env->SetObjectField (obj3, fid3_3, groupkey);
			env->DeleteLocalRef (groupkey);
		}

		env->SetObjectField (obj2, fid2_2, obj3);
		// free ref
		env->DeleteLocalRef (obj3);
	}
	else if (pRspParams->eRspType == ST_MODIFY_GROUPNAME_SUCCESS || pRspParams->eRspType == ST_QUIT_GROUP_SUCCESS || pRspParams->eRspType == ST_NOTIFY_MODIFY_GROUP || pRspParams->eRspType == ST_NOTIFY_QUIT_GROUP)
	{
		jclass    cls3    = env->NewGlobalRef (g_jni_global.clsGroupParams);
		jmethodID mid3    = env->GetMethodID(cls3, "<init>","()V");
		jfieldID  fid3_1  = env->GetFieldID(cls3, "GroupID", "Ljava/lang/String;");
		jfieldID  fid3_2  = env->GetFieldID(cls3, "GroupName", "Ljava/lang/String;");
		jfieldID  fid3_3  = env->GetFieldID(cls3, "GroupPortraitKey", "Ljava/lang/String;");
		jobject   obj3    = env->NewObject(cls3, mid3);

		jstring groupid = String2JString(env,pRspParams->pGroupParams->GroupID);

		if (groupid)
		{
			env->SetObjectField (obj3, fid3_1, groupid);
			env->DeleteLocalRef (groupid);
		}

		jstring groupname = String2JString(env,pRspParams->pGroupParams->GroupName);

		if (groupname)
		{
			env->SetObjectField (obj3, fid3_2, groupname);
			env->DeleteLocalRef (groupname);
		}

		jstring groupkey = String2JString(env,pRspParams->pGroupParams->GroupPortraitKey);

		if (groupkey)
		{
			env->SetObjectField (obj3, fid3_3, groupkey);
			env->DeleteLocalRef (groupkey);
		}

		env->SetObjectField (obj2, fid2_2, obj3);
		// free ref
		env->DeleteLocalRef (obj3);
	}
	else if (pRspParams->eRspType == ST_MODIFY_GROUPMEMBERNAME_SUCCESS || pRspParams->eRspType == ST_NOTIFY_MODIFY_GROUPMEMBERNAME)
	{
		jclass    cls3    = env->NewGlobalRef (g_jni_global.clsGroupParams);
		jmethodID mid3    = env->GetMethodID(cls3, "<init>","()V");
		jfieldID  fid3_1  = env->GetFieldID(cls3, "GroupID", "Ljava/lang/String;");
		jfieldID  fid3_2  = env->GetFieldID(cls3, "GroupCardName", "Ljava/lang/String;");

		jobject   obj3    = env->NewObject(cls3, mid3);

		jstring groupid = String2JString(env,pRspParams->pGroupParams->GroupID.c_str());

		if (groupid)
		{
			env->SetObjectField (obj3, fid3_1, groupid);
			env->DeleteLocalRef (groupid);
		}

		jstring groupname = String2JString(env,pRspParams->pGroupParams->GroupCardName.c_str());

		if (groupname)
		{
			env->SetObjectField (obj3, fid3_2, groupname);
			env->DeleteLocalRef (groupname);
		}

		env->SetObjectField (obj2, fid2_2, obj3);
		// free ref
		env->DeleteLocalRef (obj3);
	}

	jint jresp = env->CallIntMethod (uiObserver, mid, obj2);
	env->DeleteLocalRef (obj2);
	env->DeleteGlobalRef (cls2);
	env->DeleteLocalRef (cls);

	return jresp;
}

jint JNI_OnNotify_Fans (JNIEnv* env, jobject thiz, jobject uiObserver, VOID* pNotify)
{
	RspParams * pRspParams = (RspParams*)pNotify;

	LOGD ("Fans uiObserver %p\r\n",uiObserver);
	if(!uiObserver)
	{
		return -1;
	}

	jclass cls = env->GetObjectClass (uiObserver);
	jmethodID mid = env->GetMethodID (cls, "OnNotify", "(Ljava/lang/Object;)I");

//	jclass    cls2   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$RspParams");
	jclass    cls2   = (jclass) env->NewGlobalRef (g_jni_global.clsRspParams);
	jmethodID mid2   = env->GetMethodID(cls2, "<init>","()V");
	jfieldID  fid2_1 = env->GetFieldID(cls2, "eRspType", "I");
//	jfieldID  fid2_2 = env->GetFieldID(cls2, "objRsp",   "Ljava/lang/Object;");
	jobject   obj2   = env->NewObject(cls2, mid2);
	env->SetIntField(obj2, fid2_1, pRspParams->eRspType);
	jint jresp = env->CallIntMethod (uiObserver, mid, obj2);

	// free ref
	env->DeleteLocalRef (obj2);
	env->DeleteGlobalRef (cls2);
	env->DeleteLocalRef (cls);

	return jresp;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetObserver (JNIEnv* env, jobject thiz,
		jint observerType, jobject uiObserver)
{
	CIMI* pimi = CIMI::GetInstance();
	HRESULT hResult;

	enum ObserverType eObserverType = (enum ObserverType)observerType;
//	LOGD ("JNI: SetObserver, eObserverType=%d\r\n", eObserverType);

	switch (eObserverType)
	{
	case OT_LOGIN:
		if (g_jni_global.uiLoginObserver)
		{
			env->DeleteGlobalRef (g_jni_global.uiLoginObserver);
			g_jni_global.uiLoginObserver = NULL;
		}
		g_jni_global.uiLoginObserver = env->NewGlobalRef (uiObserver);
		hResult = pimi->SetObserver(eObserverType, g_jni_global.pJNILoginObserver);
		break;

	case OT_MESSAGE:
		if (g_jni_global.uiMessageObserver)
		{
			env->DeleteGlobalRef (g_jni_global.uiMessageObserver);
			g_jni_global.uiMessageObserver = NULL;
		}
		g_jni_global.uiMessageObserver = env->NewGlobalRef (uiObserver);
//		{
//			jclass cls = env->GetObjectClass (g_jni_global.uiMessageObserver);
//			g_jni_global.clsUiMessageObserver = (jclass) env->NewGlobalRef (cls);
//		}
		hResult = pimi->SetObserver(eObserverType, g_jni_global.pJNIMessageObserver);
		break;

	case OT_BUDDY:
		if (g_jni_global.uiBuddyObserver)
		{
			env->DeleteGlobalRef (g_jni_global.uiBuddyObserver);
			g_jni_global.uiBuddyObserver = NULL;
		}
		g_jni_global.uiBuddyObserver = env->NewGlobalRef (uiObserver);
		hResult = pimi->SetObserver(eObserverType, g_jni_global.pJNIBuddyObserver);
		break;

	case OT_GROUP:
		if (g_jni_global.uiGroupObserver)
		{
			env->DeleteGlobalRef (g_jni_global.uiGroupObserver);
			g_jni_global.uiGroupObserver = NULL;
		}
		g_jni_global.uiGroupObserver = env->NewGlobalRef (uiObserver);
		hResult = pimi->SetObserver(eObserverType, g_jni_global.pJNIGroupObserver);
		break;

	case OT_FANS:
		if (g_jni_global.uiFansObserver)
		{
			env->DeleteGlobalRef (g_jni_global.uiFansObserver);
			g_jni_global.uiFansObserver = NULL;
		}
		g_jni_global.uiFansObserver = env->NewGlobalRef (uiObserver);
		hResult = pimi->SetObserver(eObserverType, g_jni_global.pJNIFansObserver);
		break;

	default:
		hResult = E_FAIL;
		break;

	}

	return hResult;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_NotifyNetworkStatus (JNIEnv* env, jobject thiz,
		jint networkStatusType)
{
	CIMI* pimi = CIMI::GetInstance();
	enum NetWorkStatusType eNetworkStatusType = (enum NetWorkStatusType)networkStatusType;
//	LOGD ("eNetworkStatusType=%d\r\n", eNetworkStatusType);
	return pimi->NotifyNetworkStatus(eNetworkStatusType);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetConnectionStatus (JNIEnv* env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();
	return pimi->GetConnectionStatus();
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_CloseConnection (JNIEnv* env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();
	return pimi->CloseConnection();
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_OpenLog (JNIEnv* env, jobject thiz, jint openLog)
{
	CIMI* pimi = CIMI::GetInstance();
	return pimi->OpenLog(openLog);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Login (JNIEnv* env, jobject thiz, jobject object)
{
	CIMI* pimi = CIMI::GetInstance();

	ReqParams reqParams = {};

	jclass cls = env->GetObjectClass(object);
	jfieldID fid = env->GetFieldID(cls, "eReqType", "I");
	int intVal = env->GetIntField(object, fid);
//	LOGD ("intVal=%d\r\n", intVal);
	reqParams.eReqType = (enum ReqType)intVal;

	// free ref
	env->DeleteLocalRef (cls);

	return pimi->Login(&reqParams);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_Logout (JNIEnv* env, jobject thiz, jobject object)
{
	CIMI* pimi = CIMI::GetInstance();

	ReqParams reqParams = {};

	jclass cls = env->GetObjectClass(object);
	jfieldID fid = env->GetFieldID(cls, "eReqType", "I");
	int intVal = env->GetIntField(object, fid);
//	LOGD ("intVal=%d\r\n", intVal);
	reqParams.eReqType = (enum ReqType)intVal;

	// free ref
	env->DeleteLocalRef (cls);

	return pimi->Logout(&reqParams);
}

/*
 * 获取会话列表
 */
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSessionList (JNIEnv * env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();

	jclass    cls    = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$SessionParams");
	jmethodID mid    = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pSessionId",   "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pSessionName", "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey", "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pMsgId",       "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "eType",        "I");
	jfieldID  fid_6  = env->GetFieldID(cls, "nUnreadNum",   "I");
	jfieldID  fid_7  = env->GetFieldID(cls, "eTop",         "I");
	jfieldID  fid_8  = env->GetFieldID(cls, "eRemind",      "I");
	jfieldID  fid_9  = env->GetFieldID(cls, "pDraftContent","Ljava/lang/String;");
	jfieldID  fid_10 = env->GetFieldID(cls, "eDraftChanged","I");

	jobjectArray obj_arr = NULL;

	SessionList pSessionList = pimi->GetSessionList();
	if (pSessionList.nCount > 0)
	{
//		LOGD ("session count=%d\r\n", pSessionList->nCount);
		jclass clsstring = env->FindClass("java/lang/Object");
		obj_arr = env->NewObjectArray (pSessionList.nCount, clsstring, NULL);
		env->DeleteLocalRef (clsstring);

		for (int i=0; i<pSessionList.nCount; i++)
		{
			SessionParams pSessionParams = pSessionList.ppSessionParams[i];
//			LOGD ("[%d]: pSessionId=%s, pSessionName=%s, pPortraitKey=%s, pMsgId=%s, eType=%d, nUnreadNum=%d, eTop=%d, eRemind=%d\r\n",
//					i+1,
//					pSessionParams.pSessionId,
//					pSessionParams.pSessionName,
//					pSessionParams.pPortraitKey,
//					pSessionParams.pMsgId,
//					pSessionParams.eType,
//					pSessionParams.nUnreadNum,
//					pSessionParams.eTop,
//					pSessionParams.eRemind);

			jobject   obj   = env->NewObject(cls, mid);
			jstring str_1 = String2JString(env, pSessionParams.pSessionId);
			jstring str_2 = String2JString(env, pSessionParams.pSessionName);
			jstring str_3 = String2JString(env, pSessionParams.pPortraitKey);
			jstring str_4 = String2JString(env, pSessionParams.pMsgId);
			if (str_1)
			{
				env->SetObjectField(obj, fid_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField(obj, fid_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField(obj, fid_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			if (str_4)
			{
				env->SetObjectField(obj, fid_4, str_4);
				env->DeleteLocalRef (str_4);
			}
			env->SetIntField(obj, fid_5, pSessionParams.eType);
			env->SetIntField(obj, fid_6, pSessionParams.nUnreadNum);
			env->SetIntField(obj, fid_7, pSessionParams.eTop);
			env->SetIntField(obj, fid_8, pSessionParams.eRemind);
			jstring str_9 = String2JString(env, pSessionParams.pDraftContent);
			if (str_9)
			{
				env->SetObjectField(obj, fid_9, str_9);
				env->DeleteLocalRef (str_9);
			}
			env->SetIntField(obj, fid_10, pSessionParams.eDraftChanged);
			env->SetObjectArrayElement (obj_arr, i, obj);
			env->DeleteLocalRef (obj);

		}
	}

	// free ref
	env->DeleteLocalRef (cls);

	return obj_arr;
}

JNIEXPORT jobject JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSession (JNIEnv * env, jobject thiz,
		jstring sessionId)
{
	CIMI* pimi = CIMI::GetInstance();

	const string pSessionId = JString2String(env, sessionId);
//	LOGD ("pSessionId=%s\r\n", pSessionId);

	jclass    cls    = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$SessionParams");
	jmethodID mid    = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pSessionId",   "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pSessionName", "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey", "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pMsgId",       "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "eType",        "I");
	jfieldID  fid_6  = env->GetFieldID(cls, "nUnreadNum",   "I");
	jfieldID  fid_7  = env->GetFieldID(cls, "eTop",         "I");
	jfieldID  fid_8  = env->GetFieldID(cls, "eRemind",      "I");
	jfieldID  fid_9  = env->GetFieldID(cls, "pDraftContent","Ljava/lang/String;");
	jfieldID  fid_10 = env->GetFieldID(cls, "eDraftChanged","I");

	jobject obj = NULL;

	SessionParams pSessionParams = pimi->GetSession(pSessionId);
	if (pSessionParams.pSessionId.empty() == FALSE)
	{
//		LOGD ("pSessionId=%s, pSessionName=%s, pPortraitKey=%s, pMsgId=%s, eType=%d, nUnreadNum=%d, eTop=%d, eRemind=%d\r\n",
//				pSessionParams.pSessionId,
//				pSessionParams.pSessionName,
//				pSessionParams.pPortraitKey,
//				pSessionParams.pMsgId,
//				pSessionParams.eType,
//				pSessionParams.nUnreadNum,
//				pSessionParams.eTop,
//				pSessionParams.eRemind);

		obj   = env->NewObject(cls, mid);
		jstring str_1 = String2JString(env, pSessionParams.pSessionId);
		jstring str_2 = String2JString(env, pSessionParams.pSessionName);
		jstring str_3 = String2JString(env, pSessionParams.pPortraitKey);
		jstring str_4 = String2JString(env, pSessionParams.pMsgId);
		if (str_1)
		{
			env->SetObjectField(obj, fid_1, str_1);
			env->DeleteLocalRef (str_1);
		}
		if (str_2)
		{
			env->SetObjectField(obj, fid_2, str_2);
			env->DeleteLocalRef (str_2);
		}
		if (str_3)
		{
			env->SetObjectField(obj, fid_3, str_3);
			env->DeleteLocalRef (str_3);
		}
		if (str_4)
		{
			env->SetObjectField(obj, fid_4, str_4);
			env->DeleteLocalRef (str_4);
		}
		env->SetIntField(obj, fid_5, pSessionParams.eType);
		env->SetIntField(obj, fid_6, pSessionParams.nUnreadNum);
		env->SetIntField(obj, fid_7, pSessionParams.eTop);
		env->SetIntField(obj, fid_8, pSessionParams.eRemind);
		jstring str_9 = String2JString(env, pSessionParams.pDraftContent);
		if (str_9)
		{
			env->SetObjectField(obj, fid_9, str_9);
			env->DeleteLocalRef (str_9);
		}
		env->SetIntField(obj, fid_10, pSessionParams.eDraftChanged);
	}

	// free ref
	env->DeleteLocalRef (cls);

	return obj;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteSession (JNIEnv* env, jobject thiz, jstring sessionId,
		jint deleteMessageLogs)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pSessionId = JString2String(env, sessionId);
	return pimi->DeleteSession(pSessionId, deleteMessageLogs);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteSessionList (JNIEnv* env, jobject thiz,
		jobjectArray sessionList, jint deleteMessageLogs)
{
	CIMI* pimi = CIMI::GetInstance();
	SessionList pSessionList = {};

	// Get the value of each Integer object in the array
	jsize length = env->GetArrayLength(sessionList);
	for (int i = 0; i<length; i++)
	{
		SessionParams pSessionParams = {};
		jobject sessionId = env->GetObjectArrayElement(sessionList, i);
		pSessionParams.pSessionId = JString2String(env, sessionId);
		pSessionList.ppSessionParams.push_back(pSessionParams);
	}
	pSessionList.nCount = length;
	return pimi->DeleteSessionList(pSessionList, deleteMessageLogs);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ClearSession (JNIEnv* env, jobject thiz, jstring sessionId)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pSessionId = JString2String(env, sessionId);
	return pimi->ClearSession(pSessionId);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetSessionOption (JNIEnv* env, jobject thiz, jint optionType, jobject obj)
{
	CIMI* pimi = CIMI::GetInstance();

	SessionParams sessionParams = {};

	jclass cls = env->GetObjectClass(obj);
	jfieldID  fid_1  = env->GetFieldID(cls, "pSessionId", "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "eTop", "I");
	jfieldID  fid_3  = env->GetFieldID(cls, "eRemind", "I");
	jstring str_1 = env->GetObjectField(obj, fid_1);
	if (str_1)
	{
		sessionParams.pSessionId = JString2String(env, str_1);
	}
	sessionParams.eTop       = env->GetIntField(obj, fid_2);
	sessionParams.eRemind    = env->GetIntField(obj, fid_3);

	// free ref
	env->DeleteLocalRef (cls);

	HRESULT hResult = pimi->SetSessionOption(optionType, sessionParams);

	if (str_1)
	{
		env->DeleteLocalRef (str_1);
	}

	return hResult;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetSessionListReaded (JNIEnv* env, jobject thiz,
		jobjectArray sessionList)
{
	CIMI* pimi = CIMI::GetInstance();
	SessionList pSessionList = {};

	// Get the value of each Integer object in the array
	jsize length = env->GetArrayLength(sessionList);
	for (int i = 0; i<length; i++)
	{
		SessionParams pSessionParams = {};
		jobject sessionId = env->GetObjectArrayElement(sessionList, i);
		const string pSessionId = JString2String(env, sessionId);
		pSessionParams.pSessionId = pSessionId;
		pSessionList.ppSessionParams.push_back(pSessionParams);
	}
	pSessionList.nCount = length;
	return pimi->SetSessionListReaded(pSessionList);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetTotalUnreadNum (JNIEnv* env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();
	return pimi->GetTotalUnreadNum();
}

JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_UpdateDraft (JNIEnv * env, jobject thiz,
		jstring sessionId, jstring draftContent)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pSessionId    = JString2String(env, sessionId);
	const string pDraftContent = JString2String(env, draftContent);
	return pimi->UpdateDraft(pSessionId, pDraftContent);
}

/*
 * 获取消息列表
 */
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetMessageList (JNIEnv * env, jobject thiz,
		jstring sessionId, jstring lastMsgId, jint pageSize)
{
	CIMI* pimi = CIMI::GetInstance();

	const string pSessionId = JString2String(env, sessionId);
	const string pLastMsgId = "";
	if (lastMsgId != NULL)
	{
		pLastMsgId = JString2String(env, lastMsgId);
	}
	int nPageSize  = pageSize;
//	LOGD ("pSessionId=%s, pLastMstId=%s, nPageSize=%d\r\n", pSessionId, plastMsgId, nPageSize);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MessageParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pMsgId",     "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pSessionId", "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pSender",    "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pSenderName","Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pSenderKey", "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pReceiver",  "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pContent",   "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "pTime",      "Ljava/lang/String;");
	jfieldID  fid_9  = env->GetFieldID(cls, "eType",      "I");
	jfieldID  fid_10 = env->GetFieldID(cls, "eKind",      "I");
	jfieldID  fid_11 = env->GetFieldID(cls, "eCategory",  "I");
	jfieldID  fid_12 = env->GetFieldID(cls, "eDirection", "I");
	jfieldID  fid_13 = env->GetFieldID(cls, "eUnread",    "I");
	jfieldID  fid_14 = env->GetFieldID(cls, "ePlayed",    "I");
	jfieldID  fid_15 = env->GetFieldID(cls, "eSend",      "I");
	jfieldID  fid_16 = env->GetFieldID(cls, "eOnline",    "I");
	jfieldID  fid_17 = env->GetFieldID(cls, "pIsSave",    "I");
	jfieldID  fid_18 = env->GetFieldID(cls, "pIsShare",    "I");

	jobjectArray obj_arr = NULL;

	MessageList pMessageList = pimi->GetMessageList(pSessionId, pLastMsgId, nPageSize);
	if (pMessageList.nCount > 0)
	{
//		LOGD ("real message count=%d\r\n", pMessageList.nCount);

		jclass clsstring = env->FindClass("java/lang/Object");
		obj_arr = env->NewObjectArray (pMessageList.nCount, clsstring, NULL);
		env->DeleteLocalRef (clsstring);

		for (int i=0; i<pMessageList.nCount; i++)
		{
			MessageParams pMessageParams = pMessageList.ppMessageParams[i];

//			LOGD ("[%d]: pMsgId=%s, pSessionId=%s, pSender=%s, pSenderName=%s, pSenderKey=%s, pReceiver=%s, "
//					"pContent=%s, pTime=%s, eMessageType=%d, eKindType=%d, eCategoryType=%d, eDirection=%d, eUnread=%d, "
//					"ePlayed=%d, eSend=%d, eOnline=%d\r\n",
//					i,
//					pMessageParams.pMsgId,
//					pMessageParams.pSessionId,
//					pMessageParams.pSender,
//					pMessageParams.pSenderName,
//					pMessageParams.pSenderKey,
//					pMessageParams.pReceiver,
//					pMessageParams.pContent,
//					pMessageParams.pTime,
//					pMessageParams.eType,
//					pMessageParams.eKind,
//					pMessageParams.eCategory,
//					pMessageParams.eDirection,
//					pMessageParams.eUnread,
//					pMessageParams.ePlayed,
//					pMessageParams.eSend,
//					pMessageParams.eOnline);

			jobject   obj   = env->NewObject(cls, mid);
			jstring str_1 = String2JString(env, pMessageParams.pMsgId);
			jstring str_2 = String2JString(env, pMessageParams.pSessionId);
			jstring str_3 = String2JString(env, pMessageParams.pSender);
			jstring str_4 = String2JString(env, pMessageParams.pSenderName);
			jstring str_5 = String2JString(env, pMessageParams.pSenderKey);
			jstring str_6 = String2JString(env, pMessageParams.pReceiver);
			jstring str_7 = String2JString(env, pMessageParams.pContent);
			jstring str_8 = String2JString(env, pMessageParams.pTime);
			if (str_1)
			{
				env->SetObjectField(obj, fid_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField(obj, fid_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField(obj, fid_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			if (str_4)
			{
				env->SetObjectField(obj, fid_4, str_4);
				env->DeleteLocalRef (str_4);
			}
			if (str_5)
			{
				env->SetObjectField(obj, fid_5, str_5);
				env->DeleteLocalRef (str_5);
			}
			if (str_6)
			{
				env->SetObjectField(obj, fid_6, str_6);
				env->DeleteLocalRef (str_6);
			}
			if (str_7)
			{
				env->SetObjectField(obj, fid_7, str_7);
				env->DeleteLocalRef (str_7);
			}
			if (str_8)
			{
				env->SetObjectField(obj, fid_8, str_8);
				env->DeleteLocalRef (str_8);
			}
			env->SetIntField(obj, fid_9,  pMessageParams.eType);
			env->SetIntField(obj, fid_10, pMessageParams.eKind);
			env->SetIntField(obj, fid_11, pMessageParams.eCategory);
			env->SetIntField(obj, fid_12, pMessageParams.eDirection);
			env->SetIntField(obj, fid_13, pMessageParams.eUnread);
			env->SetIntField(obj, fid_14, pMessageParams.ePlayed);
			env->SetIntField(obj, fid_15, pMessageParams.eSend);
			env->SetIntField(obj, fid_16, pMessageParams.eOnline);
			env->SetIntField(obj, fid_17, pMessageParams.pIsSave);
			env->SetIntField(obj, fid_18, pMessageParams.pIsShare);

			env->SetObjectArrayElement (obj_arr, i, obj);
			env->DeleteLocalRef (obj);

		}
	}

	// free ref
	env->DeleteLocalRef (cls);
	return obj_arr;
}

JNIEXPORT jobject JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetMessage (JNIEnv * env, jobject thiz,
		jstring sessionId, jstring msgId, jint updateUnreadNum)
{
	CIMI* pimi = CIMI::GetInstance();

	const string pSessionId = JString2String(env, sessionId);
	const string pMsgId     = JString2String(env, msgId);
//	LOGD ("pSessionId=%s, msgId=%s\r\n", pSessionId, pMsgId);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MessageParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pMsgId",     "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pSessionId", "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pSender",    "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pSenderName","Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pSenderKey", "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pReceiver",  "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pContent",   "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "pTime",      "Ljava/lang/String;");
	jfieldID  fid_9  = env->GetFieldID(cls, "eType",      "I");
	jfieldID  fid_10 = env->GetFieldID(cls, "eKind",      "I");
	jfieldID  fid_11 = env->GetFieldID(cls, "eCategory",  "I");
	jfieldID  fid_12 = env->GetFieldID(cls, "eDirection", "I");
	jfieldID  fid_13 = env->GetFieldID(cls, "eUnread",    "I");
	jfieldID  fid_14 = env->GetFieldID(cls, "ePlayed",    "I");
	jfieldID  fid_15 = env->GetFieldID(cls, "eSend",      "I");
	jfieldID  fid_16 = env->GetFieldID(cls, "eOnline",    "I");
	jfieldID  fid_17 = env->GetFieldID(cls, "pIsSave",    "I");
	jfieldID  fid_18 = env->GetFieldID(cls, "pIsShare",    "I");

	jobject obj = NULL;

	MessageParams pMessageParams = pimi->GetMessage(pSessionId, pMsgId, updateUnreadNum);
	if (pMessageParams.pMsgId.empty() == FALSE)
	{
//		LOGD ("pMsgId=%s, pSessionId=%s, pSender=%s, pSenderName=%s, pSenderKey=%s, pReceiver=%s, "
//				"pContent=%s, pTime=%s, eMessageType=%d, eKindType=%d, eCategoryType=%d, eDirection=%d, eUnread=%d, "
//				"ePlayed=%d, eSend=%d, eOnline=%d\r\n",
//				pMessageParams.pMsgId,
//				pMessageParams.pSessionId,
//				pMessageParams.pSender,
//				pMessageParams.pSenderName,
//				pMessageParams.pSenderKey,
//				pMessageParams.pReceiver,
//				pMessageParams.pContent,
//				pMessageParams.pTime,
//				pMessageParams.eType,
//				pMessageParams.eKind,
//				pMessageParams.eCategory,
//				pMessageParams.eDirection,
//				pMessageParams.eUnread,
//				pMessageParams.ePlayed,
//				pMessageParams.eSend,
//				pMessageParams.eOnline);

		obj   = env->NewObject(cls, mid);
		jstring str_1 = String2JString(env, pMessageParams.pMsgId);
		jstring str_2 = String2JString(env, pMessageParams.pSessionId);
		jstring str_3 = String2JString(env, pMessageParams.pSender);
		jstring str_4 = String2JString(env, pMessageParams.pSenderName);
		jstring str_5 = String2JString(env, pMessageParams.pSenderKey);
		jstring str_6 = String2JString(env, pMessageParams.pReceiver);
		jstring str_7 = String2JString(env, pMessageParams.pContent);
		jstring str_8 = String2JString(env, pMessageParams.pTime);
		if (str_1)
		{
			env->SetObjectField(obj, fid_1, str_1);
			env->DeleteLocalRef (str_1);
		}
		if (str_2)
		{
			env->SetObjectField(obj, fid_2, str_2);
			env->DeleteLocalRef (str_2);
		}
		if (str_3)
		{
			env->SetObjectField(obj, fid_3, str_3);
			env->DeleteLocalRef (str_3);
		}
		if (str_4)
		{
			env->SetObjectField(obj, fid_4, str_4);
			env->DeleteLocalRef (str_4);
		}
		if (str_5)
		{
			env->SetObjectField(obj, fid_5, str_5);
			env->DeleteLocalRef (str_5);
		}
		if (str_6)
		{
			env->SetObjectField(obj, fid_6, str_6);
			env->DeleteLocalRef (str_6);
		}
		if (str_7)
		{
			env->SetObjectField(obj, fid_7, str_7);
			env->DeleteLocalRef (str_7);
		}
		if (str_8)
		{
			env->SetObjectField(obj, fid_8, str_8);
			env->DeleteLocalRef (str_8);
		}
		env->SetIntField(obj, fid_9,  pMessageParams.eType);
		env->SetIntField(obj, fid_10, pMessageParams.eKind);
		env->SetIntField(obj, fid_11, pMessageParams.eCategory);
		env->SetIntField(obj, fid_12, pMessageParams.eDirection);
		env->SetIntField(obj, fid_13, pMessageParams.eUnread);
		env->SetIntField(obj, fid_14, pMessageParams.ePlayed);
		env->SetIntField(obj, fid_15, pMessageParams.eSend);
		env->SetIntField(obj, fid_16, pMessageParams.eOnline);
		env->SetIntField(obj, fid_17, pMessageParams.pIsSave);
		env->SetIntField(obj, fid_18, pMessageParams.pIsShare);

//		env->DeleteLocalRef (obj);
	}

	// free ref
	env->DeleteLocalRef (cls);
	return obj;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteMessage (JNIEnv* env, jobject thiz,
		jstring sessionId, jstring msgId, jstring imagekey, jstring videokey)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pSessionId = JString2String(env, sessionId);
	const string pMsgId     = JString2String(env, msgId);
	const string image     = JString2String(env, imagekey);
	const string video     = JString2String(env, videokey);
	return pimi->DeleteMessage(pSessionId, pMsgId, image, video);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetAudioPlayed (JNIEnv* env, jobject thiz,
		jstring sessionId, jstring msgId)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pSessionId = JString2String(env, sessionId);
	const string pMsgId     = JString2String(env, msgId);
	return pimi->SetAudioPlayed(pSessionId, pMsgId);
}

/*
 * 获取特定消息字段列表
 */
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGivenFieldList (JNIEnv * env, jobject thiz,
		jstring sessionId, jint givenField, jint givenCondition)
{
	CIMI* pimi = CIMI::GetInstance();

	const string pSessionId = JString2String(env, sessionId);
	int nGivenField  = givenField;
	int nGivenCondition = givenCondition;
//	LOGD ("pSessionId=%s, nGivenField=%d, nGivenConditon=%d\r\n", pSessionId.c_str(), nGivenField, nGivenCondition);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MessageParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pMsgId",     "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pSessionId", "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pSender",    "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pSenderName","Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pSenderKey", "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pReceiver",  "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pContent",   "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "pTime",      "Ljava/lang/String;");
	jfieldID  fid_9  = env->GetFieldID(cls, "eType",      "I");
	jfieldID  fid_10 = env->GetFieldID(cls, "eKind",      "I");
	jfieldID  fid_11 = env->GetFieldID(cls, "eCategory",  "I");
	jfieldID  fid_12 = env->GetFieldID(cls, "eDirection", "I");
	jfieldID  fid_13 = env->GetFieldID(cls, "eUnread",    "I");
	jfieldID  fid_14 = env->GetFieldID(cls, "ePlayed",    "I");
	jfieldID  fid_15 = env->GetFieldID(cls, "eSend",      "I");
	jfieldID  fid_16 = env->GetFieldID(cls, "eOnline",    "I");

	jobjectArray obj_arr = NULL;

	MessageList pMessageList = pimi->GetGivenFieldList(pSessionId, nGivenField, nGivenCondition);
	if (pMessageList.nCount > 0)
	{
//		LOGD ("real message count=%d\r\n", pMessageList.nCount);

		jclass clsstring = env->FindClass("java/lang/Object");
		obj_arr = env->NewObjectArray (pMessageList.nCount, clsstring, NULL);
		env->DeleteLocalRef (clsstring);

		for (int i=0; i<pMessageList.nCount; i++)
		{
			MessageParams pMessageParams = pMessageList.ppMessageParams[i];

//			LOGD ("[%d]: pMsgId=%s, pSessionId=%s, pSender=%s, pSenderName=%s, pSenderKey=%s, pReceiver=%s, "
//					"pContent=%s, pTime=%s, eMessageType=%d, eKindType=%d, eCategoryType=%d, eDirection=%d, eUnread=%d, "
//					"ePlayed=%d, eSend=%d, eOnline=%d\r\n",
//					i,
//					pMessageParams.pMsgId,
//					pMessageParams.pSessionId,
//					pMessageParams.pSender,
//					pMessageParams.pSenderName,
//					pMessageParams.pSenderKey,
//					pMessageParams.pReceiver,
//					pMessageParams.pContent,
//					pMessageParams.pTime,
//					pMessageParams.eType,
//					pMessageParams.eKind,
//					pMessageParams.eCategory,
//					pMessageParams.eDirection,
//					pMessageParams.eUnread,
//					pMessageParams.ePlayed,
//					pMessageParams.eSend,
//					pMessageParams.eOnline);

			jobject   obj   = env->NewObject(cls, mid);
			jstring str_1 = String2JString(env, pMessageParams.pMsgId);
			jstring str_2 = String2JString(env, pMessageParams.pSessionId);
			jstring str_3 = String2JString(env, pMessageParams.pSender);
			jstring str_4 = String2JString(env, pMessageParams.pSenderName);
			jstring str_5 = String2JString(env, pMessageParams.pSenderKey);
			jstring str_6 = String2JString(env, pMessageParams.pReceiver);
			jstring str_7 = String2JString(env, pMessageParams.pContent);
			jstring str_8 = String2JString(env, pMessageParams.pTime);
			if (str_1)
			{
				env->SetObjectField(obj, fid_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField(obj, fid_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField(obj, fid_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			if (str_4)
			{
				env->SetObjectField(obj, fid_4, str_4);
				env->DeleteLocalRef (str_4);
			}
			if (str_5)
			{
				env->SetObjectField(obj, fid_5, str_5);
				env->DeleteLocalRef (str_5);
			}
			if (str_6)
			{
				env->SetObjectField(obj, fid_6, str_6);
				env->DeleteLocalRef (str_6);
			}
			if (str_7)
			{
				env->SetObjectField(obj, fid_7, str_7);
				env->DeleteLocalRef (str_7);
			}
			if (str_8)
			{
				env->SetObjectField(obj, fid_8, str_8);
				env->DeleteLocalRef (str_8);
			}
			env->SetIntField(obj, fid_9,  pMessageParams.eType);
			env->SetIntField(obj, fid_10, pMessageParams.eKind);
			env->SetIntField(obj, fid_11, pMessageParams.eCategory);
			env->SetIntField(obj, fid_12, pMessageParams.eDirection);
			env->SetIntField(obj, fid_13, pMessageParams.eUnread);
			env->SetIntField(obj, fid_14, pMessageParams.ePlayed);
			env->SetIntField(obj, fid_15, pMessageParams.eSend);
			env->SetIntField(obj, fid_16, pMessageParams.eOnline);

			env->SetObjectArrayElement (obj_arr, i, obj);
			env->DeleteLocalRef (obj);

		}
	}

	// free ref
	env->DeleteLocalRef (cls);
	return obj_arr;
}

/*
 * 获取特定会话中的所有离线消息
 */
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetOfflineMessageList (JNIEnv * env, jobject thiz,
		jstring sessionId)
{
	CIMI* pimi = CIMI::GetInstance();

	const string pSessionId = JString2String(env, sessionId);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MessageParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pMsgId",     "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pSessionId", "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pSender",    "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pSenderName","Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pSenderKey", "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pReceiver",  "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pContent",   "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "pTime",      "Ljava/lang/String;");
	jfieldID  fid_9  = env->GetFieldID(cls, "eType",      "I");
	jfieldID  fid_10 = env->GetFieldID(cls, "eKind",      "I");
	jfieldID  fid_11 = env->GetFieldID(cls, "eCategory",  "I");
	jfieldID  fid_12 = env->GetFieldID(cls, "eDirection", "I");
	jfieldID  fid_13 = env->GetFieldID(cls, "eUnread",    "I");
	jfieldID  fid_14 = env->GetFieldID(cls, "ePlayed",    "I");
	jfieldID  fid_15 = env->GetFieldID(cls, "eSend",      "I");
	jfieldID  fid_16 = env->GetFieldID(cls, "eOnline",    "I");

	jobjectArray obj_arr = NULL;

	MessageList pMessageList = pimi->GetOfflineMessageList(pSessionId);
	if (pMessageList.nCount > 0)
	{
//		LOGD ("real message count=%d\r\n", pMessageList.nCount);

		jclass clsstring = env->FindClass("java/lang/Object");
		obj_arr = env->NewObjectArray (pMessageList.nCount, clsstring, NULL);
		env->DeleteLocalRef (clsstring);

		for (int i=0; i<pMessageList.nCount; i++)
		{
			MessageParams pMessageParams = pMessageList.ppMessageParams[i];

//			LOGD ("[%d]: pMsgId=%s, pSessionId=%s, pSender=%s, pSenderName=%s, pSenderKey=%s, pReceiver=%s, "
//					"pContent=%s, pTime=%s, eMessageType=%d, eKindType=%d, eCategoryType=%d, eDirection=%d, eUnread=%d, "
//					"ePlayed=%d, eSend=%d, eOnline=%d\r\n",
//					i,
//					pMessageParams.pMsgId.c_str(),
//					pMessageParams.pSessionId.c_str(),
//					pMessageParams.pSender.c_str(),
//					pMessageParams.pSenderName.c_str(),
//					pMessageParams.pSenderKey.c_str(),
//					pMessageParams.pReceiver.c_str(),
//					pMessageParams.pContent.c_str(),
//					pMessageParams.pTime.c_str(),
//					pMessageParams.eType,
//					pMessageParams.eKind,
//					pMessageParams.eCategory,
//					pMessageParams.eDirection,
//					pMessageParams.eUnread,
//					pMessageParams.ePlayed,
//					pMessageParams.eSend,
//					pMessageParams.eOnline);

			jobject   obj   = env->NewObject(cls, mid);
			jstring str_1 = String2JString(env, pMessageParams.pMsgId);
			jstring str_2 = String2JString(env, pMessageParams.pSessionId);
			jstring str_3 = String2JString(env, pMessageParams.pSender);
			jstring str_4 = String2JString(env, pMessageParams.pSenderName);
			jstring str_5 = String2JString(env, pMessageParams.pSenderKey);
			jstring str_6 = String2JString(env, pMessageParams.pReceiver);
			jstring str_7 = String2JString(env, pMessageParams.pContent);
			jstring str_8 = String2JString(env, pMessageParams.pTime);
			if (str_1)
			{
				env->SetObjectField(obj, fid_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField(obj, fid_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField(obj, fid_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			if (str_4)
			{
				env->SetObjectField(obj, fid_4, str_4);
				env->DeleteLocalRef (str_4);
			}
			if (str_5)
			{
				env->SetObjectField(obj, fid_5, str_5);
				env->DeleteLocalRef (str_5);
			}
			if (str_6)
			{
				env->SetObjectField(obj, fid_6, str_6);
				env->DeleteLocalRef (str_6);
			}
			if (str_7)
			{
				env->SetObjectField(obj, fid_7, str_7);
				env->DeleteLocalRef (str_7);
			}
			if (str_8)
			{
				env->SetObjectField(obj, fid_8, str_8);
				env->DeleteLocalRef (str_8);
			}
			env->SetIntField(obj, fid_9,  pMessageParams.eType);
			env->SetIntField(obj, fid_10, pMessageParams.eKind);
			env->SetIntField(obj, fid_11, pMessageParams.eCategory);
			env->SetIntField(obj, fid_12, pMessageParams.eDirection);
			env->SetIntField(obj, fid_13, pMessageParams.eUnread);
			env->SetIntField(obj, fid_14, pMessageParams.ePlayed);
			env->SetIntField(obj, fid_15, pMessageParams.eSend);
			env->SetIntField(obj, fid_16, pMessageParams.eOnline);

			env->SetObjectArrayElement (obj_arr, i, obj);
			env->DeleteLocalRef (obj);

		}
	}

	// free ref
	env->DeleteLocalRef (cls);
	return obj_arr;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_OperateGroup (JNIEnv * env, jobject, jobject object)
{
	CIMI* pimi = CIMI::GetInstance();

	ReqParams reqParams;

	jclass cls = env->GetObjectClass(object);
	jfieldID fid_1 = env->GetFieldID(cls, "eReqType", "I");
	jfieldID fid_2 = env->GetFieldID(cls, "objReq", "Ljava/lang/Object;");
	int intVal = env->GetIntField(object, fid_1);
//	LOGD ("intVal=%d\r\n", intVal);
	reqParams.eReqType = (enum ReqType)intVal;
	HRESULT hResult;
	switch (reqParams.eReqType)
	{
		case RT_ADD_GROUP:
		{
			LOGD ("RT_ADD_GROUP Start\r\n");
			jobject obj2 = env->GetObjectField(object, fid_2);
			jclass cls2 = env->GetObjectClass (obj2);
			//jfieldID  fid2_1  = env->GetFieldID(cls2, "GroupName", "Ljava/lang/String;");
			jfieldID  fid2_2  = env->GetFieldID(cls2, "GroupPortraitKey", "Ljava/lang/String;");
			jfieldID  fid2_3  = env->GetFieldID(cls2, "MemberInfo", "Ljava/lang/Object;");
			jfieldID  fid2_4  = env->GetFieldID(cls2, "Userlist", "Ljava/lang/Object;");

//			CHAR* groupName;
//			jstring gname = env->GetObjectField(obj2, fid2_1);
//			if(gname)
//			{
//				groupName = JString2String(env, gname);
//				LOGD ("groupName %s\r\n",groupName);
//			}

			jstring gkey = (jstring)env->GetObjectField(obj2, fid2_2);
			string groupKey = "";
			if(gkey)
			{
				groupKey = JString2String(env, gkey);
				LOGD ("groupKey %s\r\n",groupKey.c_str());
			}

			jobject obj3 = env->GetObjectField(obj2, fid2_3);
			jclass cls3 = env->GetObjectClass (obj3);
			jfieldID  fid3_1  = env->GetFieldID(cls3, "MemberID", "Ljava/lang/String;");
			jfieldID  fid3_2  = env->GetFieldID(cls3, "MemberName", "Ljava/lang/String;");
			jfieldID  fid3_3  = env->GetFieldID(cls3, "PortraitKey", "Ljava/lang/String;");

			string ownerid = "";
			jstring mid = (jstring)env->GetObjectField(obj3, fid3_1);
			if(mid)
			{
				ownerid = JString2String(env, mid);
				LOGD ("ownerid %s\r\n",ownerid.c_str());
			}

			jstring mname = (jstring)env->GetObjectField(obj3, fid3_2);

			string ownername = "";
			if(mname)
			{
				ownername = JString2String(env, mname);
				LOGD ("ownername %s\r\n",ownername.c_str());
			}

			jstring mkey = (jstring)env->GetObjectField(obj3, fid3_3);

			string ownerkey = "";
			if(mkey)
			{
			    ownerkey = JString2String(env, mkey);
				LOGD ("ownerkey %s\r\n",ownerkey.c_str());
			}

			MemberParams ownerInfo;
			ownerInfo.MemberID = ownerid;
			ownerInfo.MemberName = ownername;
			ownerInfo.PortraitKey = ownerkey;

			jobjectArray inviterlist = (jobjectArray)env->GetObjectField(obj2, fid2_4);

			int size = env->GetArrayLength(inviterlist);

			vector<MemberParams> memberlist;

			for (int i = 0 ; i<size; i++)
			{
				jobject obj4 = (env->GetObjectArrayElement(inviterlist, i));
				jclass cls4 = env->GetObjectClass (obj4);

				jfieldID  fid4_1  = env->GetFieldID(cls4, "MemberID", "Ljava/lang/String;");
				jfieldID  fid4_2  = env->GetFieldID(cls4, "MemberName", "Ljava/lang/String;");
				jfieldID  fid4_3  = env->GetFieldID(cls4, "PortraitKey", "Ljava/lang/String;");

				string memberid = "";
				jstring mid = (jstring)env->GetObjectField(obj4, fid4_1);
				if(mid)
				{
					 memberid = JString2String(env, mid);
					 LOGD ("memberid %s\r\n",memberid.c_str());
				}

				jstring mname = (jstring)env->GetObjectField(obj4, fid4_2);

				string membername = "";
				if(mname)
				{
					membername = JString2String(env, mname);
					LOGD ("membername %s\r\n",membername.c_str());
				}

				jstring mkey = (jstring)env->GetObjectField(obj4, fid4_3);

				string memberkey = "";
				if(mkey)
				{
					memberkey = JString2String(env, mkey);
					LOGD ("memberkey %s\r\n",memberkey.c_str());
				}

				MemberParams memberInfo;
				memberInfo.MemberID = memberid;
				memberInfo.MemberName = membername;
				memberInfo.PortraitKey = memberkey;

				memberlist.push_back(memberInfo);
				LOGD ("memberlist push_back\r\n");

				env->DeleteLocalRef (obj4);
				env->DeleteLocalRef (cls4);
			}

			GroupParams groupParams;
			//groupParams.GroupName = groupName;
			groupParams.GroupPortraitKey = groupKey;
			groupParams.MemberInfo = ownerInfo;
			groupParams.Userlist = memberlist;

			reqParams.pGroupParams = &groupParams;

			// free ref
			env->DeleteLocalRef (cls);
			env->DeleteLocalRef (cls2);
			env->DeleteLocalRef (cls3);
			env->DeleteLocalRef (obj2);
			env->DeleteLocalRef (obj3);

			hResult = pimi->OperateGroup(&reqParams);
			LOGD ("RT_ADD_GROUP End\r\n");
		}
		break;
		case RT_INVITE_GROUP:
		{
			jobject obj2 = env->GetObjectField(object, fid_2);
			jclass cls2 = env->GetObjectClass (obj2);

			jfieldID  fid2_1  = env->GetFieldID(cls2, "GroupID", "Ljava/lang/String;");
			//jfieldID  fid2_2  = env->GetFieldID(cls2, "GroupName", "Ljava/lang/String;");
			jfieldID  fid2_3  = env->GetFieldID(cls2, "GroupPortraitKey", "Ljava/lang/String;");
			jfieldID  fid2_4  = env->GetFieldID(cls2, "MemberInfo", "Ljava/lang/Object;");
			jfieldID  fid2_5  = env->GetFieldID(cls2, "Userlist", "Ljava/lang/Object;");

			jstring gid = (jstring)env->GetObjectField(obj2, fid2_1);
			//jstring gname = env->GetObjectField(obj2, fid2_2);
			jstring gkey = (jstring)env->GetObjectField(obj2, fid2_3);

			string groupid = "";
			if(gid)
			{
				groupid = JString2String(env, gid);
			}

			//CHAR* groupname = JString2String(env, gname);
			string groupkey = "";
			if(gkey)
			{
				groupkey = JString2String(env, gkey);
			}

			jobject obj3 = env->GetObjectField(obj2, fid2_4);
			jclass cls3 = env->GetObjectClass (obj3);

			jfieldID  fid3_1  = env->GetFieldID(cls3, "MemberID", "Ljava/lang/String;");
			jfieldID  fid3_2  = env->GetFieldID(cls3, "MemberName", "Ljava/lang/String;");
			jfieldID  fid3_3  = env->GetFieldID(cls3, "PortraitKey", "Ljava/lang/String;");

			jstring mid = (jstring)env->GetObjectField(obj3, fid3_1);
			string inviterid = "";
			if(mid)
			{
				 inviterid = JString2String(env, mid);
			}

			jstring mname = (jstring)env->GetObjectField(obj3, fid3_2);
			string invitername = "";
			if(mname)
			{
				invitername = JString2String(env, mname);
			}

			jstring mkey = (jstring)env->GetObjectField(obj3, fid3_3);
			string inviterkey = "";
			if(mkey)
			{
				inviterkey = JString2String(env, mkey);
			}

			MemberParams inviterInfo;
			inviterInfo.MemberID = inviterid;
			inviterInfo.MemberName = invitername;
			inviterInfo.PortraitKey = inviterkey;

			jobjectArray inviterlist = (jobjectArray)env->GetObjectField(obj2, fid2_5);

			int size = env->GetArrayLength(inviterlist);

			vector<MemberParams> memberlist;

			for (int i = 0 ; i<size; i++)
			{
				jobject obj4 = (env->GetObjectArrayElement(inviterlist, i));
				jclass cls4 = env->GetObjectClass (obj4);

				jfieldID  fid4_1  = env->GetFieldID(cls4, "MemberID", "Ljava/lang/String;");
				jfieldID  fid4_2  = env->GetFieldID(cls4, "MemberName", "Ljava/lang/String;");
				jfieldID  fid4_3  = env->GetFieldID(cls4, "PortraitKey", "Ljava/lang/String;");

				jstring mid = (jstring)env->GetObjectField(obj4, fid4_1);
				string memberid = "";
				if(mid)
				{
					memberid = JString2String(env, mid);
				}

				jstring mname = (jstring)env->GetObjectField(obj4, fid4_2);
				string membername = "";
				if(mname)
				{
				   membername = JString2String(env, mname);
				}

				jstring mkey = (jstring)env->GetObjectField(obj4, fid4_3);
				string memberkey = "";
				if(mkey)
				{
				   memberkey = JString2String(env, mkey);
				}

				MemberParams memberInfo;
				memberInfo.MemberID = memberid;
				memberInfo.MemberName = membername;
				memberInfo.PortraitKey = memberkey;

				memberlist.push_back(memberInfo);

				env->DeleteLocalRef (obj4);
				env->DeleteLocalRef (cls4);
			}

			GroupParams groupParams;
			//groupParams.GroupName = groupname;
			groupParams.GroupID = groupid;
			groupParams.GroupPortraitKey = groupkey;
			groupParams.MemberInfo = inviterInfo;
			groupParams.Userlist = memberlist;

			reqParams.pGroupParams = &groupParams;

			// free ref
			env->DeleteLocalRef (cls);
			env->DeleteLocalRef (cls2);
			env->DeleteLocalRef (cls3);
			env->DeleteLocalRef (obj2);
			env->DeleteLocalRef (obj3);

			hResult = pimi->OperateGroup(&reqParams);
		}

			break;
		case RT_MODIFY_GROUP:
		{
			jobject obj2 = env->GetObjectField(object, fid_2);
			jclass cls2 = env->GetObjectClass (obj2);

			jfieldID  fid2_1  = env->GetFieldID(cls2, "GroupID", "Ljava/lang/String;");
			jfieldID  fid2_2  = env->GetFieldID(cls2, "GroupName", "Ljava/lang/String;");
			jfieldID  fid2_3  = env->GetFieldID(cls2, "MemberInfo", "Ljava/lang/Object;");

			jstring gid = (jstring)env->GetObjectField(obj2, fid2_1);
			jstring gname = (jstring)env->GetObjectField(obj2, fid2_2);

			string groupid = "";
			if(gid)
			{
			    groupid = JString2String(env, gid);
			}

			string groupname = "";
			if(gname)
			{
			   groupname = JString2String(env, gname);
			}

			jobject obj3 = env->GetObjectField(obj2, fid2_3);
			jclass cls3 = env->GetObjectClass (obj3);

			jfieldID  fid3_1  = env->GetFieldID(cls3, "MemberID", "Ljava/lang/String;");
			jfieldID  fid3_2  = env->GetFieldID(cls3, "MemberName", "Ljava/lang/String;");

			jstring mid = (jstring)env->GetObjectField(obj3, fid3_1);
			string modifyid = "";
			if(mid)
			{
				modifyid = JString2String(env, mid);
			}

			jstring mname = (jstring)env->GetObjectField(obj3, fid3_2);
			string modifyname = "";
			if(mname)
			{
			   modifyname = JString2String(env, mname);
			}

			MemberParams modifyInfo;
			modifyInfo.MemberID = modifyid;
			modifyInfo.MemberName = modifyname;

			GroupParams groupParams;
			groupParams.GroupName = groupname;
			groupParams.GroupID = groupid;
			groupParams.MemberInfo = modifyInfo;

			reqParams.pGroupParams = &groupParams;

			// free ref
			env->DeleteLocalRef (cls);

			hResult = pimi->OperateGroup(&reqParams);
		}
		break;
		case RT_MODIFY_GROUPMEMBER_NAME:
		{
			jobject obj2 = env->GetObjectField(object, fid_2);
			jclass cls2 = env->GetObjectClass (obj2);

			jfieldID  fid2_1  = env->GetFieldID(cls2, "GroupID", "Ljava/lang/String;");
			jfieldID  fid2_2  = env->GetFieldID(cls2, "MemberInfo", "Ljava/lang/Object;");

			jstring gid = (jstring)env->GetObjectField(obj2, fid2_1);

			string groupid = "";
			if(gid)
			{
				groupid = JString2String(env, gid);
			}

			jobject obj3 = env->GetObjectField(obj2, fid2_2);
			jclass cls3 = env->GetObjectClass (obj3);

			jfieldID  fid3_1  = env->GetFieldID(cls3, "MemberID", "Ljava/lang/String;");
			jfieldID  fid3_2  = env->GetFieldID(cls3, "MemberCardName", "Ljava/lang/String;");

			jstring mid = (jstring)env->GetObjectField(obj3, fid3_1);
			string modifyid = "";
			if(mid)
			{
				modifyid = JString2String(env, mid);
			}

			jstring mname = (jstring)env->GetObjectField(obj3, fid3_2);
			string modifyname = "";
			if(mname)
			{
			   modifyname = JString2String(env, mname);
			}

			MemberParams modifyInfo;
			modifyInfo.MemberID = modifyid;
			modifyInfo.MemberCardName = modifyname;

			GroupParams groupParams;
			groupParams.GroupID = groupid;
			groupParams.MemberInfo = modifyInfo;

			reqParams.pGroupParams = &groupParams;

			// free ref
			env->DeleteLocalRef (cls);

			hResult = pimi->OperateGroup(&reqParams);
		}
		break;
		case RT_REMOVE_GROUP:
		{
			jobject obj2 = env->GetObjectField(object, fid_2);
			jclass cls2 = env->GetObjectClass (obj2);

			jfieldID  fid2_1  = env->GetFieldID(cls2, "GroupID", "Ljava/lang/String;");
			jfieldID  fid2_2  = env->GetFieldID(cls2, "GroupName", "Ljava/lang/String;");
			jfieldID  fid2_3  = env->GetFieldID(cls2, "MemberInfo", "Ljava/lang/Object;");
			jfieldID  fid2_4  = env->GetFieldID(cls2, "GroupPortraitKey", "Ljava/lang/String;");

			jstring gid = (jstring)env->GetObjectField(obj2, fid2_1);
			jstring gname = (jstring)env->GetObjectField(obj2, fid2_2);
			jstring gkey = (jstring)env->GetObjectField(obj2, fid2_4);

			string groupid;
			if(gid)
			{
			   groupid = JString2String(env, gid);
			}

			string groupname;
			if(gname)
			{
			   groupname = JString2String(env, gname);
			}

			string groupkey;
			if(gkey)
			{
				groupkey = JString2String(env, gkey);
			}

			jobject obj3 = env->GetObjectField(obj2, fid2_3);
			jclass cls3 = env->GetObjectClass (obj3);

			jfieldID  fid3_1  = env->GetFieldID(cls3, "MemberID", "Ljava/lang/String;");
			jfieldID  fid3_2  = env->GetFieldID(cls3, "MemberName", "Ljava/lang/String;");

			jstring mid = (jstring)env->GetObjectField(obj3, fid3_1);
			string Exitid;
			if(mid)
			{
			    Exitid = JString2String(env, mid);
			}

			jstring mname = (jstring)env->GetObjectField(obj3, fid3_2);
			string Exitname;
			if(mname)
			{
				Exitname = JString2String(env, mname);
			}

			MemberParams Info;
			Info.MemberID = Exitid;
			Info.MemberName = Exitname;

			GroupParams groupParams;
			groupParams.GroupName = groupname;
			groupParams.GroupID = groupid;
			groupParams.GroupPortraitKey = groupkey;
			groupParams.MemberInfo = Info;

			reqParams.pGroupParams = &groupParams;

			// free ref
			env->DeleteLocalRef (cls);

			hResult = pimi->OperateGroup(&reqParams);
		}
		break;

		default:
		break;
	}

	return hResult;
}

JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGroupList (JNIEnv * env, jobject, jobject)
{
	CIMI* pimi = CIMI::GetInstance();

	vector<GroupParams> grouplist;
	grouplist = pimi->GetGroupList();

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$GroupParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1 = env->GetFieldID(cls, "GroupID", "Ljava/lang/String;");
	jfieldID  fid_2 = env->GetFieldID(cls, "GroupName", "Ljava/lang/String;");
	jfieldID  fid_3 = env->GetFieldID(cls, "GroupPortraitKey", "Ljava/lang/String;");
	jfieldID  fid_4 = env->GetFieldID(cls, "GroupMaxMember", "Ljava/lang/String;");
	jfieldID  fid_5 = env->GetFieldID(cls, "OwnerID", "Ljava/lang/String;");
	jfieldID  fid_6 = env->GetFieldID(cls, "GroupMemberCount", "Ljava/lang/String;");

	jobjectArray g_group_obj_arr = NULL;

	jclass clsstring = env->FindClass("java/lang/Object");
	g_group_obj_arr = env->NewObjectArray (grouplist.size(), clsstring, NULL);
	env->DeleteLocalRef (clsstring);

	for (unsigned int i = 0; i < grouplist.size(); i++)
	{
		jobject   obj   = env->NewObject(cls, mid);

		jstring groupid = String2JString(env,grouplist[i].GroupID.c_str());
		jstring groupname = String2JString(env,grouplist[i].GroupName.c_str());
		jstring groupportraitkey = String2JString(env,grouplist[i].GroupPortraitKey.c_str());
		jstring membercount = String2JString(env,grouplist[i].GroupMemberCount.c_str());
		jstring memberMaxcount = String2JString(env,grouplist[i].GroupMaxMember.c_str());
		jstring ownerid = String2JString(env,grouplist[i].OwnerID.c_str());

		if (groupid)
		{
			env->SetObjectField(obj, fid_1, groupid);
			env->DeleteLocalRef (groupid);
		}
		if (groupname)
		{
			env->SetObjectField(obj, fid_2, groupname);
			env->DeleteLocalRef (groupname);
		}
		if (groupportraitkey)
		{
			env->SetObjectField(obj, fid_3, groupportraitkey);
			env->DeleteLocalRef (groupportraitkey);
		}

		if (memberMaxcount)
		{
			env->SetObjectField(obj, fid_4, memberMaxcount);
			env->DeleteLocalRef (memberMaxcount);
		}
		if (ownerid)
		{
			env->SetObjectField(obj, fid_5, ownerid);
			env->DeleteLocalRef (ownerid);
		}
		if (membercount)
		{
			env->SetObjectField(obj, fid_6, membercount);
			env->DeleteLocalRef (membercount);
		}

		env->SetObjectArrayElement (g_group_obj_arr, i, obj);
		env->DeleteLocalRef (obj);
	}

	return g_group_obj_arr;

}
JNIEXPORT jobject JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGroupInfo(JNIEnv * env, jobject,jstring groupid)
{
	CIMI* pimi = CIMI::GetInstance();

	string groupID = JString2String(env, groupid);
	GroupParams groupinfo = pimi->GetGroupInfo(groupID);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$GroupParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1 = env->GetFieldID(cls, "GroupID", "Ljava/lang/String;");
	jfieldID  fid_2 = env->GetFieldID(cls, "GroupName", "Ljava/lang/String;");
	jfieldID  fid_3 = env->GetFieldID(cls, "GroupPortraitKey", "Ljava/lang/String;");
	jfieldID  fid_4 = env->GetFieldID(cls, "GroupMaxMember", "Ljava/lang/String;");
	jfieldID  fid_5 = env->GetFieldID(cls, "OwnerID", "Ljava/lang/String;");
	jfieldID  fid_6 = env->GetFieldID(cls, "GroupMemberCount", "Ljava/lang/String;");

	jobject   obj   = env->NewObject(cls, mid);

	jstring gid = String2JString(env,groupinfo.GroupID.c_str());
	jstring groupname = String2JString(env,groupinfo.GroupName.c_str());
	jstring groupportraitkey = String2JString(env,groupinfo.GroupPortraitKey.c_str());
	jstring membercount = String2JString(env,groupinfo.GroupMemberCount.c_str());
	jstring memberMaxcount = String2JString(env,groupinfo.GroupMaxMember.c_str());
	jstring ownerid = String2JString(env,groupinfo.OwnerID.c_str());

	if (gid)
	{
		env->SetObjectField(obj, fid_1, gid);
		env->DeleteLocalRef (gid);
	}
	if (groupname)
	{
		env->SetObjectField(obj, fid_2, groupname);
		env->DeleteLocalRef (groupname);
	}
	if (groupportraitkey)
	{
		env->SetObjectField(obj, fid_3, groupportraitkey);
		env->DeleteLocalRef (groupportraitkey);
	}

	if (memberMaxcount)
	{
		env->SetObjectField(obj, fid_4, memberMaxcount);
		env->DeleteLocalRef (memberMaxcount);
	}
	if (ownerid)
	{
		env->SetObjectField(obj, fid_5, ownerid);
		env->DeleteLocalRef (ownerid);
	}
	if (membercount)
	{
		env->SetObjectField(obj, fid_6, membercount);
		env->DeleteLocalRef (membercount);
	}

	env->DeleteLocalRef (cls);

	return obj;
}
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGroupMemberList (JNIEnv * env, jobject,jstring groupid)
{
	string groupID = JString2String(env, groupid);

	CIMI* pimi = CIMI::GetInstance();

	vector<MemberParams> memberlist;
	memberlist = pimi->GetGroupMemberList(groupID);

	jobjectArray g_member_obj_arr = NULL;

	jclass clsstring = env->FindClass("java/lang/Object");
	g_member_obj_arr = env->NewObjectArray (memberlist.size(), clsstring, NULL);
	env->DeleteLocalRef (clsstring);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MemberParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1 = env->GetFieldID(cls, "MemberID", "Ljava/lang/String;");
	jfieldID  fid_2 = env->GetFieldID(cls, "MemberName", "Ljava/lang/String;");
	jfieldID  fid_3 = env->GetFieldID(cls, "PortraitKey", "Ljava/lang/String;");
	jfieldID  fid_4 = env->GetFieldID(cls, "EmotionMood", "Ljava/lang/String;");
	jfieldID  fid_5 = env->GetFieldID(cls, "CmsID", "Ljava/lang/String;");
	jfieldID  fid_6 = env->GetFieldID(cls, "IsBuddy", "I");
	jfieldID  fid_7 = env->GetFieldID(cls, "MemberCardName", "Ljava/lang/String;");


	for (unsigned int i = 0; i < memberlist.size(); i++)
	{

		jobject   obj   = env->NewObject(cls, mid);
		jstring memberid = String2JString(env,memberlist[i].MemberID.c_str());
		jstring membername = String2JString(env,memberlist[i].MemberName.c_str());
		jstring memberkey = String2JString(env,memberlist[i].PortraitKey.c_str());
		jstring membermood = String2JString(env,memberlist[i].EmotionMood.c_str());
		jstring cmsid = String2JString(env,memberlist[i].CmsID.c_str());
		jstring membercardname = String2JString(env,memberlist[i].MemberCardName.c_str());

		if (memberid)
		{
			env->SetObjectField(obj, fid_1, memberid);
			env->DeleteLocalRef (memberid);
		}
		if (membername)
		{
			env->SetObjectField(obj, fid_2, membername);
			env->DeleteLocalRef (membername);
		}
		if (memberkey)
		{
			env->SetObjectField(obj, fid_3, memberkey);
			env->DeleteLocalRef (memberkey);
		}
		if (membermood)
		{
			env->SetObjectField(obj, fid_4, membermood);
			env->DeleteLocalRef (membermood);
		}

		if (cmsid)
		{
			env->SetObjectField(obj, fid_5, cmsid);
			env->DeleteLocalRef (cmsid);
		}

		env->SetIntField(obj, fid_6, memberlist[i].IsBuddy);

		if (membercardname)
		{
			env->SetObjectField(obj, fid_7, membercardname);
			env->DeleteLocalRef (membercardname);
		}

		env->SetObjectArrayElement (g_member_obj_arr, i, obj);
		env->DeleteLocalRef (obj);
	}

	return g_member_obj_arr;
}

JNIEXPORT jobject JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetGroupMemberInfo (JNIEnv * env, jobject,jstring memberid,jstring groupid, jint loadFromServer)
{
	CIMI* pimi = CIMI::GetInstance();

	string mid = JString2String(env, memberid);
	string gid = JString2String(env, groupid);
	MemberParams memberinfo = pimi->GetGroupMemberInfo(mid,gid,loadFromServer);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$MemberParams");
	jmethodID methodid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1 = env->GetFieldID(cls, "MemberID", "Ljava/lang/String;");
	jfieldID  fid_2 = env->GetFieldID(cls, "MemberName", "Ljava/lang/String;");
	jfieldID  fid_3 = env->GetFieldID(cls, "PortraitKey", "Ljava/lang/String;");
	jfieldID  fid_4 = env->GetFieldID(cls, "EmotionMood", "Ljava/lang/String;");
	jfieldID  fid_5 = env->GetFieldID(cls, "CmsID", "Ljava/lang/String;");
	jfieldID  fid_6 = env->GetFieldID(cls, "IsBuddy", "I");
	jfieldID  fid_7 = env->GetFieldID(cls, "MemberCardName", "Ljava/lang/String;");

	jobject   obj   = env->NewObject(cls, methodid);
	jstring gmemberid = String2JString(env,memberinfo.MemberID.c_str());
	jstring membername = String2JString(env,memberinfo.MemberName.c_str());
	jstring memberkey = String2JString(env,memberinfo.PortraitKey.c_str());
	jstring membermood = String2JString(env,memberinfo.EmotionMood.c_str());
	jstring cmsid = String2JString(env,memberinfo.CmsID.c_str());
	jstring membercardname = String2JString(env,memberinfo.MemberCardName.c_str());

	if (gmemberid)
	{
		env->SetObjectField(obj, fid_1, gmemberid);
		env->DeleteLocalRef (gmemberid);
	}
	if (membername)
	{
		env->SetObjectField(obj, fid_2, membername);
		env->DeleteLocalRef (membername);
	}

	if (memberkey)
	{
		env->SetObjectField(obj, fid_3, memberkey);
		env->DeleteLocalRef (memberkey);
	}
	if (membermood)
	{
		env->SetObjectField(obj, fid_4, membermood);
		env->DeleteLocalRef (membermood);
	}
	if (cmsid)
	{
		env->SetObjectField(obj, fid_5, cmsid);
		env->DeleteLocalRef (cmsid);
	}

	env->SetIntField(obj, fid_6, memberinfo.IsBuddy);

	if (membercardname)
	{
		env->SetObjectField(obj, fid_7, membercardname);
		env->DeleteLocalRef (membercardname);
	}
	//env->DeleteLocalRef (memberinfo.IsBuddy);

	env->DeleteLocalRef (cls);

	return obj;

}

JNIEXPORT jobject JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetBuddy (JNIEnv * env, jobject thiz, jstring userId, jint loadFromServer)
{
	CIMI* pimi = CIMI::GetInstance();

	const string pUserId = JString2String(env, userId);
//	LOGD ("pUserId=%s\r\n", pUserId);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$BuddyParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pUserId",       "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pUserName",     "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey",  "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pEmotionMood",  "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pCmsId",        "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pQrerUrl",      "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pCommunityUrl", "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "eIsBuddy",      "I");
	jfieldID  fid_9  = env->GetFieldID(cls, "pIsSave", 		 "Ljava/lang/String;");
	jfieldID  fid_10  = env->GetFieldID(cls, "pIsShare",     "Ljava/lang/String;");
	jfieldID  fid_11  = env->GetFieldID(cls, "pSubname",     "Ljava/lang/String;");

	jobject g_obj = NULL;

	BuddyParams pBuddyParams = pimi->GetBuddy(pUserId, loadFromServer);
	if (pBuddyParams.pUserId.empty() == FALSE)
	{
//		LOGD ("pUserId=%s, pUserName=%s, pPortraitKey=%s, pEmotionMood=%s, "
//			  "pCmsId=%s, pQrerUrl=%s, pCommunityUrl=%s, eIsBuddy=%d\r\n",
//				pBuddyParams.pUserId,
//				pBuddyParams.pUserName,
//				pBuddyParams.pPortraitKey,
//				pBuddyParams.pEmotionMood,
//				pBuddyParams.pCmsId,
//				pBuddyParams.pQrerUrl,
//				pBuddyParams.pCommunityUrl,
//				pBuddyParams.eIsBuddy);

		g_obj   = env->NewObject(cls, mid);
		jstring str_1 = String2JString(env, pBuddyParams.pUserId);
		jstring str_2 = String2JString(env, pBuddyParams.pUserName);
		jstring str_3 = String2JString(env, pBuddyParams.pPortraitKey);
		jstring str_4 = String2JString(env, pBuddyParams.pEmotionMood);
		jstring str_5 = String2JString(env, pBuddyParams.pCmsId);
		jstring str_6 = String2JString(env, pBuddyParams.pQrerUrl);
		jstring str_7 = String2JString(env, pBuddyParams.pCommunityUrl);
		if (str_1)
		{
			env->SetObjectField(g_obj, fid_1, str_1);
			env->DeleteLocalRef (str_1);
		}
		if (str_2)
		{
			env->SetObjectField(g_obj, fid_2, str_2);
			env->DeleteLocalRef (str_2);
		}
		if (str_3)
		{
			env->SetObjectField(g_obj, fid_3, str_3);
			env->DeleteLocalRef (str_3);
		}
		if (str_4)
		{
			env->SetObjectField(g_obj, fid_4, str_4);
			env->DeleteLocalRef (str_4);
		}
		if (str_5)
		{
			env->SetObjectField(g_obj, fid_5, str_5);
			env->DeleteLocalRef (str_5);
		}
		if (str_6)
		{
			env->SetObjectField(g_obj, fid_6, str_6);
			env->DeleteLocalRef (str_6);
		}
		if (str_7)
		{
			env->SetObjectField(g_obj, fid_7, str_7);
			env->DeleteLocalRef (str_7);
		}
		env->SetIntField(g_obj, fid_8, pBuddyParams.eIsBuddy);

		jstring str_9 = String2JString(env, pBuddyParams.pIsSave);
		if (str_9)
		{
			env->SetObjectField(g_obj, fid_9, str_9);
			env->DeleteLocalRef (str_9);
		}
		jstring str_10 = String2JString(env, pBuddyParams.pIsShare);
		if (str_10)
		{
			env->SetObjectField(g_obj, fid_10, str_10);
			env->DeleteLocalRef (str_10);
		}

		jstring str_11 = String2JString(env, pBuddyParams.pSubname);
		if (str_11)
		{
			env->SetObjectField(g_obj, fid_11, str_11);
			env->DeleteLocalRef (str_11);
		}
//		env->DeleteLocalRef (g_obj);
	}

	// free ref
	env->DeleteLocalRef (cls);
	return g_obj;
}

/*
 * 获取好友列表
 */
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetBuddyList (JNIEnv * env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$BuddyParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pUserId",       "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pUserName",     "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey",  "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pEmotionMood",  "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pCmsId",        "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pQrerUrl",      "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pCommunityUrl", "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "eIsBuddy",      "I");
	jfieldID  fid_9  = env->GetFieldID(cls, "pIsSave", 		 "Ljava/lang/String;");
	jfieldID  fid_10  = env->GetFieldID(cls, "pIsShare",     "Ljava/lang/String;");
	jfieldID  fid_11  = env->GetFieldID(cls, "pSubname",     "Ljava/lang/String;");

	jobjectArray g_obj_arr = NULL;

	BuddyList pBuddyList = pimi->GetBuddyList();
	if (pBuddyList.nCount > 0)
	{
		LOGD ("buddy count=%d\r\n", pBuddyList.nCount);
		jclass clsstring = env->FindClass("java/lang/Object");
		g_obj_arr = env->NewObjectArray (pBuddyList.nCount, clsstring, NULL);
		env->DeleteLocalRef (clsstring);

		for (int i=0; i<pBuddyList.nCount; i++)
		{
			BuddyParams pBuddyParams = pBuddyList.ppBuddyParams[i];
//			LOGD ("[%d]: pUserId=%s, pUserName=%s, pPortraitKey=%s, pEmotionMood=%s, "
//					"pCmsId=%s, pQrerUrl=%s, pCommunityUrl=%s, eIsBuddy=%d\r\n",
//					i + 1,
//					pBuddyParams.pUserId.c_str(),
//					pBuddyParams.pUserName.c_str(),
//					pBuddyParams.pPortraitKey.c_str(),
//					pBuddyParams.pEmotionMood.c_str(),
//					pBuddyParams.pCmsId.c_str(),
//					pBuddyParams.pQrerUrl.c_str(),
//					pBuddyParams.pCommunityUrl.c_str(),
//					pBuddyParams.eIsBuddy);

			jobject   obj   = env->NewObject(cls, mid);
			jstring str_1 = String2JString(env, pBuddyParams.pUserId);
			jstring str_2 = String2JString(env, pBuddyParams.pUserName);
			jstring str_3 = String2JString(env, pBuddyParams.pPortraitKey);
			jstring str_4 = String2JString(env, pBuddyParams.pEmotionMood);
			jstring str_5 = String2JString(env, pBuddyParams.pCmsId);
			jstring str_6 = String2JString(env, pBuddyParams.pQrerUrl);
			jstring str_7 = String2JString(env, pBuddyParams.pCommunityUrl);
			if (str_1)
			{
				env->SetObjectField(obj, fid_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField(obj, fid_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField(obj, fid_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			if (str_4)
			{
				env->SetObjectField(obj, fid_4, str_4);
				env->DeleteLocalRef (str_4);
			}
			if (str_5)
			{
				env->SetObjectField(obj, fid_5, str_5);
				env->DeleteLocalRef (str_5);
			}
			if (str_6)
			{
				env->SetObjectField(obj, fid_6, str_6);
				env->DeleteLocalRef (str_6);
			}
			if (str_7)
			{
				env->SetObjectField(obj, fid_7, str_7);
				env->DeleteLocalRef (str_7);
			}
			env->SetIntField(obj, fid_8, pBuddyParams.eIsBuddy);

			jstring str_9 = String2JString(env, pBuddyParams.pIsSave);
			if (str_9)
			{
				env->SetObjectField(obj, fid_9, str_9);
				env->DeleteLocalRef (str_9);
			}
			jstring str_10 = String2JString(env, pBuddyParams.pIsShare);
			if (str_10)
			{
				env->SetObjectField(obj, fid_10, str_10);
				env->DeleteLocalRef (str_10);
			}

			jstring str_11 = String2JString(env, pBuddyParams.pSubname);
			if (str_11)
			{
				env->SetObjectField(obj, fid_11, str_11);
				env->DeleteLocalRef (str_11);
			}
			env->SetObjectArrayElement (g_obj_arr, i, obj);
			env->DeleteLocalRef (obj);
		}
	}

	// free ref
	env->DeleteLocalRef (cls);
	return g_obj_arr;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_AddBuddy (JNIEnv * env, jobject thiz, jstring userId)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pUserId = JString2String(env, userId);
	return pimi->AddBuddy(pUserId);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ModifyBuddy (JNIEnv * env, jobject thiz, jobject obj)
{
	CIMI* pimi = CIMI::GetInstance();

	BuddyParams buddyParams = {};

	jclass cls = env->GetObjectClass(obj);
	jfieldID  fid_1  = env->GetFieldID(cls, "pUserId",       "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pUserName",     "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey",  "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pEmotionMood",  "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pCmsId",        "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pQrerUrl",      "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pCommunityUrl", "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "eIsBuddy",      "I");
	jstring str_1 = env->GetObjectField(obj, fid_1);
	jstring str_2 = env->GetObjectField(obj, fid_2);
	jstring str_3 = env->GetObjectField(obj, fid_3);
	jstring str_4 = env->GetObjectField(obj, fid_4);
	jstring str_5 = env->GetObjectField(obj, fid_5);
	jstring str_6 = env->GetObjectField(obj, fid_6);
	jstring str_7 = env->GetObjectField(obj, fid_7);
	if (str_1)
	{
		buddyParams.pUserId       = JString2String(env, str_1);
	}
	if (str_2)
	{
		buddyParams.pUserName     = JString2String(env, str_2);
	}
	if (str_3)
	{
		buddyParams.pPortraitKey  = JString2String(env, str_3);
	}
	if (str_4)
	{
		buddyParams.pEmotionMood  = JString2String(env, str_4);
	}
	if (str_5)
	{
		buddyParams.pCmsId        = JString2String(env, str_5);
	}
	if (str_6)
	{
		buddyParams.pQrerUrl      = JString2String(env, str_6);
	}
	if (str_7)
	{
		buddyParams.pCommunityUrl = JString2String(env, str_7);
	}
	buddyParams.eIsBuddy          = env->GetIntField(obj, fid_8);

	// free ref
	env->DeleteLocalRef (cls);

	HRESULT hResult = S_OK;//pimi->ModifyBuddy(buddyParams);

	if (str_1)
	{
		env->DeleteLocalRef (str_1);
	}
	if (str_2)
	{
		env->DeleteLocalRef (str_2);
	}
	if (str_3)
	{
		env->DeleteLocalRef (str_3);
	}
	if (str_4)
	{
		env->DeleteLocalRef (str_4);
	}
	if (str_5)
	{
		env->DeleteLocalRef (str_5);
	}
	if (str_6)
	{
		env->DeleteLocalRef (str_6);
	}
	if (str_7)
	{
		env->DeleteLocalRef (str_7);
	}

	return hResult;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_DeleteBuddy (JNIEnv * env, jobject thiz, jstring userId)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pUserId = JString2String(env, userId);
	return pimi->DeleteBuddy(pUserId);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_IsBuddyExist (JNIEnv * env, jobject thiz, jstring userId)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pUserId = JString2String(env, userId);
	return pimi->IsBuddyExist(pUserId);
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SearchContact (JNIEnv * env, jobject thiz, jstring key)
{
	CIMI* pimi = CIMI::GetInstance();
	const string pKey = JString2String(env, key);
	return pimi->SearchContact(pKey);
}

/*
 * 获取查询到的联系人列表
 */
JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSearchContact (JNIEnv * env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$BuddyParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pUserId",       "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pUserName",     "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey",  "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pEmotionMood",  "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pCmsId",        "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pQrerUrl",      "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pCommunityUrl", "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "eIsBuddy",      "I");
	jfieldID  fid_9  = env->GetFieldID(cls, "pEmail", "Ljava/lang/String;");
	jfieldID  fid_10  = env->GetFieldID(cls, "pMobile", "Ljava/lang/String;");

	jobjectArray g_obj_arr = NULL;

	BuddyList pBuddyList = pimi->GetSearchContact();
	if (pBuddyList.nCount > 0)
	{
		LOGD ("buddy count=%d\r\n", pBuddyList.nCount);
		jclass clsstring = env->FindClass("java/lang/Object");
		g_obj_arr = env->NewObjectArray (pBuddyList.nCount, clsstring, NULL);
		env->DeleteLocalRef (clsstring);

		for (int i=0; i<pBuddyList.nCount; i++)
		{
			BuddyParams pBuddyParams = pBuddyList.ppBuddyParams[i];
//			LOGD ("[%d]: pUserId=%s, pUserName=%s, pPortraitKey=%s, pEmotionMood=%s, "
//					"pCmsId=%s, pQrerUrl=%s, pCommunityUrl=%s, eIsBuddy=%d\r\n",
//					i + 1,
//					pBuddyParams.pUserId.c_str(),
//					pBuddyParams.pUserName.c_str(),
//					pBuddyParams.pPortraitKey.c_str(),
//					pBuddyParams.pEmotionMood.c_str(),
//					pBuddyParams.pCmsId.c_str(),
//					pBuddyParams.pQrerUrl.c_str(),
//					pBuddyParams.pCommunityUrl.c_str(),
//					pBuddyParams.eIsBuddy);

			jobject   obj   = env->NewObject(cls, mid);
			jstring str_1 = String2JString(env, pBuddyParams.pUserId);
			jstring str_2 = String2JString(env, pBuddyParams.pUserName);
			jstring str_3 = String2JString(env, pBuddyParams.pPortraitKey);
			jstring str_4 = String2JString(env, pBuddyParams.pEmotionMood);
			jstring str_5 = String2JString(env, pBuddyParams.pCmsId);
			jstring str_6 = String2JString(env, pBuddyParams.pQrerUrl);
			jstring str_7 = String2JString(env, pBuddyParams.pCommunityUrl);
			jstring str_9 = String2JString(env, pBuddyParams.pEmail);
			jstring str_10 = String2JString(env, pBuddyParams.pMobile);
			if (str_1)
			{
				env->SetObjectField(obj, fid_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField(obj, fid_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField(obj, fid_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			if (str_4)
			{
				env->SetObjectField(obj, fid_4, str_4);
				env->DeleteLocalRef (str_4);
			}
			if (str_5)
			{
				env->SetObjectField(obj, fid_5, str_5);
				env->DeleteLocalRef (str_5);
			}
			if (str_6)
			{
				env->SetObjectField(obj, fid_6, str_6);
				env->DeleteLocalRef (str_6);
			}
			if (str_7)
			{
				env->SetObjectField(obj, fid_7, str_7);
				env->DeleteLocalRef (str_7);
			}

			env->SetIntField(obj, fid_8, pBuddyParams.eIsBuddy);

			if (str_9)
			{
				env->SetObjectField(obj, fid_9, str_9);
				env->DeleteLocalRef (str_9);
			}
			if (str_10)
			{
				env->SetObjectField(obj, fid_10, str_10);
				env->DeleteLocalRef (str_10);
			}

			env->SetObjectArrayElement (g_obj_arr, i, obj);
			env->DeleteLocalRef (obj);
		}
	}

	// free ref
	env->DeleteLocalRef (cls);
	return g_obj_arr;
}

JNIEXPORT jobject JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetContact (JNIEnv * env, jobject thiz, jstring userId, BOOL bLoadFromServer)
{
	CIMI* pimi = CIMI::GetInstance();

	const string pUserId = JString2String(env, userId);
//	LOGD ("pUserId=%s\r\n", pUserId);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$BuddyParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pUserId",       "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pUserName",     "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey",  "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pEmotionMood",  "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pCmsId",        "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pQrerUrl",      "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pCommunityUrl", "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "eIsBuddy",      "I");
	jfieldID  fid_9  = env->GetFieldID(cls, "pIsSave", 		 "Ljava/lang/String;");
	jfieldID  fid_10  = env->GetFieldID(cls, "pIsShare",     "Ljava/lang/String;");
	jfieldID  fid_11  = env->GetFieldID(cls, "pSubname",     "Ljava/lang/String;");

	jobject g_obj = NULL;

	BuddyParams pBuddyParams = pimi->GetContact(pUserId, bLoadFromServer);
	if (pBuddyParams.pUserId.empty() == FALSE)
	{
//		LOGD ("pUserId=%s, pUserName=%s, pPortraitKey=%s, pEmotionMood=%s, "
//			  "pCmsId=%s, pQrerUrl=%s, pCommunityUrl=%s, eIsBuddy=%d\r\n",
//				pBuddyParams.pUserId,
//				pBuddyParams.pUserName,
//				pBuddyParams.pPortraitKey,
//				pBuddyParams.pEmotionMood,
//				pBuddyParams.pCmsId,
//				pBuddyParams.pQrerUrl,
//				pBuddyParams.pCommunityUrl,
//				pBuddyParams.eIsBuddy);

		g_obj   = env->NewObject(cls, mid);
		jstring str_1 = String2JString(env, pBuddyParams.pUserId);
		jstring str_2 = String2JString(env, pBuddyParams.pUserName);
		jstring str_3 = String2JString(env, pBuddyParams.pPortraitKey);
		jstring str_4 = String2JString(env, pBuddyParams.pEmotionMood);
		jstring str_5 = String2JString(env, pBuddyParams.pCmsId);
		jstring str_6 = String2JString(env, pBuddyParams.pQrerUrl);
		jstring str_7 = String2JString(env, pBuddyParams.pCommunityUrl);
		if (str_1)
		{
			env->SetObjectField(g_obj, fid_1, str_1);
			env->DeleteLocalRef (str_1);
		}
		if (str_2)
		{
			env->SetObjectField(g_obj, fid_2, str_2);
			env->DeleteLocalRef (str_2);
		}
		if (str_3)
		{
			env->SetObjectField(g_obj, fid_3, str_3);
			env->DeleteLocalRef (str_3);
		}
		if (str_4)
		{
			env->SetObjectField(g_obj, fid_4, str_4);
			env->DeleteLocalRef (str_4);
		}
		if (str_5)
		{
			env->SetObjectField(g_obj, fid_5, str_5);
			env->DeleteLocalRef (str_5);
		}
		if (str_6)
		{
			env->SetObjectField(g_obj, fid_6, str_6);
			env->DeleteLocalRef (str_6);
		}
		if (str_7)
		{
			env->SetObjectField(g_obj, fid_7, str_7);
			env->DeleteLocalRef (str_7);
		}
		env->SetIntField(g_obj, fid_8, pBuddyParams.eIsBuddy);

		jstring str_9 = String2JString(env, pBuddyParams.pIsSave);
		if (str_9)
		{
			env->SetObjectField(g_obj, fid_9, str_9);
			env->DeleteLocalRef (str_9);
		}
		jstring str_10 = String2JString(env, pBuddyParams.pIsShare);
		if (str_10)
		{
			env->SetObjectField(g_obj, fid_10, str_10);
			env->DeleteLocalRef (str_10);
		}

		jstring str_11 = String2JString(env, pBuddyParams.pSubname);
		if (str_11)
		{
			env->SetObjectField(g_obj, fid_11, str_11);
			env->DeleteLocalRef (str_11);
		}

//		env->DeleteLocalRef (g_obj);
	}

	// free ref
	env->DeleteLocalRef (cls);
	return g_obj;
}

JNIEXPORT jobject JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetFans (JNIEnv * env, jobject thiz, jstring userId, jint loadFromServer)
{
	CIMI* pimi = CIMI::GetInstance();

	const string pUserId = JString2String(env, userId);
//	LOGD ("pUserId=%s\r\n", pUserId);

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$BuddyParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pUserId",       "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pUserName",     "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey",  "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pEmotionMood",  "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pCmsId",        "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pQrerUrl",      "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pCommunityUrl", "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "eIsBuddy",      "I");

	jobject g_obj = NULL;

	BuddyParams pBuddyParams = pimi->GetFans(pUserId, loadFromServer);
	if (pBuddyParams.pUserId.empty() == FALSE)
	{
//		LOGD ("pUserId=%s, pUserName=%s, pPortraitKey=%s, pEmotionMood=%s, "
//			  "pCmsId=%s, pQrerUrl=%s, pCommunityUrl=%s, eIsBuddy=%d\r\n",
//				pBuddyParams.pUserId,
//				pBuddyParams.pUserName,
//				pBuddyParams.pPortraitKey,
//				pBuddyParams.pEmotionMood,
//				pBuddyParams.pCmsId,
//				pBuddyParams.pQrerUrl,
//				pBuddyParams.pCommunityUrl,
//				pBuddyParams.eIsBuddy);

		g_obj   = env->NewObject(cls, mid);
		jstring str_1 = String2JString(env, pBuddyParams.pUserId);
		jstring str_2 = String2JString(env, pBuddyParams.pUserName);
		jstring str_3 = String2JString(env, pBuddyParams.pPortraitKey);
		jstring str_4 = String2JString(env, pBuddyParams.pEmotionMood);
		jstring str_5 = String2JString(env, pBuddyParams.pCmsId);
		jstring str_6 = String2JString(env, pBuddyParams.pQrerUrl);
		jstring str_7 = String2JString(env, pBuddyParams.pCommunityUrl);
		if (str_1)
		{
			env->SetObjectField(g_obj, fid_1, str_1);
			env->DeleteLocalRef (str_1);
		}
		if (str_2)
		{
			env->SetObjectField(g_obj, fid_2, str_2);
			env->DeleteLocalRef (str_2);
		}
		if (str_3)
		{
			env->SetObjectField(g_obj, fid_3, str_3);
			env->DeleteLocalRef (str_3);
		}
		if (str_4)
		{
			env->SetObjectField(g_obj, fid_4, str_4);
			env->DeleteLocalRef (str_4);
		}
		if (str_5)
		{
			env->SetObjectField(g_obj, fid_5, str_5);
			env->DeleteLocalRef (str_5);
		}
		if (str_6)
		{
			env->SetObjectField(g_obj, fid_6, str_6);
			env->DeleteLocalRef (str_6);
		}
		if (str_7)
		{
			env->SetObjectField(g_obj, fid_7, str_7);
			env->DeleteLocalRef (str_7);
		}
		env->SetIntField(g_obj, fid_8, pBuddyParams.eIsBuddy);

//		env->DeleteLocalRef (g_obj);
	}

	// free ref
	env->DeleteLocalRef (cls);
	return g_obj;
}

JNIEXPORT jobjectArray JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetFansList (JNIEnv * env, jobject thiz)
{
	CIMI* pimi = CIMI::GetInstance();

	jclass    cls   = env->FindClass("cn/com/xmatrix/ii/imsdkjni/IMSDKJni$BuddyParams");
	jmethodID mid   = env->GetMethodID(cls, "<init>","()V");
	jfieldID  fid_1  = env->GetFieldID(cls, "pUserId",       "Ljava/lang/String;");
	jfieldID  fid_2  = env->GetFieldID(cls, "pUserName",     "Ljava/lang/String;");
	jfieldID  fid_3  = env->GetFieldID(cls, "pPortraitKey",  "Ljava/lang/String;");
	jfieldID  fid_4  = env->GetFieldID(cls, "pEmotionMood",  "Ljava/lang/String;");
	jfieldID  fid_5  = env->GetFieldID(cls, "pCmsId",        "Ljava/lang/String;");
	jfieldID  fid_6  = env->GetFieldID(cls, "pQrerUrl",      "Ljava/lang/String;");
	jfieldID  fid_7  = env->GetFieldID(cls, "pCommunityUrl", "Ljava/lang/String;");
	jfieldID  fid_8  = env->GetFieldID(cls, "eIsBuddy",      "I");

	jobjectArray g_obj_arr = NULL;

	BuddyList pBuddyList = pimi->GetFansList();
	if (pBuddyList.nCount > 0)
	{
		LOGD ("buddy count=%d\r\n", pBuddyList.nCount);
		jclass clsstring = env->FindClass("java/lang/Object");
		g_obj_arr = env->NewObjectArray (pBuddyList.nCount, clsstring, NULL);
		env->DeleteLocalRef (clsstring);

		for (int i=0; i<pBuddyList.nCount; i++)
		{
			BuddyParams pBuddyParams = pBuddyList.ppBuddyParams[i];
//			LOGD ("[%d]: pUserId=%s, pUserName=%s, pPortraitKey=%s, pEmotionMood=%s, "
//					"pCmsId=%s, pQrerUrl=%s, pCommunityUrl=%s, eIsBuddy=%d\r\n",
//					i + 1,
//					pBuddyParams.pUserId.c_str(),
//					pBuddyParams.pUserName.c_str(),
//					pBuddyParams.pPortraitKey.c_str(),
//					pBuddyParams.pEmotionMood.c_str(),
//					pBuddyParams.pCmsId.c_str(),
//					pBuddyParams.pQrerUrl.c_str(),
//					pBuddyParams.pCommunityUrl.c_str(),
//					pBuddyParams.eIsBuddy);

			jobject   obj   = env->NewObject(cls, mid);
			jstring str_1 = String2JString(env, pBuddyParams.pUserId);
			jstring str_2 = String2JString(env, pBuddyParams.pUserName);
			jstring str_3 = String2JString(env, pBuddyParams.pPortraitKey);
			jstring str_4 = String2JString(env, pBuddyParams.pEmotionMood);
			jstring str_5 = String2JString(env, pBuddyParams.pCmsId);
			jstring str_6 = String2JString(env, pBuddyParams.pQrerUrl);
			jstring str_7 = String2JString(env, pBuddyParams.pCommunityUrl);
			if (str_1)
			{
				env->SetObjectField(obj, fid_1, str_1);
				env->DeleteLocalRef (str_1);
			}
			if (str_2)
			{
				env->SetObjectField(obj, fid_2, str_2);
				env->DeleteLocalRef (str_2);
			}
			if (str_3)
			{
				env->SetObjectField(obj, fid_3, str_3);
				env->DeleteLocalRef (str_3);
			}
			if (str_4)
			{
				env->SetObjectField(obj, fid_4, str_4);
				env->DeleteLocalRef (str_4);
			}
			if (str_5)
			{
				env->SetObjectField(obj, fid_5, str_5);
				env->DeleteLocalRef (str_5);
			}
			if (str_6)
			{
				env->SetObjectField(obj, fid_6, str_6);
				env->DeleteLocalRef (str_6);
			}
			if (str_7)
			{
				env->SetObjectField(obj, fid_7, str_7);
				env->DeleteLocalRef (str_7);
			}
			env->SetIntField(obj, fid_8, pBuddyParams.eIsBuddy);
			env->SetObjectArrayElement (g_obj_arr, i, obj);
			env->DeleteLocalRef (obj);
		}
	}

	// free ref
	env->DeleteLocalRef (cls);
	return g_obj_arr;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetNewFansCount(JNIEnv *, jobject)
{
	CIMI* pimi = CIMI::GetInstance();
	int newfanscount = pimi->GetNewFansCount();
	return newfanscount;
}
JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ClearNewFansCount(JNIEnv *, jobject)
{
	CIMI* pimi = CIMI::GetInstance();
	pimi->ClearNewFansCount();
	return 0;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_GetSnsCount(JNIEnv *, jobject,jint snstype)
{
	CIMI* pimi = CIMI::GetInstance();
	int snscount = pimi->GetSnsCount(snstype);
	return snscount;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_ClearSnsCount(JNIEnv *, jobject,jint snstype)
{
	CIMI* pimi = CIMI::GetInstance();
	pimi->ClearSnsCount(snstype);
	return 0;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetSavePath(JNIEnv * env, jobject thiz, jstring filepath)
{
	CIMI* pimi = CIMI::GetInstance();
	string path = JString2String(env, filepath);
	pimi->SetSavePath(path);

	return 0;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetMapPoint(JNIEnv * env, jobject thiz,jstring xpoint,jstring ypoint)
{
	CIMI* pimi = CIMI::GetInstance();
	string x = JString2String(env, xpoint);
	string y = JString2String(env, ypoint);
	pimi->SetMapPoint(x, y);

	return 0;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetFileServer(JNIEnv * env, jobject thiz, jstring srvip, jint srvport)
{
	CIMI* pimi = CIMI::GetInstance();
	int port = srvport;
	string ip = JString2String(env, srvip);
	pimi->SetFileServer(ip, port);

	return 0;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SendImage(JNIEnv * env, jobject thiz,
		jstring fromuser, jstring touser, jstring imagepath, jstring txtbuffer, jint imagewidth, jint imageheight, jint msgtype)
{
	CIMI* pimi = CIMI::GetInstance();
	string to   = JString2String(env, touser);
	string txt   = JString2String(env, txtbuffer);
	string from   = JString2String(env, fromuser);
	string image = JString2String(env, imagepath);

	pimi->SendImage(from, to, image, txt, imagewidth, imageheight, msgtype);

	return 0;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SendVideo(JNIEnv * env, jobject thiz,
		jstring fromuser, jstring touser, jstring videopath, jstring faceimagepath, jstring txtbuffer, jstring videotitle, jint videolength, jint msgtype)
{
	CIMI* pimi = CIMI::GetInstance();
	string to   = JString2String(env, touser);
	string txt   = JString2String(env, txtbuffer);
	string from   = JString2String(env, fromuser);
	string video = JString2String(env, videopath);
	string title = JString2String(env, videotitle);
	string image = JString2String(env, faceimagepath);

	pimi->SendVideo(from, to, video, image, txt, title, videolength, msgtype);

	return 0;
}

JNIEXPORT jint JNICALL Java_cn_com_xmatrix_ii_imsdkjni_IMSDKJni_SetBlackListFlag(JNIEnv * env, jobject thiz, jstring blackid, jint flag)
{
	CIMI* pimi = CIMI::GetInstance();
	int type = flag;
	string id = JString2String(env, blackid);
	pimi->SetBlackListFlag(id, type);

	return 0;
}

#ifdef __cplusplus
}
#endif
