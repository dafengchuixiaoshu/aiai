#include "lanchat.h"
#include "immodule.h"
#include "BizCommon.h"

extern CQueue<int> g_lanchatawuqueue;

void CBusListener::FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
	m_lanchat->m_bus->EnableConcurrentCallbacks();

    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);

    if(ER_OK == m_lanchat->m_bus->JoinSession(name, m_lanchat->m_chatport, this, m_lanchat->m_sessionId, opts))
    {
        uint32_t timeout = 20;
        
        if(ER_OK == m_lanchat->m_bus->SetLinkTimeout(m_lanchat->m_sessionId, timeout))
        {
        	static int awuvalue = 1;

        	g_lanchatawuqueue.Push(awuvalue);

        	LOGD ("FoundAdvertisedName:%s,SessionId:%d successful\n", name, m_lanchat->m_sessionId);

            return;
        }
    }

    LOGD ("FoundAdvertisedName: %s not found\n", name);
}

void CBusListener::LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
	LOGD("Got LostAdvertisedName for %s from transport 0x%x\n", name, transport);
}

void CBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
	LOGD("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s\n", busName, previousOwner ? previousOwner : "<none>",
        newOwner ? newOwner : "<none>");
}

bool CBusListener::AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts)
{
    if(sessionPort == m_lanchat->m_chatport)
    {
    	LOGD("Accepting join session request from %s (opts.proximity=%x, opts.traffic=%x, opts.transports=%x)\n",
    			joiner, opts.proximity, opts.traffic, opts.transports);

    	return true;
    }
    else
    {
    	LOGD("Rejecting join attempt on non-chat session port %d\n", sessionPort);

    	return false;
    }
}

void CBusListener::SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner)
{
	uint32_t timeout = 20;

    m_lanchat->m_sessionId = id;

    LOGD("SessionJoined with %s (id=%d)\n", joiner, id);

    m_lanchat->m_bus->EnableConcurrentCallbacks();

    if (ER_OK == m_lanchat->m_bus->SetLinkTimeout(m_lanchat->m_sessionId, timeout))
    {
    	LOGD("Set link timeout to %d\n", timeout);
    }
    else
    {
    	LOGD("Set link timeout failed\n");
    }
}

static const char LAN_CHAT_NAME[] = "lanchat.aiai_";

CLanChat::CLanChat()
: BusObject("/chatService")//path
, m_job(0)
, m_bus(NULL)
, m_sessionId(0)
, m_chatsignalmember(NULL)
, m_sessionname(LAN_CHAT_NAME)
{
}

CLanChat::~CLanChat()
{
    if(NULL != m_bus)
    {
        delete m_bus;
    }
}

bool CLanChat::StartS(SessionPort chatport, string sessionname)
{
	m_chatport = chatport;

    return Start('s', sessionname);
}

bool CLanChat::StartJ(SessionPort chatport, string sessionname)
{
	m_chatport = chatport;

    return Start('j', sessionname);
}

