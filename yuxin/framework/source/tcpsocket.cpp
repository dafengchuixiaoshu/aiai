#include "tcpsocket.h"

CStartSocket* g_startSocket = &CSingleton<CStartSocket>::Instance();

CStartSocket::CStartSocket()
{
#ifdef WIN32
    WSAData    wsaData;

    if(NO_ERROR != WSAStartup(MAKEWORD(2,2), &wsaData))
    {
        printf("WSAStartup failed\n");

        exit(0);
    }
#endif

    PUSH_OPNAME(OP_SOCKET, SOCKET_CONNECT, "SOCKET_CONNECT");
    PUSH_OPNAME(OP_SOCKET, SOCKET_TIMEOUT, "SOCKET_TIMEOUT");

    PUSH_OPNAME(OP_SOCKET, SOCKET_RECV_ERROR, "SOCKET_RECV_ERROR");
    PUSH_OPNAME(OP_SOCKET, SOCKET_RECV_CLOSED, "SOCKET_RECV_CLOSED");
    PUSH_OPNAME(OP_SOCKET, SOCKET_RECV_MESSAGE, "SOCKET_RECV_MESSAGE");

    PUSH_OPNAME(OP_SOCKET, SOCKET_EPOLL_ERROR, "SOCKET_EPOLL_ERROR");

    PUSH_OPNAME(OP_SOCKET, SOCKET_SEND_START, "SOCKET_SEND_START");
    PUSH_OPNAME(OP_SOCKET, SOCKET_SEND_ERROR, "SOCKET_SEND_ERROR");
    PUSH_OPNAME(OP_SOCKET, SOCKET_SEND_ACTIVE, "SOCKET_SEND_ACTIVE");
    PUSH_OPNAME(OP_SOCKET, SOCKET_SEND_CLOSED, "SOCKET_SEND_CLOSED");
    PUSH_OPNAME(OP_SOCKET, SOCKET_SEND_DISCONNECT, "SOCKET_SEND_DISCONNECT");
}

CSocketInfo::CSocketInfo(int serviceid, int socketfd, const char* ip, int port, int timeout)
: m_timeout(timeout)
, m_socketfd(socketfd)
, m_closeflag(0)
, m_serviceid(serviceid)
{
    m_uptime = time(NULL);

    char addrinfo[128 + 1] = { 0 };

    snprintf(addrinfo, 128, "%d_%s:%d@%d_%x", socketfd, (NULL == ip) ? "127.0.0.1" : ip, port, serviceid, m_uptime);

    m_addrinfo = addrinfo;
}

THREAD_HANDLE(tcpsocket_recvmsg, param)
{
    CThreadParam<CTcpSocket>* worker = (CThreadParam<CTcpSocket>*)param;

    worker->_object->RecvThread(worker->_index);

    delete worker;

    return 0;
}

THREAD_HANDLE(tcpsocket_sendmsg, param)
{
    CThreadParam<CTcpSocket>* worker = (CThreadParam<CTcpSocket>*)param;

    worker->_object->SendThread(worker->_index);

    delete worker;

    return 0;
}

CTcpSocket::CTcpSocket()
: m_istart(false)
, m_threadcount(1)
{
    m_log = GET_LOG("socket");
}

CTcpSocket::~CTcpSocket()
{
    m_istart = false;

    sleep_s(1);

    CPacket* packet = NULL;

    for(int index = 0; index < m_threadcount; ++index)
    {
        while(m_sendqueues[index].Pop(packet, -1) >= 0)
        {
            delete packet;
        }

        while(m_recvqueues[index].Pop(packet, -1) >= 0)
        {
            delete packet;
        }
    }

    erase_map<socket_map>(m_socketmap);
}

bool CTcpSocket::DoStart()
{
    if(!m_istart)
    {
        m_istart = true;

        for(int index = 0; index < m_threadcount; ++index)
        {
            CThreadParam<CTcpSocket>* param = new CThreadParam<CTcpSocket>(index, this);

            if(NULL != param)
            {
                if(m_recvthreads[index].DoStart(tcpsocket_recvmsg, param))
                {
                    param = new CThreadParam<CTcpSocket>(index, this);

                    if(NULL != param)
                    {
                        if(m_sendthreads[index].DoStart(tcpsocket_sendmsg, param))
                        {
                            continue;
                        }
                    }
                }
            }

            m_istart = false;

            sleep_s(1);

            SAFE_DEL_SINGLE(param);

            break;
        }
    }

    return m_istart;
}

