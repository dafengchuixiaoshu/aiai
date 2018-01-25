#include "immsg.h"
#include "imsocket.h"

#ifdef NO_LANCHAT
#define LOGD printf
#else
#include "BizCommon.h"
#endif

extern "C" void on_thread_closed (int tid);

int byterecv_toblock(CSocketMsg& socketmsg, const char* inbuf, int inlen)
{
    CProtoByte protobyte;

    string sbuf; sbuf.append(inbuf, inlen);

    string::size_type pos = sbuf.find("&");

    if(string::npos != pos)
	{
		int packlen = atoi(sbuf.c_str());

		if(packlen > 0 && packlen + pos + 1 <= inlen)
		{
			im::im protobuffer;

			if(protobuffer.ParseFromArray(inbuf + pos + 1, packlen))
			{
				im::im_Type tag = protobuffer.msgtype();

				if(im::im_Type_IM_LOGIN == tag)
				{
					socketmsg.m_im = new CImLogin();
				}
				else if(im::im_Type_IM_LOGOUT == tag)
				{
					socketmsg.m_im = new CImLogout();
				}
				else if(im::im_Type_IM_CHAT == tag)
				{
					socketmsg.m_im = new CImChat();
				}
				else if(im::im_Type_IM_ACK == tag)
				{
					socketmsg.m_im = new CImAck();
				}
				else if(im::im_Type_IM_KEEPALIVE == tag)
				{
					socketmsg.m_im = new CImKeepAlive();
				}
				else if(im::im_Type_IM_KICKOUT == tag)
				{
					socketmsg.m_im = new CImKickout();
				}
				else if(im::im_Type_IM_PGCREATE == tag)
				{
					socketmsg.m_im = new CImPGCreate();
				}
				else if(im::im_Type_IM_PGINVITE == tag)
				{
					socketmsg.m_im = new CImPGInvite();
				}
				else if(im::im_Type_IM_PGQUIT == tag)
				{
					socketmsg.m_im = new CImPGQuit();
				}
				else if(im::im_Type_IM_PGMODIFY == tag)
				{
					socketmsg.m_im = new CImPGModify();
				}
				else if(im::im_Type_IM_PGCARD == tag)
				{
					socketmsg.m_im = new CImPGCard();
				}
				else if(im::im_Type_IM_SNS == tag)
				{
					socketmsg.m_im = new CImSns();
				}
				else if(im::im_Type_IM_FANSMODIFY == tag)
				{
					socketmsg.m_im = new CImFansModify();
				}
				else if(im::im_Type_IM_FANSAPPEND == tag)
				{
					socketmsg.m_im = new CImFansAppend();
				}
				else if(im::im_Type_IM_FANSDELETE == tag)
				{
					socketmsg.m_im = new CImFansDelete();
				}
				else if(im::im_Type_IM_MESSAGE == tag)
				{
					socketmsg.m_im = new CImMessage();
				}
				else if(im::im_Type_IM_BLACKLIST == tag)
				{
					socketmsg.m_im = new CImBlackList();
				}

				if(NULL == socketmsg.m_im)
				{
					LOGD("byterecv_toblock socketmsg.m_im is null [tag:%d]\n", tag);
				}
				else if(socketmsg.m_im->ToBlock(protobuffer))
				{
					return packlen + pos + 1;
				}
				else
				{
					LOGD("byterecv_toblock ToBlock failed [tag:%d]\n", tag);
				}
			}
			else
			{
				LOGD("byterecv_toblock ParseFromArray failed\n");
			}

			return -1;
		}
	}

    //LOGD("byterecv_toblock can not find pack [%d:%s]\n", inlen, inbuf);

    return 0;
}