bool CLanChat::Start(const char job, string& sessionname)
{
    m_job = job;

    m_sessionId = 0;

    m_sessionname += sessionname;

    if(NULL != m_bus) delete m_bus;

    const char* name = m_sessionname.c_str();

    m_bus = new BusAttachment("chat"/*name*/, true);

    if(NULL == m_bus)
    {
    	LOGD ("CLanChat::Start job:%c,name:%s bus is null\n", job, name);

    	return false;
    }

    QStatus status;

    InterfaceDescription* chatIntf = NULL;

    if(ER_OK != (status = m_bus->CreateInterface("org.alljoyn.bus.samples.chat"/*name*/, chatIntf)))
    {
    	LOGD ("CLanChat::Start job:%c,name:%s CreateInterface, %s\n", job, name, QCC_StatusText(status));

    	return false;
    }

    if(NULL == chatIntf)
    {
    	LOGD ("CLanChat::Start job:%c,name:%s interface is null\n", job, name);

    	return false;
    }

    chatIntf->AddSignal("Chat"/*name*/, "s",  "str", 0);

    chatIntf->Activate();

	m_busListener.SetChat(this);

	m_bus->RegisterBusListener(m_busListener);

	if (ER_OK != (status = m_bus->Start()))
	{
		LOGD ("CLanChat::Start job:%c,name:%s bus start, %s\n", job, name, QCC_StatusText(status));

		return false;
	}

	const InterfaceDescription* interface = m_bus->GetInterface("org.alljoyn.bus.samples.chat");

    if(NULL == interface)
    {
    	LOGD ("CLanChat::Start job:%c,name:%s get interface is null\n", job, name);

    	return false;
    }

	AddInterface(*interface);

	if(NULL == (m_chatsignalmember = interface->GetMember("Chat"/*name*/)))
	{
		LOGD ("CLanChat::Start job:%c,name:%s GetMember\n", job, name);

		return false;
	}

	if(ER_OK != (status = m_bus->RegisterSignalHandler(this,
			static_cast<MessageReceiver::SignalHandler>(&CLanChat::ChatSignalHandler),
			m_chatsignalmember, NULL)))
	{
		LOGD ("CLanChat::Start job:%c,name:%s RegisterSignalHandler, %s\n", job, name, QCC_StatusText(status));

		return false;
	}

	if(ER_OK != (status = m_bus->RegisterBusObject(*this)))
	{
		LOGD ("CLanChat::Start job:%c,name:%s RegisterBusObject, %s\n", job, name, QCC_StatusText(status));

		return false;
	}

	if(ER_OK != (status = m_bus->Connect()))
	{
		LOGD ("CLanChat::Start job:%c,name:%s Connect to %s, %s\n",
				job, name, m_bus->GetConnectSpec().c_str(), QCC_StatusText(status));

		return false;
	}

	if('s' == m_job)
	{
		SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES,
			true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);

		if(ER_OK != (status = m_bus->RequestName(name, DBUS_NAME_FLAG_DO_NOT_QUEUE)))
		{
			LOGD ("CLanChat::Start job:%c,name:%s RequestName, %s\n", job, name, QCC_StatusText(status));

			return false;
		}

		if(ER_OK != (status = m_bus->BindSessionPort(m_chatport, opts, m_busListener)))
		{
			LOGD ("CLanChat::Start job:%c,name:%s BindSessionPort, %s\n", job, name, QCC_StatusText(status));

			return false;
		}

		if(ER_OK != (status = m_bus->AdvertiseName(name, TRANSPORT_ANY)))
		{
			LOGD ("CLanChat::Start job:%c,name:%s AdvertiseName, %s\n", job, name, QCC_StatusText(status));

			return false;
		}
	}
	else// if('j' == m_job)
	{
		if(ER_OK != (status = m_bus->FindAdvertisedName(name)))
		{
			LOGD ("CLanChat::Start job:%c,name:%s FindAdvertisedName, %s\n", job, name, QCC_StatusText(status));

			return false;
		}
	}

	LOGD ("CLanChat::Start job:%c,name:%s successful\n", job, name);

	return true;
}

bool CLanChat::SendChatSignal(const char* msg)
{
    uint8_t flags = 0;

    MsgArg msgarg("s", msg);

    if(0 == m_sessionId)
    {
    	LOGD ("CLanChat::SendChatSignal:%s no sessionid\n", msg);

    	return false;
    }

    QStatus status = Signal(NULL, m_sessionId, *m_chatsignalmember, &msgarg, 1, 0, flags);

    if(ER_OK == status)
    {
    	LOGD ("CLanChat::SendChatSignal ok [%d]\n", strlen(msg));
        return true;
    }
    else
    {
    	LOGD ("CLanChat::SendChatSignal failed [%s]\n", QCC_StatusText(status));
        return false;
    }
}

void CLanChat::ChatSignalHandler(const InterfaceDescription::Member* member, const char* srcpath, Message& msg)
{
	LOGD("ChatSignalHandler this recv[%s:%d]\n", msg->GetSender(), msg->GetArg(0)->v_string.len);//str);

	m_recvbuffer += msg->GetArg(0)->v_string.str;

    string::size_type pos = m_recvbuffer.find("@");

    if(string::npos != pos)
	{
		int packlen = atoi(m_recvbuffer.c_str());

		if(packlen > 0 && packlen + pos + 1 <= m_recvbuffer.size())
		{
			string temp = m_recvbuffer.substr(pos + 1, packlen);

			char* outbuf;

			int outlen = base64_decode(outbuf, temp.c_str(), temp.size());

			if(outlen > 0)
			{
				string recvbuf; recvbuf.append(outbuf, outlen);

				delete outbuf;

				sImModule.DoSocket(recvbuf, MSG_RECV_MESSAGE, true);

				m_recvbuffer.erase(0, packlen + pos + 1);
			}
			else
			{
				LOGD ("ChatSignalHandler decode packet failed\n");
			}
		}
		else
		{
		    LOGD ("ChatSignalHandler packet is lost\n");
		}
	}

    LOGD ("ChatSignalHandler end:%s\n", msg->GetSender());
}
