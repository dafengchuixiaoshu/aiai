/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.izp.imsdkjni;

import java.util.Arrays;
import java.util.Vector;

import android.app.Activity;
import android.util.Log;
import android.widget.TextView;
import android.os.Bundle;

import java.lang.String;

import com.izp.imsdkjni.IMSDKJni.BuddyParams;
import com.izp.imsdkjni.IMSDKJni.CategoryType;
import com.izp.imsdkjni.IMSDKJni.GivenConditionType;
import com.izp.imsdkjni.IMSDKJni.GivenFieldType;
import com.izp.imsdkjni.IMSDKJni.MemberParams;
import com.izp.imsdkjni.IMSDKJni.GroupParams;
import com.izp.imsdkjni.IMSDKJni.IqParams;
import com.izp.imsdkjni.IMSDKJni.IsBuddyType;
import com.izp.imsdkjni.IMSDKJni.KindType;
import com.izp.imsdkjni.IMSDKJni.MessageParams;
import com.izp.imsdkjni.IMSDKJni.MessageType;
import com.izp.imsdkjni.IMSDKJni.ObserverType;
import com.izp.imsdkjni.IMSDKJni.OfflineMessageParams;
import com.izp.imsdkjni.IMSDKJni.RemindStatus;
import com.izp.imsdkjni.IMSDKJni.ReqParams;
import com.izp.imsdkjni.IMSDKJni.ReqType;
import com.izp.imsdkjni.IMSDKJni.RspParams;
import com.izp.imsdkjni.IMSDKJni.RspType;
import com.izp.imsdkjni.IMSDKJni.SessionOptionType;
import com.izp.imsdkjni.IMSDKJni.SessionParams;
import com.izp.imsdkjni.IMSDKJni.SnsType;
import com.izp.imsdkjni.IMSDKJni.TopStatus;


public class MainActivity extends Activity {
	/** Called when the activity is first created. */
	
	public UIMessageObserver uiMessageObserver = new UIMessageObserver();
	public UIBuddyObserver uiBuddyObserver = new UIBuddyObserver();

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		int nTimes = 0;

		//while (true)
		//{
			nTimes ++;

			Log.d ("imsdk", "TRY TIMES " + nTimes);
		
			IMSDKJni.test1();
	
			IMSDKJni imsdkjni = IMSDKJni.getInstance();
			
			imsdkjni.SetRootDir(/*getCacheDir()+*/"/storage/sdcard" + "/izpimsdk");
//			imsdkjni.SetServerAddr("111.47.240.46", "8000");
			imsdkjni.SetServerAddr("10.0.247.179", "8000");
			//imsdkjni.SetServerAddr("223.202.100.242", "8000");
			imsdkjni.SetUserId("wangqiang2");
			String userId = imsdkjni.GetUserId();
			Log.d("imsdk", "UI: userId=" + userId);
			imsdkjni.OpenLog(1);
			int result = imsdkjni.Init ();
			if (result < 0)
			{
				Log.d("imsdk", "Init failed.");
			}
	
			Log.d("imsdk", "Init success.");

			imsdkjni.SetObserver(ObserverType.OT_MESSAGE.value(), uiMessageObserver);
			imsdkjni.SetObserver(ObserverType.OT_BUDDY.value(), uiBuddyObserver);
			UILoginObserver uiLoginObserver = new UILoginObserver();
			imsdkjni.SetObserver(ObserverType.OT_LOGIN.value(), uiLoginObserver);
		
			{
				// login
				ReqParams reqParams = new ReqParams();
				reqParams.eReqType = ReqType.RT_LOGIN.value();
				imsdkjni.Login(reqParams);
			}

