#include "immodule.h"

#ifdef NO_LANCHAT
#define LOGD printf
#else
#include "BizCommon.h"
#endif

CQueue<int> g_lanchatawuqueue;

THREAD_HANDLE(module_upload, param)
{
	CImModule* immodule = (CImModule*)param;

	immodule->UploadThread();

#if defined(ANDROID)
    on_thread_closed(pthread_self());
#endif

    return 0;
}

THREAD_HANDLE(module_download, param)
{
	CImModule* immodule = (CImModule*)param;

	immodule->DownloadThread();

#if defined(ANDROID)
    on_thread_closed(pthread_self());
#endif

    return 0;
}

CImModule::~CImModule()
{
    m_istarthttpthread = false;

    sleep_s(1);

#ifndef NO_LANCHAT
    if(NULL != m_selflanchat)
	{
		delete m_selflanchat;
	}

	if(NULL != m_sendlanchat)
	{
		delete m_sendlanchat;
	}
#endif
}

bool CImModule::FindLanChatRoom(string& touser)
{
#ifndef NO_LANCHAT
	if(NULL != m_sendlanchat)
	{
		delete m_sendlanchat;
	}

	static int auwvalue = 1;

	m_sendlanchat = new CLanChat();

	while(g_lanchatawuqueue.Pop(auwvalue, -1) >= 0);

	if(NULL != m_sendlanchat && m_sendlanchat->StartJ(lan_chat_port, touser))
	{
		g_lanchatawuqueue.Pop(auwvalue, 1000);

		if(0 != m_sendlanchat->GetSessionId())
		{
			LOGD("FindLanChatRoom: %s successful\n\n", touser.c_str());

			return true;
		}
	}

	LOGD("FindLanChatRoom: %s failed\n\n", touser.c_str());
#endif

	return false;
}

bool CImModule::SendLanChat(CImMessage& immessage)
{
	bool isok = false;

#ifndef NO_LANCHAT
	m_lanchatmutex.Lock();

	if(FindLanChatRoom(immessage.m_touser))
	{
		TString ackstr;

		TString outstr;

		CImAck imack(immessage.m_id);

		if(immessage.ToLanchatBuffer(outstr, m_prototype))
        {
            char* outbuf;

            int outlen = base64_encode(outbuf, outstr.GetBuffer(), outstr.GetSize());

            if(outlen > 0)
            {
				string sendbuffer = to_string<int>(outlen);
				sendbuffer += "@";
				sendbuffer += outbuf;
				delete outbuf;

				if(imack.ToLanchatBuffer(ackstr, m_prototype)
					&& m_sendlanchat->SendChatSignal(sendbuffer.c_str()))
				{
					string ack; ack.append(ackstr.GetBuffer(), ackstr.GetSize());

					DoSocket(ack, MSG_RECV_MESSAGE, true);

					isok = true;
				}
            }
        }
	}

	m_lanchatmutex.Unlock();

	LOGD("SendLanChatMessage %s from:%s,to:%s\n\n",
			isok ? "successful" :"failed", immessage.m_fromuser.c_str(), immessage.m_touser.c_str());
#endif

    return isok;
}

