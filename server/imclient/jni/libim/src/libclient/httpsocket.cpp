
#include "httpsocket.h"
#include "BizCommon.h"

void CHttpSocket::set_linger(int fd)
{
    static struct linger ln; 

    ln.l_onoff = 1; ln.l_linger = 0;

    static const int len = sizeof(ln);

    setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&ln, len);
}

void CHttpSocket::set_sendbuf_size(int fd)
{
    int size = 512 * 1024;

    static const int len = sizeof(size);

    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&size, len);
}

void CHttpSocket::set_recvbuf_size(int fd)
{
    int size = 512 * 1024;

    static const int len = sizeof(size);

    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&size, len);
}

#ifdef WIN32

void CHttpSocket::set_nonblock(int fd)
{
    unsigned long x = 1;

    ioctlsocket(fd, FIONBIO, (unsigned long*)&x);
}

void CHttpSocket::set_send_timeout(int fd)
{
    int timeout = 30 * 1000;

    static const int len = sizeof(timeout);

    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, len);
}

void CHttpSocket::set_recv_timeout(int fd)
{
    int timeout = 30 * 1000;

    static const int len = sizeof(timeout);

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, len);
}

#else

void CHttpSocket::set_blocking(int fd, bool isblock)
{
   int opts = fcntl(fd, F_GETFL);

   if(isblock) opts &= ~O_NONBLOCK; else opts |= O_NONBLOCK;

   fcntl(fd, F_SETFL, opts);
}

void CHttpSocket::set_send_timeout(int fd)
{
    struct timeval timeout = { 30, 0 };

    static const int len = sizeof(timeout);

    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
}

void CHttpSocket::set_recv_timeout(int fd)
{
    struct timeval timeout = { 30, 0 };

    static const int len = sizeof(timeout);

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len);
}

#endif

 bool CHttpSocket::get_host_by_name(struct sockaddr_in& addr_in, const char* address)
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

CHttpSocketInfo::CHttpSocketInfo()
: m_socketfd(0)
{
}

CHttpSocketInfo::~CHttpSocketInfo()
{
	clear();
}

int CHttpSocketInfo::size()
{
    return m_recvbuf.size();
}

int CHttpSocketInfo::getfd()
{
    int fd = m_socketfd;
    return fd;
}

void CHttpSocketInfo::clear()
{
    if(m_socketfd > 0)
    {
        closesocket(m_socketfd);

        m_socketfd = 0;
    }

    m_recvbuf.clear();
}

const char* CHttpSocketInfo::c_str()
{
    return m_recvbuf.c_str();
}

bool CHttpSocketInfo::erase(int outlen)
{
    return m_recvbuf.erase(outlen);
}

void CHttpSocketInfo::getaddr(string& ip, int& port)
{
    ip = m_ip; port = m_port;
}

void CHttpSocketInfo::setaddr(const char* ip, int port)
{
    m_ip = ip; m_port = port;
}

bool CHttpSocketInfo::append(const char* inbuf, int inlen)
{
    return m_recvbuf.append(inbuf, inlen, 256);
}


CHttpSocket::CHttpSocket()
{
}

CHttpSocket::~CHttpSocket()
{
	Release();
}

void CHttpSocket::Release()
{
	m_mutex.Lock();
    if(m_socketinfo.m_socketfd > 0)
    {
        closesocket(m_socketinfo.m_socketfd);
        m_socketinfo.m_socketfd = 0;
    }
    LOGD("CHttpSocket::Release().\n");
    m_mutex.Unlock();
}

bool CHttpSocket::Connect()
{
    string ip; int port = 0;

    m_socketinfo.clear();

    m_socketinfo.getaddr(ip, port);

    //ip = "223.202.100.246";

    if(ip.empty() || 0 == port) return false;

    m_mutex.Lock();
    int fd = socket(AF_INET, SOCK_STREAM, 0); 
    m_socketinfo.m_socketfd = fd;

    if(fd <= 0)
    {
    	m_mutex.Unlock();
        return false;
    }
    m_mutex.Unlock();

    set_linger(fd);

    set_sendbuf_size(fd);

    set_recvbuf_size(fd);

    set_send_timeout(fd);

    set_recv_timeout(fd);

    set_blocking(fd,false);//设为非阻塞

    struct sockaddr_in addr_in;

    addr_in.sin_family = AF_INET;

    addr_in.sin_port = htons(port);

    get_host_by_name(addr_in, ip.c_str());

    int retrytime = 0;

    int uptime = 0;
    uptime = time(NULL);

    while(retrytime <= 3)//连接失败重连3次
    {
	    if(m_socketinfo.m_socketfd <= 0)
	    {
	    	LOGD("Socket is invalid.\n");
	    	return false;
	    }

    	int ret = connect(fd,(struct sockaddr*)&addr_in,sizeof addr_in);

		if(ret < 0)
		{
			struct timeval tv;
			tv.tv_sec = 0;//轮询，不设置超时，否则程序退出会阻塞住
			tv.tv_usec = 0;

			fd_set wset;
			FD_ZERO(&wset);
			FD_SET(fd,&wset);

		   select(fd+1,NULL,&wset,NULL,&tv);

		   if(FD_ISSET(fd, &wset) > 0)
		   {
			    set_blocking(fd,true);//设为阻塞
				m_socketinfo.m_socketfd = fd;
				//LOGD("Connect Server Success.\n");
				return true;
		   }

		   int curtime = time(NULL);

		   if(curtime - uptime > 10)//通过时间计算是否超时，超时进行重连,10秒超时
		   {
			   if(retrytime == 3)
			   {
				    LOGD("Connect Server 3 Times.Not Try Again.\n");
					close(fd);
					return false;
			   }
			   retrytime++;
			   uptime = curtime;
			   LOGD("Connect Server TimeOut. Retry:%d\n",retrytime);

		   }

		}
		else if (ret == 0)
		{
			set_blocking(fd,true);//设为阻塞
			m_socketinfo.m_socketfd = fd;
		//	LOGD("Connect Server Success.\n");
			return true;
		}
		else
		{
			LOGD("Connect Server Unknown Error.\n");
			close(fd);
			return false;
		}
    }

    return false;

}