int xmlrecv_toblock(CSocketMsg& socketmsg, const char* inbuf, int inlen)
{
    CProtoXml protoxml;

    int packetlen = protoxml.ToBlock(inbuf, inlen);
    
    //LOGD("xmlrecv_toblock packlen:%d,inbuf:%s\n", packetlen, inbuf);

    if(packetlen > 0)
    {
        CByte& key = protoxml.GetKey();

        //LOGD("xmlrecv_toblock key:%s\n", key.c_str());

        if(key.compare("stream:open"))
        {
            socketmsg.m_im = new CImLogin();
        }
        else if(key.compare("stream:close"))
        {
            socketmsg.m_im = new CImLogout();
        }
        else if(key.compare("message"))
        {
        	const char* lanchat;

        	if(get_proto_value(&protoxml, "lanchat", lanchat, BYTE_SIGN_ATTR) > 0)
            {
        		socketmsg.m_im = new CImMessage();
            }
        	else
        	{
        		socketmsg.m_im = new CImChat();
        	}
        }
        else if(key.compare("iq"))
        {
            const char* type;

            if(get_proto_value(&protoxml, "type", type, BYTE_SIGN_ATTR) > 0)
            {
                if(0 == strcmp(type, "ack"))
                {
                    socketmsg.m_im = new CImAck();
                }
                else if(0 == strcmp(type, "kickout"))
                {
                    socketmsg.m_im = new CImKickout();
                }
                else if(0 == strcmp(type, "keepalive"))
                {
                    socketmsg.m_im = new CImKeepAlive();
                }
                else if(0 == strcmp(type, "pgcreate"))
                {
                    socketmsg.m_im = new CImPGCreate();
                }
                else if(0 == strcmp(type, "pginvite"))
                {
                    socketmsg.m_im = new CImPGInvite();
                }
                else if(0 == strcmp(type, "pgquit"))
                {
                    socketmsg.m_im = new CImPGQuit();
                }
                else if(0 == strcmp(type, "pgmodify"))
                {
                    socketmsg.m_im = new CImPGModify();
                }
                else if(0 == strcmp(type, "pgcard"))
                {
                    socketmsg.m_im = new CImPGCard();
                }            
                else if(0 == strcmp(type, "sns"))
                {
                    socketmsg.m_im = new CImSns();
                }            
                else if(0 == strcmp(type, "fansmodify"))
                {
                    socketmsg.m_im = new CImFansModify();
                }            
                else if(0 == strcmp(type, "fansappend"))
                {
                    socketmsg.m_im = new CImFansAppend();
                }            
                else if(0 == strcmp(type, "fansdelete"))
                {
                    socketmsg.m_im = new CImFansDelete();
                }
            }
        }

        if(NULL == socketmsg.m_im)
        {
        	//LOGD("socketmsg.m_im is null\n");

            return -1;
        }
        else if(!socketmsg.m_im->ToBlock(&protoxml))
        {
        	//LOGD("socketmsg.m_im ToBlock failed\n");

            return -1;
        }
    }

    return packetlen;
}

int recv_toblock(CSocketMsg& socketmsg, const char* inbuf, int inlen, string& prototype)
{
    if("XML" == prototype)
    {
        return xmlrecv_toblock(socketmsg, inbuf, inlen);
    }
    else if("BYTE" == prototype)
    {
        return byterecv_toblock(socketmsg, inbuf, inlen);
    }
    else
    {
        return -1;
    }
}

void set_linger(int fd)
{
    static struct linger ln; 

    ln.l_onoff = 1; ln.l_linger = 0;

    static const int len = sizeof(ln);

    setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&ln, len);
}

void set_sendbuf_size(int fd)
{
    int size = 512 * 1024;

    static const int len = sizeof(size);

    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&size, len);
}

void set_recvbuf_size(int fd)
{
    int size = 512 * 1024;

    static const int len = sizeof(size);

    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&size, len);
}

#ifdef WIN32

void set_nonblock(int fd)
{
    unsigned long x = 1;

    ioctlsocket(fd, FIONBIO, (unsigned long*)&x);
}

void set_send_timeout(int fd)
{
    int timeout = 30 * 1000;

    static const int len = sizeof(timeout);

    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, len);
}

void set_recv_timeout(int fd)
{
    int timeout = 30 * 1000;

    static const int len = sizeof(timeout);

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, len);
}

#else

void set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void set_send_timeout(int fd)
{
    struct timeval timeout = { 30, 0 };

    static const int len = sizeof(timeout);

    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
}

void set_recv_timeout(int fd)
{
    struct timeval timeout = { 30, 0 };

    static const int len = sizeof(timeout);

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len);
}

#endif

