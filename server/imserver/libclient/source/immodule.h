#ifndef _LIBCLIENT_IMMODULE_H_
#define _LIBCLIENT_IMMODULE_H_

#include "immsg.h"
#include "imsocket.h"

#ifndef NO_LANCHAT
#include "lanchat.h"
#endif

typedef void im_module_callback(int msgtype, CSocketMsg& socketmsg);

class CImModule : public CImSocket
{
    friend class CSingleton<CImModule>;

    friend THREAD_HANDLE(module_upload, param);

    friend THREAD_HANDLE(module_download, param);

    CImModule():m_callback(NULL),m_fileserverport(0),m_istarthttpthread(false)
#ifndef NO_LANCHAT
        ,m_selflanchat(NULL),m_sendlanchat(NULL)
#endif
    {}

    void UploadThread();

    void DownloadThread();

    bool FindLanChatRoom(string& touser);

    bool SendHttRequest(string& key, string& request, int sendhttptype);

    bool GetRequest(string& request, string& filepath, int sendhttptype);

    bool Start(const char* ip, int port, int timeout, string prototype, im_module_callback* callback);//second

public:

    virtual ~CImModule();

    bool PushSend(CIm& immsg);

    bool SendLanChat(CImMessage& immessage);

    virtual void OnCallBack(int msgtype, CSocketMsg& socketmsg, bool islanchat);

    bool ClearQueue(int& msgtype, CSocketMsg& socketmsg);

    bool StartXml(const char* ip, int port, int timeout, im_module_callback* callback);

    bool StartByte(const char* ip, int port, int timeout, im_module_callback* callback);

    void SetSavePath(string filepath){ m_filepath = filepath; }

    void SetMapPoint(string xpoint, string ypoint);

    void SetFileServer(string ip, int port){ m_fileserverip = ip; m_fileserverport = port; }

    void SetDeleteServer(string ip, int port){ m_deleteserverip = ip; m_deleteserverport = port; }

private:

    string m_xpoint;

    string m_ypoint;

    string m_filepath;

    string m_prototype;

    string m_fileserverip;

    int m_fileserverport;

    string m_deleteserverip;

    int m_deleteserverport;

    CMutex m_lanchatmutex;

#ifndef NO_LANCHAT
    CLanChat* m_sendlanchat;

    CLanChat* m_selflanchat;
#endif

    im_module_callback* m_callback;

    bool m_istarthttpthread;

    CThread m_uploadthread;
    
    CThread m_downloadthread;

    CQueue<CImChat> m_downloadqueue;

    CQueue<CImMessage> m_uploadqueue;
};

#define sImModule CSingleton<CImModule>::Instance()

#endif //_LIBCLIENT_IMMODULE_H_
