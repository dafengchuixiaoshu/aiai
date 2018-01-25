#include "thread_env_manager.h"
#include "IMSDKJni.h"



thread_env_manager::thread_env_manager(void)
{
#ifndef WIN32
	pthread_mutexattr_t ma;
	pthread_mutexattr_init(&ma);
	pthread_mutexattr_settype (&ma, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init (&m_locker, &ma);		
#endif
}

thread_env_manager::~thread_env_manager(void)
{
	pthread_mutex_destroy (&m_locker);
}

thread_env_manager g_env_man;

JNIEnv* thread_env_manager::get_env () {
	pthread_mutex_lock (&m_locker);

	///< 当前线程id
	int t_id = pthread_self ();

	///< 根据当前线程id，查找该线程对应的env
	for (int i=0; i<m_envs.size (); i++) {
		if (m_envs[i].m_thread_id == t_id) {
			m_envs[i].m_ref_count ++;
			JNIEnv* env_ret = m_envs[i].m_env;
			pthread_mutex_unlock (&m_locker);
//			LOGD ("thread_env_manager::get_env --- use exist ....\n");
			return env_ret;
		}
	}

	///< 没有查到该线程的id，添加新的env
	thread_env te;
	g_jni_global.jvm->AttachCurrentThread (&te.m_env, NULL);
	te.m_ref_count = 1;
	te.m_thread_id = pthread_self ();
	m_envs.push_back (te);

	pthread_mutex_unlock (&m_locker);
//	LOGD ("thread_env_manager::get_env --- new create ....\n");
	return te.m_env;
}

extern "C" void on_thread_closed (int id) {
	g_env_man.on_thread_closed (id);
}

void thread_env_manager::on_thread_closed (int thread_id) {
	///< 当前线程id
	int t_id = thread_id;
	///< lock
	pthread_mutex_lock (&m_locker);

	///< 查找并删除env
	for (int i=0; i<m_envs.size (); i++) {
		if (m_envs[i].m_thread_id == t_id) {
			///< 被关闭的env的引用计数应该为0 
			if (m_envs[i].m_ref_count > 0) {
				LOGD ("xxxxxxxx error free env\n");
			}
			g_jni_global.jvm->DetachCurrentThread ();
			m_envs.erase (m_envs.begin () + i);
			break;
		}
	}

	///< unlock
	pthread_mutex_unlock (&m_locker);
}

void thread_env_manager::release_env (JNIEnv* env) {
	pthread_mutex_lock (&m_locker);

	///< 查找env
	for (int i=0; i<m_envs.size (); i++) {
		if (m_envs[i].m_env == env) {
			m_envs[i].m_ref_count --;
			break;
		}
	}
	
	pthread_mutex_unlock (&m_locker);
}

void thread_env_manager::lock_env (JNIEnv* env) {
	pthread_mutex_lock (&m_locker);

	///< 当前线程id
	int t_id = pthread_self ();

	///< 根据当前线程id，查找该线程对应的env
	for (int i=0; i<m_envs.size (); i++) {
		if (m_envs[i].m_thread_id == t_id) {
			m_envs[i].m_ref_count ++;
			pthread_mutex_unlock (&m_locker);
//			LOGD ("thread_env_manager::get_env --- use exist ....\n");
			return;
		}
	}

	///< 没有查到该线程的id，添加新的env
	thread_env te;
	te.m_env = env;
	te.m_ref_count = 1;
	te.m_thread_id = pthread_self ();
	m_envs.push_back (te);

	pthread_mutex_unlock (&m_locker);
//	LOGD ("thread_env_manager::get_env --- new create ....\n");
}

void thread_env_manager::unlock_env (JNIEnv* env) {
	pthread_mutex_lock (&m_locker);

	///< 查找env
	for (int i=0; i<m_envs.size (); i++) {
		if (m_envs[i].m_env == env) {
			m_envs[i].m_ref_count --;

			if (m_envs[i].m_ref_count <= 0) {
				m_envs.erase (m_envs.begin () + i);
//				LOGD ("thread_env_manager::release_env --- removed ....\n");
			}
			else {
//				LOGD ("thread_env_manager::release_env --- still have ref-count ....\n");
			}
			break;
		}
	}
	
	pthread_mutex_unlock (&m_locker);
}

