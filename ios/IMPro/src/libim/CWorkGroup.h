#ifndef _C_WORK_GROUP_H
#define _C_WORK_GROUP_H

#include "BizCommon.h"
#include "CWorker.h"
#include "immsg.h"

class CWorkGroup : public CWorker
{
public:
	CWorkGroup (void);
	~CWorkGroup (void);

	static CWorkGroup* GetInstance();//创建实例

	HRESULT Init();//初始化实例
	VOID Release();//销毁实例

	HRESULT CreateTable();

	virtual HRESULT Invoke (VOID* pInvoke);//调用函数
	virtual HRESULT OnNotify (VOID* pNotify);//回调函数
	virtual HRESULT OnNotifyInner (VOID* pNotify);//内部回调函数
	virtual HRESULT OnNotifySaveDatabase (GroupInfo info,XCAP_OPER oper,BOOL bself = FALSE);//内部回调函数
	HRESULT OnNotifyGroup (RspParams* pRspParams);
	HRESULT AddGroup(ReqParams* pReqParams);
	HRESULT InviteGroup(ReqParams* pReqParams);
	HRESULT ModifyGroup(ReqParams* pReqParams);//修改群名称
	HRESULT ModifyGroupMemberName(ReqParams* pReqParams);//修改群名片
	HRESULT DeleteGroup(ReqParams* pReqParams);//主动退出群
	vector<GroupParams> GetGroupList();//获取群组列表
	GroupParams GetGroupInfo(string groupid);//获取群组信息
	MemberParams GetMember (const string inMemberId,string groupid,BOOL loadfromserver = TRUE);// 获取成员信息
	vector<MemberParams> GetMemberList (const string groupid);// 批量获取用户信息
	void UpdateGroupTmpName(string groupid);//更新群临时名称
	HRESULT IsGroupExist (string pGroupId);
	HRESULT LoadGroupList (BOOL fromUI = TRUE);
	HRESULT NotifyNetworkStatus(enum NetWorkStatusType eNetworkStatusType);

	void OnNotifyOperateGroup(int oper,CIm* pim);
	void GetGroupinfoByID(string groupid);

private:
	static CWorkGroup* g_pWorkGroup;
	string m_strGroupName;//群名称
	string m_owner;//群主
	GroupParams   m_groupParams;
	RspParams     m_rspParams;

};

#endif
