#include "xml.h"
#include "Config.h"
#include "imsocket.h"
#include "baseparam.h"

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

#define SET_PROTO_PARAM(s) object->set_##s(m_##s)

#define GET_PROTO_PARAM(s) if(object.has_##s()) m_##s = object.s(); else return false;

#define GET_ATTR_STRING(s) { TXMLAttr* attr = xmlelem->GetAttr(#s); if(NULL == attr) return false; m_##s = attr->getValue(); }

#define GET_ATTR_VALUE(s) { TXMLAttr* attr = xmlelem->GetAttr(#s); if(NULL == attr) return false; m_##s = atoi(attr->getValue()); }

#define GET_NODE_STRING(s) { TXMLElem* elem = xmlelem->GetSubElem(#s); if(NULL == elem) return false; m_##s = elem->getContent(); }

bool parse_recvmsg(TBaseParam* &baseparam, im::im& inbuf)
{
    im::im_Type tag = inbuf.msgtype();

    if(im::im_Type_IM_LOGIN == tag)
    {
        baseparam = new TImLogin("BYTE");
    }
    else if(im::im_Type_IM_LOGOUT == tag)
    {
        baseparam = new TImLogout("BYTE");
    }
    else if(im::im_Type_IM_CHAT == tag)
    {
        baseparam = new TImChat("BYTE");
    }
    else if(im::im_Type_IM_UPSTREAM == tag)
    {
        baseparam = new TImUpStream("BYTE");
    }
    else if(im::im_Type_IM_ACK == tag)
    {
        baseparam = new TImAck("BYTE");
    }
    else if(im::im_Type_IM_CLOSE == tag)
    {
        baseparam = new TImClose("BYTE");
    }
    else if(im::im_Type_IM_KEEPALIVE == tag)
    {
        baseparam = new TImKeepAlive("BYTE");
    }
    else if(im::im_Type_IM_PGCREATE == tag)
    {
        baseparam = new TImPGCreate("BYTE");
    }
    else if(im::im_Type_IM_PGINVITE== tag)
    {
        baseparam = new TImPGInvite("BYTE");
    }
    else if(im::im_Type_IM_PGQUIT== tag)
    {
        baseparam = new TImPGQuit("BYTE");
    }
    else if(im::im_Type_IM_PGMODIFY== tag)
    {
        baseparam = new TImPGModify("BYTE");
    }
    else if(im::im_Type_IM_PGCARD == tag)
    {
        baseparam = new TImPGCard("BYTE");
    }
    else if(im::im_Type_IM_MAPPOINT == tag)
    {
        baseparam = new TImMapPoint("BYTE");
    }
    else if(im::im_Type_IM_DELETE == tag)
    {
        baseparam = new TImDelete("BYTE");
    }
    else if(im::im_Type_IM_BLACKLIST == tag)
    {
        baseparam = new TImBlackList("BYTE");
    }

    if(NULL == baseparam)
    {
        return false;
    }
    else if(baseparam->ToBlock(inbuf))
    {
        return true;
    }
    else
    {
        delete baseparam;

        return false;
    }
}

