#ifdef WIN32
#include <afx.h>
#include <Winsock2.h>
#else
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <strings.h>
#endif

#include "Config.h"
#include "xml.h"
#include "Log.h"
#include "imsocket.h"
#include "baseparam.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern TConfig m_Config;

TString::TString(const char* inbuf, int inlen)
: m_rpos(0)
{
    m_wpos = inlen;

    m_buffer.resize(inlen);

    memcpy(&m_buffer[0], inbuf, inlen);
}

int TString::Read(char* outbuf, int outlen)
{
    if(outlen > 0 && NULL != outbuf && outlen + m_rpos <= m_buffer.size())
    {
        memcpy(outbuf, &m_buffer[m_rpos], outlen);

        m_rpos += outlen;

        return outlen;
    }

    return -1;
}

bool TString::Write(const char* inbuf, int inlen)
{
    if(inlen > 0 && NULL != inbuf)
    {
        if(m_buffer.size() < m_wpos + inlen)
        {
            m_buffer.resize(m_wpos + inlen + 128);
        }

        memcpy(&m_buffer[m_wpos], inbuf, inlen);

        m_wpos += inlen;

        return true;
    }

    return false;
}

TXmppServer::TXmppServer(int localPort,TTcpSocket_Callback* pCallback)
:TTcpServer(localPort,pCallback,NULL)
{
    SetTimeout(10);
}

void TXmppServer::OnRecvMsg(TSocketMsg* pInfo)
{
    while(pInfo->msginfo.Size() > 0)
    {
        int packlen = 0;

        char* buf = (char*)(pInfo->msginfo.Str());

        TXMLElem* xmlelem = ParseXML(buf, packlen);

        if(NULL == xmlelem)
        {
            break;
        }
        else
        {
            if('/' == buf[packlen] && '<' == buf[packlen-1])
            {
                --packlen;
            }
            else if('<' != buf[packlen])
            {
                ++packlen;
            }

            TBaseParam* baseparam = NULL;

            if(parse_recvmsg(baseparam, xmlelem))
            {
                (*m_pSockCallback)(pInfo,0,packlen,SOCK_TYPE_MESSAGE,baseparam);

                pInfo->msginfo.Erase(packlen);

                delete xmlelem;
            }
            else
            {
                CloseSocket(pInfo);

                delete xmlelem;

                break;
            }
        }
    }
}

TBinaryServer::TBinaryServer(int localPort,TTcpSocket_Callback* pCallback)
:TTcpServer(localPort,pCallback,NULL)
{
    SetTimeout(10);
}

void TBinaryServer::OnRecvMsg(TSocketMsg* pInfo)
{
    string::size_type pos;
    
    size_t buflen = pInfo->msginfo.Size();
    
    const char* buffer = pInfo->msginfo.Str();

    while(string::npos != (pos = pInfo->msginfo.buffer.find("&")))
    {
        int packlen = atoi(pInfo->msginfo.buffer.c_str());

        if(packlen > 0 && packlen + pos + 1 <= buflen)
        {
            im::im inbuf;

            TBaseParam* baseparam = NULL;

            if(inbuf.ParseFromArray(buffer + pos + 1, packlen))
            {
                if(parse_recvmsg(baseparam, inbuf))
                {
                    (*m_pSockCallback)(pInfo,0,packlen+pos + 1,SOCK_TYPE_MESSAGE,baseparam);

                    pInfo->msginfo.Erase(packlen+pos+1);

                    continue;
                }
            }

            CloseSocket(pInfo);
        }

        break;
    }
}

THREAD_HANDLER_DECL(upstream_reconn, arg)
{
    TUpStream* upstream = (TUpStream*)arg;

    upstream->WorkThread();

    return 0;
}

void upstream_callback(TSocketMsg* pInfo,size_t startPos,size_t packlen,int msgtype,TBaseParam* param)
{
    if(SOCK_TYPE_SENDFAIL == msgtype)
    {
        static TLogFile* logfile = GET_LOG(upstream);

        LOG_ERROR(logfile, "upstream_callback send failed");
    }
}