bool get_host_by_name(struct sockaddr_in& addr_in, const char* address)
{
    int iplen = strlen(address);

    if(iplen < 7 || iplen > 15) return false;

    bool isip = true;

    const char* temp = address;

    while('\0' != *temp)
    {
        if(('.' != *temp) && (*temp < '0'||*temp > '9'))
        {
            isip = false; break;
        }

        ++temp;
    }

    if (isip)
    {
        addr_in.sin_addr.s_addr = inet_addr(address);
    }
    else
    {
        static CMutex mutex;

        mutex.Lock();

        struct hostent* myhost = gethostbyname(address);

        if(NULL == myhost)
        {
            mutex.Unlock(); return false;
        }
        else
        {
            memcpy(&(addr_in.sin_addr), *(myhost->h_addr_list), sizeof(struct in_addr));
        }

        mutex.Unlock();
    }

    return true;
}

static CStartSocket& g_startSocket = CSingleton<CStartSocket>::Instance();

CSocketInfo::CSocketInfo()
: m_socketfd(0)
, m_loginstatus(0)
{
}

CSocketInfo::~CSocketInfo()
{
    if(m_socketfd > 0)
    {
        closesocket(m_socketfd);
    }
}

int CSocketInfo::size()
{
    return m_recvbuf.size();
}

int CSocketInfo::getfd()
{
    m_mutex.Lock();

    int fd = m_socketfd;

    m_mutex.Unlock();

    return fd;
}

void CSocketInfo::clear()
{
    m_mutex.Lock();

    if(m_socketfd > 0)
    {
        closesocket(m_socketfd);

        m_socketfd = 0;
    }

    m_recvbuf.clear();

    CSocketMsg* socketmsg;

    while(m_ackqueue.Pop(socketmsg, -1) >= 0) delete socketmsg;

    m_mutex.Unlock();
}

const char* CSocketInfo::c_str()
{
    return m_recvbuf.c_str();
}

int CSocketInfo::getloginstatus()
{
    m_mutex.Lock();

    int loginstatus = m_loginstatus;

    m_mutex.Unlock();

    return loginstatus;
}

bool CSocketInfo::erase(int outlen)
{
    return m_recvbuf.erase(outlen);
}

void CSocketInfo::getaddr(string& ip, int& port)
{
    m_mutex.Lock();

    ip = m_ip; port = m_port;

    m_mutex.Unlock();
}

bool CSocketInfo::append(const char* inbuf, int inlen)
{
    return m_recvbuf.append(inbuf, inlen, 256);
}

THREAD_HANDLE(tcpsocket_recvmsg, param)
{
    CImSocket* tcpsocket = (CImSocket*)param;

    tcpsocket->RecvThread();

#if defined(ANDROID)
    on_thread_closed(pthread_self());
#endif

    return 0;
}

THREAD_HANDLE(tcpsocket_sendmsg, param)
{
    CImSocket* tcpsocket = (CImSocket*)param;

    tcpsocket->SendThread();

#if defined(ANDROID)
    on_thread_closed(pthread_self());
#endif

    return 0;
}

CImSocket::CImSocket()
: m_istart(false)
, m_networkstatus(1)
{
}

CImSocket::~CImSocket()
{
    m_istart = false;

    sleep_s(1);
}

void CImSocket::Release()
{
	//LOGD("CImSocket::Release 0\n");

    m_socketinfo.m_mutex.Lock();

    m_socketinfo.m_loginstatus = 0;

    if(m_socketinfo.m_socketfd > 0)
    {
        closesocket(m_socketinfo.m_socketfd);

        m_socketinfo.m_socketfd = 0;
    }

//  DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_WORK_CLEAR);

    m_socketinfo.m_mutex.Unlock();

    //LOGD("CImSocket::Release 1\n");
}

bool CImSocket::Connect()
{
    string ip; int port = 0;

    m_socketinfo.clear();

    m_socketinfo.getaddr(ip, port);

    if(ip.empty() || 0 == port) return false;

    m_socketinfo.m_mutex.Lock();

    int fd = socket(AF_INET, SOCK_STREAM, 0); 

    if(fd <= 0)
    {
        m_socketinfo.m_mutex.Unlock();

        return false;
    }

    set_linger(fd);

    set_sendbuf_size(fd);

    set_recvbuf_size(fd);

    set_send_timeout(fd);

    set_recv_timeout(fd);

    struct sockaddr_in addr_in;

    addr_in.sin_family = AF_INET;

    addr_in.sin_port = htons(port);

    get_host_by_name(addr_in, ip.c_str());

    if(connect(fd, (struct sockaddr*)&addr_in, sizeof(sockaddr_in)) >= 0)
    {
        set_nonblock(fd);

        if(DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_SOCKET_CONNECT))
        {
        	if(0 == m_networkstatus)
        	{
        		m_networkstatus = 1;
        	}

            m_socketinfo.m_socketfd = fd;

            m_socketinfo.m_uptime = time(NULL);

            m_socketinfo.m_mutex.Unlock();

            return true;
        }
    }

    closesocket(fd); m_socketinfo.m_mutex.Unlock(); return false;
}

