#ifndef _I_INVOKE_H
#define _I_INVOKE_H

#include "BizCommon.h"
#include "IBase.h"

// 请求接口
class IInvoke : public IBase
{
public:
	virtual HRESULT Invoke (VOID* pInvoke) = 0;
};

#endif