void CTcpSocket::SetThreadCount(int threadcount)
{
    m_threadcount = (threadcount < min_thread_count) ? min_thread_count :
        ((threadcount < max_thread_count) ? threadcount : max_thread_count);
}

int CTcpSocket::GetRecvPacket(int index, CPacket* &packet)
{
    if(index >= 0 && index < m_threadcount)
    {
        return m_recvqueues[index].Pop(packet);
    }

    return -1;
}

string CTcpSocket::AppendSocketInfo(CSocketInfo& socketinfo)
{
    CSocketInfo* pinfo = new CSocketInfo(socketinfo);

    if(NULL != pinfo && DoRecvSocket(SOCKET_CONNECT, pinfo))
    {
        INFO_LOG("addrinfo:%s,serviceid:%s a new connection",
            pinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, pinfo->m_serviceid));

        m_socketmutex.Lock();

        m_socketmap[pinfo->m_addrinfo] = pinfo;

        m_socketmutex.Unlock();

        m_workmutexs[pinfo->m_socketfd % m_threadcount].Lock();

        m_acceptqueues[pinfo->m_socketfd % m_threadcount].Push(pinfo);

        m_workmutexs[pinfo->m_socketfd % m_threadcount].Unlock();

        return pinfo->m_addrinfo;
    }

    SAFE_DEL_SINGLE(pinfo);

    ERR_LOG("addrinfo:%s,serviceid:%s push a new connection failed",
        socketinfo.m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo.m_serviceid));

    return "";
}

bool CTcpSocket::PushSend(int serviceid, const string& addrinfo, const string& msgid, CProtoByte* protobyte, int closeflag)
{
    CPacket* packet = new CPacket(serviceid, addrinfo, msgid, protobyte, closeflag);

    if(NULL == packet)
    {
        ERR_LOG("addrinfo:%s,serviceid:%s,msgid:%s packet is null",
            addrinfo.c_str(), GET_OPNAME(OP_SERVICE, serviceid), msgid.c_str());

        return false;
    }
    else
    {
        //packet->m_sockettype = SOCKET_SEND_START; removed to constructor by lqguo

        packet->m_threadindex = atoi(addrinfo.c_str()) % m_threadcount;

        m_sendqueues[packet->m_threadindex].Push(packet);

        return true;
    }
}

#if defined(WIN32) || defined(USE_SELECT)

static const int max_recv_size = 1024;

void CTcpSocket::RecvThread(int index)
{
    fd_set ready;

    int maxsocketfd = 0;

    socket_list worklist;

    CSocketInfo* socketinfo;

    CMutex& workmutex = m_workmutexs[index];

    socket_queue& acceptqueue = m_acceptqueues[index];

    while(m_istart)
    {
        FD_ZERO(&ready);

        maxsocketfd = 0;

        workmutex.Lock();

        int worksize = worklist.size();

        while(worksize < max_recv_size)
        {
            if(acceptqueue.Pop(socketinfo, -1) < 0)
            {
                break;
            }
            else
            {
                worklist.push_back(socketinfo);

                ++worksize;
            }
        }

        workmutex.Unlock();

        for(socket_list::iterator it = worklist.begin(); it != worklist.end();)
        {
            socketinfo = *it;

            if(0 != socketinfo->m_closeflag)
            {
                INFO_LOG("addrinfo:%s,serviceid:%s remove this connection",
                    socketinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid));
             
                it = worklist.erase(it);

                delete socketinfo;
            }
            else if(socketinfo->m_timeout > 0 && socketinfo->m_uptime + socketinfo->m_timeout < time(NULL))
            {
                INFO_LOG("addrinfo:%s,serviceid:%s this connection is timeout",
                    socketinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid));
             
                CloseSocketInfo(SOCKET_TIMEOUT, socketinfo->m_addrinfo);

                it = worklist.erase(it);

                delete socketinfo;
            }
            else
            {
                ++it;

                FD_SET(socketinfo->m_socketfd, &ready);

                if(socketinfo->m_socketfd > maxsocketfd)
                {
                    maxsocketfd = socketinfo->m_socketfd;
                }
            }
        }

        struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 10 * 1000;

        if(worklist.empty())
        {
            acceptqueue.Pop(socketinfo, 0, false);
        }
        else if(select(maxsocketfd + 1, &ready, NULL, NULL, &tv) > 0)
        {
            for(socket_list::const_iterator it = worklist.begin(); it != worklist.end(); ++it)
            {
                if(FD_ISSET((*it)->m_socketfd, &ready) > 0)
                {
                    ReadMsg(*it);
                }
            }
        }
    }
}

