
#pragma once
#include <vector>
#include <string>
using namespace std;

namespace XCapStrategy
{

	//请求包头
	class CHttpRequest
	{
	public:
		CHttpRequest();
	public:
		string csReqUri;
		vector<string> vecHead;
		string csBody;

	};

	class CRequestBuilder
	{
	public:
		CRequestBuilder(void);

		virtual ~CRequestBuilder(void);

	public:

		virtual	CHttpRequest GetRequestPacket() = 0;

		
		virtual void SetUserSelfId(const string &csUserSelfId)
		{
			m_csUserSelfId = csUserSelfId;
		}

		virtual string GetUserSelfId(){return m_csUserSelfId ;}

	private:
		string m_csUserSelfId;
	};

}
