#include "BizCommon.h"
#include "CConfigure.h"
#include "CWorkerContainer.h"
#include "CXmppStack.h"
#include "CObserverContainer.h"
#include "CWorkGroup.h"
#include "CWorkContact.h"
#include "CWorkLogin.h"
#include "CWorkMessage.h"
#include "IXCapInterface.h"

#ifndef USE_SQLITE
#include "memsave.h"
#endif

CWorkMessage* CWorkMessage::g_pWorkMessage = NULL;

template<class T>
static int find(vector<T> &vec, T data)
{
	int index = 0;
	typename vector<T>::iterator iter;
	for (iter = vec.begin(); iter != vec.end(); iter++)
	{
		if (*iter == data)
		{
			// found it
			return index;
		}

		index++;
	}

	// can't find
	return -1;
}

CWorkMessage::CWorkMessage(void)
{
}

CWorkMessage::~CWorkMessage(void)
{
}

CWorkMessage* CWorkMessage::GetInstance()
{
	if (g_pWorkMessage == NULL)
	{
		g_pWorkMessage = new CWorkMessage();
	}
	return g_pWorkMessage;
}

HRESULT CWorkMessage::Init()
{
	return S_OK;
}

VOID CWorkMessage::Release()
{
	if (g_pWorkMessage != NULL)
	{
		delete g_pWorkMessage;
		g_pWorkMessage = NULL;
	}
}

HRESULT CWorkMessage::CreateTable()
{
#ifdef USE_SQLITE
	// create table
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	if (sqliteImpl)
	{
		const char * sSQL1 =
				"CREATE TABLE IF NOT EXISTS [sessions] ("
				"[rowid] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
				"[session_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[msg_id] VARCHAR(255) DEFAULT (''),"
				"[type] INTEGER NOT NULL DEFAULT (0),"
				"[kind] INTEGER NOT NULL DEFAULT (1),"
				"[unread_num] INTEGER NOT NULL DEFAULT (0),"
				"[top] INTEGER NOT NULL DEFAULT (0),"
				"[remind] INTEGER NOT NULL DEFAULT (1),"
				"[draft_content] VARCHAR(2048) DEFAULT (''),"
				"[draft_changed] INTEGER NOT NULL DEFAULT (0),"
				"[update_time] TIMESTAMP DEFAULT (''),"
				"UNIQUE ([session_id] ASC)"
				");"
				"CREATE INDEX IF NOT EXISTS [idx_sessions_rowid] ON [sessions] ([rowid] ASC); "
				"CREATE INDEX IF NOT EXISTS [idx_sessions_sessionid] ON [sessions] ([session_id] ASC); "
				"CREATE INDEX IF NOT EXISTS [idx_sessions_msgid] ON [sessions] ([msg_id] ASC); "
				"CREATE TABLE IF NOT EXISTS [messages] ("
				"[rowid] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
				"[msg_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[session_id] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[sender] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[receiver] VARCHAR(255) NOT NULL DEFAULT (''),"
				"[save] INTEGER NOT NULL DEFAULT (0),"
				"[share] INTEGER NOT NULL DEFAULT (0),"
				"[type] INTEGER NOT NULL DEFAULT (0),"
				"[kind] INTEGER NOT NULL DEFAULT (0),"
				"[direction] INTEGER NOT NULL DEFAULT (0),"
				"[unread] INTEGER NOT NULL DEFAULT (0),"
				"[played] INTEGER NOT NULL DEFAULT (0),"
				"[send] INTEGER NOT NULL DEFAULT (0),"
				"[category] INTEGER NOT NULL DEFAULT (0),"
				"[online] INTEGER NOT NULL DEFAULT (1),"
				"[content] VARCHAR(2048) NOT NULL DEFAULT (0),"
				"[client_time] TIMESTAMP NOT NULL DEFAULT (''),"
				"[server_time] TIMESTAMP NOT NULL DEFAULT (''),"
				"[create_time] TIMESTAMP NOT NULL DEFAULT (datetime('now', 'localtime')),"
				"UNIQUE ([msg_id] ASC)"
				");"
				"CREATE INDEX IF NOT EXISTS [idx_messages_rowid] ON [messages] ([rowid] ASC); "
				"CREATE INDEX IF NOT EXISTS [idx_messages_msgid] ON [messages] ([msg_id] ASC); "
				"CREATE INDEX IF NOT EXISTS [idx_messages_sessionid] ON [messages] ([session_id] ASC); "
				;
//		LOGD("sSQL1=%s\n", sSQL1);
		sqliteImpl->ExecSql(sSQL1);
	}
#endif

	return S_OK;
}

void CWorkMessage::SendImage(string& fromuser, string& touser, string& imagepath,
		string& txtbuffer, int imagewidth, int imageheight, int msgtype)
{
	CXmppStack* pXmppStack = CXmppStack::GetInstance();

	pXmppStack->SendImage(fromuser, touser, imagepath, txtbuffer, imagewidth, imageheight, msgtype);
}

void CWorkMessage::SendVideo(string& fromuser, string& touser, string& videopath,
		string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, int msgtype)
{
	CXmppStack* pXmppStack = CXmppStack::GetInstance();

	pXmppStack->SendVideo(fromuser, touser, videopath, faceimagepath, txtbuffer, videotitle, videolength, msgtype);
}

void CWorkMessage::SetBlackListFlag(string& blackid, int type)
{
	CXmppStack* pXmppStack = CXmppStack::GetInstance();

	pXmppStack->SetBlackListFlag(blackid, type);
}

HRESULT CWorkMessage::Invoke (VOID* pInvoke)
{
	ReqParams* pReqParams = (ReqParams*)pInvoke;
	enum ReqType eReqType = pReqParams->eReqType;

	switch (eReqType)
	{
	case RT_SEND_MESSAGE:
		//SendMessage(*pReqParams->pMessageParams);
		break;

	case RT_RESEND_MESSAGE:
		//ResendMessage(pReqParams->pMessageParams->pSessionId, pReqParams->pMessageParams->pMsgId);
		break;

	default:
		break;
	}

	return S_OK;
}