bool CHttpSocket::Start(const char* ip, int port)
{
    m_socketinfo.setaddr(ip, port);
    return Connect();
}

bool CHttpSocket::SendHttpRequest(int method,string uri, string body,int body_size,int& retcode,int& errcode,int& extend,string &retcontect)
{
	string MethodName;
	string url = "/" + uri;
	string request;
	string head;

	switch (method)
	{
	case 0:
		MethodName = "PUT";
		break;
	case 1:
		MethodName = "DELETE";
		break;
	case 2:
		MethodName = "GET";
		break;
	case 3:
		MethodName = "POST";
		break;
	}

	char lengthHeader[256];
	if(body_size > 0)
	{
		sprintf(lengthHeader,"Content-Length:%d\r\n",body_size);
	}
	else
	{
		sprintf(lengthHeader,"Content-Length:%d\r\n",0);
	}

	head = MethodName + " " + url + " " + "HTTP/1.1\r\n" + lengthHeader + "\r\n";

	request = head + body;

	bool ret = sendrequest((char*)request.c_str(),request.length(),retcode,errcode,extend,retcontect);

	return ret;

}
bool CHttpSocket::sendrequest(char* sendbuf,int bufferlen,int& retcode,int& errcode,int& extend,string &retcontect)
{
	int socketfd = m_socketinfo.getfd();

	int retrytime = 0;
	bool sendok = true;

	while(retrytime <= 3)//发送失败重连3次
	{
		int len = 0;
		while(bufferlen > 0)
		{
			len = send(socketfd, sendbuf, bufferlen, 0);

			if(len <= 0)
			{
				LOGD("sendrequest send error.\r\n");
				if(retrytime == 3)
				{
					closesocket(socketfd);
					return false;
				}
				retrytime++;
				LOGD("sendrequest retry times %d.\r\n",retrytime);
				sendok = false;
				break;
			}
			else
			{
				bufferlen -= len;
				sendbuf += len;
			}
		}

		if(sendok)
		{
			break;
		}
	}

	bool stillrecv = true;

	while(stillrecv)
	{
		char inbuf[1024] = { 0 };

		int inlen = recv(socketfd, inbuf, 1024, 0);

		if (inlen == 0)
		{
			stillrecv = false;
			break;
		}

		if(inlen <0)
		{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
				continue;

			closesocket(socketfd);
			stillrecv = false;
			LOGD("sendrequest recv error.\r\n");
			return false;
		}

		m_socketinfo.append(inbuf, inlen);
	}

	string recvbuffer = m_socketinfo.m_recvbuf.c_str();

	parseresponse(recvbuffer,retcode,errcode,extend,retcontect);

	closesocket(socketfd);

	if(retcode == 200)
	{
		return true;
	}
	else
	{
		return false;
	}

}
void CHttpSocket::parseresponse(string buffer,int& retcode,int& errcode,int& extend,string& body)
{
	if(buffer.empty())
	{
		LOGD("parseresponse buffer is empty.\r\n");
		return;
	}
	LOGD("parseresponse buffer is %s.\r\n",buffer.c_str());

	int nBodyOffset = 0;
	int nIdxBeg = buffer.find(" ");//第一个空格
	int nIdxEnd = buffer.find(" ", nIdxBeg + 1);//查找第二个空格
	string responseCode = buffer.substr( nIdxBeg + 1, nIdxEnd - nIdxBeg - 1);

	//返回状态值
	retcode = atoi(responseCode.c_str());
	LOGD("parseresponse statuscode:%s.\r\n",responseCode.c_str());

	int warnid = 0;
	int extendid = 0;

	if(responseCode == "403")
	{
		static const char fmt[] = "Warnid: ";
		static const int fmt_len = strlen(fmt);
		warnid = atoi(&(buffer[buffer.find(fmt) + fmt_len]));
		errcode = warnid;

		static const char fmt2[] = "Warning: ";
		static const int fmt_len2 = strlen(fmt2);
		extendid = atoi(&(buffer[buffer.find(fmt2) + fmt_len2]));
		extend = extendid;
		LOGD("warnid:%d,warning:%d.\r\n",errcode,extend);
	}

	//获取实际的包体数据
	nIdxBeg = buffer.find("\r\n\r\n");

	int nDataIdx = nBodyOffset + nIdxBeg + 4;

	body = buffer.substr(nDataIdx,buffer.length() - nDataIdx);
	LOGD("parseresponse body:%s.\r\n",body.c_str());
	return;
}