int CImSocket::GetNetworkStatus()
{
    m_networkmutex.Lock();

    int networkstatus = m_networkstatus;

    m_networkmutex.Unlock();

    return networkstatus;
}

void CImSocket::SetNetworkStatus(int networkstatus)
{
    m_networkmutex.Lock();

    m_networkstatus = networkstatus;

    if(networkstatus > 0)
    {
        m_awuqueue.Push(1);
    }

    //DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_WORK_CLEAR);

    //m_socketinfo.clear();

    m_networkmutex.Unlock();
}

bool CImSocket::Start(const char* ip, int port, int timeout, string& prototype)
{
	//LOGD("CImSocket::Start 0\n");

    m_prototype = prototype;

    m_socketinfo.m_mutex.Lock();

    //LOGD("CImSocket::Start 1\n");

    m_socketinfo.m_ip = ip;
    
    m_socketinfo.m_port = port;

    m_socketinfo.m_loginstatus = 1;

    m_socketinfo.m_timeout = timeout;

    m_socketinfo.m_mutex.Unlock();

    //LOGD("CImSocket::Start 2\n");

    m_socketinfo.clear();

    //LOGD("CImSocket::Start 3\n");

    if(!m_istart)
    {
        m_istart = true;

        if(!m_recvthread.Start(tcpsocket_recvmsg, this))
        {
            m_istart = false; return false;
        }

        if(!m_sendthread.Start(tcpsocket_sendmsg, this))
        {
            m_istart = false; return false;
        }
    }

    //LOGD("CImSocket::Start 4\n");

    m_awuqueue.Push(1);

    //LOGD("CImSocket::Start 5\n");

    return true;
}

bool CImSocket::PushToSendQueue(int imtype, string sendmsgid, TString& outstr)
{
    CSocketMsg* socketmsg = new CSocketMsg();

    if(NULL == socketmsg) return false;

    socketmsg->m_imtype = imtype;

    socketmsg->m_sendmsgid = sendmsgid;

    socketmsg->m_outstr.Write(outstr.GetBuffer(), outstr.GetSize());

    m_sendqueue.Push(socketmsg);

    return true;
}

bool CImSocket::DoSocket(string& recvbuffer, int msgtype, bool islanchat, string sendmsgid)
{
	static CMutex mutex;

	mutex.Lock();

    CSocketMsg socketmsg;

    socketmsg.m_sendmsgid = sendmsgid;

    //LOGD("DoSocket msgtype:%d\n", msgtype);

    if(MSG_RECV_MESSAGE == msgtype)
    {
        int inlen = recvbuffer.size();

        const char* inbuf = recvbuffer.c_str();

        int packetlen = recv_toblock(socketmsg, inbuf, inlen, m_prototype);

        if(packetlen < 0)
        {
        	recvbuffer.clear();

            mutex.Unlock();

            return false;
        }
        else if(packetlen > 0 && packetlen <= inlen)
        {
            if(islanchat)
            {
                recvbuffer.erase(0, packetlen);
            }
            else
            {
                m_socketinfo.erase(packetlen);
            }
        }
        else
        {
        	mutex.Unlock();

            return false;
        }
    }

    OnCallBack(msgtype, socketmsg, islanchat);

    mutex.Unlock();

    return true;
}