SessionList CWorkMessage::GetSessionList ()
{
	LOGD ("Get Session List\r\n");

	int nResult = E_FAIL;
	SessionList pSessionList = {};
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT DISTINCT "
			"[session_id], [msg_id], [type], [kind], [unread_num], [top], [remind], "
			"[draft_content], [draft_changed] "
			"FROM [sessions] "
			"ORDER BY [top] DESC, [update_time] DESC; "
			);
//	LOGD("sSQL1=%s\n", sSQL1.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pSessionList;
	}

	// query something
	pSessionList.nCount = query_rows.n_rows;

	for (int i=0; i<query_rows.n_rows; i++)
	{
		SessionParams pSessionParams = {};

		pSessionParams.pSessionId    =                    sqliteImpl->GetStrValue(&query_rows, i, 0);
		pSessionParams.pMsgId        =                    sqliteImpl->GetStrValue(&query_rows, i, 1);
		pSessionParams.eType         = (enum MessageType) sqliteImpl->GetIntValue(&query_rows, i, 2);
		pSessionParams.eKind         = (enum KindType)    sqliteImpl->GetIntValue(&query_rows, i, 3);
		pSessionParams.nUnreadNum    =                    sqliteImpl->GetIntValue(&query_rows, i, 4);
		pSessionParams.eTop          = (enum TopStatus)   sqliteImpl->GetIntValue(&query_rows, i, 5);
		pSessionParams.eRemind       = (enum RemindStatus)sqliteImpl->GetIntValue(&query_rows, i, 6);
		pSessionParams.pDraftContent =                    sqliteImpl->GetStrValue(&query_rows, i, 7);
		pSessionParams.eDraftChanged = (enum DraftChanged)sqliteImpl->GetIntValue(&query_rows, i, 8);

		// session name and portrait key need to be set data by message type
		map<string, string> mapData = RetrieveContactOrGroup (pSessionParams.eType, pSessionParams.pSessionId);
		pSessionParams.pSessionName = mapData["SessionName"];
		pSessionParams.pPortraitKey = mapData["PortraitKey"];

		pSessionList.ppSessionParams.push_back(pSessionParams);
	}

	return pSessionList;
}

SessionParams CWorkMessage::GetSession (const string& pSessionId)
{
	LOGD ("Get Session\r\n");

	int nResult = E_FAIL;
	SessionParams pSessionParams = {};
	struct query_rows_s query_rows = {};

	if (pSessionId.empty())
	{
		LOGD ("SessionId params error\r\n");
		return pSessionParams;
	}

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT "
			"[session_id], [msg_id], [type], [kind], [unread_num], [top], [remind], "
			"[draft_content], [draft_changed] "
			"FROM [sessions] "
			"WHERE [session_id]='%s';"
			,
			pSessionId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pSessionParams;
	}

	// query something
	for (int i=0; i<query_rows.n_rows; i++)
	{
		pSessionParams.pSessionId    =                    sqliteImpl->GetStrValue(&query_rows, i, 0);
		pSessionParams.pMsgId        =                    sqliteImpl->GetStrValue(&query_rows, i, 1);
		pSessionParams.eType         = (enum MessageType) sqliteImpl->GetIntValue(&query_rows, i, 2);
		pSessionParams.eKind         = (enum KindType)    sqliteImpl->GetIntValue(&query_rows, i, 3);
		pSessionParams.nUnreadNum    =                    sqliteImpl->GetIntValue(&query_rows, i, 4);
		pSessionParams.eTop          = (enum TopStatus)   sqliteImpl->GetIntValue(&query_rows, i, 5);
		pSessionParams.eRemind       = (enum RemindStatus)sqliteImpl->GetIntValue(&query_rows, i, 6);
		pSessionParams.pDraftContent =                    sqliteImpl->GetStrValue(&query_rows, i, 7);
		pSessionParams.eDraftChanged = (enum DraftChanged)sqliteImpl->GetIntValue(&query_rows, i, 8);

		// session name and portrait key need to be set data by message type
		map<string, string> mapData = RetrieveContactOrGroup (pSessionParams.eType, pSessionParams.pSessionId);
		pSessionParams.pSessionName = mapData["SessionName"];
		pSessionParams.pPortraitKey = mapData["PortraitKey"];
	}

	return pSessionParams;
}

HRESULT CWorkMessage::DeleteSession (const string& pSessionId, BOOL bDeleteMessageLogs)
{
	LOGD ("Delete Session, pSessionId=%s, bDeleteMessageLogs=%d\r\n",
			pSessionId.c_str(), bDeleteMessageLogs);

	if (pSessionId.empty())
	{
		LOGD ("pSessionId is empty\r\n");
		return E_FAIL;
	}

	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
    string sSQL1;
	if (bDeleteMessageLogs == TRUE)
	{
		sSQL1 = StringFormat (
				"DELETE FROM [messages] WHERE [session_id]='%s'; "
				"DELETE FROM [sessions] WHERE [session_id]='%s'; "
				,
				pSessionId.c_str(), pSessionId.c_str());
	}
	else
	{
		sSQL1 = StringFormat (
				"DELETE FROM [sessions] WHERE [session_id]='%s'; "
				,
				pSessionId.c_str(), pSessionId.c_str());
	}
//	LOGD("sSQL1=%s\r\n", sSQL1.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
}

HRESULT CWorkMessage::DeleteSessionList (const SessionList& pSessionList, BOOL bDeleteMessageLogs)
{
	LOGD ("Delete Session List\r\n");

	if (pSessionList.nCount <= 0)
	{
		LOGD ("pSessionList is empty\r\n");
		return E_FAIL;
	}

	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
    string sSQL1 = "";
	if (bDeleteMessageLogs == TRUE)
	{
		sSQL1 = "DELETE FROM [messages] WHERE ";

		for (int i=0; i<pSessionList.nCount; i++)
		{
			SessionParams pSessionParams = pSessionList.ppSessionParams[i];
			sSQL1 += "[session_id]='" + pSessionParams.pSessionId + "'";
			if (i < pSessionList.nCount - 1)
			{
				sSQL1 += " OR ";
			}
			else
			{
				sSQL1 += "; ";
			}
		}
	}

	sSQL1 += "DELETE FROM [sessions] WHERE ";

	for (int i=0; i<pSessionList.nCount; i++)
	{
		SessionParams pSessionParams = pSessionList.ppSessionParams[i];
		sSQL1 += "[session_id]='" + pSessionParams.pSessionId + "'";
		if (i < pSessionList.nCount - 1)
		{
			sSQL1 += " OR ";
		}
		else
		{
			sSQL1 += "; ";
		}
	}

//	LOGD("sSQL1=%s\r\n", sSQL1.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
}