bool CImModule::GetRequest(string& request, string& filepath, int sendhttptype)
{
    static string PREFIX = "--", LINEND = "\r\n";
    static string BOUNDARY = create_coreid();
    static string MULTIPART_FROM_DATA = "multipart/form-data";

    if(1 == sendhttptype)
    {
        string::size_type pos = filepath.rfind("/");

        if(string::npos == pos)
        {
            LOGD("parse filepath failed: %s\n\n", filepath.c_str());

            return false;
        }

        string prefix = "*";

        string filename = filepath.substr(pos + 1);

        pos = filename.rfind(".");

        if(string::npos != pos)
        {
            prefix = filename.substr(pos + 1);
        }

        int outlen; char* outbuf;

        if(0 == (outlen = file_read(outbuf, filepath.c_str())))
        {
            LOGD("open filepath failed: %s\n\n", filepath.c_str());

            return false;
        }

        request = "POST /upload HTTP/1.1\r\nAccept: text/html, application/xhtml+xml, */*\r\nReferer: http://123.57.251.23:4869/dadi\r\nAccept-Language: zh-Hans-CN,zh-Hans;q=0.5\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko\r\nHost: "
            + m_fileserverip + ":" + to_string<int>(m_fileserverport)
            + "\r\nContent-Type: multipart/form-data; boundary=" + BOUNDARY + 
            "\r\nDNT: 1\r\nCache-Control: no-cache\r\nConnection: Keep-Alive\r\nContent-Length: ";

        string body = PREFIX;
        body.append(BOUNDARY);
        body.append(LINEND);

        body.append("Content-Disposition: form-data; name=\"userfile\"; filename=\"");
        body.append(filename);
        body.append("\"");
        body.append(LINEND);
        
        body.append("Content-Type: image/");
        body.append(prefix);
        body.append("\"");
        body.append(LINEND);
        
        body.append(LINEND);
        body.append(outbuf, outlen);//body.append("12345");//
        body.append(LINEND);
        
        body.append(PREFIX);
        body.append(BOUNDARY);
        body.append(PREFIX);
        body.append(LINEND);

        int size = body.size();

        request += to_string<int>(size) + "\r\n\r\n" + body;

        delete outbuf;

        return true;
    }
    else if(0 == sendhttptype)
    {
        request = "GET /";
        request += filepath;
        request += " HTTP/1.1\r\nHost: " + m_fileserverip + ":" + to_string<int>(m_fileserverport);
        request += "\r\nContent-Length: 0\r\n\r\n";

        return true;
    }
    else if(2 == sendhttptype)
    {
        request = "GET /SpreeCMS/deleteKey.do?key=";
        request += filepath;
        request += " HTTP/1.1\r\nHost: " + m_deleteserverip + ":" + to_string<int>(m_deleteserverport);
        request += "\r\nContent-Length: 0\r\n\r\n";

        return true;
    }
    else
    {
    	return false;
    }
}

bool CImModule::SendHttRequest(string& key, string& request, int sendhttptype)
{
    if(0 == m_fileserverport || m_fileserverip.empty())
    {
        key = "http server is empty";

        return false;
    }
    else
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0); 

        if(fd <= 0)
        {
            key = "create http server socket failed";

            return false;
        }

        set_linger(fd);

        set_sendbuf_size(fd);

        set_recvbuf_size(fd);

        set_send_timeout(fd);

        set_recv_timeout(fd);

        struct sockaddr_in addr_in;

        addr_in.sin_family = AF_INET;

        addr_in.sin_port = htons(m_fileserverport);

        get_host_by_name(addr_in, m_fileserverip.c_str());

        if(connect(fd, (struct sockaddr*)&addr_in, sizeof(sockaddr_in)) < 0)
        {
            closesocket(fd);

            key = "connect to http server failed";

            return false;
        }

        if(SendMsg(fd, request.c_str(), request.size()))
        {
            if(1 == sendhttptype)
            {
                char recvbuffer[1024 + 1] = { 0 };

                int readlen = recv(fd, recvbuffer, 1024, 0);

                if(readlen > 0)
                {
                    string srecv(recvbuffer);

                    if(string::npos == srecv.find("success"))
                    {
                        key = "get error response1: ";
                        key += recvbuffer;

                        closesocket(fd);

                        return false;
                    }

                    static const char fmt[] = "\":\"";
                    static const int fmt_size = strlen(fmt);

                    string::size_type pos = srecv.find(fmt);

                    if(string::npos == pos || 0 == strnicmp(&srecv[pos + fmt_size], "error", 5))
                    {
                        key = "get error response2: ";
                        key += recvbuffer;

                        closesocket(fd);

                        return false;
                    }

                    key = srecv.substr(pos + fmt_size);

                    pos = key.find("\",\"");

                    if(string::npos == pos)
                    {
                        key = "parse response failed: ";

                        key += recvbuffer;

                        closesocket(fd);

                        return false;
                    }

                    key.erase(pos);

                    closesocket(fd);

                    LOGD("upload get %s response: %s\n\n", (key.size() < 9) ? "error" : "successful", recvbuffer);

                    return (key.size() < 9) ? false : true;
                }
                else
                {
                    key = "recv failed";

                    closesocket(fd);

                    return false;
                }
            }
            else if(0 == sendhttptype)
            {
                string srecv;

                while(1)
                {
                    char recvbuffer[1024 + 1] = { 0 };

                    int readlen = recv(fd, recvbuffer, 1024, 0);

                    if(readlen > 0)
                    {
                        srecv.append(recvbuffer, readlen);
                    }
                    else
                    {
                        break;
                    }
                }

                if(srecv.empty())
                {
                    key = "recv failed: ";
                    
                    key += request;

                    return false;
                }

                string filename = m_filepath + key;

                string::size_type pos = srecv.find("\r\n\r\n");

                if(string::npos == pos)
                {
                    key = "recv error: ";
                    
                    key += request;

                    return false;
                }

                string suvrecv = srecv.substr(pos + 4);

                if(0 == file_write(filename.c_str(), suvrecv.c_str(), suvrecv.size()))
                {
                    key = "save file failed: "; key += filename;

                    return false;
                }
                else
                {
                    key = "save file success: ";
                    
                    key += filename;

                    return true;
                }
            }
            else if(2 == sendhttptype)
            {
                char recvbuffer[1024 + 1] = { 0 };

                int readlen = recv(fd, recvbuffer, 1024, 0);

                if(readlen > 0)
                {
					LOGD ("CImModule::SendHttRequest delete [request:%s][response:%s]\r\n", request.c_str(), recvbuffer);
				}
                else
                {
					LOGD ("CImModule::SendHttRequest delete failed [request:%s]\r\n", request.c_str());
                }

                return true;
            }
            else
            {
            	return false;
            }
        }
        else
        {
            key = "send failed";

            closesocket(fd);

            return false;
        }
    }
}

