#include "BizCommon.h"
#include "CConfigure.h"
#include "CLog.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#if defined(USE_LOG)

CLog* CLog::g_pLog = NULL;
BOOL g_bOpenLog = FALSE;
char g_pContent[30720] = {};

CLog::CLog(void)
{
	m_fp = NULL;
}

CLog::~CLog(void)
{
	if (m_fp)
	{
		fclose (m_fp);
		m_fp = NULL;
	}
}

CLog* CLog::GetInstance()
{
	if (g_pLog == NULL)
	{
		g_pLog = new CLog();
	}
	return g_pLog;
}

HRESULT CLog::OpenLog(BOOL bOpenLog)
{
	g_bOpenLog = bOpenLog;
	return S_OK;
}

HRESULT CLog::Init()
{
	return S_OK;
}

VOID CLog::Release()
{
	if (g_pLog != NULL)
	{
		delete g_pLog;
		g_pLog = NULL;
	}
}

HRESULT CLog::OpenFile()
{
	if (g_bOpenLog == TRUE)
	{
		// open file
		string strLogFilename = "";
		CConfigure*        pConfigure     = CConfigure::GetInstance();
		string strRootDir = pConfigure->ReadProfile ("rootdir");
		Logp ("strRootDir=%s\n", strRootDir.c_str());
		if (strRootDir.length() > 0)
		{
			string strDateTime = MkIntDateTime2();
//			strLogFilename = GetLogDir(strRootDir) + LOG_FILENAME;
			strLogFilename = GetLogDir(strRootDir) + strDateTime + ".txt";
			m_fp = fopen(strLogFilename.c_str(), "w");
		}
		else
		{
			m_fp = NULL;
		}
	}
	return S_OK;
}

HRESULT CLog::PrintLog (const CHAR* content)
{
	string strLogTime = MkIntDateTime ();

#ifdef ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "libim", "%s--->%s", strLogTime.c_str(), content);
#else
	printf ("%s--->%s", strLogTime.c_str(), content);
#endif
	return S_OK;
}

HRESULT CLog::WriteRow (const CHAR* content)
{
	if (m_fp)
	{
		string strLogTime = MkIntDateTime ();
		fwrite(strLogTime.c_str(), 1, strLogTime.size(), m_fp);
		fwrite("--->", 1, 4, m_fp);
		fwrite(content, 1, strlen(content), m_fp);
		fflush (m_fp);
	}
	return S_OK;
}

HRESULT CLog::Logp (const char *format, ...)
{
	if (g_bOpenLog)
	{
		CLog* pLog = CLog::GetInstance();
		va_list args;
		va_start(args, format);
		vsnprintf (g_pContent, sizeof(g_pContent) - 1, format, args);
		va_end(args);
		pLog->PrintLog(g_pContent);
	}
	return S_OK;
}

HRESULT CLog::Logd (const char *format, ...)
{
	if (g_bOpenLog)
	{
		CLog* pLog = CLog::GetInstance();
		if(pLog)
		{
			va_list args;
			va_start(args, format);
			vsnprintf (g_pContent, sizeof(g_pContent) - 1, format, args);
			va_end(args);
			pLog->PrintLog (g_pContent);
			#if defined(USE_LOG_TO_FILE)
			pLog->WriteRow (g_pContent);
			#endif
		}
		else
		{
			printf ("pLog alloc failed.\r\n");
		}
	}
	return S_OK;
}

#endif