HRESULT CWorkMessage::ClearSession (const string& pSessionId)
{
	LOGD ("Clear Session\r\n");

	if (pSessionId.empty())
	{
		LOGD ("SessionId params error\r\n");
		return E_FAIL;
	}

	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"DELETE FROM [messages] WHERE [session_id]='%s'; "
			"UPDATE [sessions] "
			"SET [msg_id]='', [unread_num]=0, [update_time]='' "
			"WHERE [session_id]='%s';"
			,
			pSessionId.c_str(), pSessionId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
}

HRESULT CWorkMessage::SetSessionOption (enum SessionOptionType eOptionType, SessionParams& pSessionParams)
{
	LOGD ("Set Session Option\r\n");

	if (pSessionParams.pSessionId.empty())
	{
		LOGD ("SessionId params error\r\n");
		return E_FAIL;
	}

	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
    string sSQL1;
	switch (eOptionType)
	{
	case SOT_TOP:
		sSQL1 = StringFormat (
				"UPDATE [sessions] SET [top]=%d WHERE [session_id]='%s';"
				,
				pSessionParams.eTop, pSessionParams.pSessionId.c_str());
		break;

	case SOT_REMIND:
		sSQL1 = StringFormat (
				"UPDATE [sessions] SET [remind]=%d WHERE [session_id]='%s';"
				,
				pSessionParams.eRemind, pSessionParams.pSessionId.c_str());
		break;

	default:
		return E_FAIL;
		break;
	}
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
}

HRESULT CWorkMessage::SetSessionListReaded (const SessionList& pSessionList)
{
	LOGD ("Set Session List Readed\r\n");

	if (pSessionList.nCount <= 0)
	{
		LOGD ("SessionId params error\r\n");
		return E_FAIL;
	}

	int nResult = E_FAIL;

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1 = StringFormat (
			"UPDATE [messages] SET [unread]=%d WHERE [unread]=%d AND "
			,
			PS_PLAYED, PS_UNPLAY);

	for (int i=0; i<pSessionList.nCount; i++)
	{
		SessionParams pSessionParams = pSessionList.ppSessionParams[i];
		sSQL1 += "([session_id]='" + pSessionParams.pSessionId + "'";
		if (i < pSessionList.nCount - 1)
		{
			sSQL1 += " OR ";
		}
		else
		{
			sSQL1 += "); ";
		}
	}

	sSQL1 += "UPDATE [sessions] SET [unread_num]=0 WHERE ";

	for (int i=0; i<pSessionList.nCount; i++)
	{
		SessionParams pSessionParams = pSessionList.ppSessionParams[i];
		sSQL1 += "[session_id]='" + pSessionParams.pSessionId + "'";
		if (i < pSessionList.nCount - 1)
		{
			sSQL1 += " OR ";
		}
		else
		{
			sSQL1 += "; ";
		}
	}


//	LOGD("sSQL1=%s\r\n", sSQL1.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
}

UINT CWorkMessage::GetSessionUnreadNum (const string& pSessionId)
{
	LOGD ("Get Session Unread Num\r\n");

	if (pSessionId.empty())
	{
		LOGD ("SessionId params error\r\n");
		return 0;
	}

	UINT nRowNum = 0;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT [unread_num] FROM [sessions] WHERE [session_id]='%s';"
			,
			pSessionId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	LOGD ("Session %s unread number is %d\r\n", pSessionId.c_str(), nRowNum);
	return nRowNum;
}

UINT CWorkMessage::GetTotalUnreadNum ()
{
	LOGD ("Get Total Unread Num\r\n");

	UINT nRowNum = 0;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT [unread_num] FROM [sessions];"
			);
//	LOGD("sSQL1=%s\n", sSQL1);
	sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum += sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	LOGD ("Total unread number is %d\r\n", nRowNum);
	return nRowNum;
}

UINT CWorkMessage::GetSessionCount ()
{
	LOGD ("Get Session Count\r\n");

	int nResult = E_FAIL;
	UINT nRowNum = 0;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat ("SELECT count([rowid]) FROM [sessions];");
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return 0;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum       = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	return nRowNum;
}

HRESULT CWorkMessage::UpdateDraft (const string& pSessionId, const string& pDraftContent)
{
	LOGD ("Update Draft\r\n");

	if (pSessionId.empty())
	{
		LOGD ("SessionId params error\r\n");
		return E_FAIL;
	}

	// operate sqlite
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [sessions] "
			"SET [draft_content]='%s', [draft_changed]=%d "
			"WHERE [session_id]='%s';"
			,
			pDraftContent.c_str(), DC_CHANGED, pSessionId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	sqliteImpl->ExecSql(sSQL1);
	return S_OK;
}

