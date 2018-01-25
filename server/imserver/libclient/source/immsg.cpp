#include "immsg.h"
#include "mutex.h"

#include <string>
using std::string;
#include <fstream>

#ifdef NO_LANCHAT
#define LOGD printf
#else
#include "BizCommon.h"
#endif

int g_coreindex = 0;

int g_corenumber = 0;

CMutex g_coremutex;

int file_read(char* &outbuf, const char* filename)
{
    std::ifstream is(filename, std::ios::binary);

    if(is)
    {
        is.seekg(0, is.end);

        int outlen = is.tellg();

        is.seekg(0, is.beg);

        if(outlen > 0)
        {
            outbuf = new char[outlen + 1];

            if(NULL != outbuf)
            {
                is.read(outbuf, outlen);

                outbuf[outlen] = '\0';

                is.close();

                return outlen;
            }
        }

        is.close();
    }

    return 0;
}

int file_write(const char* filename, const char* inbuf, int inlen)
{
    std::ios_base::openmode mode = std::ios::binary;

    std::ofstream os(filename, mode);

    if(os)
    {
        os.write(inbuf, inlen);

        os.flush();

        os.close();

        return inlen;
    }

    return 0;
}

string create_coreid()
{
    char outbuf[128] = { 0 };

#ifdef WIN32
    unsigned int tickcount = GetTickCount();
#else
    struct timeval tv; gettimeofday(&tv, NULL);
    unsigned int tickcount = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif

    g_coremutex.Lock();

    sprintf(outbuf, "%X%X%X", g_corenumber, tickcount, ++g_coreindex);

    g_coremutex.Unlock();

    return outbuf;
}

