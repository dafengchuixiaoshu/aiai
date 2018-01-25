
#pragma once
#include "RequestBuilder.h"
#include "shared_ptr.h"
#include "AutoBuffer.h"

using namespace util;

namespace XCapStrategy
{
 	class CResponseProcessor
	{
	public:
		CResponseProcessor(string csNotifyAsyncSuccessID = _T(""),string csNotifyAsyncFailureID = _T(""),string strSipUri = _T("")):
		  m_nHttpRespCode(0),
		  m_strSipUri(strSipUri)
		{

		}
		virtual ~CResponseProcessor(void){}
	public:
		
		virtual BOOL ProcessResponse(int nHttpRespCode,int nHttpErrorCode,int nHttpExtendCode,const string& csHead,util::shared_ptr<CAutoBuffer> spRespBodyABuf)
		{
			m_nHttpRespCode = nHttpRespCode;
			return IsResponseOK(nHttpRespCode);
		}

		virtual BOOL IsResponseOK(int nHttpRespCode )
		{
			return (nHttpRespCode >= 200 && nHttpRespCode < 300);
		}

		virtual void SetUserSelfId(const string &csUserSelfId)
		{
			m_csUserSelfId = csUserSelfId;
		}

		virtual string GetUserSelfId(){return m_csUserSelfId ;}

		virtual void SetRequestBuilder(util::shared_ptr<CRequestBuilder> spRB){ m_spReqBuilder = spRB;}
        util::shared_ptr<CRequestBuilder> GetRequestBuilder(){return m_spReqBuilder; }

		int GetResponseCode(){return m_nHttpRespCode;}

		virtual void set_cancel_flag (char cancel) {}		// 1 取消 0 执行 （默认（企业通讯录外）其他没有实现

	protected:
		int m_nHttpRespCode;

	private:
		string m_csUserSelfId;
		string m_strSipUri;
        util::shared_ptr<CRequestBuilder> m_spReqBuilder;

	};

}