MessageList CWorkMessage::GetMessageList (const string& pSessionId, const string& pLastMsgId, int nPageSize)
{
	LOGD ("Get Message List\r\n");

	int nResult = E_FAIL;
	MessageList pMessageList = {};
	struct query_rows_s query_rows = {};

	if (pSessionId.empty() || nPageSize <= 0)
	{
		LOGD("SessionId or PaseSize params error\r\n");
		return pMessageList;
	}

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1;
//	if (pLastMsgId.empty())
//	{
////		sSQL1 = StringFormat (
////				"SELECT "
////				"[msg_id], [session_id], [sender], [receiver], "
////				"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
////				"[content], [client_time], [server_time], [messages].[rowid] "
////				"FROM [messages] "
////				"WHERE [session_id]='%s' "
////				"ORDER BY [messages].[rowid] DESC "
////				"LIMIT 0, %d; "
////				"UPDATE [messages] SET [unread]=%d WHERE [session_id]='%s' AND [unread]=%d; "
////				"UPDATE [sessions] SET [unread_num]=0 WHERE [session_id]='%s';"
////				,
////				pSessionId.c_str(), nPageSize, US_READ, pSessionId.c_str(), US_UNREAD, pSessionId.c_str());
//		sSQL1 = StringFormat (
//				"SELECT "
//				"[msg_id], [session_id], [sender], [receiver], "
//				"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
//				"[content], [client_time], [server_time], [messages].[rowid] "
//				"FROM [messages] "
//				"WHERE [session_id]='%s' AND [unread]=%d "
//				"ORDER BY [messages].[rowid] DESC "
//				"LIMIT 0, %d"
//				,
//				pSessionId.c_str(), US_UNREAD, nPageSize);
//	}
//	else
//	{
////		sSQL1 = StringFormat (
////				"SELECT "
////				"[msg_id], [session_id], [sender], [receiver], "
////				"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
////				"[content], [client_time], [server_time], [messages].[rowid] "
////				"FROM [messages] "
////				"WHERE [session_id]='%s' "
////				"AND [client_time]<(SELECT [client_time] FROM [messages] WHERE [msg_id]='%s') "
////				"ORDER BY [messages].[rowid] DESC "
////				"LIMIT 0, %d; "
////				"UPDATE [messages] SET [unread]=%d WHERE [session_id]='%s' AND [unread]=%d; "
////				"UPDATE [sessions] SET [unread_num]=0 WHERE [session_id]='%s';"
////				,
////				pSessionId.c_str(), pLastMsgId.c_str(), nPageSize, US_READ, pSessionId.c_str(), US_UNREAD, pSessionId.c_str());
//		sSQL1 = StringFormat (
//				"SELECT "
//				"[msg_id], [session_id], [sender], [receiver], "
//				"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
//				"[content], [client_time], [server_time], [messages].[rowid] "
//				"FROM [messages] "
//				"WHERE [session_id]='%s' AND [unread]=%d "
//				"AND [client_time]<(SELECT [client_time] FROM [messages] WHERE [msg_id]='%s') "
//				"ORDER BY [messages].[rowid] DESC "
//				"LIMIT 0, %d"
//				,
//				pSessionId.c_str(), US_UNREAD, pLastMsgId.c_str(), nPageSize);
//	}
//	LOGD("sSQL1=%s\n", sSQL1.c_str());

	sSQL1 = StringFormat (
			"SELECT "
			"[msg_id], [session_id], [sender], [receiver], "
			"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
			"[content], [client_time], [server_time], [save], [share], [messages].[rowid] "
			"FROM [messages] "
			"WHERE [session_id]='%s' AND [unread]=%d "
			"ORDER BY [messages].[rowid] DESC "
			"LIMIT 0, %d"
			,
			pSessionId.c_str(), US_UNREAD, nPageSize);

	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pMessageList;
	}

	// query something
	pMessageList.nCount = query_rows.n_rows;

	// 反序赋值
	for (int i=query_rows.n_rows - 1; i>=0; i--)
	{
		MessageParams pMessageParams = {};

		pMessageParams.pMsgId       =                     sqliteImpl->GetStrValue(&query_rows, i, 0);
		pMessageParams.pSessionId   =                     sqliteImpl->GetStrValue(&query_rows, i, 1);
		pMessageParams.pSender      =                     sqliteImpl->GetStrValue(&query_rows, i, 2);
		pMessageParams.pReceiver    =                     sqliteImpl->GetStrValue(&query_rows, i, 3);
		pMessageParams.eType        = (enum MessageType)  sqliteImpl->GetIntValue(&query_rows, i, 4);
		pMessageParams.eKind        = (enum KindType)     sqliteImpl->GetIntValue(&query_rows, i, 5);
		pMessageParams.eDirection   = (enum DirectionType)sqliteImpl->GetIntValue(&query_rows, i, 6);
		pMessageParams.eUnread      = (enum UnreadStatus) sqliteImpl->GetIntValue(&query_rows, i, 7);
		pMessageParams.ePlayed      = (enum PlayedStatus) sqliteImpl->GetIntValue(&query_rows, i, 8);
		pMessageParams.eSend        = (enum SendStatus)   sqliteImpl->GetIntValue(&query_rows, i, 9);
		pMessageParams.eCategory    = (enum CategoryType) sqliteImpl->GetIntValue(&query_rows, i, 10);
		pMessageParams.eOnline      = (enum OnlineStatus) sqliteImpl->GetIntValue(&query_rows, i, 11);
		pMessageParams.pContent     =                     sqliteImpl->GetStrValue(&query_rows, i, 12);

		// if direction is receive, then use server time
		// if direction is send and has server time, then use it; if has not, then use client time
		pMessageParams.pTime        =                     sqliteImpl->GetStrValue(&query_rows, i, 14); // servertime
		if (pMessageParams.pTime.empty())
		{
			pMessageParams.pTime    =                     sqliteImpl->GetStrValue(&query_rows, i, 13); // clienttime
		}

		pMessageParams.pIsSave      =                     sqliteImpl->GetIntValue(&query_rows, i, 15);
		pMessageParams.pIsShare     =                     sqliteImpl->GetIntValue(&query_rows, i, 16);

		// 如果发言人有群昵称，则用群昵称替换pSenderName
		map<string, string> mapData = RetrieveContactOrMember (pMessageParams.eType,
				pMessageParams.pSender, pMessageParams.pSessionId);
		pMessageParams.pSenderName  = mapData["SenderName"];
		pMessageParams.pSenderKey   = mapData["SenderKey"];

		pMessageList.ppMessageParams.push_back(pMessageParams);
	}

	return pMessageList;
}

