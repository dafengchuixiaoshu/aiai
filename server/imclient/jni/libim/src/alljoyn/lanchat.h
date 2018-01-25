#ifndef _ALLJOYN_LANCHAT_H_
#define _ALLJOYN_LANCHAT_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>
#include <qcc/Log.h>
#include <qcc/String.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <signal.h>

#include "header.h"

using namespace ajn;

class CLanChat;

class CBusListener : public BusListener, public SessionPortListener, public SessionListener
{
public:

	void SetChat(CLanChat* lanchat){ m_lanchat = lanchat; }

	virtual void FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);
	//void FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);

	virtual void LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);
	//void LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);

	virtual void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);
	//void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);

	virtual bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts);
	//bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts);

	virtual void SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner);
	//void SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner);

private:

	CLanChat* m_lanchat;
};

const int lan_chat_port = 27;

class CLanChat : public BusObject
{
	friend class CBusListener;

	bool Start(const char job, string& sessionname);

public:

	CLanChat();

	~CLanChat();

    SessionId GetSessionId(){ return m_sessionId; }

	bool StartS(SessionPort chatport, string sessionname);

	bool StartJ(SessionPort chatport, string sessionname);

	bool SendChatSignal(const char* msg);

    void ChatSignalHandler(const InterfaceDescription::Member* member, const char* srcpath, Message& msg);

private:

    char m_job;

    string m_recvbuffer;

    SessionId m_sessionId;

    SessionPort m_chatport;

    string m_sessionname;

    ajn::BusAttachment* m_bus;

    CBusListener m_busListener;

    const InterfaceDescription::Member* m_chatsignalmember;
};

#endif //_ALLJOYN_LANCHAT_H_