string get_cur_time(int type)
{
    struct tm* timeinfo;

    time_t now = time(NULL);

#ifdef WIN32
    timeinfo = localtime(&now);
#else
    struct tm t; timeinfo = localtime_r(&now, &t);
#endif

    char curtime[20] = { 0 };

    if(0 == type)
    {
        snprintf(curtime, sizeof(curtime), "%04d-%02d-%02d %02d:%02d:%02d",
            timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }
    else
    {
        snprintf(curtime, sizeof(curtime), "%04d-%02d-%02dT%02d:%02d:%02d",
            timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }

    return curtime;
}

#define SET_PROTO_PARAM(s) object->set_##s(m_##s)

#define GET_PROTO_PARAM(s) if(object.has_##s()) m_##s = object.s(); else return false;

#define GET_ATTR_STRING(s) const char* s; if(get_proto_value(protobyte, #s, s, BYTE_SIGN_ATTR) < 0) return false; else m_##s = s;

#define GET_ATTR_VALUE(s) const char* s; if(get_proto_value(protobyte, #s, s, BYTE_SIGN_ATTR) < 0) return false; else m_##s = atoi(s);

#define GET_NODE_STRING(s) const char* s; if(get_proto_value(protobyte, #s, s, BYTE_SIGN_NODE) < 0) return false; else m_##s = s;

#define PUSH_PROTO_VALUE() \
    if(outbuf.SerializeToString(&buf))\
{\
    int value = buf.size();\
    \
    string svalue = to_string<int>(value) + "&";\
    \
    outstr.Write(svalue.c_str(), svalue.size());\
    \
    if(outstr.Write(buf.c_str(), buf.size()))\
{\
    return true;\
}\
}

CIm::CIm()
: m_imtype(0)
{
    m_id = create_coreid();
}

CImAck::CImAck()
{
    m_imtype = IM_ACK;
}

CImAck::CImAck(string& id)
{
    m_id = id;

    m_imtype = IM_ACK;
}

bool CImAck::ToBuffer(TString& outstr, string& prototype)
{
    if("XML" == prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"ack\" id=\"%s\"/>";

        sprintf(txt, fmt, m_id.c_str());

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        outbuf.set_msgtype(im::im_Type_IM_ACK);

        im::im_imack* object = outbuf.mutable_ack();

        SET_PROTO_PARAM(id);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool CImAck::ToLanchatBuffer(TString& outstr, string& prototype)
{
    if("XML" == prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"ack\" id=\"%s\" time=\"%s\"/>";

        sprintf(txt, fmt, m_id.c_str(), get_cur_time().c_str());

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        m_time = get_cur_time();

        outbuf.set_msgtype(im::im_Type_IM_ACK);

        im::im_imack* object = outbuf.mutable_ack();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(time);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool CImAck::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_ack())
    {
        const im::im_imack& object = inbuf.ack();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(time);

        return true;
    }

    return false;
}

bool CImAck::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(time);

    return true;
}

CImChat::CImChat()
{
    m_imtype = IM_CHAT;
}

CImChat::CImChat(string& from, string& to, string& type, string& category, string& body)
: m_from(from)
, m_to(to)
, m_type(type)
, m_category(category)
, m_body(body)
{
    m_imtype = IM_CHAT;
}

CImChat::CImChat(string& from, string& to, string& type, string& category, string& body, string& id)
: m_from(from)
, m_to(to)
, m_type(type)
, m_category(category)
, m_body(body)
{
    m_id = id;

    m_imtype = IM_CHAT;
}

bool CImChat::ToBuffer(TString& outstr, string& prototype)
{
    if("XML" == prototype)
    {
        char txt[2048] = { 0 };

        static const char fmt[] = "<message id=\"%s\" from=\"%s\" to=\"%s\" type=\"%s\" category=\"%s\"><body>%s</body></message>";

        sprintf(txt, fmt, m_id.c_str(), m_from.c_str(), m_to.c_str(), m_type.c_str(), m_category.c_str(), m_body.c_str());

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        outbuf.set_msgtype(im::im_Type_IM_CHAT);

        im::im_imchat* object = outbuf.mutable_chat();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(from);

        SET_PROTO_PARAM(to);

        SET_PROTO_PARAM(type);

        SET_PROTO_PARAM(category);

        SET_PROTO_PARAM(body);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool CImChat::ToBlock(CProtoByte* protobyte)
{    
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(to);

    GET_NODE_STRING(body);

    GET_ATTR_STRING(time);

    GET_ATTR_STRING(from);

    GET_ATTR_STRING(type);

    GET_ATTR_STRING(category);

//    GET_ATTR_STRING(issave);

 //   GET_ATTR_STRING(isshare);

    return true;
}

bool CImChat::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_chat())
    {
        const im::im_imchat& object = inbuf.chat();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(to);

        GET_PROTO_PARAM(body);

        GET_PROTO_PARAM(time);

        GET_PROTO_PARAM(from);

        GET_PROTO_PARAM(type);

        GET_PROTO_PARAM(category);

        GET_PROTO_PARAM(issave);

        GET_PROTO_PARAM(isshare);

        return true;
    }

    return false;
}

CImLogin::CImLogin()
{
    m_imtype = IM_LOGIN;
}

CImLogin::CImLogin(string& from, string& domain, string& resource, string& platfrom)
: m_from(from)
, m_domain(domain)
, m_resource(resource)
, m_platform(platfrom)
{
    m_imtype = IM_LOGIN;
}

bool CImLogin::ToBuffer(TString& outstr, string& prototype)
{
    const char* sfrom = m_from.c_str();

    g_coremutex.Lock();

    g_coreindex = 0;

    g_corenumber = 0;

    while('\0' != *sfrom)
    {
        g_corenumber += (unsigned char)*sfrom; ++sfrom;
    }

    g_coremutex.Unlock();

    m_id = create_coreid();

    if("XML" == prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<stream:open from=\"%s\" domain=\"%s\" platform=\"%s\" resource=\"%s\"/>";

        sprintf(txt, fmt, m_from.c_str(), m_domain.c_str(), m_platform.c_str(), m_resource.c_str());

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        outbuf.set_msgtype(im::im_Type_IM_LOGIN);

        im::im_imlogin* object = outbuf.mutable_login();

        SET_PROTO_PARAM(from);

        SET_PROTO_PARAM(domain);

        SET_PROTO_PARAM(platform);

        SET_PROTO_PARAM(resource);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool CImLogin::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_VALUE(status);

    return true;
}

bool CImLogin::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_login())
    {
        const im::im_imlogin& object = inbuf.login();

        GET_PROTO_PARAM(status);

        return true;
    }

    return false;
}

CImLogout::CImLogout()
{
    m_imtype = IM_LOGOUT;
}

bool CImLogout::ToBuffer(TString& outstr, string& prototype)
{
    if("XML" == prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<stream:close/>";

        sprintf(txt, fmt, m_status);

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        outbuf.set_msgtype(im::im_Type_IM_LOGOUT);

        im::im_imlogout* object = outbuf.mutable_logout();

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool CImLogout::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_VALUE(status);

    return true;
}

bool CImLogout::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_logout())
    {
        const im::im_imlogout& object = inbuf.logout();

        GET_PROTO_PARAM(status);

        return true;
    }

    return false;
}

CImKickout::CImKickout()
{
    m_imtype = IM_KICKOUT;
}

CImKeepAlive::CImKeepAlive()
{
    m_imtype = IM_KEEPALIVE;
}

bool CImKeepAlive::ToBuffer(TString& outstr, string& prototype)
{
    if("XML" == prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"keepalive\" id=\"%s\"/>";

        sprintf(txt, fmt, m_id.c_str());

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        outbuf.set_msgtype(im::im_Type_IM_KEEPALIVE);

        im::im_imkeepalive* object = outbuf.mutable_keepalive();

        SET_PROTO_PARAM(id);

        PUSH_PROTO_VALUE();

        return false;
    }
}

CImPGCreate::CImPGCreate()
{
    m_imtype = IM_PGCREATE;
}

bool CImPGCreate::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_VALUE(type);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(time);

    return true;
}