MessageParams CWorkMessage::GetMessage (const string& pSessionId, const string& pMsgId, BOOL bUpdateUnreadNum)
{
	LOGD ("Get Message\r\n");

	int nResult = E_FAIL;
	MessageParams pMessageParams = {};
	struct query_rows_s query_rows = {};

	if (pMsgId.empty())
	{
		LOGD ("MsgId params error\r\n");
		return pMessageParams;
	}

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	string sSQL1;
	if (pSessionId.length() > 0)
	{
//		if (bUpdateUnreadNum == TRUE)
//		{
//			sSQL1 = StringFormat (
//					"SELECT "
//					"[msg_id], [session_id], [sender], [receiver], "
//					"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
//					"[content], [client_time], [server_time] "
//					"FROM [messages] "
//					"WHERE [session_id]='%s' AND [msg_id]='%s'; "
//					"UPDATE [messages] SET [unread]=%d WHERE [session_id]='%s' AND [msg_id]='%s' AND [unread]=%d;"
//					"UPDATE [sessions] SET [unread_num]="
//					"(CASE WHEN [unread_num]<=0 THEN 0 ELSE [unread_num]-1 END) "
//					"WHERE [session_id]='%s'; "
//					,
//					pSessionId.c_str(), pMsgId.c_str(), US_READ, pSessionId.c_str(), pMsgId.c_str(), US_UNREAD, pSessionId.c_str());
//		}
//		else
		{
			sSQL1 = StringFormat (
					"SELECT "
					"[msg_id], [session_id], [sender], [receiver], "
					"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
					"[content], [client_time], [server_time], [save], [share] "
					"FROM [messages] "
					"WHERE [session_id]='%s' AND [msg_id]='%s'; "
					,
					pSessionId.c_str(), pMsgId.c_str());
		}
	}
	else
	{
		sSQL1 = StringFormat (
				"SELECT "
				"[msg_id], [session_id], [sender], [receiver], "
				"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
				"[content], [client_time], [server_time], [save], [share] "
				"FROM [messages] "
				"WHERE [msg_id]='%s'; "
				,
				pMsgId.c_str());
	}
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pMessageParams;
	}

	// query something
	for (int i=0; i<query_rows.n_rows; i++)
	{
		pMessageParams.pMsgId       =                     sqliteImpl->GetStrValue(&query_rows, i, 0);
		pMessageParams.pSessionId   =                     sqliteImpl->GetStrValue(&query_rows, i, 1);
		pMessageParams.pSender      =                     sqliteImpl->GetStrValue(&query_rows, i, 2);
		pMessageParams.pReceiver    =                     sqliteImpl->GetStrValue(&query_rows, i, 3);
		pMessageParams.eType        = (enum MessageType)  sqliteImpl->GetIntValue(&query_rows, i, 4);
		pMessageParams.eKind        = (enum KindType)     sqliteImpl->GetIntValue(&query_rows, i, 5);
		pMessageParams.eDirection   = (enum DirectionType)sqliteImpl->GetIntValue(&query_rows, i, 6);
		pMessageParams.eUnread      = (enum UnreadStatus) sqliteImpl->GetIntValue(&query_rows, i, 7);
		pMessageParams.ePlayed      = (enum PlayedStatus) sqliteImpl->GetIntValue(&query_rows, i, 8);
		pMessageParams.eSend        = (enum SendStatus)   sqliteImpl->GetIntValue(&query_rows, i, 9);
		pMessageParams.eCategory    = (enum CategoryType) sqliteImpl->GetIntValue(&query_rows, i, 10);
		pMessageParams.eOnline      = (enum OnlineStatus) sqliteImpl->GetIntValue(&query_rows, i, 11);
		pMessageParams.pContent     =                     sqliteImpl->GetStrValue(&query_rows, i, 12);

		// if direction is receive, then use server time
		// if direction is send and has server time, then use it; if has not, then use client time
		pMessageParams.pTime        =                     sqliteImpl->GetStrValue(&query_rows, i, 14); // servertime
		if (pMessageParams.pTime.empty())
		{
			pMessageParams.pTime    =                     sqliteImpl->GetStrValue(&query_rows, i, 13); // clienttime
		}

		pMessageParams.pIsSave      =                     sqliteImpl->GetIntValue(&query_rows, i, 15);
		pMessageParams.pIsShare     =                     sqliteImpl->GetIntValue(&query_rows, i, 16);

		// 如果发言人有群昵称，则用群昵称替换pSenderName
		map<string, string> mapData = RetrieveContactOrMember (pMessageParams.eType,
				pMessageParams.pSender, pMessageParams.pSessionId);
		pMessageParams.pSenderName  = mapData["SenderName"];
		pMessageParams.pSenderKey   = mapData["SenderKey"];
	}

	return pMessageParams;
}

void delete_file(string msgid, string imagekey, string videokey)
{
	if(!imagekey.empty())
	{
		if(unlink(imagekey.c_str()) < 0)
		{
			LOGD ("delete_file failed msgid:%s, imagekey:%s\r\n", msgid.c_str(), imagekey.c_str());
		}

		string::size_type pos = imagekey.rfind("/");

		if(string::npos == pos)
		{
			LOGD ("delete_file can not find imagekey msgid:%s, filekey:%s\r\n", msgid.c_str(), imagekey.c_str());

			imagekey = "";
		}
		else
		{
			imagekey.erase(0, pos + 1);
		}
	}

	if(!videokey.empty())
	{
		if(unlink(videokey.c_str()) < 0)
		{
			LOGD ("delete_file failed msgid:%s, videokey:%s\r\n", msgid.c_str(), videokey.c_str());
		}

		string::size_type pos = videokey.rfind("/");

		if(string::npos == pos)
		{
			LOGD ("delete_file can not find key msgid:%s, videokey:%s\r\n", msgid.c_str(), videokey.c_str());

			videokey = "";
		}
		else
		{
			videokey.erase(0, pos + 1);
		}
	}

	if(!imagekey.empty() || !videokey.empty())
	{
		LOGD ("delete_file start msgid:%s, imagekey:%s, videokey:%s\r\n", msgid.c_str(), imagekey.c_str(), videokey.c_str());

		CImDelete imdelete(msgid, imagekey, videokey);

		sImModule.PushSend(imdelete);
	}
}

HRESULT CWorkMessage::DeleteMessage (const string& pSessionId, const string& pMsgId, const string& imagekey, const string& videokey)
{
	LOGD ("Delete Message\r\n");

	if (pSessionId.empty() || pMsgId.empty())
	{
		LOGD ("SessionId or MsgId params error\r\n");
		return E_FAIL;
	}

	if(imagekey.empty() && videokey.empty())
	{
		LOGD ("imagekey and videokey is empty\r\n");
		//return E_FAIL;
	}
	else
	{
		delete_file(pMsgId, imagekey, videokey);
	}

	int nResult = E_FAIL;
	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"DELETE FROM [messages] WHERE [msg_id]='%s'; "
			"UPDATE [sessions] "
			"SET [msg_id]="
			"(SELECT [msg_id] FROM [messages] WHERE rowid="
			"(SELECT max(rowid) FROM [messages] WHERE [session_id]='%s')) "
			"WHERE [session_id]='%s';"
			"UPDATE [sessions] SET [unread_num]=CASE WHEN [unread_num]>0 THEN [unread_num]-1 ELSE 0 END WHERE [session_id]='%s';"
			,
			pMsgId.c_str(), pSessionId.c_str(), pSessionId.c_str(), pSessionId.c_str());
	LOGD("CWorkMessage::DeleteMessage sSQL1=%s\n", sSQL1.c_str());
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
}