#else

static const int max_recv_size = 10 * 1024;

void CTcpSocket::RecvThread(int index)
{
    CSocketInfo* socketinfo;

    socket_list worklist;

    epoll_event events[max_recv_size];

    CMutex& workmutex = m_workmutexs[index];

    int epollfd = epoll_create(max_recv_size);

    socket_queue& acceptqueue = m_acceptqueues[index];

    while(m_istart)
    {
        workmutex.Lock();

        int worksize = worklist.size();

        while(worksize < max_recv_size)
        {
            if(acceptqueue.Pop(socketinfo, -1) < 0)
            {
                break;
            }
            else
            {
                worklist.push_back(socketinfo);

                epoll_event ev; ev.events = EPOLLIN; ev.data.ptr = socketinfo;

                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, socketinfo->m_socketfd, &ev) < 0)
                {
                    CloseSocketInfo(SOCKET_EPOLL_ERROR, socketinfo->m_addrinfo);
                    
                    ERR_LOG("addrinfo:%s,serviceid:%s epoll append failed",
                        socketinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid));
                }
                else
                {
                    ++worksize;
                }
            }
        }

        workmutex.Unlock();

        for(socket_list::iterator it = worklist.begin(); it != worklist.end();)
        {
            socketinfo = *it;

            if(0 != socketinfo->m_closeflag)
            {
                INFO_LOG("addrinfo:%s,serviceid:%s remove this connection",
                    socketinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid));
             
                it = worklist.erase(it);

                epoll_ctl(epollfd, EPOLL_CTL_DEL, socketinfo->m_socketfd, NULL);

                delete socketinfo;
            }
            else if(socketinfo->m_timeout > 0 && socketinfo->m_uptime + socketinfo->m_timeout < time(NULL))
            {
                INFO_LOG("addrinfo:%s,serviceid:%s this connection is timeout",
                    socketinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid));
             
                it = worklist.erase(it);

                epoll_ctl(epollfd, EPOLL_CTL_DEL, socketinfo->m_socketfd, NULL);

                CloseSocketInfo(SOCKET_TIMEOUT, socketinfo->m_addrinfo);

                delete socketinfo;
            }
            else
            {
                ++it;
            }
        }

        if(worklist.empty())
        {
            acceptqueue.Pop(socketinfo, 0, false);
        }
        else
        {
            int nfds = epoll_wait(epollfd, events, max_recv_size, 10);

            for(int index = 0; index < nfds; ++index)
            {
                ReadMsg((CSocketInfo*)(events[index].data.ptr));
            }
        }
    }
}

#endif

