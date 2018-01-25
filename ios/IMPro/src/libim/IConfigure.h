#ifndef _I_CONFIGURE_H
#define _I_CONFIGURE_H

#include "BizCommon.h"
#include "IBase.h"

// 配置观察者接口
class IConfigure : public IBase
{
public:
	virtual const string ReadProfile (const string key) = 0;
	virtual const string WriteProfile (const string key, const string value) = 0;
};

#endif