UINT CWorkMessage::GetMessageCount (const string& pSessionId)
{
	LOGD ("Get Message Count\r\n");

	if (pSessionId.empty())
	{
		LOGD ("SessionId params error\r\n");
		return 0;
	}

	int nResult = E_FAIL;
	UINT nRowNum = 0;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT count([rowid]) FROM [messages] WHERE [session_id]='%s';"
			,
			pSessionId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return 0;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum       = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	return nRowNum;
}

BOOL CWorkMessage::IsMessageExist (const string& pMsgId)
{
	if (pMsgId.empty())
	{
		LOGD ("MsgId Params error\r\n");
		return FALSE;
	}

	int nResult = E_FAIL;
	UINT nRowNum = 0;
	struct query_rows_s query_rows = {};

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat(
			"SELECT count([rowid]) FROM [messages] WHERE [msg_id]='%s';"
			, pMsgId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return FALSE;
	}

	for (int i=0; i<query_rows.n_rows; i++)
	{
		nRowNum       = sqliteImpl->GetIntValue(&query_rows, i, 0);
	}

	return ((nRowNum == 0) ? (FALSE) : (TRUE));
}

HRESULT CWorkMessage::SetAudioPlayed (const string& pSessionId, const string& pMsgId)
{
	LOGD ("Set Audio Played\r\n");

	if (pSessionId.empty() || pMsgId.empty())
	{
		LOGD ("SessionId or MsgId params error\r\n");
		return E_FAIL;
	}

	int nResult = E_FAIL;

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"UPDATE [messages] SET [played]=%d "
			"WHERE [session_id]='%s' AND [msg_id]='%s' AND [played]=%d; "
			,
			PS_PLAYED, pSessionId.c_str(), pMsgId.c_str(), PS_UNPLAY);
//	LOGD("sSQL1=%s\n", sSQL1);
	nResult = sqliteImpl->ExecSql(sSQL1);
	return nResult;
}

MessageList CWorkMessage::GetGivenFieldList (const string& pSessionId, enum GivenFieldType eGivenField, enum GivenConditionType eGivenCondition)
{
	LOGD ("Get Given Filed List\r\n");

	int nResult = E_FAIL;
	MessageList pMessageList = {};
	struct query_rows_s query_rows = {};
	enum CategoryType eCategoryType;

	if (pSessionId.empty())
	{
		LOGD ("SessionId params error\r\n");
		return pMessageList;
	}

	switch (eGivenCondition)
	{
	case GCT_IMAGE:
		eCategoryType = CT_IMAGE;
		break;

	default:
		LOGD ("GivenCondition params error\r\n");
		return pMessageList;
		break;
	}

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT "
			"[msg_id], [session_id], [sender], [receiver], "
			"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
			"[content], [client_time], [server_time], [messages].[rowid] "
			"FROM [messages] "
			"WHERE [session_id]='%s' AND [category]=%d "
			"ORDER BY [messages].[rowid] DESC; "
			,
			pSessionId.c_str(), eCategoryType);
//	LOGD("sSQL1=%s\n", sSQL1.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pMessageList;
	}

	// query something
	pMessageList.nCount = query_rows.n_rows;

	// 反序赋值
	for (int i=query_rows.n_rows - 1; i>=0; i--)
	{
		MessageParams pMessageParams = {};

		pMessageParams.pMsgId = sqliteImpl->GetStrValue(&query_rows, i, 0);

		switch (eGivenField)
		{
		case GFT_CONTENT:
			pMessageParams.pContent = sqliteImpl->GetStrValue(&query_rows, i, 12);
			break;

		default:
			break;
		}

		pMessageList.ppMessageParams.push_back(pMessageParams);
	}

	return pMessageList;
}