void CImModule::UploadThread()
{
	CImMessage immessage;

    while(m_istarthttpthread)
    {
        if(m_uploadqueue.Pop(immessage) >= 0)
        {
            string request, errmsg;

        	if(!immessage.m_filekey.empty())
        	{
				if(GetRequest(request, immessage.m_filekey, 2))
				{
					SendHttRequest(errmsg, request, 2);
				}

        		continue;
        	}

            CImChat imchat;

        	TString outstr;

            char buffer[2048 + 1] = { 0 };

			imchat.m_id = immessage.m_id;
			imchat.m_to = immessage.m_touser;
			imchat.m_from = immessage.m_fromuser;
            imchat.m_type = immessage.m_msgtype;
            imchat.m_category = immessage.m_category;

			if(!immessage.m_videokey.empty())
			{
				if(GetRequest(request, immessage.m_videokey, 1))
				{
					if(SendHttRequest(errmsg, request, 1))
					{
#ifndef WIN32
//						if(unlink(immessage.m_videokey.c_str()) < 0)
//						{
//							LOGD ("CImModule::UploadThread delete video failed\r\n", immessage.m_videokey.c_str());
//						}
#endif
						immessage.m_videokey = errmsg;
					}
					else if(SendHttRequest(errmsg, request, 1))
					{
						immessage.m_videokey = errmsg;
					}
					else
					{
						goto UPLOADCONTINUE;
					}
				}
				else
				{
					errmsg = "get videokey request failed";

					goto UPLOADCONTINUE;
				}
			}

			if(immessage.m_imagekey.empty())
			{
				errmsg = "successful";
			}
			else if(GetRequest(request, immessage.m_imagekey, 1))
			{
				if(SendHttRequest(errmsg, request, 1))
				{
#ifndef WIN32
//					if(unlink(immessage.m_imagekey.c_str()) < 0)
//					{
//						LOGD ("CImModule::UploadThread delete image failed\r\n", immessage.m_imagekey.c_str());
//					}
#endif
					immessage.m_imagekey = errmsg;

					errmsg = "successful";
				}
				else if(SendHttRequest(errmsg, request, 1))
				{
					immessage.m_imagekey = errmsg;

					errmsg = "successful";
				}
				else
				{
					goto UPLOADCONTINUE;
				}
			}
			else
			{
				errmsg = "get imagekey request failed";

				goto UPLOADCONTINUE;
			}

            if(immessage.m_videokey.empty())
            {
            	static const char fmt[] = "{\"type\":1,\"img\":\"%s\",\"width\":%s,\"height\":%s,\"longitude\":%s,\"latitude\":%s,\"text\":\"%s\"}";

            	if(snprintf(buffer, 2048, fmt, immessage.m_imagekey.c_str(), immessage.m_imagewidth.c_str(),
            			immessage.m_imageheight.c_str(), (m_xpoint.empty()) ? "0" : m_xpoint.c_str(), (m_ypoint.empty()) ? "0" : m_ypoint.c_str(), immessage.m_txtbuffer.c_str()) <= 0)
            	{
            		errmsg = "snprintf body failed";

            		goto UPLOADCONTINUE;
            	}
            }
            else
            {
            	static const char fmt[] = "{\"type\":2,\"img\":\"%s\",\"video\":\"%s\",\"length\":%s,\"title\":\"%s\",\"longitude\":%s,\"latitude\":%s,\"text\":\"%s\"}";

            	if(snprintf(buffer, 2048, fmt, immessage.m_imagekey.c_str(), immessage.m_videokey.c_str(),
            			immessage.m_videolength.c_str(), immessage.m_videotitle.c_str(), (m_xpoint.empty()) ? "0" : m_xpoint.c_str(), (m_ypoint.empty()) ? "0" : m_ypoint.c_str(), immessage.m_txtbuffer.c_str()) <= 0)
            	{
            		errmsg = "snprintf body failed";

            		goto UPLOADCONTINUE;
            	}
            }

            imchat.m_body = buffer;

            if(imchat.ToBuffer(outstr, m_prototype))
            {
                PushToSendQueue(imchat.m_imtype, imchat.GetMsgId(), outstr);
            }
            else
            {
                errmsg = "ToBuffer failed";
            }

UPLOADCONTINUE:

			if("successful" != errmsg && NULL != m_callback)
			{
	            CSocketMsg socketmsg;

	            socketmsg.m_im = &imchat;

                socketmsg.m_imtype = imchat.m_imtype;

                socketmsg.m_sendmsgid = imchat.m_id;

	            (*m_callback)(MSG_SEND_ERROR, socketmsg);

	            socketmsg.m_im = NULL;
			}

            LOGD("UploadThread %s, fromuser:%s,touser:%s,body:%s\n\n",
            		errmsg.c_str(), imchat.m_from.c_str(), imchat.m_to.c_str(), imchat.m_body.c_str());
        }
    }
}

