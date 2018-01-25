#include "header.h"
#include "immodule.h"

int rand_base(int size)
{
    if(size <= 0) return 0;

    srand((unsigned)time(NULL));

    if(0 == size)
    {
#ifdef WIN32
        return rand();
#else
        return random();
#endif
    }
    else
    {
#ifdef WIN32
        int index = rand() % size;
#else
        int index = random() % size;
#endif

        return index;
    }
}

void libclient_callback(int msgtype, CSocketMsg& socketmsg)
{
    switch(msgtype)
    {
    case MSG_WORK_CLEAR:
    case MSG_SEND_ERROR:
    case MSG_SEND_CLOSED:
    case MSG_RECV_CLOSED:
    case MSG_SOCKET_TIMEOUT:
        {
        }
        break;

    case MSG_SOCKET_CONNECT:
        {
            string from = "liuqianguo";
            string platfrom = "ios";
            string resource = "win32";
            string domain = "im.izp.com";

            CImLogin imlogin(from, domain, resource, platfrom);

            sImModule.PushSend(imlogin);

            printf("===== connect success =====\n\n");
        }
        break;

    case MSG_RECV_MESSAGE:
        {
            switch(socketmsg.m_im->m_imtype)
            {
            case IM_CHAT:
                {
                    CImChat* imchat = dynamic_cast<CImChat*>(socketmsg.m_im);

                    if(NULL == imchat)
                    {
                        printf("can not find chat pointer\n\n");

                        sImModule.Release();
                    }
                    else if("customchat" == imchat->m_type && "system" == imchat->m_category)
                    {
                        printf("do custom gettouser: [id:%s,talkid:%s,custom:%s]\n\n",
                            imchat->m_id.c_str(), imchat->m_from.c_str(), imchat->m_body.c_str());
                    }
                    else
                    {
                        printf("do message: [id:%s,to:%s,from:%s,type:%s,category:%s,time:%s,body:%s]\n\n",
                            imchat->m_id.c_str(), imchat->m_to.c_str(), imchat->m_from.c_str(),
                            imchat->m_type.c_str(), imchat->m_category.c_str(), imchat->m_time.c_str(),
                            imchat->m_body.c_str());
                    }
                }
                break;

            case IM_ACK:
                {
                    CImAck* imack = dynamic_cast<CImAck*>(socketmsg.m_im);

                    if(NULL == imack)
                    {
                        printf("can not find ack pointer\n\n");

                        sImModule.Release();
                    }
                    else
                    {
                        printf("do ack: [id:%s,time:%s]\n\n", imack->m_id.c_str(), imack->m_time.c_str());
                    }
                }
                break;

            case IM_LOGIN:
                {
                    CImLogin* imlogin = dynamic_cast<CImLogin*>(socketmsg.m_im);

                    if(NULL == imlogin)
                    {
                        printf("can not find login pointer\n\n");

                        sImModule.Release();
                    }
                    else if(0 != imlogin->m_status)
                    {
                        printf("login failed: %d\n\n", imlogin->m_status);

                        sImModule.Release();
                    }
                    else
                    {
                        printf("login success\n\n");

                        //string blackid = "lqguo";
                        //CImBlackList blacklist1(blackid, 1);
                        //printf("add blacklist id=%s\n\n", blacklist1.m_id.c_str());
                        //sImModule.PushSend(blacklist1);

                        //CImBlackList blacklist2(blackid, 0);
                        //printf("del blacklist id=%s\n\n", blacklist2.m_id.c_str());
                        //sImModule.PushSend(blacklist2);
                    }
                }
                break;

            case IM_LOGOUT:
                {
                    CImLogout* imlogout = dynamic_cast<CImLogout*>(socketmsg.m_im);

                    if(NULL == imlogout)
                    {
                        printf("can not find logout pointer\n\n");

                        sImModule.Release();
                    }
                    else if(0 != imlogout->m_status)
                    {
                        printf("logout failed: %d\n\n", imlogout->m_status);
                    }
                    else
                    {
                        sImModule.Release();

                        printf("logout success\n\n");
                    }
                }
                break;

            case IM_KICKOUT:
                {
                    CImKickout* imkickout = dynamic_cast<CImKickout*>(socketmsg.m_im);

                    if(NULL == imkickout)
                    {
                        printf("can not find kickout pointer\n\n");
                    }
                    else
                    {
                        printf("kickout\n\n");
                    }
                }
                break;

            case IM_PGCREATE:
                {
                    CImPGCreate* impgcreate = dynamic_cast<CImPGCreate*>(socketmsg.m_im);
                    
                    if(NULL == impgcreate)
                    {
                        printf("can not find pgcreate pointer\n\n");
                    }
                    else
                    {
                        printf("IM_PGCREATE id=%s,gid=%s,name:%s,type:%d,time=%s\n\n", impgcreate->m_id.c_str(),
                            impgcreate->m_gid.c_str(), impgcreate->m_name.c_str(), impgcreate->m_type, impgcreate->m_time.c_str());
                    }
                }
                break;

            case IM_PGINVITE:
                {
                    CImPGInvite* impginvite = dynamic_cast<CImPGInvite*>(socketmsg.m_im);
                    
                    if(NULL == impginvite)
                    {
                        printf("can not find pginvite pointer\n\n");
                    }
                    else
                    {
                        printf("IM_PGINVITE id=%s,gid=%s,name:%s,type:%d,intites:%s,time=%s\n\n", impginvite->m_id.c_str(), impginvite->m_gid.c_str(),
                            impginvite->m_name.c_str(), impginvite->m_type, impginvite->m_invites.c_str(), impginvite->m_time.c_str());
                    }
                }
                break;

            case IM_PGQUIT:
                {
                    CImPGQuit* impgquit = dynamic_cast<CImPGQuit*>(socketmsg.m_im);
                    
                    if(NULL == impgquit)
                    {
                        printf("can not find pgquit pointer\n\n");
                    }
                    else
                    {
                        printf("IM_PGQUIT id=%s,uid=%s,gid=%s,name:%s,type:%d,gkey:%s,time=%s\n\n",
                            impgquit->m_id.c_str(), impgquit->m_uid.c_str(), impgquit->m_gid.c_str(), 
                            impgquit->m_name.c_str(), impgquit->m_type, impgquit->m_gkey.c_str(),
                            impgquit->m_time.c_str());
                    }
                }
                break;

            case IM_PGMODIFY:
                {
                    CImPGModify* impgmodify = dynamic_cast<CImPGModify*>(socketmsg.m_im);
                    
                    if(NULL == impgmodify)
                    {
                        printf("can not find pgmodify pointer\n\n");
                    }
                    else
                    {
                        printf("IM_PGMODIFY id=%s,gid=%s,name:%s,type:%d,gname:%s,time=%s\n\n", impgmodify->m_id.c_str(), impgmodify->m_gid.c_str(),
                            impgmodify->m_name.c_str(), impgmodify->m_type, impgmodify->m_gname.c_str(), impgmodify->m_time.c_str());
                    }
                }
                break;

            case IM_PGCARD:
                {
                    CImPGCard* impgcard= dynamic_cast<CImPGCard*>(socketmsg.m_im);
                    
                    if(NULL == impgcard)
                    {
                        printf("can not find pgcard pointer\n\n");
                    }
                    else
                    {
                        printf("IM_PGCARD id=%s,gid=%s,uid:%s,cardname:%d,time=%s\n\n",
                            impgcard->m_id.c_str(), impgcard->m_gid.c_str(),
                            impgcard->m_uid.c_str(), impgcard->m_cardname.c_str(),
                            impgcard->m_time.c_str());
                    }
                }
                break;

            case IM_SNS:
                {
                    CImSns* imsns = dynamic_cast<CImSns*>(socketmsg.m_im);
                    
                    if(NULL == imsns)
                    {
                        printf("can not find imsns pointer\n\n");
                    }
                    else
                    {
                        printf("IM_SNS id=%s,subtype=%s\n\n",
                            imsns->m_id.c_str(), imsns->m_subtype.c_str());
                    }
                }
                break;

            case IM_FANSMODIFY:
                {
                    CImFansModify* imfansmodify = dynamic_cast<CImFansModify*>(socketmsg.m_im);
                    
                    if(NULL == imfansmodify)
                    {
                        printf("can not find fansmodify pointer\n\n");
                    }
                    else
                    {
                        printf("IM_FANSMODIFY id=%s,uid=%s,name=%s,key=%s,info=%s\n\n",
                            imfansmodify->m_id.c_str(), imfansmodify->m_uid.c_str(),
                            imfansmodify->m_name.c_str(), imfansmodify->m_key.c_str(),
                            imfansmodify->m_info.c_str());
                    }
                }
                break;

            case IM_FANSAPPEND:
                {
                    CImFansAppend* imfansappend = dynamic_cast<CImFansAppend*>(socketmsg.m_im);
                    
                    if(NULL == imfansappend)
                    {
                        printf("can not find fansappend pointer\n\n");
                    }
                    else
                    {
                        printf("IM_FANSAPPEND id=%s,subtype=%s,uid=%s,name=%s,key=%s,info=%s,cmsid=%s\n\n",
                            imfansappend->m_id.c_str(), imfansappend->m_subtype.c_str(),
                            imfansappend->m_uid.c_str(), imfansappend->m_name.c_str(),
                            imfansappend->m_key.c_str(), imfansappend->m_info.c_str(),
                            imfansappend->m_cmsid.c_str());
                    }
                }
                break;

            case IM_FANSDELETE:
                {
                    CImFansDelete* imfansdelete = dynamic_cast<CImFansDelete*>(socketmsg.m_im);
                    
                    if(NULL == imfansdelete)
                    {
                        printf("can not find fansdelete pointer\n\n");
                    }
                    else
                    {
                        printf("IM_FANSDELETE id=%s,uid=%s\n\n",
                            imfansdelete->m_id.c_str(), imfansdelete->m_uid.c_str());
                    }
                }
                break;

            case IM_BLACKLIST:
                {
                    CImBlackList* imblacklist = dynamic_cast<CImBlackList*>(socketmsg.m_im);
                    
                    if(NULL == imblacklist)
                    {
                        printf("can not find imblacklist pointer\n\n");
                    }
                    else
                    {
                        printf("IM_BLACKLIST blackid=%s,type=%d\n\n",
                            imblacklist->m_blackid.c_str(), imblacklist->m_type);
                    }
                }
                break;
            }

            break;
        }
    }
}

