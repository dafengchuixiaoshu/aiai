#ifndef _PGM_CONTROL_H_
#define _PGM_CONTROL_H_

class TBaseModule;
class THttpServer;
class TXmppServer;//add by chixueli 2014-04-29
class TBinaryServer;

void _System_Run();

class TControl
{
public:

    TControl();
    virtual ~TControl();

    int    InitModule();

    void   SetModule(TBaseModule* pModule);

    TBaseModule*  GetModule();
    THttpServer*  GetHttpServer();
    TXmppServer*  GetXmppServer();
    TBinaryServer* GetBinaryServer();

private:

    TBaseModule* m_pModule;
    THttpServer* m_pHttpServer;
    TXmppServer* m_pXmppServer;//add by chixueli 2014-04-29
    TBinaryServer* m_pBinaryServer;
};

#endif //_PGM_CONTROL_H_
