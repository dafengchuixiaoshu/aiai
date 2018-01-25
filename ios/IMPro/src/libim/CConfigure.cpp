#include "BizCommon.h"
#include "CConfigure.h"

CConfigure* CConfigure::g_pConfigure = NULL;

CConfigure::CConfigure(void)
{
}

CConfigure::~CConfigure(void)
{
}

CConfigure* CConfigure::GetInstance()
{
	if (g_pConfigure == NULL)
	{
		g_pConfigure = new CConfigure();
	}
	return g_pConfigure;
}

VOID CConfigure::Release()
{
	if (g_pConfigure)
	{
		delete g_pConfigure;
		g_pConfigure = NULL;
	}
}

const string CConfigure::ReadProfile (const string key)
{
	if (key.empty())
	{
		return "";
	}

	return m_sets [key];
}

const string CConfigure::WriteProfile (const string key, const string value)
{
	if (key.empty())
	{
		return "";
	}

	m_sets [key] = value;
	return value;
}