void CImSocket::RecvThread()
{
    fd_set ready; CSocketMsg* socketmsg;

    static int time_ = 3 * 60 * 1000;//3 minute

    int autowakeup = 0; int trytimes = 0;

    int curfd = 0;

    while(m_istart)
    {
        if(0 == m_socketinfo.getloginstatus())
        {
            m_awuqueue.Pop(autowakeup, 0);

            m_awuqueue.Pop(autowakeup, -1);

            trytimes = 0; sleep_s(1); continue;
        }
        else
        {
            curfd = m_socketinfo.getfd();

            if(0 == curfd)
            {
                if(trytimes > 2)
                {
                    m_awuqueue.Pop(autowakeup, (0 == GetNetworkStatus()) ? 0 : time_);

                    m_awuqueue.Pop(autowakeup, -1);

                    trytimes = 0; sleep_s(1); continue;
                }
                else if(Connect())
                {
                    while(m_awuqueue.Pop(autowakeup, -1) >= 0);

                    continue;
                }
                else
                {
                    ++trytimes; sleep_s(5); continue;
                }
            }
        }

        if(m_socketinfo.m_ackqueue.Pop(socketmsg, -1, false) >= 0)
        {
            if(time(NULL) - socketmsg->m_sendtime > m_socketinfo.m_timeout)
            {
                DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_SOCKET_TIMEOUT);

                m_socketinfo.clear();

                continue;
            }
        }

        FD_ZERO(&ready); FD_SET(curfd, &ready);

        struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 10 * 1000;

        if(select(curfd + 1, &ready, NULL, NULL, &tv) > 0)
        {
            if(FD_ISSET(curfd, &ready) > 0)
            {
                char inbuf[1024+1] = { 0 };

                int inlen = recv(curfd, inbuf, 1024, 0);

                LOGD("imsocket recv [%d:%s]\n", inlen, inbuf);

                if(inlen > 0)
                {
                    m_socketinfo.m_mutex.Lock();

                    m_socketinfo.m_uptime = time(NULL);

                    m_socketinfo.m_mutex.Unlock();

                    m_socketinfo.append(inbuf, inlen);

                    while(DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_RECV_MESSAGE))
                    {
                        sleep_ms(1);
                    }
                }
                else
                {
                    DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_RECV_CLOSED);

                    m_socketinfo.clear();
                }
            }
        }

        m_socketinfo.m_mutex.Lock();

        if(time(NULL) - m_socketinfo.m_uptime > 60)
        {
            CSocketMsg* socketmsg = new CSocketMsg();

            if(NULL != socketmsg)
            {
                CImKeepAlive keepalive;

                keepalive.ToBuffer(socketmsg->m_outstr, m_prototype);

                m_sendqueue.Push(socketmsg);
            }
        }

        m_socketinfo.m_mutex.Unlock();
    }
}

void CImSocket::SendThread()
{
    CSocketMsg* socketmsg = NULL;

    while(m_istart)
    {
        if(m_sendqueue.Pop(socketmsg) >= 0)
        {
            int curfd = m_socketinfo.getfd();

            string& sendmsgid = socketmsg->m_sendmsgid;

            if(0 == socketmsg->m_outstr.GetSize())
            {
                DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_SEND_DISCONNECT);

                m_socketinfo.clear();
            }
            else
            {
                if(0 == curfd)
                {
                    DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_SEND_CLOSED, false, sendmsgid);
                }
                else
                {
                    if(SendMsg(curfd, socketmsg->m_outstr.GetBuffer(), socketmsg->m_outstr.GetSize()))
                    {
                        m_socketinfo.m_mutex.Lock();

                        m_socketinfo.m_uptime = time(NULL);

                        m_socketinfo.m_mutex.Unlock();

#ifdef WIN32
                        if("XML" == m_prototype)
                        {
                            printf("send message: %s\n", socketmsg->m_outstr.GetBuffer());
                        }
#endif

                        if(IM_CHAT == socketmsg->m_imtype || IM_KEEPALIVE == socketmsg->m_imtype)
                        {
                            socketmsg->m_sendtime = time(NULL);

                            m_socketinfo.m_ackqueue.Push(socketmsg);

                            continue;
                        }
                    }
                    else
                    {
                        DoSocket(m_socketinfo.m_recvbuf.m_buffer, MSG_SEND_ERROR, false, sendmsgid);
                    }
                }
            }

            delete socketmsg;
        }    
    }
}

bool CImSocket::SendMsg(int socketfd, const char* outbuf, int outlen)
{
    int count = 0;

    fd_set ready; FD_ZERO(&ready); FD_SET(socketfd, &ready);

    struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 10 * 1000;

    while(outlen > 0)
    {
        ++count; count %= 10;

        if(0 == count) sleep_ms(1);

        if(select(socketfd + 1, NULL, &ready, NULL, &tv) > 0)
        {
            int len = send(socketfd, outbuf, outlen, 0);

            if(len <= 0) return false;

            outlen -= len; outbuf += len;
        }
        else
        {
            return false;
        }
    }

    return true;
}