bool parse_recvmsg(TBaseParam* &baseparam, TXMLElem* xmlelem)
{
    const char* tag = xmlelem->getTag();

    if(0 == strcmp(tag, "stream:open"))
    {
        baseparam = new TImLogin();
    }
    else if(0 == strcmp(tag, "stream:close"))
    {
        baseparam = new TImLogout();
    }
    else if(0 == strcmp(tag, "message"))
    {
        baseparam = new TImChat();
    }
    else if(0 == strcmp(tag, "upstream"))
    {
        baseparam = new TImUpStream();
    }
    else if(0 == strcmp(tag, "mappoint"))
    {
        baseparam = new TImMapPoint();
    }
    else if(0 == strcmp(tag, "iq"))
    {
        string m_type;

        GET_ATTR_STRING(type);

        if("ack" == m_type)
        {
            baseparam = new TImAck();
        }
        else if("close" == m_type)
        {
            baseparam = new TImClose();
        }
        else if("keepalive" == m_type)
        {
            baseparam = new TImKeepAlive();
        }
        else if("pgcreate" == m_type)
        {
            baseparam = new TImPGCreate();
        }
        else if("pginvite" == m_type)
        {
            baseparam = new TImPGInvite();
        }
        else if("pgmodify" == m_type)
        {
            baseparam = new TImPGModify();
        }
        else if("pgcard" == m_type)
        {
            baseparam = new TImPGCard();
        }
        else if("pgquit" == m_type)
        {
            baseparam = new TImPGQuit();
        }
        else if("fansapp" == m_type)
        {
        }
        else if("fansdel" == m_type)
        {
        }
    }

    if(NULL == baseparam)
    {
        return false;
    }
    else if(baseparam->ToBlock(xmlelem))
    {
        return true;
    }
    else
    {
        delete baseparam;

        return false;
    }
}

TBaseParam::TBaseParam(string prototype)
: m_msgtype(0)
, m_prototype(prototype)
{
}

TImAck::TImAck(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_ACK;
}

TImAck::TImAck(string& id, string time, string prototype)
: TBaseParam(prototype)
, m_id(id)
, m_time(time)
{
    m_msgtype = IM_ACK;
}

TImAck::TImAck(string& id, string time, string& name, string& addr, string prototype)
: TBaseParam(prototype)
, m_id(id)
, m_addr(addr)
, m_name(name)
, m_time(time)
{
    m_msgtype = IM_ACK;
}

bool TImAck::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        if(m_addr.empty())
        {
            static const char fmt[] = "<iq type=\"ack\" id=\"%s\" time=\"%s\"/>";

            sprintf(txt, fmt, m_id.c_str(), m_time.c_str());
        }
        else
        {
            static const char fmt[] = "<iq type=\"ack\" id=\"%s\" time=\"%s\" name=\"%s\" addr=\"%s\"/>";

            sprintf(txt, fmt, m_id.c_str(), m_time.c_str(), m_name.c_str(), m_addr.c_str());
        }

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

        if(m_addr.empty())
        {
            SET_PROTO_PARAM(id);

            SET_PROTO_PARAM(time);
        }
        else
        {
            SET_PROTO_PARAM(id);

            SET_PROTO_PARAM(time);

            SET_PROTO_PARAM(name);

            SET_PROTO_PARAM(addr);
        }

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImAck::ToBlock(TXMLElem* xmlelem)
{
    TXMLAttr* attr = xmlelem->GetAttr("time");

    GET_ATTR_STRING(id);

    if(NULL != attr)
    {
        GET_ATTR_STRING(addr);

        GET_ATTR_STRING(name);

        GET_ATTR_STRING(time);
    }

    return true;
}

bool TImAck::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_ack())
    {
        const im::im_imack& object = inbuf.ack();

        GET_PROTO_PARAM(id);

        if(object.has_time())
        {
            GET_PROTO_PARAM(addr);

            GET_PROTO_PARAM(name);

            GET_PROTO_PARAM(time);
        }

        return true;
    }

    return false;
}

TImChat::TImChat(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_CHAT;
}

TImChat::TImChat(string& id, string addr, string prototype)
: TBaseParam(prototype)
, m_id(id)
, m_addr(addr)
, m_issave(0)
, m_isshare(0)
{
    m_msgtype = IM_CHAT;
}

TImChat::TImChat(string& id, string& from, string& to, string& type, string& category, string recvtime, string& body, int issave, int isshare, string prototype)
: TBaseParam(prototype)
, m_id(id)
, m_from(from)
, m_to(to)
, m_type(type)
, m_category(category)
, m_time(recvtime)
, m_body(body)
, m_issave(issave)
, m_isshare(isshare)
{
    m_msgtype = IM_CHAT;
}