MessageList& CWorkMessage::GetOfflineMessageList (const string& pSessionId)
{
	LOGD ("Get Offline Message List\r\n");

	int nResult = E_FAIL;
	static MessageList pMessageList = {};  // 必须用static，因为该函数返回值是引用
	struct query_rows_s query_rows = {};

	pMessageList.nCount = 0;
	pMessageList.ppMessageParams.clear();

	if (pSessionId.empty())
	{
		LOGD("SessionId params error\r\n");
		return pMessageList;
	}

	SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
	const string sSQL1 = StringFormat (
			"SELECT "
			"[msg_id], [session_id], [sender], [receiver], "
			"[type], [kind], [direction], [unread], [played], [send], [category], [online], "
			"[content], [client_time], [server_time], [save], [share], [messages].[rowid] "
			"FROM [messages] "
			"WHERE [session_id]='%s' AND [unread]=%d AND [online]=%d "
			"ORDER BY [messages].[rowid] DESC; "
			"UPDATE [messages] SET [unread]=%d WHERE [session_id]='%s' AND [unread]=%d; "
			"UPDATE [sessions] SET [unread_num]=0 WHERE [session_id]='%s';"
			,
			pSessionId.c_str(), US_UNREAD, OS_OFFLINE, US_READ, pSessionId.c_str(), US_UNREAD, pSessionId.c_str());
//	LOGD("sSQL1=%s\n", sSQL1.c_str());
	nResult = sqliteImpl->ExecSqlWithResult(sSQL1, &query_rows);

	if (nResult < 0 || query_rows.n_rows == 0)
	{
		// query nothing
		LOGD ("Query nothing\r\n");
		return pMessageList;
	}

	// query something
	pMessageList.nCount = query_rows.n_rows;

	// 反序赋值
	for (int i=query_rows.n_rows - 1; i>=0; i--)
	{
		MessageParams pMessageParams = {};

		pMessageParams.pMsgId       =                     sqliteImpl->GetStrValue(&query_rows, i, 0);
		pMessageParams.pSessionId   =                     sqliteImpl->GetStrValue(&query_rows, i, 1);
		pMessageParams.pSender      =                     sqliteImpl->GetStrValue(&query_rows, i, 2);
		pMessageParams.pReceiver    =                     sqliteImpl->GetStrValue(&query_rows, i, 3);
		pMessageParams.eType        = (enum MessageType)  sqliteImpl->GetIntValue(&query_rows, i, 4);
		pMessageParams.eKind        = (enum KindType)     sqliteImpl->GetIntValue(&query_rows, i, 5);
		pMessageParams.eDirection   = (enum DirectionType)sqliteImpl->GetIntValue(&query_rows, i, 6);
		pMessageParams.eUnread      = (enum UnreadStatus) sqliteImpl->GetIntValue(&query_rows, i, 7);
		pMessageParams.ePlayed      = (enum PlayedStatus) sqliteImpl->GetIntValue(&query_rows, i, 8);
		pMessageParams.eSend        = (enum SendStatus)   sqliteImpl->GetIntValue(&query_rows, i, 9);
		pMessageParams.eCategory    = (enum CategoryType) sqliteImpl->GetIntValue(&query_rows, i, 10);
		pMessageParams.eOnline      = (enum OnlineStatus) sqliteImpl->GetIntValue(&query_rows, i, 11);
		pMessageParams.pContent     =                     sqliteImpl->GetStrValue(&query_rows, i, 12);

		// if direction is receive, then use server time
		// if direction is send and has server time, then use it; if has not, then use client time
		pMessageParams.pTime        =                     sqliteImpl->GetStrValue(&query_rows, i, 14); // servertime
		if (pMessageParams.pTime.empty())
		{
			pMessageParams.pTime    =                     sqliteImpl->GetStrValue(&query_rows, i, 13); // clienttime
		}

		pMessageParams.pIsSave      =                     sqliteImpl->GetIntValue(&query_rows, i, 15);
		pMessageParams.pIsShare     =                     sqliteImpl->GetIntValue(&query_rows, i, 16);

		// 如果发言人有群昵称，则用群昵称替换pSenderName
		map<string, string> mapData = RetrieveContactOrMember (pMessageParams.eType,
				pMessageParams.pSender, pMessageParams.pSessionId);
		pMessageParams.pSenderName  = mapData["SenderName"];
		pMessageParams.pSenderKey   = mapData["SenderKey"];

		pMessageList.ppMessageParams.push_back(pMessageParams);
	}

	return pMessageList;
}

HRESULT CWorkMessage::SetOfflineMessageList (MessageList& pMessageList)
{
	LOGD ("Set Offline Message List\r\n");

	static OfflineMessageParams pOfflineMessageParams = {};

	// clear offline message params
	pOfflineMessageParams.ppSessionId.clear();
	pOfflineMessageParams.nSessionCount = 0;
	pOfflineMessageParams.nOfflineTotal = 0;

	// enums every message
	for (int i=0; i<pMessageList.nCount; i++)
	{
		MessageParams pMessagParams = pMessageList.ppMessageParams[i];

		const string from           = pMessagParams.pSender;
		const string to             = pMessagParams.pReceiver;
		const string id             = pMessagParams.pMsgId;
		const MessageType type      = pMessagParams.eType;
		const CategoryType category = pMessagParams.eCategory;
		const string servertime     = pMessagParams.pTime;
		const string body           = pMessagParams.pContent;

		LOGD ("Recv New Offline Message, from=%s, to=%s, id=%s, type=%d, category=%d, time=%s, body=%s\r\n",
				from.c_str(), to.c_str(), id.c_str(), type, category, servertime.c_str(), body.c_str());

		if (from.empty() || to.empty() || id.empty() || body.empty())
		{
			LOGD ("Save Bad Offline Message\r\n");
			continue;  // next offline message
		}

		// 根据个人还是群组还是客服，确定type和sessiodId
		enum MessageType eMessageType = pMessagParams.eType;;
		string pSessionId = from;
		switch (eMessageType)
		{
		case MT_PERSONAL:
			pSessionId = from;
			break;

		case MT_GROUP:
			pSessionId = to;
			break;

//		case MT_CUSTOM:
//			// to=小号
//			// from=商品ID
//			pSessionId = to;
//			break;
//
//		case MT_MAISHOU:
//			pSessionId = from;
//			break;
		}

		enum CategoryType eCategoryType = pMessagParams.eCategory;
		const string clienttime         = MkIntDateTimeLong();
		string servertime2        = DatetimeStrToLong (servertime);
		if (servertime.empty())
		{
			servertime2 = clienttime;
		}

		if(!IfContactOrGroupExist(eMessageType, pSessionId))
		{
			LOGD("Group not Exist.\r\n");
			return S_FALSE;
		}

		CWorkMessage*      pWorkMessage      = CWorkMessage::GetInstance();
		HRESULT hResult = pWorkMessage->IsMessageExist(id);
		if (hResult == FALSE)
		{
			struct query_rows_s query_rows = {};
			SqliteImpl* sqliteImpl = SqliteImpl::GetInstance();
			const string sSQL1 = StringFormat (
					"INSERT OR IGNORE INTO [messages] "
					"([msg_id], [session_id], [sender], [receiver], [save], [share], "
					"[type], [kind], [direction], [unread], [send], [category], [online], "
					"[content], [client_time], [server_time]) "
					"VALUES "
					"('%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', '%s'); "
					"INSERT OR IGNORE INTO [sessions] ([session_id], [msg_id], [type], [kind]) "
					"VALUES ('%s', '%s', %d, %d); "
					"UPDATE [sessions] "
					"SET [msg_id]='%s', [unread_num]=[unread_num]+1, "
					"[update_time]='%s', [draft_changed]=%d "
					"WHERE [session_id]='%s';"
					,
					id.c_str(), pSessionId.c_str(), from.c_str(), to.c_str(), pMessagParams.pIsSave, pMessagParams.pIsShare,
					eMessageType, KT_ORDINARY, DT_RECV, US_UNREAD, SS_UNSENT, eCategoryType, OS_OFFLINE,
					body.c_str(), clienttime.c_str(), servertime2.c_str(),
					pSessionId.c_str(), id.c_str(), eMessageType, KT_ORDINARY,
					id.c_str(), servertime2.c_str(), DC_UNCHANGED, pSessionId.c_str());
	//		LOGD("sSQL1=%s\r\n", sSQL1.c_str());
			sqliteImpl->ExecSql(sSQL1);
		}
		else
		{
			LOGD ("Found it's a repetitive message, ignore it\r\n");
			continue;  // next offline message
		}

		IfContactOrGroupExist(eMessageType, pSessionId);

		// retrieve contact info or group info by from and to
		map<string, string> mapData = RetrieveContactOrMember (eMessageType, from, to);

		MessageParams pMessageParams = {};
		pMessageParams.pMsgId      = id;
		pMessageParams.pSessionId  = pSessionId;
		pMessageParams.pSender     = from;
		pMessageParams.pSenderName = mapData["SenderName"];
		pMessageParams.pSenderKey  = mapData["SenderKey"];
		pMessageParams.pReceiver   = to;
		pMessageParams.pContent    = body;
		pMessageParams.pTime       = clienttime;
		pMessageParams.eType       = eMessageType;
		pMessageParams.eKind       = KT_ORDINARY;
		pMessageParams.eDirection  = DT_RECV;
		pMessageParams.eUnread     = US_UNREAD;
		pMessageParams.ePlayed     = PS_PLAYED;
		pMessageParams.eSend       = SS_UNSENT;
		pMessageParams.eCategory   = eCategoryType;
		pMessageParams.eOnline     = OS_OFFLINE;

		// 离线消息
		// 累加离线消息总条数
		pOfflineMessageParams.nOfflineTotal ++;

		// 查找已经存在的会话
		int nFoundIndex = find (pOfflineMessageParams.ppSessionId, pMessageParams.pSessionId);
		if (nFoundIndex >= 0)
		{
			// found it
			LOGD ("Offline SessionId found, pSessionId=%s\r\n", pMessageParams.pSessionId.c_str());

			// do nothing
		}
		else
		{
			// can't find
			LOGD ("Offline SessionId not found, pSessionId=%s\r\n", pMessageParams.pSessionId.c_str());
			LOGD ("Offline message insert into a new session\r\n");

			// 添加一个新的会话ID
			pOfflineMessageParams.ppSessionId.push_back (pMessageParams.pSessionId);
			pOfflineMessageParams.nSessionCount ++;
		}
	}

	//notify offline message list to UI
	RspParams rspParams;
	rspParams.eRspType              = ST_RECV_OFFLINE_MESSAGE;
	rspParams.pOfflineMessageParams = &pOfflineMessageParams;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_MESSAGE);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);

	return S_OK;
}