string::size_type parse_chat_buffer(string& outbuffer, string& body, string::size_type startpos)
{
	string::size_type findpos = body.find("\":\"", startpos);

	if(string::npos != findpos)
	{
		findpos += 3;

		string::size_type endpos = body.find("\",\"", findpos);

		if(string::npos != findpos)
		{
            if(endpos == findpos)
            {
                outbuffer = "";
            }
            else
            {
                outbuffer = body.substr(findpos, endpos - findpos);
            }

			return endpos + 3;
		}
	}

	return string::npos;
}

void CImModule::DownloadThread()
{
	CImChat imchat;

    while(m_istarthttpthread)
    {
        if(m_downloadqueue.Pop(imchat) >= 0)
        {
        	TString outstr;

        	bool ispush = false;

        	int networkstatus = 1;

            CSocketMsg socketmsg;

            string request, errmsg;

            socketmsg.m_im = &imchat;

        	string videokey, imagekey;

    		string::size_type startpos = 0;

    		if(string::npos == (startpos = parse_chat_buffer(imagekey, imchat.m_body, startpos)))
    		{
    			ispush = true;

    			errmsg = "a old protocol msg";

				goto DOWNLOADCONTINUE;
    		}

    		networkstatus = GetNetworkStatus();

    		LOGD("DownloadThread networkstatus: %d\n\n", networkstatus);

    		if(1 == networkstatus || 3 == networkstatus)
			{
				if(!imagekey.empty())
				{
					if(GetRequest(request, imagekey, 0))
					{
						if(!SendHttRequest(imagekey, request, 0))
						{
							errmsg = imagekey;

							goto DOWNLOADCONTINUE;
						}
					}
					else
					{
						errmsg = "get imagekey request failed";

						goto DOWNLOADCONTINUE;
					}
				}

				if("video" == imchat.m_category)
				{
					if(string::npos == (startpos = parse_chat_buffer(videokey, imchat.m_body, startpos)))
					{
						errmsg = "get videokey failed";

						goto DOWNLOADCONTINUE;
					}

					if(GetRequest(request, videokey, 0))
					{
						if(!SendHttRequest(videokey, request, 0))
						{
							errmsg = videokey;

							goto DOWNLOADCONTINUE;
						}
					}
					else
					{
						errmsg = "get videokey request failed";

						goto DOWNLOADCONTINUE;
					}
				}
			}

			ispush = true;

			errmsg = "successful";

DOWNLOADCONTINUE:

            if(ispush && NULL != m_callback)
            {
            	CImAck imack(socketmsg.m_im->m_id);

            	PushSend(imack);

                (*m_callback)(MSG_RECV_MESSAGE, socketmsg);
            }

            LOGD("DownloadThread %s, fromuser:%s,touser:%s,buffer:%s\n\n",
                errmsg.c_str(), imchat.m_from.c_str(), imchat.m_to.c_str(), imchat.m_body.c_str());
        
            socketmsg.m_im = NULL;
        }
    }
}