bool TImChat::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        char txt[2048] = { 0 };

        if(m_from.empty())
        {
            static const char fmt[] = "<message id=\"%s\" addr=\"%s\"/>";

            sprintf(txt, fmt, m_id.c_str(), m_addr.c_str());
        }
        else
        {
            static const char fmt[] = "<message id=\"%s\" from=\"%s\" to=\"%s\" type=\"%s\" category=\"%s\" time=\"%s\"><body>%s</body></message>";

            sprintf(txt, fmt, m_id.c_str(), m_from.c_str(), m_to.c_str(), m_type.c_str(), m_category.c_str(), m_time.c_str(), m_body.c_str());
        }

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

        if(m_from.empty())
        {
            SET_PROTO_PARAM(id);

            SET_PROTO_PARAM(addr);
        }
        else
        {
            SET_PROTO_PARAM(id);

            SET_PROTO_PARAM(from);

            SET_PROTO_PARAM(to);

            SET_PROTO_PARAM(type);

            SET_PROTO_PARAM(category);

            SET_PROTO_PARAM(time);

            SET_PROTO_PARAM(body);

            SET_PROTO_PARAM(issave);

            SET_PROTO_PARAM(isshare);
        }

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImChat::ToBlock(TXMLElem* xmlelem)
{
    TXMLAttr* attr = xmlelem->GetAttr("addr");

    GET_ATTR_STRING(id);

    if(NULL == attr)
    {
        GET_ATTR_STRING(to);

        GET_NODE_STRING(body);

        GET_ATTR_STRING(from);

        GET_ATTR_STRING(type);

        GET_ATTR_STRING(category);
    }
    else
    {
        GET_ATTR_STRING(addr);
    }

    return true;
}

bool TImChat::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_chat())
    {
        const im::im_imchat& object = inbuf.chat();

        GET_PROTO_PARAM(id);

        if(object.has_addr())
        {
            GET_PROTO_PARAM(addr);
        }
        else
        {
            GET_PROTO_PARAM(to);

            GET_PROTO_PARAM(body);

            GET_PROTO_PARAM(from);

            GET_PROTO_PARAM(type);

            GET_PROTO_PARAM(category);
        }

        return true;
    }

    return false;
}

TImClose::TImClose(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_CLOSE;
}

TImClose::TImClose(string& user, string addr, string prototype)
: TBaseParam(prototype)
, m_user(user)
, m_addr(addr)
{
    m_msgtype = IM_CLOSE;
}

bool TImClose::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"close\", user=\"%s\", addr=\"%s\"/>";

        sprintf(txt, fmt, m_user.c_str(), m_addr.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_CLOSE);

        im::im_imclose* object = outbuf.mutable_close();

        SET_PROTO_PARAM(user);

        SET_PROTO_PARAM(addr);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImClose::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(user);

    GET_ATTR_STRING(addr);

    return true;
}

bool TImClose::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_close())
    {
        const im::im_imclose& object = inbuf.close();

        GET_PROTO_PARAM(user);

        GET_PROTO_PARAM(addr);

        return true;
    }

    return false;
}

TImLogin::TImLogin(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_LOGIN;
}

TImLogin::TImLogin(int status, string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_LOGIN;

    m_status = status;
}

TImLogin::TImLogin(string& from, string& domain, string& resource, string& platfrom, string prototype)
: TBaseParam(prototype)
, m_from(from)
, m_domain(domain)
, m_resource(resource)
, m_platform(platfrom)
{
    m_msgtype = IM_LOGIN;
}

bool TImLogin::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<stream:open status=\"%d\"/>";

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

        outbuf.set_msgtype(im::im_Type_IM_LOGIN);

        im::im_imlogin* object = outbuf.mutable_login();

        SET_PROTO_PARAM(status);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImLogin::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(from);

    GET_ATTR_STRING(domain);

    GET_ATTR_STRING(platform);

    GET_ATTR_STRING(resource);

    return true;
}

