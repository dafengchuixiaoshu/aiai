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

import com.izp.imsdkjni.IMSDKJni.CategoryType;
import com.izp.imsdkjni.IMSDKJni.KindType;
import com.izp.imsdkjni.IMSDKJni.MessageParams;
import com.izp.imsdkjni.IMSDKJni.MessageType;
import com.izp.imsdkjni.IMSDKJni.ReqParams;
import com.izp.imsdkjni.IMSDKJni.ReqType;
import com.izp.imsdkjni.IMSDKJni.SessionParams;

import android.util.Log;

import java.lang.String;

/**
 * 通过实现Runnable接口创建一个线程
 * @author DreamSea
 */
public class ThreadTest implements Runnable 
{
	public void messageTest() {
		IMSDKJni imsdkjni = IMSDKJni.getInstance();

/*		
		{
			// send personal message
			Log.d ("imsdk", "UI: Send personal message\n");
			ReqParams reqParams = new ReqParams();
			reqParams.eReqType = ReqType.RT_SEND_MESSAGE.value();
			MessageParams messageParams = new MessageParams();
			messageParams.pSender   = "chenxi";
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
*/
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
	}
	
    public void run()
    {
    	Log.d ("ThreadTest", "I'm running!");
    	
    	while (true)
    	{
    		messageTest ();
    		
			try {
				Thread.sleep(3*1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    	}
    }
}
