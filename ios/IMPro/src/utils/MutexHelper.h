#pragma once

#ifndef WIN32
#include <pthread.h>
#endif

#ifndef ANDROID
#define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
#endif

#define MUTEX_LOCK()				\
	CMutexHelper mutex_lock_helper_xxxx ( (CMutexHelperBase* ) this)

#define MUTEX_CLEAR()				\
	mutex_lock_helper_xxxx.clear ()

#define MUTEX_GLOCK()				\
	CMutexHelper_global mutex_lock_global_helper_xxxx

#define MUTEX_GCLEAR()				\
	mutex_lock_global_helper_xxxx.clear ()

#define MUTEX_GLOCK_SWITCH()		\
	CMutexHelper_GlobalSwitch mutexhelper_globalswitch_xxx;

class CMutexHelper;

#ifdef WIN32
class CMutexHelperBase
{
public:
	CMutexHelperBase () {
//		InitializeCriticalSection (&m_cs);
	}
	~CMutexHelperBase () {
//		DeleteCriticalSection (&m_cs);
	}
	friend class CMutexHelper;

protected:
//	CRITICAL_SECTION m_cs;
};

class CMutexHelper
{
public:
	CMutexHelper(CMutexHelperBase* pmb) {
//		EnterCriticalSection (&pmb->m_cs);
//		m_cs = &pmb->m_cs;
	}
	~CMutexHelper(void) {
//		if (m_cs != NULL) {
//			LeaveCriticalSection (m_cs);
//		}
	}
	void clear () {
//		m_cs = NULL;
	}

protected:
//	CRITICAL_SECTION* m_cs;
};

#else 
class CMutexHelperBase
{
public:
	CMutexHelperBase () {
		pthread_mutexattr_t ma;
		pthread_mutexattr_init(&ma);
		pthread_mutexattr_settype (&ma, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init (&m_cs, &ma);	
	}
	~CMutexHelperBase () {
		pthread_mutex_destroy  (&m_cs);
	}
	friend class CMutexHelper;

protected:
	pthread_mutex_t m_cs;
};


class CMutexHelper
{
public:
	CMutexHelper(CMutexHelperBase* pmb) {
		pthread_mutex_lock (&pmb->m_cs);
		m_cs = &pmb->m_cs;
	}
	~CMutexHelper(void) {
		if (m_cs != NULL) {
			pthread_mutex_unlock (m_cs);
		}
	}
	void clear () {
		m_cs = NULL;
	}

protected:
	pthread_mutex_t* m_cs;
};
#endif

//extern IGlobal* g_Global;
class CMutexHelper_global
{
public:
	CMutexHelper_global() {
//		if (g_Global != NULL) {
//			g_Global->GLock (TRUE);
//		}
	}
	~CMutexHelper_global(void) {
//		if (g_Global != NULL) {
//			g_Global->GLock (FALSE);
//		}
	}
	void clear () {
//		if (g_Global != NULL) {
//			g_Global->GLock (FALSE);
//		}
	}
};

//////////////////////////////////////////////////////////////////////////
///
///	@brief	该对象只用在 WaitForSingleObject (hThread)函数之前，用来为线程提供一个窗口
///
///	@author	lkx
///	@version	
///
//////////////////////////////////////////////////////////////////////////
class CMutexHelper_GlobalSwitch {
public:
	CMutexHelper_GlobalSwitch () {
		//if (g_Global != NULL) {
		//	g_Global->SwitchLock ( (UINT_PTR* ) &m_cs, &m_RecursionCount);
		//}
	}
	~CMutexHelper_GlobalSwitch () {
		//if (m_cs != NULL) {
		//	while (m_RecursionCount > 0) {
		//		g_Global->GLock (TRUE);
		//		m_RecursionCount --;
		//	}
		//}
	}

protected:
	//CRITICAL_SECTION* m_cs;
 //   LONG m_RecursionCount;
};