void CTcpSocket::SendThread(int index)
{
    CPacket* packet = NULL;

    packet_queue& sendqueue = m_sendqueues[index];

    while(m_istart)
    {
        if(sendqueue.Pop(packet) >= 0)
        {
            if(NULL == packet->m_protobyte)
            {
                CloseSocketInfo(SOCKET_SEND_DISCONNECT, packet);

                INFO_LOG("addrinfo:%s,serviceid:%s,msgid:%s close this connect when sending a empty packet",
                    packet->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, packet->m_serviceid), packet->m_msgid.c_str()); 

                continue;
            }
            else if(packet->m_protobyte->GetBuffer().empty())
            {
                if(packet->m_protobyte->ToBuffer() < 0)
                {
                    ERR_LOG("addrinfo:%s,serviceid:%s,msgid:%s packet to buffer failed",
                        packet->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, packet->m_serviceid), packet->m_msgid.c_str()); 

                    continue;
                }
            }

            int outlen = packet->m_protobyte->GetBuffer().size();

            if(0 == outlen)
            {
                CloseSocketInfo(SOCKET_SEND_DISCONNECT, packet);

                INFO_LOG("addrinfo:%s,serviceid:%s,msgid:%s close this connect when sending a empty buffer",
                    packet->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, packet->m_serviceid), packet->m_msgid.c_str()); 

                continue;
            }

            int socketfd = 0;

            m_socketmutex.Lock();

            socket_map::const_iterator it = m_socketmap.find(packet->m_addrinfo);

            if(m_socketmap.end() != it)
            {
                socketfd = it->second->m_socketfd;
            }

            m_socketmutex.Unlock();

            const char* outbuf = packet->m_protobyte->GetBuffer().c_str();

            if(0 == socketfd)
            {                
                INFO_LOG("addrinfo:%s,serviceid:%s,msgid:%s this connect has removed",
                    packet->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, packet->m_serviceid), packet->m_msgid.c_str()); 

                CloseSocketInfo(SOCKET_SEND_CLOSED, packet);
            }
            else if(SendMsg(socketfd, outbuf, outlen))
            {
                DoRecvSocket(SOCKET_SEND_ACTIVE, packet);

                if(1 == packet->m_closeflag)
                {
                    INFO_LOG("addrinfo:%s,serviceid:%s,msgid:%s send packet successful with closeflag",
                        packet->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, packet->m_serviceid), packet->m_msgid.c_str()); 

                    CloseSocketInfo(SOCKET_SEND_DISCONNECT, packet->m_addrinfo);
                }
                else
                {
                    INFO_LOG("addrinfo:%s,serviceid:%s,msgid:%s send packet successful",
                        packet->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, packet->m_serviceid), packet->m_msgid.c_str()); 
                }
            }
            else
            {
                INFO_LOG("addrinfo:%s,serviceid:%s,msgid:%s send packet failed",
                    packet->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, packet->m_serviceid), packet->m_msgid.c_str()); 

                CloseSocketInfo(SOCKET_SEND_ERROR, packet);
            }
        }
    }
}

void CTcpSocket::ReadMsg(CSocketInfo* socketinfo)
{
    char inbuf[1024+1] = { 0 };

    int inlen = recv(socketinfo->m_socketfd, inbuf, 1024, 0);

    INFO_LOG("addrinfo:%s,serviceid:%s,recv[%d:%s] get packet",
        socketinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid), inlen, inbuf); 

    if(inlen > 0)
    {
        socketinfo->m_uptime = time(NULL);

        socketinfo->append(inbuf, inlen);

        while(!socketinfo->m_recvbuffer.empty())
        {
            CProtoByte* protobyte = new CProtoByte();

            if(NULL != protobyte)
            {
                int packetlen = protobyte->ToBlock(socketinfo->m_recvbuffer.c_str(), socketinfo->m_recvbuffer.size());

                if(packetlen > 0)
                {
                    CPacket* packet = new CPacket(socketinfo->m_serviceid, socketinfo->m_addrinfo, protobyte, 0);

                    if(NULL != packet)
                    {
                        socketinfo->erase(packetlen);

                        INFO_LOG("addrinfo:%s,serviceid:%s,packetlen:%d get packet successful",
                            socketinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid), packetlen); 

                        DoRecvSocket(SOCKET_RECV_MESSAGE, packet);

                        continue;
                    }
                }
                else if(packetlen < 0)
                {
                    ERR_LOG("addrinfo:%s,serviceid:%s,buffer[%d,%s] get packet failed", socketinfo->m_addrinfo.c_str(),
                        GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid), socketinfo->m_recvbuffer.size(), socketinfo->m_recvbuffer.c_str()); 

                    CloseSocketInfo(SOCKET_RECV_ERROR, socketinfo->m_addrinfo);
                }
            }

            SAFE_DEL_SINGLE(protobyte);

            break;
        }
    }
    else
    {
        CloseSocketInfo(SOCKET_RECV_CLOSED, socketinfo->m_addrinfo);
    }
}

#if defined(WIN32) || defined(USE_SELECT)

