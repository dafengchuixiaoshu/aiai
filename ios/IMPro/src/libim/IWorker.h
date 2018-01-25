#ifndef _I_WORKER_H
#define _I_WORKER_H

#include "BizCommon.h"
#include "IBase.h"

// 工作者接口
class IWorker : public IBase
{
public:
	virtual HRESULT Invoke (VOID* pInvoke) = 0;
	virtual HRESULT OnNotify (VOID* pNotify) = 0;
	virtual HRESULT OnNotifyInner (VOID* pNotify) = 0;
	virtual void SendImage(string& fromuser, string& touser, string& imagepath,
			string& txtbuffer, int imagewidth, int imageheight, int msgtype){}
	virtual void SendVideo(string& fromuser, string& touser, string& videopath,
			string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, int msgtype){}
	void SetBlackListFlag(string& blackid, int type){}
};

#endif