bool CImModule::Start(const char* ip, int port, int timeout, string prototype, im_module_callback* callback)
{
    m_callback = callback;

    m_prototype = prototype;

    if(CImSocket::Start(ip, port, timeout, m_prototype))
    {
        if(m_istarthttpthread)
        {
            return true;
        }

        m_istarthttpthread = true;

        if(m_uploadthread.Start(module_upload, this))
        {
            if(m_downloadthread.Start(module_download, this))
            {
                return true;
            }
        }
    }

    m_istarthttpthread = false;

    return false;
}

bool CImModule::StartXml(const char* ip, int port, int timeout, im_module_callback* callback)
{
	return Start(ip, port, timeout, "XML", callback);
}

bool CImModule::StartByte(const char* ip, int port, int timeout, im_module_callback* callback)
{
	return Start(ip, port, timeout, "BYTE", callback);
}

bool CImModule::PushSend(CIm& im)
{
#ifndef NO_LANCHAT
    if(IM_LOGIN == im.m_imtype)
	{
    	if(NULL != m_selflanchat)
    	{
    		delete m_selflanchat;
    	}

        m_selflanchat = new CLanChat();

        if(NULL == m_selflanchat)
        {
        	LOGD("start lan chat pointer is null\n\n");

        	return false;
        }
        else if(((CImLogin&)im).m_from.empty())
        {
        	LOGD("start lan chat from empty\n\n");

        	return false;
        }
        else if(!m_selflanchat->StartS(lan_chat_port, ((CImLogin&)im).m_from))
		{
        	LOGD("start lan chat: %s failed\n\n", ((CImLogin&)im).m_from.c_str());

			//return false;
		}
		else
		{
			LOGD("start lan chat: %s successful\n\n", ((CImLogin&)im).m_from.c_str());
    	}

        m_socketinfo.m_loginstatus = 1;
	}
#endif

    if(IM_MESSAGE == im.m_imtype)
    {
    	m_uploadqueue.Push((CImMessage&)im);

    	return true;
    }

	TString outstr;

    if(!im.ToBuffer(outstr, m_prototype)) return false;

    return PushToSendQueue(im.m_imtype, im.GetMsgId(), outstr);
}

