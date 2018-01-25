
#include "RequestBuilder.h"
#include "XCapCore.h"

namespace XCapStrategy
{

	CHttpRequest::CHttpRequest()
	{

	}

	CRequestBuilder::CRequestBuilder(void)
	{
		//更改BUG，在构造函数中直接构建请求包，用户ID还没有设置进去的BUG
		SetUserSelfId(CXCapCore::GetInstance()->m_csUserID);
	}

	CRequestBuilder::~CRequestBuilder(void)
	{
	}

}