bool TImLogin::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_login())
    {
        const im::im_imlogin& object = inbuf.login();

        GET_PROTO_PARAM(from);

        GET_PROTO_PARAM(domain);

        GET_PROTO_PARAM(platform);

        GET_PROTO_PARAM(resource);

        return true;
    }

    return false;
}

TImLogout::TImLogout(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_LOGOUT;
}

TImLogout::TImLogout(int status, string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_LOGOUT;

    m_status = status;
}

bool TImLogout::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<stream:close status=\"%d\"/>";

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

        SET_PROTO_PARAM(status);

        PUSH_PROTO_VALUE();

        return false;
    }
}

TImKickout::TImKickout(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_KICKOUT;
}

bool TImKickout::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        static const char kickout[] = "<iq type=\"kickout\"/>";

        static const int kickoutlen = strlen(kickout);

        if(outstr.Write(kickout, kickoutlen))
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

        outbuf.set_msgtype(im::im_Type_IM_KICKOUT);

        PUSH_PROTO_VALUE();

        return false;
    }
}

TImKeepAlive::TImKeepAlive(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_KEEPALIVE;
}

bool TImKeepAlive::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        static const char keepalive[] = "<iq type=\"keepalive\"/>";

        static const int keepalivelen = strlen(keepalive);

        if(outstr.Write(keepalive, keepalivelen))
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

        PUSH_PROTO_VALUE();

        return false;
    }
}

TImUpStream::TImUpStream(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_UPSTREAM;
}

TImUpStream::TImUpStream(string& from, string& domain, string& resource, string& platfrom, string prototype)
: TBaseParam(prototype)
, m_from(from)
, m_domain(domain)
, m_resource(resource)
, m_platform(platfrom)
{
    m_msgtype = IM_UPSTREAM;
}

bool TImUpStream::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<upstream from=\"%s\" domain=\"%s\" platform=\"%s\" resource=\"%s\"/>";

        sprintf(txt, fmt, m_from.c_str(), m_domain.c_str(), m_resource.c_str(), m_platform.c_str() );

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

        outbuf.set_msgtype(im::im_Type_IM_UPSTREAM);

        im::im_imupstream* object = outbuf.mutable_upstream();

        SET_PROTO_PARAM(from);

        SET_PROTO_PARAM(domain);

        SET_PROTO_PARAM(platform);

        SET_PROTO_PARAM(resource);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImUpStream::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(from);

    GET_ATTR_STRING(domain);

    GET_ATTR_STRING(platform);

    GET_ATTR_STRING(resource);

    return true;
}

bool TImUpStream::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_upstream())
    {
        const im::im_imupstream& object = inbuf.upstream();

        GET_PROTO_PARAM(from);

        GET_PROTO_PARAM(domain);

        GET_PROTO_PARAM(platform);

        GET_PROTO_PARAM(resource);

        return true;
    }

    return false;
}

string get_cur_time(bool space);

TImPGCreate::TImPGCreate(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_PGCREATE;
}

TImPGCreate::TImPGCreate(string& id, string& gid, string& name, int type, string prototype)
: TBaseParam(prototype)
, m_type(type)
, m_id(id)
, m_gid(gid)
, m_name(name)
{
    m_msgtype = IM_PGCREATE;
}

bool TImPGCreate::ToBuffer(TString& outstr)
{
    m_time = get_cur_time(true);

    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"pgcreate\" id=\"%s\" gid=\"%s\" name=\"%s\" type=\"%d\" time=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_gid.c_str(), m_name.c_str(), m_type, m_time.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_PGCREATE);

        im::im_impgcreate* object = outbuf.mutable_pgcreate();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(gid);

        SET_PROTO_PARAM(name);

        SET_PROTO_PARAM(time);

        SET_PROTO_PARAM(type);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImPGCreate::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(id);

    GET_ATTR_VALUE(type);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(time);

    return true;
}