HRESULT CWorkMessage::OnNotify (VOID* pNotify)
{
	RspParams* pRspParams = (RspParams*)pNotify;
	enum RspType eRspType = pRspParams->eRspType;

	switch (eRspType)
	{
	case ST_SEND_MESSAGE_SUCCESS:
	case ST_SEND_MESSAGE_FAILURE:
	case ST_SEND_MESSAGE_PROCESS:
	case ST_RECV_MESSAGE:
	case ST_RECV_OFFLINE_MESSAGE:
	case ST_RECV_CUSTOM_SMALLNUM:
	case ST_WRITE_MESSAGE_SUCCESS:
	case ST_WRITE_MESSAGE_FAILURE:
	//case ST_lANCHAT_WRITE_BINARY:
		OnNotifyMessage(pRspParams);
		break;

	default:
		break;

	}

	return S_OK;
}

HRESULT CWorkMessage::OnNotifyMessage (RspParams* pRspParams)
{
	int nResult = E_FAIL;
	IObserverContainer*  observerContainer = CObserverContainer::GetInstance();
	IObserver* observer = observerContainer->FindObserver(OT_MESSAGE);
	if (observer)
	{
		nResult = observer->OnNotify((VOID*)pRspParams);
	}
	else
	{
		LOGD ("CWorkMessage: can't find MESSAGE observer\n");
	}
	return nResult;
}

void CWorkMessage::GetOfflineMsg()
{
	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->GetOfflineMsg(strUserId);
}

void CWorkMessage::NotifySendOfflineMsgAck(list<string> msgidlist)
{
	IConfigure* pConfigure = CConfigure::GetInstance();
	string strUserId = pConfigure->ReadProfile ("reg_id");
	CXCapImpl* xcap = CXCapImpl::GetInstance();
	xcap->GetOfflineMsgAck(strUserId,msgidlist);
}

void CWorkMessage::NotifyLoginXmpp()
{
	// Login to XMPP server
	CWorkLogin*         pWorkLogin         = CWorkLogin::GetInstance();
	pWorkLogin->LoginXmppServer ();
	LOGD("CWorkMessage::Start Login Xmpp Server.\n");
}

void CWorkMessage::NotifyWriteMessageSuccess (const string pMsgId, const string pSessionId)
{
	LOGD ("Write Message Success, pSessionId=%s, pMsgId=%s\r\n", pSessionId.c_str(), pMsgId.c_str());

	MessageParams messageParams = {};
	messageParams.pMsgId     = pMsgId;
	messageParams.pSessionId = pSessionId;

	RspParams rspParams;
	rspParams.eRspType       = ST_WRITE_MESSAGE_SUCCESS;
	rspParams.pMessageParams = &messageParams;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_MESSAGE);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}

void CWorkMessage::NotifyWriteMessageFailure (const string pMsgId, const string pSessionId)
{
	LOGD ("Write Message Failure, pSessionId=%s, pMsgId=%s\r\n", pSessionId.c_str(), pMsgId.c_str());

	MessageParams messageParams = {};
	messageParams.pMsgId     = pMsgId;
	messageParams.pSessionId = pSessionId;

	RspParams rspParams;
	rspParams.eRspType       = ST_WRITE_MESSAGE_FAILURE;
	rspParams.pMessageParams = &messageParams;
	IWorkerContainer*  pWorkerContainer = CWorkerContainer::GetInstance();
	IWorker* pWorker = pWorkerContainer->FindWorker(WT_MESSAGE);
	if (pWorker)
		pWorker->OnNotify((VOID*)&rspParams);
}
