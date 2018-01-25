#pragma once

#include <jni.h>
#include <pthread.h>
#include <vector>
#include "BizCommon.h"

struct thread_env {
	thread_env () {
		m_env = 0;
		m_thread_id = 0;
		m_ref_count = 0;
	}

	thread_env (const thread_env & te) {
		m_env = te.m_env;
		m_thread_id = te.m_thread_id;
		m_ref_count = te.m_ref_count;
	}

	JNIEnv* m_env;
	int m_thread_id;
	int m_ref_count;
};

/*
 * 当线程关闭之前，调用该函数，以关闭env
 */
extern "C" void on_thread_closed (int id);

class thread_env_manager
{
public:
	thread_env_manager(void);
	~thread_env_manager(void);

	/* 返回一个当前线程的env
	* @return	env
	*/
	JNIEnv* get_env ();

	/* 释放env
	* @param 
	*/
	void release_env (JNIEnv* env);

	/** 锁定当前 env （如果调用流程如下 java-->jni-->java，为了不在 jni-->java的回调完成后 释放 env, 必须在 java-->jni之前，调用 lock_env）
	* @param env	java-->jni 调用时传入的env
	*/
	void lock_env (JNIEnv* env);
	/** 对应于 lock_env （注意：此处只释放引用，并不真正释放 env，应为该env 是从 java-->jni中传入，java中还需要继续使用）
	*
	*/
	void unlock_env (JNIEnv* env);
	/** 线程结束前，关闭环境
	* 
	*/
	void on_thread_closed (int thread_id);

protected:
	std::vector<thread_env> m_envs;
	pthread_mutex_t m_locker;
};

extern thread_env_manager g_env_man;