bool TImPGCreate::ToBlock(const im::im& inbuf)
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

TImPGInvite::TImPGInvite(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_PGINVITE;
}

TImPGInvite::TImPGInvite(string& id, string& gid, string& name, int type, string& invites, string prototype)
: TBaseParam(prototype)
, m_type(type)
, m_id(id)
, m_gid(gid)
, m_name(name)
, m_invites(invites)
{
    m_msgtype = IM_PGINVITE;
}

bool TImPGInvite::ToBuffer(TString& outstr)
{
    m_time = get_cur_time(true);

    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"pginvite\" id=\"%s\" gid=\"%s\" name=\"%s\" type=\"%d\" invites=\"%s\" time=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_gid.c_str(), m_name.c_str(), m_type, m_invites.c_str(), m_time.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_PGINVITE);

        im::im_impginvite* object = outbuf.mutable_pginvite();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(gid);

        SET_PROTO_PARAM(name);

        SET_PROTO_PARAM(time);

        SET_PROTO_PARAM(type);

        SET_PROTO_PARAM(invites);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImPGInvite::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(id);

    GET_ATTR_VALUE(type);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(time);

    GET_ATTR_STRING(invites);

    return true;
}

bool TImPGInvite::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_pgcreate())
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

TImPGQuit::TImPGQuit(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_PGQUIT;
}

TImPGQuit::TImPGQuit(string& id, string& uid, string& gid, string& name, int type, string& gkey, string prototype)
: TBaseParam(prototype)
, m_type(type)
, m_id(id)
, m_uid(uid)
, m_gid(gid)
, m_gkey(gkey)
, m_name(name)
{
    m_msgtype = IM_PGQUIT;
}

bool TImPGQuit::ToBuffer(TString& outstr)
{
    m_time = get_cur_time(true);

    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"pgquit\" id=\"%s\" uid=\"%s\" gid=\"%s\" name=\"%s\" type=\"%d\" gkey=\"%s\" time=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_uid.c_str(), m_gid.c_str(), m_name.c_str(), m_type, m_gkey.c_str(), m_time.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_PGQUIT);

        im::im_impgquit* object = outbuf.mutable_pgquit();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(uid);

        SET_PROTO_PARAM(gid);

        SET_PROTO_PARAM(gkey);

        SET_PROTO_PARAM(name);

        SET_PROTO_PARAM(time);

        SET_PROTO_PARAM(type);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImPGQuit::ToBlock(TXMLElem* xmlelem)
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

bool TImPGQuit::ToBlock(const im::im& inbuf)
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

TImPGModify::TImPGModify(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_PGMODIFY;
}

TImPGModify::TImPGModify(string& id, string& gid, string& name, int type, string& gname, string prototype)
: TBaseParam(prototype)
, m_type(type)
, m_id(id)
, m_gid(gid)
, m_name(name)
, m_gname(gname)
{
    m_msgtype = IM_PGMODIFY;
}

bool TImPGModify::ToBuffer(TString& outstr)
{
    m_time = get_cur_time(true);

    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"pgmodify\" id=\"%s\" gid=\"%s\" name=\"%s\" type=\"%d\" gname=\"%s\" time=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_gid.c_str(), m_name.c_str(), m_type, m_gname.c_str(), m_time.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_PGMODIFY);

        im::im_impgmodify* object = outbuf.mutable_pgmodify();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(gid);

        SET_PROTO_PARAM(gname);

        SET_PROTO_PARAM(name);

        SET_PROTO_PARAM(time);

        SET_PROTO_PARAM(type);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImPGModify::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(id);

    GET_ATTR_VALUE(type);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(gname);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(time);

    return true;
}

bool TImPGModify::ToBlock(const im::im& inbuf)
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

TImPGCard::TImPGCard(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_PGCARD;
}

TImPGCard::TImPGCard(string& id, string& gid, string& uid, string& cardname, string prototype)
: TBaseParam(prototype)
, m_id(id)
, m_gid(gid)
, m_uid(uid)
, m_cardname(cardname)
{
    m_msgtype = IM_PGCARD;
}

bool TImPGCard::ToBuffer(TString& outstr)
{
    m_time = get_cur_time(true);

    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"pgcard\" id=\"%s\" gid=\"%s\" uid=\"%s\" cardname=\"%s\" time=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_gid.c_str(), m_uid.c_str(), m_cardname.c_str(), m_time.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_PGCARD);

        im::im_impgcard* object = outbuf.mutable_pgcard();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(uid);

        SET_PROTO_PARAM(gid);

        SET_PROTO_PARAM(time);

        SET_PROTO_PARAM(cardname);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImPGCard::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(gid);

    GET_ATTR_STRING(time);

    GET_ATTR_STRING(cardname);

    return true;
}

bool TImPGCard::ToBlock(const im::im& inbuf)
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

TImSns::TImSns(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_SNS;
}

TImSns::TImSns(string& id, string& subtype, string prototype)
: TBaseParam(prototype)
, m_id(id)
, m_subtype(subtype)
{
    m_msgtype = IM_SNS;
}

bool TImSns::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"sns\" id=\"%s\" subtype=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_subtype.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_SNS);

        im::im_imsns* object = outbuf.mutable_sns();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(subtype);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImSns::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(subtype);

    return true;
}