bool CImPGCreate::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_pgcreate())
    {
        const im::im_impgcreate& object = inbuf.pgcreate();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(gid);

        GET_PROTO_PARAM(name);

        GET_PROTO_PARAM(time);

        GET_PROTO_PARAM(type);

        return true;
    }

    return false;
}

CImPGInvite::CImPGInvite()
{
    m_imtype = IM_PGINVITE;
}

bool CImPGInvite::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_VALUE(type);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(time);

    GET_ATTR_STRING(invites);

    return true;
}

bool CImPGInvite::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_pginvite())
    {
        const im::im_impginvite& object = inbuf.pginvite();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(gid);

        GET_PROTO_PARAM(name);

        GET_PROTO_PARAM(time);

        GET_PROTO_PARAM(type);

        GET_PROTO_PARAM(invites);

        return true;
    }

    return false;
}

CImPGQuit::CImPGQuit()
{
    m_imtype = IM_PGQUIT;
}

bool CImPGQuit::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_VALUE(type);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(gkey);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(time);

    return true;
}

bool CImPGQuit::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_pgquit())
    {
        const im::im_impgquit& object = inbuf.pgquit();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(uid);

        GET_PROTO_PARAM(gid);

        GET_PROTO_PARAM(gkey);

        GET_PROTO_PARAM(name);

        GET_PROTO_PARAM(time);

        GET_PROTO_PARAM(type);

        return true;
    }

    return false;
}

CImPGModify::CImPGModify()
{
    m_imtype = IM_PGMODIFY;
}

bool CImPGModify::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_VALUE(type);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(gname);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(time);

    return true;
}

bool CImPGModify::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_pgmodify())
    {
        const im::im_impgmodify& object = inbuf.pgmodify();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(gid);

        GET_PROTO_PARAM(gname);

        GET_PROTO_PARAM(name);

        GET_PROTO_PARAM(time);

        GET_PROTO_PARAM(type);

        return true;
    }

    return false;
}

CImPGCard::CImPGCard()
{
    m_imtype = IM_PGCARD;
}

bool CImPGCard::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(time);

    GET_ATTR_STRING(cardname);

    return true;
}

bool CImPGCard::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_pgcard())
    {
        const im::im_impgcard& object = inbuf.pgcard();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(uid);

        GET_PROTO_PARAM(gid);

        GET_PROTO_PARAM(time);

        GET_PROTO_PARAM(cardname);

        return true;
    }

    return false;
}

CImSns::CImSns()
{
    m_imtype = IM_SNS;
}

bool CImSns::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(subtype);

    return true;
}

bool CImSns::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_sns())
    {
        const im::im_imsns& object = inbuf.sns();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(subtype);

        return true;
    }

    return false;
}

CImFansModify::CImFansModify()
{
    m_imtype = IM_FANSMODIFY;
}

bool CImFansModify::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(key);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(info);

    return true;
}

bool CImFansModify::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_fansmodify())
    {
        const im::im_imfansmodify& object = inbuf.fansmodify();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(uid);

        GET_PROTO_PARAM(key);

        GET_PROTO_PARAM(name);

        GET_PROTO_PARAM(info);

        return true;
    }

    return false;
}

CImFansAppend::CImFansAppend()
{
    m_imtype = IM_FANSAPPEND;
}

bool CImFansAppend::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(key);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(info);

    GET_ATTR_STRING(subtype);

    GET_ATTR_STRING(cmsid);

    GET_ATTR_STRING(time);

    return true;
}