void CImModule::OnCallBack(int msgtype, CSocketMsg& socketmsg, bool islanchat)
{
	LOGD("===== OnCallBack msgtype: %d =====\n\n", msgtype);

    switch(msgtype)
    {
    case MSG_WORK_CLEAR:
    case MSG_SEND_ERROR:
    case MSG_SEND_CLOSED:
    case MSG_RECV_CLOSED:
    case MSG_SOCKET_CONNECT:
    case MSG_SOCKET_TIMEOUT:
        {
            CSocketMsg* ackmsg;

            int acktype = (MSG_SOCKET_TIMEOUT == msgtype) ? MSG_SOCKET_TIMEOUT : MSG_WORK_CLEAR;

            while(m_socketinfo.m_ackqueue.Pop(ackmsg, -1) >= 0)
            {
                if(NULL != m_callback)
                {
                    (*m_callback)(acktype, *ackmsg);
                }

                delete ackmsg;
            }
        }
        break;

    case MSG_RECV_MESSAGE:
        {
            switch(socketmsg.m_im->m_imtype)
            {
            case IM_PGCREATE:
            case IM_PGINVITE:
            case IM_PGQUIT:
            case IM_PGMODIFY:
            case IM_PGCARD:
            case IM_SNS:
            case IM_FANSMODIFY:
            case IM_FANSAPPEND:
            case IM_FANSDELETE:
                {
                	if(!islanchat)
                    {
                		CImAck imack(socketmsg.m_im->m_id);

                		PushSend(imack);
                    }
                }
                break;

            case IM_CHAT:
                {
                	CImChat* imchat = (CImChat*)(socketmsg.m_im);

                	m_downloadqueue.Push(*imchat);
                }
                return;

            case IM_ACK:
                {
                	if(!islanchat)
					{
						if(ClearQueue(msgtype, socketmsg))
						{
							return;
						}
					}
                }
                break;

            case IM_KEEPALIVE:
                {
                    ClearQueue(msgtype, socketmsg);
                }
                return;

            case IM_KICKOUT:
                {
                    Release();
                }
                break;

			case IM_MESSAGE:
                {
                	string imagekey, videokey;

                	char buffer[2048 + 1] = { 0 };

                	CImMessage* immessage = (CImMessage*)(socketmsg.m_im);

                	if(file_decode(imagekey, m_filepath, immessage->m_imagekey))
                	{
                		if(immessage->m_videokey.empty())
                		{
                        	static const char fmt[] = "{\"type\":1,\"img\":\"%s\",\"width\":%s,\"height\":%s,\"longitude\":%s,\"latitude\":%s,\"text\":\"%s\"}";

                        	if(snprintf(buffer, 2048, fmt, imagekey.c_str(), immessage->m_imagewidth.c_str(),
                        			immessage->m_imageheight.c_str(), (m_xpoint.empty()) ? "0" : m_xpoint.c_str(), (m_ypoint.empty()) ? "0" : m_ypoint.c_str(), immessage->m_txtbuffer.c_str()) <= 0)
                        	{
    							LOGD("immessage sprintf imagekey failed, fromuser:%s,touser:%s\n\n",
    									immessage->m_fromuser.c_str(), immessage->m_touser.c_str());
    							return;
                        	}
                		}
                		else if(file_decode(videokey, m_filepath, immessage->m_videokey))
                		{
                        	static const char fmt[] = "{\"type\":2,\"img\":\"%s\",\"video\":\"%s\",\"length\":%s,\"title\":\"%s\",\"longitude\":%s,\"latitude\":%s,\"text\":\"%s\"}";

                        	if(snprintf(buffer, 2048, fmt, imagekey.c_str(), videokey.c_str(),
                        			immessage->m_videolength.c_str(), immessage->m_videotitle.c_str(), (m_xpoint.empty()) ? "0" : m_xpoint.c_str(), (m_ypoint.empty()) ? "0" : m_ypoint.c_str(), immessage->m_txtbuffer.c_str()) <= 0)
                        	{
    							LOGD("immessage sprintf videokey failed, fromuser:%s,touser:%s\n\n",
    									immessage->m_fromuser.c_str(), immessage->m_touser.c_str());
    							return;
                        	}
                		}
                		else
						{
							LOGD("immessage file_decode videokey failed, fromuser:%s,touser:%s\n\n",
									immessage->m_fromuser.c_str(), immessage->m_touser.c_str());
							return;
						}
                	}
                	else
                	{
        				LOGD("immessage file_decode imagekey failed, fromuser:%s,touser:%s\n\n",
        						immessage->m_fromuser.c_str(), immessage->m_touser.c_str());

        				return;
                	}

                	CSocketMsg msg;

                	string sbuffer(buffer);

                	CImChat imchat(immessage->m_fromuser, immessage->m_touser,
                			immessage->m_msgtype, immessage->m_category, sbuffer, immessage->m_id);

                	msg.m_im = &imchat;

                    if(NULL != m_callback)
                    {
                        (*m_callback)(MSG_RECV_MESSAGE, msg);
                    }

                    LOGD("recv lanchat message successful, fromuser:%s,touser:%s\n\n",
                    		immessage->m_fromuser.c_str(), immessage->m_touser.c_str());

                    msg.m_im = NULL;
                }
            	return;
            }
        }
        break;

    default:
        return;
    }

    if(NULL != m_callback)
    {
        (*m_callback)(msgtype, socketmsg);
    }
}

bool CImModule::ClearQueue(int& msgtype, CSocketMsg& socketmsg)
{
    CSocketMsg* ackmsg = NULL;

    int key = m_socketinfo.m_ackqueue.Pop(ackmsg, -1);

    if(key < 0)
    {
        return true;
    }
    else if(ackmsg->m_sendmsgid != socketmsg.m_im->m_id)
    {
        do
        {
            if(NULL != m_callback)
            {
                (*m_callback)(MSG_WORK_CLEAR, *ackmsg);
            }

            delete ackmsg;

        }while(m_socketinfo.m_ackqueue.Pop(ackmsg, -1) >= 0);

        m_socketinfo.clear();

        return true;
    }

    return false;
}

void CImModule::SetMapPoint(string xpoint, string ypoint)
{
	m_xpoint = xpoint; m_ypoint = ypoint;

	CImMapPoint immappoint(xpoint, ypoint);

	PushSend(immappoint);
}