bool TImSns::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_pgcard())
    {
        const im::im_imsns& object = inbuf.sns();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(subtype);

        return true;
    }

    return false;
}

TImFansModify::TImFansModify(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_FANSMODIFY;
}

TImFansModify::TImFansModify(string& uid, string& name, string& key, string& info, string prototype)
: TBaseParam(prototype)
, m_uid(uid)
, m_name(name)
, m_key(key)
, m_info(info)
{
    m_msgtype = IM_FANSMODIFY;
}

bool TImFansModify::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"fansmodify\" id=\"%s\" uid=\"%s\" name=\"%s\" key=\"%s\" info=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_uid.c_str(), m_name.c_str(), m_key.c_str(), m_info.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_FANSMODIFY);

        im::im_imfansmodify* object = outbuf.mutable_fansmodify();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(uid);

        SET_PROTO_PARAM(name);

        SET_PROTO_PARAM(key);

        SET_PROTO_PARAM(info);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImFansModify::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(key);

    GET_ATTR_STRING(info);

    return true;
}

bool TImFansModify::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_fansmodify())
    {
        const im::im_imfansmodify& object = inbuf.fansmodify();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(uid);

        GET_PROTO_PARAM(name);

        GET_PROTO_PARAM(key);

        GET_PROTO_PARAM(info);

        return true;
    }

    return false;
}

TImFansAppend::TImFansAppend(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_FANSAPPEND;
}

TImFansAppend::TImFansAppend(string& id, string& subtype, string& uid, string& name, string& key, string& info, string& cmsid, string prototype)
: TBaseParam(prototype)
, m_id(id)
, m_uid(uid)
, m_key(key)
, m_subtype(subtype)
, m_name(name)
, m_info(info)
, m_cmsid(cmsid)
{
    m_msgtype = IM_FANSAPPEND;
}

bool TImFansAppend::ToBuffer(TString& outstr)
{
    m_time = get_cur_time(true);

    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"fansappend\" id=\"%s\" subtype=\"%s\" uid=\"%s\" name=\"%s\" key=\"%s\" info=\"%s\" cmsid=\"%s\" time=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_subtype.c_str(), m_uid.c_str(), m_name.c_str(), m_key.c_str(), m_info.c_str(), m_cmsid.c_str(), m_time.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_FANSAPPEND);

        im::im_imfansappend* object = outbuf.mutable_fansappend();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(uid);

        SET_PROTO_PARAM(key);

        SET_PROTO_PARAM(time);

        SET_PROTO_PARAM(subtype);

        SET_PROTO_PARAM(name);

        SET_PROTO_PARAM(info);

        SET_PROTO_PARAM(cmsid);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImFansAppend::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(key);

    GET_ATTR_STRING(time);

    GET_ATTR_STRING(subtype);

    GET_ATTR_STRING(name);

    GET_ATTR_STRING(info);

    GET_ATTR_STRING(cmsid);

    return true;
}