bool CTcpSocket::SendMsg(int socketfd, const char* outbuf, int outlen)
{
    fd_set ready;

    while(outlen > 0)
    {
        FD_ZERO(&ready);

        FD_SET(socketfd, &ready);

        struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 10 * 1000;

        if(select(socketfd + 1, NULL, &ready, NULL, &tv) > 0)
        {
            int len = send(socketfd, outbuf, outlen, 0);

            if(len <= 0)
            {
                return false;
            }

            outlen -= len;

            outbuf += len;
        }
        else
        {
            return false;
        }
    }

    return true;
}

#else

bool CTcpSocket::SendMsg(int socketfd, const char* outbuf, int outlen)
{
    struct pollfd client[1];

    client[0].fd = socketfd;

    client[0].events = POLLOUT;

    while(outlen > 0)
    {
        if(poll(client, 1, 5) > 0)
        {
            int len = send(socketfd, outbuf, outlen, 0);

            if(len <= 0)
            {
                return false;
            }

            outlen -= len;

            outbuf += len;
        }
        else
        {
            return false;
        }
    }

    return true;
}

#endif

void CTcpSocket::CloseSocketInfo(int type, CPacket* packet)
{
    m_socketmutex.Lock();

    const socket_map::iterator it = m_socketmap.find(packet->m_addrinfo);

    if(m_socketmap.end() != it)
    {
        it->second->m_closeflag = 1;

        DoRecvSocket(type, packet);

        m_socketmap.erase(it);
    }

    m_socketmutex.Unlock();
}

void CTcpSocket::CloseSocketInfo(int type, const string& addrinfo)
{
    m_socketmutex.Lock();

    const socket_map::iterator it = m_socketmap.find(addrinfo);

    if(m_socketmap.end() != it)
    {
        it->second->m_closeflag = 1;

        DoRecvSocket(type, it->second);

        m_socketmap.erase(it);
    }

    m_socketmutex.Unlock();
}

bool CTcpSocket::DoRecvSocket(int sockettype, CPacket* packet)
{
    packet->m_sockettype = sockettype;

    packet->m_threadindex = (atoi(packet->m_addrinfo.c_str())) % m_threadcount;

    m_recvqueues[packet->m_threadindex].Push(packet);

    return true;
}

bool CTcpSocket::DoRecvSocket(int sockettype, CSocketInfo* socketinfo)
{
    CPacket* packet = new CPacket(socketinfo->m_serviceid, socketinfo->m_addrinfo, NULL, 0);

    if(NULL == packet)
    {
        ERR_LOG("addrinfo:%s,serviceid:%s do recv packet is null",
            socketinfo->m_addrinfo.c_str(), GET_OPNAME(OP_SERVICE, socketinfo->m_serviceid)); 

        return false;
    }

    return DoRecvSocket(sockettype, packet);
}

static void de_addrinfo(const char* addrinfo, int& socketfd, char* ip, int& port)
{
    socketfd = atoi(addrinfo);

    const char* subip = strchr(addrinfo, '_');

    const char* subport = strchr(addrinfo, ':');

    strncpy(ip, subip + 1, subport - subip - 1);

    port = atoi(subport + 1);
}

string Connect(CTcpSocket& tcpsocket, int serviceid, const char* serverip, int serverport, int timeout)
{
    int socketfd;

    string addrinfo;

    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) > 0)
    {
        set_linger(socketfd);

        set_sendbuf_size(socketfd);

        set_recvbuf_size(socketfd);

        set_send_timeout(socketfd);

        set_recv_timeout(socketfd);

        struct sockaddr_in addr_in;

        addr_in.sin_family = AF_INET;

        addr_in.sin_port = htons(serverport);

        get_host_by_name(addr_in, serverip);

        if(connect(socketfd, (struct sockaddr*)&addr_in, sizeof(sockaddr_in)) >= 0)
        {
            set_nonblock(socketfd);

            CSocketInfo socketinfo(serviceid, socketfd, serverip, serverport, timeout);

            addrinfo = tcpsocket.AppendSocketInfo(socketinfo);
        }

        if(addrinfo.empty())
        {
            closesocket(socketfd);
        }
    }

    return addrinfo;
}