TUpStream::TUpStream(const char* ip, int port, int index, string addr, string& prototype)
{
    if(!addr.empty())
    {
        m_addr = addr + "_" + to_string<int>(index);
    }

    m_prototype = prototype;

    m_logfile = GET_LOG(upstream);

    if(NULL == m_logfile)
    {
        term("router is null"); exit(0);
    }

    m_addrinfo[0] = '\0';

    m_port = port; strcpy(m_ip, ip);

    SetSockCallback(upstream_callback);

    sprintf(m_msgtype, "%s:%d_%d", m_ip, m_port, index);

    TImKeepAlive keepalive(prototype); keepalive.ToBuffer(m_keepalivestring);
}

TUpStream::~TUpStream()
{
}

void TUpStream::Run(string domain, string resource, string platfrom)
{
    SetTimeout(0);

    SetThreadCount(1, 1);

    TTcpSocket::Run(); 

    TImUpStream upstream(m_addr, domain, resource, platfrom, m_prototype);

    upstream.ToBuffer(m_loginstring);

    m_thread = new kernel::CTThread(upstream_reconn, this);
}

bool TUpStream::Connect()
{
    int fd = ConnectToServer(m_ip, m_port);

    if(fd <= 0) return false;

    setNonBlock(fd);

    strcpy(m_addrinfo, AddSocket(fd, m_ip, m_port)->addrinfo);

    term("connect:%s success", m_msgtype);

    if(m_addr.empty())
    {
        return true;
    }
    else
    {
        return SendMsg(m_loginstring.GetBuffer(), m_loginstring.GetSize());
    }
}

void TUpStream::WorkThread()
{
    int index = 0;

    bool isprint = true;

    while(m_bThreadRun)
    {
        ++index;

        sleep_s(1);

        if(0 != GetConnectionSize())
        {
            if(!isprint) isprint = true;

            if(m_keepalivestring.GetSize() > 0 && 0 == index % 300)
            {
                SendMsg(m_keepalivestring.GetBuffer(), m_keepalivestring.GetSize());
            }

            continue;
        }

        if(!Connect())
        {
            if(isprint)
            {
                term("connect: %s fail", m_msgtype); isprint = false;
            }
        }
    }
}

bool TUpStream::SendMsg(TBaseParam& param)
{
    TString sendstring;

    if(param.ToBuffer(sendstring))
    {
        return SendMsg(sendstring.GetBuffer(), sendstring.GetSize());
    }

    return false;
}

bool TUpStream::SendMsg(const char* msg, int length)
{
    if("XML" == m_prototype)
    {
        LOG_INFO(m_logfile, "msg: %s", msg);
    }

    TSocketMsg* pInfo = new TSocketMsg;
    pInfo->msgType = MSG_SERVER_RESPONSE;
    pInfo->msginfo.Append(msg, length);
    strcpy(pInfo->addrinfo, m_addrinfo);
    strcpy(pInfo->msgsign, m_msgtype);
    PutSendMsgToQueue(pInfo); 

    return true;
}

void start_upstream(const char* addr, string& upstr, vector<TUpStream*>& upstreams, string prototype)
{
    const char* delim = strstr(addr, ":");

    string ip; ip.append(addr, delim - addr);

    int port = atoi(++delim);

    static const int threadcount = m_Config.GetWorkCount();

    for(int index = 0; index < threadcount + 1; ++index)
    {
        TUpStream* upstream = new TUpStream(ip.c_str(), port, index, "", prototype);

        if(NULL == upstream)
        {
            term("UPSTREAM is null");

            exit(0);
        }

        upstream->Run();

        upstreams.push_back(upstream);
    }

    upstr = split_to_string("%s:%d", ("127.0.0.1" == ip) ? m_Config.GetImSockInfo().m_ip.c_str() : ip.c_str(), port);
}