bool TImFansAppend::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_fansappend())
    {
        const im::im_imfansappend& object = inbuf.fansappend();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(uid);

        GET_PROTO_PARAM(key);

        GET_PROTO_PARAM(time);

        GET_PROTO_PARAM(subtype);

        GET_PROTO_PARAM(name);

        GET_PROTO_PARAM(info);

        GET_PROTO_PARAM(cmsid);

        return true;
    }

    return false;
}

TImFansDelete::TImFansDelete(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_FANSDELETE;
}

TImFansDelete::TImFansDelete(string& id, string& uid, string prototype)
: TBaseParam(prototype)
, m_id(id)
, m_uid(uid)
{
    m_msgtype = IM_FANSDELETE;
}

bool TImFansDelete::ToBuffer(TString& outstr)
{
    m_time = get_cur_time(true);

    if("XML" == m_prototype)
    {
        char txt[1024] = { 0 };

        static const char fmt[] = "<iq type=\"fansdelete\" id=\"%s\" uid=\"%s\" time=\"%s\"></iq>";

        sprintf(txt, fmt, m_id.c_str(), m_uid.c_str(), m_time.c_str());

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

        outbuf.set_msgtype(im::im_Type_IM_FANSDELETE);

        im::im_imfansdelete* object = outbuf.mutable_fansdelete();

        SET_PROTO_PARAM(id);

        SET_PROTO_PARAM(uid);

        SET_PROTO_PARAM(time);

        PUSH_PROTO_VALUE();

        return false;
    }
}

bool TImFansDelete::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(id);

    GET_ATTR_STRING(uid);

    GET_ATTR_STRING(time);

    return true;
}

bool TImFansDelete::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_fansappend())
    {
        const im::im_imfansappend& object = inbuf.fansappend();

        GET_PROTO_PARAM(id);

        GET_PROTO_PARAM(uid);

        GET_PROTO_PARAM(time);

        return true;
    }

    return false;
}

TImMapPoint::TImMapPoint(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_MAPPOINT;
}

bool TImMapPoint::ToBlock(TXMLElem* xmlelem)
{
    GET_ATTR_STRING(xpoint);

    GET_ATTR_STRING(ypoint);

    return true;
}

bool TImMapPoint::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_mappoint())
    {
        const im::im_immappoint& object = inbuf.mappoint();

        GET_PROTO_PARAM(xpoint);

        GET_PROTO_PARAM(ypoint);

        return true;
    }

    return false;
}

TImDelete::TImDelete(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_DELETE;
}

bool TImDelete::ToBlock(const im::im& inbuf)
{
    if(inbuf.has_delete_())
    {
        const im::im_imdelete& object = inbuf.delete_();

        GET_PROTO_PARAM(msgid);

        GET_PROTO_PARAM(imagekey);

        GET_PROTO_PARAM(videokey);

        return true;
    }

    return false;
}

TImBlackList::TImBlackList(string prototype)
: TBaseParam(prototype)
{
    m_msgtype = IM_BLACKLIST;
}

TImBlackList::TImBlackList(string& blackid, int type, string prototype)
: TBaseParam(prototype)
, m_blackid(blackid)
, m_type(type)
{
    m_msgtype = IM_BLACKLIST;
}

bool TImBlackList::ToBuffer(TString& outstr)
{
    if("XML" == m_prototype)
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

bool TImBlackList::ToBlock(const im::im& inbuf)
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