			try {
				Thread.sleep(5*1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

//			{
//				// logout
//				ReqParams reqParams = new ReqParams();
//				reqParams.eReqType = ReqType.RT_LOGOUT.value();
//				imsdkjni.Logout(reqParams);
//			}
			int a = imsdkjni.GetSnsCount(SnsType.IT_HUDONG.value());
			Log.d ("imsdk", "UI: sns count=" + a);
			
			//buddyTest();
			groupTest();
			//messageTest();
			
//			for (int i=0; i<2; i++)
//			{
//				ThreadTest tt = new ThreadTest();
//				Thread t = new Thread(tt);
//				t.start();
//			}
//			
//			try {
//				Thread.sleep(6*1000);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//
//			messageTest();

//			Log.d("imsdk", "UI: Notify network status GOOD\n");
//			imsdkjni.NotifyNetworkStatus(NetWorkStatusType.NST_GOOD.value());

//			Log.d("imsdk", "UI: Notify network status ERROR\n");
//			imsdkjni.NotifyNetworkStatus(NetWorkStatusType.NST_ERROR.value());

//			messageTest();
			
//			try {
//				Thread.sleep(20000*1000);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
			
//			imsdkjni.Release ();
//			Log.d("imsdk", "Release success.");
	
			try {
				Thread.sleep(6*1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		//}
	}
	
	public void groupTest() 
	{
		IMSDKJni imsdkjni = IMSDKJni.getInstance();
			
		UIGroupObserver uiGroupObserver = new UIGroupObserver();
		imsdkjni.SetObserver(ObserverType.OT_GROUP.value(), uiGroupObserver);
		
		//imsdkjni.GetGroupList();
/*		
		ReqParams reqParams = new ReqParams();
		reqParams.eReqType = ReqType.RT_MODIFY_GROUPMEMBER_NAME.value();
		
		GroupParams groupParams = new GroupParams();
		
		groupParams.GroupID = "5407029D2738315524";
		GroupMemberParams member = new GroupMemberParams();
		member.MemberID = "cuixiaoyu";
		member.MemberCardName = "尤文图斯";
		groupParams.MemberInfo = member;
		
		reqParams.objReq = groupParams;
	
		imsdkjni.OperateGroup(reqParams);
		*/

//		ReqParams reqParams = new ReqParams();
//		 //test add group
//		reqParams.eReqType = ReqType.RT_ADD_GROUP.value();
//		GroupParams groupParams = new GroupParams();
//		
//		groupParams.GroupName = "HelloWorld";
//		groupParams.GroupPortraitKey = "1234567890";
//		
//		GroupMemberParams Ownerinfo = new GroupMemberParams();
//		Ownerinfo.MemberID = "8008";
//		Ownerinfo.MemberName = "chixueli";
//		Ownerinfo.portraitKey = "1234567890";
//		
//		groupParams.MemberInfo = Ownerinfo;
//		
//		GroupMemberParams memberinfo1 = new GroupMemberParams();
//		memberinfo1.MemberID = "8009";
//		memberinfo1.MemberName = "chenxi";
//		memberinfo1.portraitKey = "1234567890";
//		
//		GroupMemberParams memberinfo2 = new GroupMemberParams();
//		memberinfo2.MemberID = "456";
//		memberinfo2.MemberName = "cuixiaoyu";
//		memberinfo2.portraitKey = "1234567890";
//		
//		GroupMemberParams memberinfo3 = new GroupMemberParams();
//		memberinfo3.MemberID = "789";
//		memberinfo3.MemberName = "liuqianguo";
//		memberinfo3.portraitKey = "1234567890";
//		
//		Object[] listarray = new Object[3];
//		listarray[0] = memberinfo1;
//		listarray[1] = memberinfo2;
//		listarray[2] = memberinfo3;
//		
//		groupParams.Userlist = listarray;
//		
//		reqParams.objReq = groupParams;
//
//		imsdkjni.OperateGroup(reqParams);
//		
//		try {
//			Thread.sleep(5*1000);
//		} catch (InterruptedException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}

		 //test invite group

//		reqParams.eReqType = ReqType.RT_INVITE_GROUP.value();
//		groupParams.GroupID = A.groupID;
//		groupParams.GroupName = A.groupName;
//		
//		GroupMemberParams Inviterinfo = new GroupMemberParams();
//		Inviterinfo.MemberID = "8018";
//		Inviterinfo.MemberName = "chixueli";
//		Inviterinfo.portraitKey = "1234567890";
//		
//		groupParams.MemberInfo = Inviterinfo;
//		
//		GroupMemberParams memberinfo4 = new GroupMemberParams();
//		memberinfo4.MemberID = "aaa";
//		memberinfo4.MemberName = "yangfan";
//		memberinfo4.portraitKey = "1234567890";
//		
//		GroupMemberParams memberinfo5 = new GroupMemberParams();
//		memberinfo5.MemberID = "bbb";
//		memberinfo5.MemberName = "liuzhongxia";
//		memberinfo5.portraitKey = "1234567890";
//		
//		GroupMemberParams memberinfo6 = new GroupMemberParams();
//		memberinfo6.MemberID = "ccc";
//		memberinfo6.MemberName = "liyunhao";
//		memberinfo6.portraitKey = "1234567890";
//		
//		Object[] listarray1 = new Object[3];
//		listarray1[0] = memberinfo4;
//		listarray1[1] = memberinfo5;
//		listarray1[2] = memberinfo6;
//		
//		groupParams.Userlist = listarray1;
//		reqParams.objReq = groupParams;
//
//		imsdkjni.OperateGroup(reqParams);
//		
//		try {
//			Thread.sleep(5*1000);
//		} catch (InterruptedException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//		
//		 //test modify group
//		
//		reqParams.eReqType = ReqType.RT_MODIFY_GROUP.value();
//		groupParams.GroupID = A.groupID;
//		groupParams.GroupName = "WorldCup";
//		
//		GroupMemberParams modifyInfo = new GroupMemberParams();
//		modifyInfo.MemberID = "123";
//		modifyInfo.MemberName = "chixueli";
//		groupParams.MemberInfo = modifyInfo;
//
//		reqParams.objReq = groupParams;
//
//		imsdkjni.OperateGroup(reqParams);
//		
//		
//		try {
//			Thread.sleep(5*1000);
//		} catch (InterruptedException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//		
		
		//test Quit group
		ReqParams reqParams = new ReqParams();
		reqParams.eReqType = ReqType.RT_REMOVE_GROUP.value();
		
		GroupParams groupParams = new GroupParams();
		groupParams.GroupID = "wangqiang2102AE5AD41";
		MemberParams MemberInfo = new MemberParams();
		MemberInfo.MemberID = "wangqiang2";
		MemberInfo.MemberName = "wangqiang2";
		groupParams.MemberInfo = MemberInfo;
		reqParams.objReq = groupParams;

		imsdkjni.OperateGroup(reqParams);

				
	}
	
	public void messageTest() {
		IMSDKJni imsdkjni = IMSDKJni.getInstance();

//		for (int i=0; i<5; i++)
		{
			// send personal message
			Log.d ("imsdk", "UI: Send personal message\n");
			ReqParams reqParams = new ReqParams();
			reqParams.eReqType = ReqType.RT_SEND_MESSAGE.value();
			MessageParams messageParams = new MessageParams();
			messageParams.pSender   = "chenxi";
//			messageParams.pReceiver = "maishou2";
			messageParams.pReceiver = "chenxi";
//			messageParams.pContent  = "f002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff009ff002ff011ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff011ff002ff009ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff002ff009ff002ff011ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff009ff002ff011ff002ff009ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011ff002ff011f";
			messageParams.pContent  = "hello world!";
//			messageParams.pContent  = "{\"type\":1,\"text\":\"{\\\"status:msg, content:\\/\\\"fjehghdgsdhgksdj\\/\\\"}\"}";
			messageParams.eType     = MessageType.MT_PERSONAL.value();
			messageParams.eKind     = KindType.KT_ORDINARY.value();
			messageParams.eCategory = CategoryType.CT_PLAIN_TEXT.value();
			reqParams.objReq = messageParams;
			imsdkjni.SendMessage(reqParams);
		}

/*
		for (int i=0; i<100; i++)
		{
			// resend personal message
			Log.d ("imsdk", "UI: Resend personal message\n");
			ReqParams reqParams = new ReqParams();
			reqParams.eReqType = ReqType.RT_RESEND_MESSAGE.value();
			MessageParams messageParams = new MessageParams();
			messageParams.pSessionId = "maishou2";
			messageParams.pMsgId     = "545097F45";
			reqParams.objReq = messageParams;
			imsdkjni.ResendMessage(reqParams);
		}
*/
/*
		for (int i=0; i<10; i++)
		{
			// send group message
			Log.d ("imsdk", "UI: Send group message\n");
			ReqParams reqParams = new ReqParams();
			reqParams.eReqType = ReqType.RT_SEND_MESSAGE.value();
			MessageParams messageParams = new MessageParams();
			messageParams.pSender   = "8012";
			messageParams.pReceiver = "540E7904273835D925B";
			messageParams.pContent  = "hello world!";
			messageParams.eType     = MessageType.MT_GROUP.value();
			messageParams.eKind     = KindType.KT_ORDINARY.value();
			messageParams.eCategory = CategoryType.CT_PLAIN_TEXT.value();
			reqParams.objReq = messageParams;
			imsdkjni.SendMessage(reqParams);
	
			try {
				Thread.sleep(5*1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}


		{
			// write message
			Log.d ("imsdk", "UI: Write custom message\n");
			MessageParams messageParams = new MessageParams();
//			messageParams.pSender   = "";
			messageParams.pReceiver = "20427";
			messageParams.pContent  = "hello world!";
			messageParams.eType     = MessageType.MT_CUSTOM.value();
			messageParams.eKind     = KindType.KT_NOTIFICATION.value();
			messageParams.eCategory = CategoryType.CT_PLAIN_TEXT.value();
			int nResult = imsdkjni.WriteMessage(messageParams);
			if (nResult == IMSDKJni.S_OK)
			{
				Log.d ("imsdk", "UI: Write message success\n");
			}
			else
			{
				Log.d ("imsdk", "UI: Write message failed\n");
			}
		}

		{
			// send custom message
			Log.d ("imsdk", "UI: Send custom message\n");
			ReqParams reqParams = new ReqParams();
			reqParams.eReqType = ReqType.RT_SEND_MESSAGE.value();
			MessageParams messageParams = new MessageParams();
//			messageParams.pSender   = "";
			messageParams.pReceiver = "20427";
			messageParams.pContent  = "hello world!";
			messageParams.eType     = MessageType.MT_CUSTOM.value();
			messageParams.eKind     = KindType.KT_ORDINARY.value();
			messageParams.eCategory = CategoryType.CT_PLAIN_TEXT.value();
			reqParams.objReq = messageParams;
			imsdkjni.SendMessage(reqParams);
	
			try {
				Thread.sleep(5*1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}


		{
			// set personal session top
			Log.d ("imsdk", "UI: Set personal session chenxi's top\n");
			SessionParams sessionParams = new SessionParams();
			sessionParams.pSessionId = "chenxi";
			sessionParams.eTop = TopStatus.TS_TOPPED.value();
			int nResult = imsdkjni.SetSessionOption(SessionOptionType.SOT_TOP.value(), sessionParams);
			if (nResult == IMSDKJni.S_OK)
			{
				Log.d ("imsdk", "UI: Set session top success\n");
			}
			else
			{
				Log.d ("imsdk", "UI: Set session top failed\n");
			}		
		}

		{
			// set group session top
			Log.d ("imsdk", "UI: Set group session 53D1F30E27383A8A82B's top\n");
			SessionParams sessionParams = new SessionParams();
			sessionParams.pSessionId = "53D1F30E27383A8A82B";
			sessionParams.eTop = TopStatus.TS_TOPPED.value();
			int nResult = imsdkjni.SetSessionOption(SessionOptionType.SOT_TOP.value(), sessionParams);
			if (nResult == IMSDKJni.S_OK)
			{
				Log.d ("imsdk", "UI: Set session top success\n");
			}
			else
			{
				Log.d ("imsdk", "UI: Set session top failed\n");
			}		
		}


		{
			// set session remind
			Log.d ("imsdk", "UI: Set session chenxi's remind\n");
			SessionParams sessionParams = new SessionParams();
			sessionParams.pSessionId = "chenxi";
			sessionParams.eTop = RemindStatus.RS_OPEN.value();
			int nResult = imsdkjni.SetSessionOption(SessionOptionType.SOT_REMIND.value(), sessionParams);
			if (nResult == IMSDKJni.S_OK)
			{
				Log.d ("imsdk", "UI: Set session remind success\n");
			}
			else
			{
				Log.d ("imsdk", "UI: Set session remind failed\n");
			}		
		}
*/
/*
		{
			// get session list
			final Object[] objs = imsdkjni.GetSessionList();
			if (objs == null) {
				Log.d ("imsdk", "UI: Get session list failed\n");
				return;
			}
			SessionParams[] sessionParamsArray = Arrays.copyOf(objs, objs.length, SessionParams[].class);
			Log.d ("imsdk", "UI: session count=" + sessionParamsArray.length);
			int i = 0;
			for (SessionParams sessionParams : sessionParamsArray) {
				Log.d("imsdk", "UI: [" + (++i) + "]: "
						+ "pSessionId="     + sessionParams.pSessionId
						+ ", pSessionName=" + sessionParams.pSessionName
						+ ", pPortraitKey=" + sessionParams.pPortraitKey
						+ ", pMsgId="       + sessionParams.pMsgId
						+ ", eType="        + sessionParams.eType
						+ ", nUnreadNum="   + sessionParams.nUnreadNum
						+ ", eTop="         + sessionParams.eTop
						+ ", eRemind="      + sessionParams.eRemind);

				MessageParams messageParams = (MessageParams)imsdkjni.GetMessage(
						sessionParams.pSessionId, sessionParams.pMsgId, 0);
				if (messageParams != null)
				{
					Log.d ("imsdk", "UI: Get one message\n");
					Log.d ("imsdk", "UI: "
							+ "pMsgId="          + messageParams.pMsgId
							+ ", pSessionId="    + messageParams.pSessionId
							+ ", pSender="       + messageParams.pSender
							+ ", pSenderName="   + messageParams.pSenderName
							+ ", pSenderKey="    + messageParams.pSenderKey
							+ ", pReceiver="     + messageParams.pReceiver
							+ ", pContent="      + messageParams.pContent
							+ ", pTime="         + messageParams.pTime
							+ ", eMessageType="  + messageParams.eType
							+ ", eKindType="     + messageParams.eKind
							+ ", eCategoryType=" + messageParams.eCategory
							+ ", eDirection="    + messageParams.eDirection
							+ ", eUnread="       + messageParams.eUnread
							+ ", ePlayed="       + messageParams.ePlayed
							+ ", eSend="         + messageParams.eSend
							+ ", eOnline="       + messageParams.eOnline);
				}

			}
		}
*/
/*
		{
			// get session
			Log.d ("imsdk", "UI: Get chenxi's session");
			String sessionId = "chenxi";
			final SessionParams sessionParams = (SessionParams)imsdkjni.GetSession(sessionId);
			if (sessionParams != null) {
				Log.d("imsdk", "UI: "
						+ "pSessionId="     + sessionParams.pSessionId
						+ ", pSessionName=" + sessionParams.pSessionName
						+ ", pPortraitKey=" + sessionParams.pPortraitKey
						+ ", pMsgId="       + sessionParams.pMsgId
						+ ", nUnreadNum="   + sessionParams.nUnreadNum
						+ ", eType="        + sessionParams.eType
						+ ", eTop="         + sessionParams.eTop
						+ ", eRemind="      + sessionParams.eRemind);
			}
		}
*/
/*
		{
			// get first page message list
			Log.d ("imsdk", "UI: Get the first page message list");
			String sessionId = "540E7904273835D925B";
			String lastMsgId = null;
			int pageSize = 30;
			final Object[] objs = imsdkjni.GetMessageList(sessionId, lastMsgId, pageSize);
			if (objs != null)
			{
				MessageParams[] messageParamsArray = Arrays.copyOf(objs, objs.length, MessageParams[].class);
				Log.d ("imsdk", "UI: real message count=" + messageParamsArray.length);
				int i = 0;
				for (MessageParams messageParams : messageParamsArray)
				{
					Log.d("imsdk", "UI: [" + (++i) + "]:"
						+ " pMsgId="       + messageParams.pMsgId
						+ ", pSessionId="  + messageParams.pSessionId
						+ ", pSender="     + messageParams.pSender
						+ ", pSenderName=" + messageParams.pSenderName
						+ ", pSenderKey="  + messageParams.pSenderKey
						+ ", pReceiver="   + messageParams.pReceiver
						+ ", pContent="    + messageParams.pContent
						+ ", pTime="       + messageParams.pTime
						+ ", eType="       + messageParams.eType
						+ ", eKind="       + messageParams.eKind
						+ ", eCategory="   + messageParams.eCategory
						+ ", eDirection="  + messageParams.eDirection
						+ ", eUnread="     + messageParams.eUnread
						+ ", ePlayed="     + messageParams.ePlayed
						+ ", eSend="       + messageParams.eSend
						+ ", eOnline="     + messageParams.eOnline);
				}
			}
		}
*/
/*
		{
			// get other pages message list
			Log.d ("imsdk", "UI: Get chenxi's other pages message list");
			String sessionId = "5400331527383DE165458";
//			String lastMsgId = "53F5CAB0271AC324E598";
			String lastMsgId = "";
			int pageSize = 30;
			final Object[] objs = imsdkjni.GetMessageList(sessionId, lastMsgId, pageSize);
			if (objs != null)
			{
				MessageParams[] messageParamsArray = Arrays.copyOf(objs, objs.length, MessageParams[].class);
				Log.d ("imsdk", "UI: real message count=" + messageParamsArray.length);
				int i = 0;
				for (MessageParams messageParams : messageParamsArray)
				{
					Log.d("imsdk", "UI: [" + (++i) + "]:"
						+ " pMsgId="       + messageParams.pMsgId
						+ ", pSessionId="  + messageParams.pSessionId
						+ ", pSender="     + messageParams.pSender
						+ ", pSenderName=" + messageParams.pSenderName
						+ ", pSenderKey="  + messageParams.pSenderKey
						+ ", pReceiver="   + messageParams.pReceiver
						+ ", pContent="    + messageParams.pContent
						+ ", pTime="       + messageParams.pTime
						+ ", eType="       + messageParams.eType
						+ ", eKind="       + messageParams.eKind
						+ ", eCategory="   + messageParams.eCategory
						+ ", eDirection="  + messageParams.eDirection
						+ ", eUnread="     + messageParams.eUnread
						+ ", ePlayed="     + messageParams.ePlayed
						+ ", eSend="       + messageParams.eSend
						+ ", eOnline="     + messageParams.eOnline);
				}
			}
		}


	{
		// Delete message
		String sessionId = "chenxi";
		String msgId = "5400331527383DE165458";
		Log.d ("imsdk", "UI: Delete message by pSessionId=" +  sessionId
				+ ", pMsgId=" + msgId);
		int nResult = imsdkjni.DeleteMessage(sessionId, msgId);
		if (nResult == IMSDKJni.S_OK)
		{
			Log.d ("imsdk", "UI: Delete message success\n");
		}
		else
		{
			Log.d ("imsdk", "UI: Delete message failed\n");
		}
	}

	
	{
		// Set audio play
		String sessionId = "chenxi";
		String msgId = "5400331527383DE165458";
		Log.d ("imsdk", "UI: Set audio play by pSessionId=" + sessionId
				+ ", pMsgId=" + msgId);
		int nResult = imsdkjni.SetAudioPlayed(sessionId, msgId);
		if (nResult == IMSDKJni.S_OK)
		{
			Log.d ("imsdk", "UI: Set audio play success\n");
		}
		else
		{
			Log.d ("imsdk", "UI: Set audio play failed\n");
		}
	}


		{
			// delete session
			Log.d ("imsdk", "UI: Delete session\n");
			String sessionId = "1";
			int nResult = imsdkjni.DeleteSession(sessionId, 0);
			if (nResult == IMSDKJni.S_OK)
			{
				Log.d ("imsdk", "UI: Delete session success\n");
			}
			else
			{
				Log.d ("imsdk", "UI: Delete session failed\n");
			}
		}

		{
			// delete session and message logs
			Log.d ("imsdk", "UI: Delete session and message logs\n");
			String sessionId = "1";
			int nResult = imsdkjni.DeleteSession(sessionId, 1);
			if (nResult == IMSDKJni.S_OK)
			{
				Log.d ("imsdk", "UI: Delete session and message logs success\n");
			}
			else
			{
				Log.d ("imsdk", "UI: Delete session and message logs failed\n");
			}
		}


		{
			// clear session
			Log.d ("imsdk", "UI: Clear session chenxi\n");
			String sessionId = "chenxi";
			int nResult = imsdkjni.ClearSession(sessionId);
			if (nResult == IMSDKJni.S_OK)
			{
				Log.d ("imsdk", "UI: Clear session success\n");
			}
			else
			{
				Log.d ("imsdk", "UI: Clear session failed\n");
			}
		}
*/
/*
		{
			// get the given message field list
			Log.d ("imsdk", "UI: get the given message field list");
			String sessionId = "540E7904273835D925B"; // "540EBE032738511C27A5";
			final Object[] objs = imsdkjni.GetGivenFieldList(sessionId,
					GivenFieldType.GFT_CONTENT.value(), GivenConditionType.GCT_IMAGE.value());
			if (objs != null)
			{
				MessageParams[] messageParamsArray = Arrays.copyOf(objs, objs.length, MessageParams[].class);
				Log.d ("imsdk", "UI: real message count=" + messageParamsArray.length);
				int i = 0;
				for (MessageParams messageParams : messageParamsArray)
				{
					Log.d("imsdk", "UI: [" + (++i) + "]:"
						+ ", pContent="    + messageParams.pContent);
				}
			}
		}
*/	
	}

	public void buddyTest() {
		IMSDKJni imsdkjni = IMSDKJni.getInstance();
		
		//imsdkjni.SearchContact("wang");
		imsdkjni.AddBuddy("wangniancai");
		
		//final Object[] objs = imsdkjni.GetBuddyList();
//		{
//			// add buddy
//			Log.d ("imsdk", "UI: add 1 buddy\n");
//			BuddyParams buddyParams = new BuddyParams();
//			buddyParams.pUserId       = "chenxi";
//			buddyParams.pUserName     = "name_chenxi";
//			buddyParams.pPortraitKey  = "1234567890";
//			buddyParams.pEmotionMood  = "Hello World!";
//			buddyParams.pCmsId        = "12345";
//			buddyParams.pQrerUrl      = "http://www.haixuan.com/";
//			buddyParams.pCommunityUrl = "http://www.haixuan.com/";
//			buddyParams.eIsBuddy = IsBuddyType.IBT_BUDDY.value();
//			imsdkjni.AddBuddy(buddyParams);
//		}

/*
		// add buddy
		Log.d ("imsdk", "UI: add 10 buddy includes buddy or not buddy\n");
		for (int i=0; i<10; i++)
		{
			String userId = "800" + i;
			String name = "n" + userId;
			String cmsId = "" + i;
			BuddyParams buddyParams = new BuddyParams();
			buddyParams.pUserId       = userId;
			buddyParams.pUserName     = name;
			buddyParams.pPortraitKey  = "1234567890";
			buddyParams.pEmotionMood  = "Hello World!";
			buddyParams.pCmsId        = cmsId;
			buddyParams.pQrerUrl      = "http://www.haixuan.com/";
			buddyParams.pCommunityUrl = "http://www.haixuan.com/";
			if (i % 2 == 0)
			{
				buddyParams.eIsBuddy = IsBuddyType.IBT_BUDDY.value();
			}
			else
			{
				buddyParams.eIsBuddy = IsBuddyType.IBT_NOTBUDDY.value();
			}
			imsdkjni.AddBuddy(buddyParams);
		}
*/
/*
		{
			// get buddy list
			final Object[] objs = imsdkjni.GetBuddyList();
			if (objs == null) {
				Log.d ("imsdk", "UI: Get buddy list failed\n");
				return;
			}
			BuddyParams[] buddyParamsArray = Arrays.copyOf(objs, objs.length, BuddyParams[].class);
			Log.d ("imsdk", "UI: buddy count=" + buddyParamsArray.length);
			int i = 0;
			for (BuddyParams buddyParams : buddyParamsArray) {
				Log.d("imsdk", "UI: [" + (++i) + "]: "
						+ "pUserId="         + buddyParams.pUserId
						+ ", pUserName="     + buddyParams.pUserName
						+ ", pPortraitKey="  + buddyParams.pPortraitKey
						+ ", pEmotionMood="  + buddyParams.pEmotionMood
						+ ", pCmsId="        + buddyParams.pCmsId
						+ ", pQrerUrl="      + buddyParams.pQrerUrl
						+ ", pCommunityUrl=" + buddyParams.pCommunityUrl
						+ ", eIsBuddy="      + buddyParams.eIsBuddy);
			}
		}

		{
			// modify buddy
			Log.d ("imsdk", "UI: modify buddy\n");
			BuddyParams buddyParams = new BuddyParams();
			buddyParams.pUserId = "8006";
			buddyParams.pUserName = "刘德华好";
			Log.d ("imsdk", "UI: [1] "
					+ "pUserId=" + buddyParams.pUserId
					+ ", pUserName=" + buddyParams.pUserName);
			imsdkjni.ModifyBuddy(buddyParams);
			buddyParams.pUserId = "8005";
			buddyParams.pUserName = "陈奕迅唱";
			Log.d ("imsdk", "UI: [2] "
					+ "pUserId=" + buddyParams.pUserId
					+ ", pUserName=" + buddyParams.pUserName);
			imsdkjni.ModifyBuddy(buddyParams);
			buddyParams.pUserId = "8002";
			buddyParams.pUserName = "巴国布衣";
			Log.d ("imsdk", "UI: [3] "
					+ "pUserId=" + buddyParams.pUserId
					+ ", pUserName=" + buddyParams.pUserName);
			imsdkjni.ModifyBuddy(buddyParams);
		}

		{
			// get buddy list
			final Object[] objs = imsdkjni.GetBuddyList();
			if (objs == null) {
				Log.d ("imsdk", "UI: Get buddy list failed\n");
				return;
			}
			BuddyParams[] buddyParamsArray = Arrays.copyOf(objs, objs.length, BuddyParams[].class);
			Log.d ("imsdk", "UI: buddy count=" + buddyParamsArray.length);
			int i = 0;
			for (BuddyParams buddyParams : buddyParamsArray) {
				Log.d("imsdk", "UI: [" + (++i) + "]: "
						+ "pUserId="         + buddyParams.pUserId
						+ ", pUserName="     + buddyParams.pUserName
						+ ", pPortraitKey="  + buddyParams.pPortraitKey
						+ ", pEmotionMood="  + buddyParams.pEmotionMood
						+ ", pCmsId="        + buddyParams.pCmsId
						+ ", pQrerUrl="      + buddyParams.pQrerUrl
						+ ", pCommunityUrl=" + buddyParams.pCommunityUrl
						+ ", eIsBuddy="      + buddyParams.eIsBuddy);
			}
		}

		{
			// delete buddy
			Log.d ("imsdk", "UI: Delete buddy 8006\n");
			String userId = "8006";
			int nResult = imsdkjni.DeleteBuddy(userId);
			if (nResult == IMSDKJni.S_OK)
			{
				Log.d ("imsdk", "UI: Delete buddy success\n");
			}
			else
			{
				Log.d ("imsdk", "UI: Delete buddy failed\n");
			}
		}

		{
			// get buddy list
			final Object[] objs = imsdkjni.GetBuddyList();
			if (objs == null) {
				Log.d ("imsdk", "UI: Get buddy list failed\n");
				return;
			}
			BuddyParams[] buddyParamsArray = Arrays.copyOf(objs, objs.length, BuddyParams[].class);
			Log.d ("imsdk", "UI: buddy count=" + buddyParamsArray.length);
			int i = 0;
			for (BuddyParams buddyParams : buddyParamsArray) {
				Log.d("imsdk", "UI: [" + (++i) + "]: "
						+ "pUserId="         + buddyParams.pUserId
						+ ", pUserName="     + buddyParams.pUserName
						+ ", pPortraitKey="  + buddyParams.pPortraitKey
						+ ", pEmotionMood="  + buddyParams.pEmotionMood
						+ ", pCmsId="        + buddyParams.pCmsId
						+ ", pQrerUrl="      + buddyParams.pQrerUrl
						+ ", pCommunityUrl=" + buddyParams.pCommunityUrl
						+ ", eIsBuddy="      + buddyParams.eIsBuddy);
			}
		}

		{
			// get buddy
			String userId = "chenxi";
			final BuddyParams buddyParams = (BuddyParams)imsdkjni.GetBuddy(userId, 0);
			if (buddyParams == null) {
				Log.d ("imsdk", "UI: Get buddy failed\n");
				return;
			}
			Log.d("imsdk", "UI: Get one buddy\n");
			Log.d("imsdk", "pUserId="   + buddyParams.pUserId
					+ ", pUserName="     + buddyParams.pUserName
					+ ", pPortraitKey="  + buddyParams.pPortraitKey
					+ ", pEmotionMood="  + buddyParams.pEmotionMood
					+ ", pCmsId="        + buddyParams.pCmsId
					+ ", pQrerUrl="      + buddyParams.pQrerUrl
					+ ", pCommunityUrl=" + buddyParams.pCommunityUrl
					+ ", eIsBuddy="      + buddyParams.eIsBuddy);
		}
*/
	}
	
	//UI登录观察者
	public class UILoginObserver
	{
		public int OnNotify (Object pNotify) {
			RspParams rspParams = (RspParams) pNotify;
			RspType eRspType = RspType.valueOf(rspParams.eRspType);
			switch (eRspType)
			{
			case ST_NETWORK_FAILURE:
				Log.d ("imsdk", "UI: Network failure\n");
				break;

			case ST_LOGIN_SUCCESS:
				Log.d ("imsdk", "UI: Login success\n");
				break;

			case ST_LOGIN_FAILURE:
				Log.d ("imsdk", "UI: Login failure\n");
				break;

			case ST_LOGIN_PROCESS:
				Log.d ("imsdk", "UI: Login process\n");
				break;
				
			case ST_LOGIN_ELSEWHERE:
				Log.d ("imsdk", "UI: Login elsewhere\n");
				break;
				
			case ST_LOGIN_VERSIONLOW:
				Log.d ("imsdk", "UI: Login version low\n");
				break;
				
			case ST_LOGOUT_SUCCESS:
				Log.d ("imsdk", "UI: Logout success\n");
				break;

			case ST_LOGOUT_FAILURE:
				Log.d ("imsdk", "UI: Logout failure\n");
				break;

			default:
				break;
			}

			return IMSDKJni.S_OK;
		}
	}

	//UI消息观察者
	public class UIMessageObserver
	{
		public int OnNotify (Object pNotify) {
			RspParams rspParams = (RspParams) pNotify;
			RspType eRspType = RspType.valueOf(rspParams.eRspType);
			switch (eRspType)
			{
			case ST_SEND_MESSAGE_SUCCESS:
				{
					MessageParams messageParams = (MessageParams)rspParams.objRsp;
					Log.d ("imsdk", "UI: Send message success, " + "pMsgId=" + messageParams.pMsgId
							+ ", pSessionId="  + messageParams.pSessionId);
				}
				break;

			case ST_SEND_MESSAGE_FAILURE:
				{
					MessageParams messageParams = (MessageParams)rspParams.objRsp;
					Log.d ("imsdk", "UI: Send message failure, " + "pMsgId=" + messageParams.pMsgId
							+ ", pSessionId="  + messageParams.pSessionId);
				}
				break;

			case ST_SEND_MESSAGE_PROCESS:
				{
					MessageParams messageParams = (MessageParams)rspParams.objRsp;
					Log.d ("imsdk", "UI: Send message process, " + "pMsgId=" + messageParams.pMsgId
							+ ", pSessionId="  + messageParams.pSessionId);
				}
				break;
				
			case ST_RECV_MESSAGE:
				Log.d ("imsdk", "UI: Recv message\n");
				{
					MessageParams messageParams = (MessageParams)rspParams.objRsp;
					Log.d ("imsdk", "UI: " + "pMsgId=" + messageParams.pMsgId
							+ ", pSessionId="  + messageParams.pSessionId
							+ ", pSender="     + messageParams.pSender
							+ ", pSenderName=" + messageParams.pSenderName
							+ ", pReceiver="   + messageParams.pReceiver
							+ ", pContent="    + messageParams.pContent
							+ ", pTime="       + messageParams.pTime
							+ ", eType="       + messageParams.eType
							+ ", eKind="       + messageParams.eKind
							+ ", eCategory="   + messageParams.eCategory
							+ ", eOnline="     + messageParams.eOnline);
				}
				break;
	
			case ST_RECV_OFFLINE_MESSAGE:
				Log.d ("imsdk", "UI: Recv offline messages\n");
				{
					OfflineMessageParams offlineMessageParams = (OfflineMessageParams)rspParams.objRsp;
					Log.d ("imsdk", "UI: nOfflineTotal=" + offlineMessageParams.nOfflineTotal
							+ ", nSessionCount=" + offlineMessageParams.nSessionCount);
					
//					String[] sessionIdArray = Arrays.copyOf(offlineMessageParams.ppSessionId,
//							offlineMessageParams.ppSessionId.length, String[].class);
					
					for (int i=0; i<offlineMessageParams.nSessionCount; i++)
					{
						final Object[] sessionIds = (Object[])offlineMessageParams.ppSessionId;
						String sessionId = sessionIds[i].toString();
						Log.d ("imsdk", "UI: [" + (i + 1) + "]: "
								+ "SessionId=[" + sessionId + "]");
						
						IMSDKJni imsdkjni = IMSDKJni.getInstance();
						final Object[] objs = imsdkjni.GetOfflineMessageList(sessionId);
						if (objs != null)
						{
							MessageParams[] messageParamsArray = Arrays.copyOf(objs, objs.length, MessageParams[].class);
							Log.d ("imsdk", "UI: real message count=" + messageParamsArray.length);
							int j = 0;
							for (MessageParams messageParams : messageParamsArray)
							{
								Log.d("imsdk", "UI: [" + (++j) + "]:"
									+ " pMsgId="       + messageParams.pMsgId
									+ ", pSessionId="  + messageParams.pSessionId
									+ ", pSender="     + messageParams.pSender
									+ ", pSenderName=" + messageParams.pSenderName
									+ ", pSenderKey="  + messageParams.pSenderKey
									+ ", pReceiver="   + messageParams.pReceiver
									+ ", pContent="    + messageParams.pContent
									+ ", pTime="       + messageParams.pTime
									+ ", eType="       + messageParams.eType
									+ ", eKind="       + messageParams.eKind
									+ ", eCategory="   + messageParams.eCategory
									+ ", eDirection="  + messageParams.eDirection
									+ ", eUnread="     + messageParams.eUnread
									+ ", ePlayed="     + messageParams.ePlayed
									+ ", eSend="       + messageParams.eSend
									+ ", eOnline="     + messageParams.eOnline);
							}
						}
					}
				}
				break;

			case ST_RECV_CUSTOM_SMALLNUM:
				Log.d ("imsdk", "UI: Recv message\n");
				{
					IqParams iqParams = (IqParams)rspParams.objRsp;
					Log.d ("imsdk", "eType=" + iqParams.eType
							+ ", pTalkId="    + iqParams.pTalkId
							+ ", pSmallNum="  + iqParams.pSmallNum);
				}
				break;
				
			default:
				break;
			}

			return IMSDKJni.S_OK;
		}
	}
	
	public static class A
	{
		public static String groupID;
		public static String groupName;
	}
	
	//UI群组观察者
	public class UIGroupObserver
	{
	
		public int OnNotify (Object pNotify) {
			
			RspParams rspParams = (RspParams) pNotify;
			RspType eRspType = RspType.valueOf(rspParams.eRspType);
			switch (eRspType)
			{
			case ST_ADD_GROUP_SUCCESS:
				Log.d ("imsdk", "UI: AddGroup success!!!!\r\n");
				GroupParams groupParams = (GroupParams)rspParams.objRsp;
				Log.d ("imsdk", "GroupID:" + groupParams.GroupID + "GroupName:"  + groupParams.GroupName );
				
//				IMSDKJni imsdkjni = IMSDKJni.getInstance();
//				Object[] grouplist = imsdkjni.GetGroupList();
//				
//				for(int i = 0; i < grouplist.length; i++)
//				{
//					GroupParams groupParams1 = (GroupParams)grouplist[i];
//					//Log.d ("imsdk", "GroupID:" + groupParams1.GroupID + "GroupName:" + groupParams1.GroupName + "GroupPortraitKey:" + groupParams1.GroupPortraitKey + "GroupMaxMember:" + groupParams1.GroupMaxMember + "OwnerID:" + groupParams1.OwnerID + "GroupMemberCount:" + groupParams1.GroupMemberCount);
//				}
//				
//				Object[] Memberlist = imsdkjni.GetGroupMemberList(groupParams.GroupID);
//				
//				for(int i = 0; i < Memberlist.length; i++)
//				{
//					GroupMemberParams memberParams = (GroupMemberParams)Memberlist[i];
//					//Log.d ("imsdk", "MemberID:" + memberParams.MemberID + "MemberName:" + memberParams.MemberName + "portraitKey:" + memberParams.portraitKey + "EmotionMood:" + memberParams.EmotionMood );
//		
//					Object obj = imsdkjni.GetGroupMemberInfo(memberParams.MemberID);
//					GroupMemberParams memberParams1 = (GroupMemberParams)obj;
//					Log.d ("imsdk", "MemberID:" + memberParams1.MemberID + "MemberName:" + memberParams1.MemberName + "portraitKey:" + memberParams1.portraitKey + "EmotionMood:" + memberParams1.EmotionMood );
//					
//				}
//				
//				Object obj = imsdkjni.GetGroupInfo(groupParams.GroupID);
//				GroupParams groupinfo = (GroupParams)obj;
//				//Log.d ("imsdk", "GroupID:" + groupinfo.GroupID + "GroupName:" + groupinfo.GroupName + "GroupPortraitKey:" + groupinfo.GroupPortraitKey + "GroupMaxMember:" + groupinfo.GroupMaxMember + "OwnerID:" + groupinfo.OwnerID + "GroupMemberCount:" + groupinfo.GroupMemberCount);
//				
				A.groupID = groupParams.GroupID;
				A.groupName = groupParams.GroupName;
				
				break;
			case ST_NOTIFY_CREATE_GROUP:
				Log.d ("imsdk", "UI: Received Invited JoinGroup Notify!!!!\r\n");
				GroupParams groupParams5 = (GroupParams)rspParams.objRsp;
				Log.d ("imsdk", "GroupID:" + groupParams5.GroupID);
				Log.d ("imsdk", "GroupName:" + groupParams5.GroupName);
				break;
				
			case ST_JOINGRUOP_SUCCESS:
				Log.d ("imsdk", "UI: InviteGroup success!!!!\r\n");
				GroupParams groupParams1 = (GroupParams)rspParams.objRsp;
				Log.d ("imsdk", "GroupID:" + groupParams1.GroupID + "GroupName:" + groupParams1.GroupName);
				
				break;
			case ST_NOTIFY_REFRESH_GROUP:
				Log.d ("imsdk", "UI: Received Refresh Group Notify!!!!\r\n");
				GroupParams groupParams6 = (GroupParams)rspParams.objRsp;
				Log.d ("imsdk", "GroupID:" + groupParams6.GroupID);
				Log.d ("imsdk", "GroupName:" + groupParams6.GroupName);
				break;
			case ST_MODIFY_GROUPNAME_SUCCESS:
				Log.d ("imsdk", "UI: ModifyGroup success!!!!\r\n");
				GroupParams groupParams2 = (GroupParams)rspParams.objRsp;
				Log.d ("imsdk", "GroupID:" + groupParams2.GroupID + " GroupName:" + groupParams2.GroupName);
				A.groupName = groupParams2.GroupName;
				break;
			case ST_NOTIFY_MODIFY_GROUP:
				Log.d ("imsdk", "UI: Received ModifyGroup Notify!!!!\r\n");
				GroupParams groupParams7 = (GroupParams)rspParams.objRsp;
				Log.d ("imsdk", "GroupID:" + groupParams7.GroupID + " GroupName:" + groupParams7.GroupName);
				messageTest ();
				break;	
			case ST_QUIT_GROUP_SUCCESS:
				Log.d ("imsdk", "UI: QuitGroup success!!!!\r\n");
				GroupParams groupParams3 = (GroupParams)rspParams.objRsp;
				Log.d ("imsdk", "GroupID:" + groupParams3.GroupID + " GroupName:" + groupParams3.GroupName);
				break;
			case ST_NOTIFY_QUIT_GROUP:
				Log.d ("imsdk", "UI: Received QuitGroup Notify!!!!\r\n");
				GroupParams groupParams8 = (GroupParams)rspParams.objRsp;
				Log.d ("imsdk", "GroupID:" + groupParams8.GroupID);
				break;
			case ST_GETGROUPLIST_SUCCESS:
				Log.d ("imsdk", "UI: GetGrouplist success!!!!\r\n");
				break;
			default:
				break;
			}

			return IMSDKJni.S_OK;
		}
	}
	
	//UI好友观察者
	public class UIBuddyObserver
	{
		public int OnNotify (Object pNotify) {
			RspParams rspParams = (RspParams) pNotify;
			RspType eRspType = RspType.valueOf(rspParams.eRspType);
			switch (eRspType)
			{
			case ST_LOAD_BUDDY_LIST:
				{
					Log.d ("imsdk", "UI: Load Buddy List\n");
					// get buddy list
					IMSDKJni imsdkjni = IMSDKJni.getInstance();
					final Object[] objs = imsdkjni.GetBuddyList();
					if (objs == null) {
						Log.d ("imsdk", "UI: Get buddy list failed\n");
						break;
					}
					BuddyParams[] buddyParamsArray = Arrays.copyOf(objs, objs.length, BuddyParams[].class);
					Log.d ("imsdk", "UI: buddy count=" + buddyParamsArray.length);
					int i = 0;
					for (BuddyParams buddyParams : buddyParamsArray) {
						Log.d("imsdk", "UI: [" + (++i) + "]: "
								+ "pUserId=" + buddyParams.pUserId
								+ ", pUserName=" + buddyParams.pUserName
								+ ", pPortraitKey=" + buddyParams.pPortraitKey
								+ ", pEmotionMood=" + buddyParams.pEmotionMood
								+ ", pQrerUrl=" + buddyParams.pQrerUrl
								+ ", pCommunityUrl=" + buddyParams.pCommunityUrl
								+ ", eIsBuddy=" + buddyParams.eIsBuddy);
					}
				}
				break;

			case ST_LOAD_BUDDY:
				{
					// get buddy
					String userId = "chenxi";
					IMSDKJni imsdkjni = IMSDKJni.getInstance();
					final BuddyParams buddyParams = (BuddyParams)imsdkjni.GetBuddy(userId, 0);
					if (buddyParams == null) {
						Log.d ("imsdk", "UI: Get buddy failed\n");
						break;
					}
					Log.d("imsdk", "UI: Get one buddy\n");
					Log.d("imsdk", "pUserId=" + buddyParams.pUserId
							+ ", pUserName=" + buddyParams.pUserName
							+ ", pPortraitKey=" + buddyParams.pPortraitKey
							+ ", pEmotionMood=" + buddyParams.pEmotionMood
							+ ", pQrerUrl=" + buddyParams.pQrerUrl
							+ ", pCommunityUrl=" + buddyParams.pCommunityUrl
							+ ", eIsBuddy=" + buddyParams.eIsBuddy);
				}
				break;

			default:
				break;
			}

			return IMSDKJni.S_OK;
		}
	}

}