//int recv_toblock(CSocketMsg& socketmsg, const char* inbuf, int inlen, string& prototype);

int main(int argc, char* argv[])
{
    sImModule.SetSavePath("D:/");
    sImModule.SetFileServer("123.57.251.23", 4869);

    //sImModule.StartXml("123.57.251.23", 8010, 30, libclient_callback);
    sImModule.StartByte("123.57.251.23", 8010, 30, libclient_callback);

    //sImModule.StartXml("127.0.0.1", 8010, 30, libclient_callback);
    //sImModule.StartByte("127.0.0.1", 8010, 30, libclient_callback);

    while(1)
    {
        sleep_s(5);

        //string blackid = "lqguo";
        //CImBlackList blacklist1(blackid, 1);
        //printf("add blacklist id=%s\n\n", blacklist1.m_id.c_str());
        //sImModule.PushSend(blacklist1);

        //sleep_s(5);
        //CImBlackList blacklist2(blackid, 0);
        //printf("del blacklist id=%s\n\n", blacklist2.m_id.c_str());
        //sImModule.PushSend(blacklist2);

        continue;

        string type = "chat";
        string to = "13810783099";
        string from = "13810783099";
        string txtbuffer = "12345";

        //string delid = "T1J_KTB5AT1RCvBVdK";
        //CImMessage deletemessage(delid);
        //sImModule.PushSend(deletemessage);

        //string category = "plain-text";
        //string body = "eyJ0eXBlIjoxLCJ0ZXh0IjoiXC9mMDA1ZlwvIn0=";
        //CImChat imchat(from, to, type, category, body);
        //printf("send chat id=%s\n\n", imchat.m_id.c_str());
        //sImModule.PushSend(imchat);

        string imagepath = "D:/1.jpg";
        CImMessage imagemessage(from, to, imagepath, txtbuffer, 480, 512, type);
        printf("send chat id=%s\n\n", imagemessage.m_id.c_str());
        sImModule.PushSend(imagemessage);


        string videopath = "D:/1.jpg";
        string faceimagepath = "";//"D:/1.jpg";
        string videotitle = "12345";
        int videolength = 15;
        CImMessage videomessage(from, to, videopath, faceimagepath, txtbuffer, videotitle, videolength, type);
        printf("send chat id=%s\n\n", videomessage.m_id.c_str());
        //sImModule.PushSend(videomessage);

        //sImModule.SetMapPoint(to_string<int>(rand_base(100)), to_string<int>(rand_base(100)));

		TString outstr;
        string prototype = "BYTE";
        //if(imagemessage.ToLanchatBuffer(outstr, prototype))
        if(videomessage.ToLanchatBuffer(outstr, prototype))
        {
            char* buffer = new char[outstr.GetSize() + 1];

            strcpy(buffer, outstr.GetBuffer());

            int buffersize = strlen(buffer);

            if(buffersize == outstr.GetSize())
            {
                string outbuf(buffer);

                sImModule.DoSocket(outbuf, MSG_RECV_MESSAGE, true);
                
                /*CSocketMsg socketmsg;

                if(buffer.size() != recv_toblock(socketmsg, buffer.c_str(), buffer.size(), prototype))
                {
                    printf("lanchat buffer toblock failed\n\n\n");
                }*/
            }
            else
            {
                printf("lanchat buffer failed\n\n\n");
            }

            delete buffer;
        }
    }

    return 0;
}
