
#pragma once
#include "ResponseProcessor.h"


namespace XCapStrategy
{
	namespace Main  ///< 主要算法集
	{

		class COperPublicGroupRespProcessor : public CResponseProcessor 
		{
		public:
			COperPublicGroupRespProcessor(XCAP_OPER eXO):
						  m_eXO(eXO)
		{

		}
		COperPublicGroupRespProcessor(XCAP_OPER eXO,BOOL fromui):
		  m_eXO(eXO),m_bfromUI(fromui)
		  {

		  }
		protected:
			virtual BOOL ProcessResponse(int nHttpRespCode,int nHttpErrorCode,int nHttpExtendCode,const string& csHead,util::shared_ptr<CAutoBuffer> spRespBodyABuf);
		private:
			XCAP_OPER m_eXO;
			BOOL ProcessGroupChange(string id,string status,GroupInfo groupinfo);
			BOOL m_bfromUI;//判断是否界面发起的请求

		};

		class COperBuddyRespProcessor : public CResponseProcessor
		{
		public:
			COperBuddyRespProcessor(XCAP_OPER eXO):
			  m_eXO(eXO)
			  {

			  }
			COperBuddyRespProcessor(XCAP_OPER eXO,BOOL fromui):
			  m_eXO(eXO),m_bfromUI(fromui)
			  {

			  }

		protected:
			virtual BOOL ProcessResponse(int nHttpRespCode,int nHttpErrorCode,int nHttpExtendCode,const string& csHead,util::shared_ptr<CAutoBuffer> spRespBodyABuf);
			XCAP_OPER m_eXO;
			BOOL m_bfromUI;
		};

		class COperOfflineMsgRespProcessor : public CResponseProcessor
		{
		public:
			COperOfflineMsgRespProcessor(XCAP_OPER eXO):
			  m_eXO(eXO)
			  {}
		protected:
			virtual BOOL ProcessResponse(int nHttpRespCode,int nHttpErrorCode,int nHttpExtendCode,const string& csHead,util::shared_ptr<CAutoBuffer> spRespBodyABuf);
			XCAP_OPER m_eXO;
		};

	}

}