bool CImFansAppend::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_fansappend())
    {
        const im::im_imfansappend& object = inbuf.fansappend();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(uid);

        GET_PROTO_PARAM(key);

        GET_PROTO_PARAM(name);

        GET_PROTO_PARAM(info);

        GET_PROTO_PARAM(subtype);

        GET_PROTO_PARAM(cmsid);

        GET_PROTO_PARAM(time);

        return true;
    }

    return false;
}

CImFansDelete::CImFansDelete()
{
    m_imtype = IM_FANSDELETE;
}

bool CImFansDelete::ToBlock(CProtoByte* protobyte)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(time);

    return true;
}

bool CImFansDelete::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_fansdelete())
    {
        const im::im_imfansdelete& object = inbuf.fansdelete();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(uid);

        GET_PROTO_PARAM(time);

        return true;
    }

    return false;
}

CImMessage::CImMessage()
{
    m_imtype = IM_MESSAGE;
}

CImMessage::CImMessage(string& filekey)
: m_filekey(filekey)
{
    m_imtype = IM_MESSAGE;
}

CImMessage::CImMessage(string& fromuser, string& touser,
                       string& imagepath, string& txtbuffer, int imagewidth, int imageheight, string& msgtype)
                       : m_touser(touser)
                       , m_msgtype(msgtype)
                       , m_fromuser(fromuser)
                       , m_imagekey(imagepath)
{
    char* outbuf;

    m_imtype = IM_MESSAGE;

    m_category = "image";

    m_imagewidth = to_string<int>(imagewidth);

    m_imageheight = to_string<int>(imageheight);

    if(base64_encode(outbuf, txtbuffer.c_str(), txtbuffer.size()) > 0)
    {
        m_txtbuffer = outbuf;

        delete outbuf;
    }
}

CImMessage::CImMessage(string& fromuser, string& touser, string& videopath,
                       string& faceimagepath, string& txtbuffer, string& videotitle, int videolength, string& msgtype)
                       : m_touser(touser)
                       , m_msgtype(msgtype)
                       , m_fromuser(fromuser)
                       , m_videokey(videopath)
                       , m_imagekey(faceimagepath)
                       , m_txtbuffer(txtbuffer)
                       , m_videotitle(videotitle)
{
    char* outbuf;

    m_imtype = IM_MESSAGE;

    m_category = "video";

    m_videolength = to_string<int>(videolength);

    if(base64_encode(outbuf, txtbuffer.c_str(), txtbuffer.size()) > 0)
    {
        m_txtbuffer = outbuf;

        delete outbuf;
    }
}

bool file_encode(string& file)
{
	if(file.empty()) return true;

    int filelen; char* filebuf;

    if((filelen = file_read(filebuf, file.c_str())) > 0)
    {
        char* outbuf;

        if(base64_encode(outbuf, filebuf, filelen) > 0)
        {
            file = outbuf;

            delete outbuf;

            delete filebuf;

            return true;
        }

        delete filebuf;
    }

    return false;
}

bool file_decode(string& filename, string& savepath, string& file)
{
	if(file.empty()) return true;

    int outlen; char* outbuf;

    if((outlen = base64_decode(outbuf, file.c_str(), file.size())) > 0)
    {
        filename = create_coreid();

        string path = savepath + "/" + filename;

        if(file_write(path.c_str(), outbuf, outlen) > 0)
        {
            delete outbuf;

            return true;
        }

        delete outbuf;
    }

    return false;
}

bool CImMessage::ToLanchatBuffer(TString& outstr, string& prototype)
{
    string imagekey = m_imagekey;
    string videokey = m_videokey;

    if("BYTE" == prototype)
    {
        if(file_encode(m_imagekey))
        {
            string buf;

            im::im outbuf;

            outbuf.set_msgtype(im::im_Type_IM_MESSAGE);

            im::im_immessage* object = outbuf.mutable_message();

            SET_PROTO_PARAM(id);

            SET_PROTO_PARAM(touser);

            SET_PROTO_PARAM(msgtype);

            SET_PROTO_PARAM(fromuser);

            SET_PROTO_PARAM(category);

            SET_PROTO_PARAM(imagekey);

            SET_PROTO_PARAM(txtbuffer);

            if(m_videokey.empty())
            {
                SET_PROTO_PARAM(imagewidth);

                SET_PROTO_PARAM(imageheight);

                PUSH_PROTO_VALUE();
            }
            else if(file_encode(m_videokey))
            {
                SET_PROTO_PARAM(videokey);

                SET_PROTO_PARAM(videotitle);

                SET_PROTO_PARAM(videolength);

                m_imagekey = imagekey;

                m_videokey = videokey;

                PUSH_PROTO_VALUE();
            }
            else
            {
                LOGD("CImMessage::ToLanchatBuffer can not open videokey:%s\n\n", m_videokey.c_str());
            }
        }
        else
        {
            LOGD("CImMessage::ToLanchatBuffer can not open imagekey:%s\n\n", m_imagekey.c_str());
        }
    }
    else
    {
        LOGD("CImMessage::ToLanchatBuffer must be BYTE protocol\n\n");
    }

    m_imagekey = imagekey;
    m_videokey = videokey;

    return false;
}

bool CImMessage::ToBlock(CProtoByte* protobyte)
{
    LOGD("CImMessage::ToBlock must be BYTE protocol\n\n");

    return false;
}

bool CImMessage::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_message())
    {
        const im::im_immessage& object = inbuf.message();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(touser);

        GET_PROTO_PARAM(msgtype);

        GET_PROTO_PARAM(fromuser);

        GET_PROTO_PARAM(category);

        GET_PROTO_PARAM(imagekey);

        GET_PROTO_PARAM(txtbuffer);

        if(object.has_videokey())
        {
            GET_PROTO_PARAM(videokey);

            GET_PROTO_PARAM(videotitle);

            GET_PROTO_PARAM(videolength);
        }
        else
        {
            GET_PROTO_PARAM(imagewidth);

            GET_PROTO_PARAM(imageheight);
        }

        return true;
    }

    return false;
}

CImMapPoint::CImMapPoint()
{
    m_imtype = IM_MAPPOINT;
}

CImMapPoint::CImMapPoint(string& xpoint, string& ypoint)
: m_xpoint(xpoint)
, m_ypoint(ypoint)
{
    m_imtype = IM_MAPPOINT;
}

bool CImMapPoint::ToBuffer(TString& outstr, string& prototype)
{
    if("XML" == prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<mappoint x=\"%s\" y=\"%s\"/>";

        sprintf(txt, fmt, m_xpoint.c_str(), m_ypoint.c_str());

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        outbuf.set_msgtype(im::im_Type_IM_MAPPOINT);

        im::im_immappoint* object = outbuf.mutable_mappoint();

        SET_PROTO_PARAM(xpoint);

        SET_PROTO_PARAM(ypoint);

        PUSH_PROTO_VALUE();

        return false;
    }
}

CImDelete::CImDelete()
{
    m_imtype = IM_DELETE;
}

CImDelete::CImDelete(string& msgid, string& imagekey, string& videokey)
: m_msgid(msgid)
, m_imagekey(imagekey)
, m_videokey(videokey)
{
    m_imtype = IM_DELETE;
}

bool CImDelete::ToBuffer(TString& outstr, string& prototype)
{
    if("XML" == prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<delete msgid=\"%s\" imagekey=\"%s\" videokey=\"%s\"/>";

        sprintf(txt, fmt, m_msgid.c_str(), m_imagekey.c_str(), m_videokey.c_str());

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        outbuf.set_msgtype(im::im_Type_IM_DELETE);

        im::im_imdelete* object = outbuf.mutable_delete_();

        SET_PROTO_PARAM(msgid);

        SET_PROTO_PARAM(imagekey);

        SET_PROTO_PARAM(videokey);

        PUSH_PROTO_VALUE();

        return false;
    }
}


CImBlackList::CImBlackList()
{
    m_imtype = IM_BLACKLIST;
}

CImBlackList::CImBlackList(string& blackid, int type)
: m_type(type)
, m_blackid(blackid)
{
    m_imtype = IM_BLACKLIST;
}

bool CImBlackList::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_blacklist_())
    {
        const im::im_imblacklist& object = inbuf.blacklist_();

        GET_PROTO_PARAM(type);

        GET_PROTO_PARAM(blackid);

        return true;
    }

    return false;
}

bool CImBlackList::ToBuffer(TString& outstr, string& prototype)
{
    if("XML" == prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<blacklist blackid=\"%s\" type=\"%d\"/>";

        sprintf(txt, fmt, m_blackid.c_str(), m_type);

        if(outstr.Write(txt, strlen(txt)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        string buf;

        im::im outbuf;

        outbuf.set_msgtype(im::im_Type_IM_BLACKLIST);

        im::im_imblacklist* object = outbuf.mutable_blacklist_();

        SET_PROTO_PARAM(type);

        SET_PROTO_PARAM(blackid);

        PUSH_PROTO_VALUE();

        return false;
    }
}
