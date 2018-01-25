#include "mzim.login.h"

CMZIMLogin::CMZIMLogin()
: m_status(0)
{
    m_cmdno = MZ::IM_CMD_CMD_LOGIN;
}

CMZIMLogin::CMZIMLogin(int status)
: m_status(status)
{
    m_cmdno = MZ::IM_CMD_CMD_LOGIN;
}

CMZIMLogin::CMZIMLogin(const string& username, const string& password)
: m_status(0)
, m_username(username)
, m_password(password)
{
    m_cmdno = MZ::IM_CMD_CMD_LOGIN;
}

int CMZIMLogin::ToBuffer(string& outbuf)
{
    MZ::IM  m_mzim;

    m_mzim.set_cmdno(MZ::IM_CMD_CMD_LOGIN);

    MZ::IM::LOGIN* object = m_mzim.mutable_login();

    if(NULL == object)
    {
        return MZIM_TOBUFFER_NULL;
    }

    SET_PROTO_PARAM(status);

    SET_PROTO_PARAM(username);

    SET_PROTO_PARAM(password);

    if(m_mzim.SerializeToString(&outbuf))
    {
        return outbuf.size();
    }
    else
    {
        return MZIM_TOBUFFER_FAILED;
    }
}

bool CMZIMLogin::ToBlock(const MZ::IM& inbuf)
{
    if(inbuf.has_login())
    {
        const MZ::IM::LOGIN& object = inbuf.login();

        GET_PROTO_PARAM(status);

        GET_PROTO_PARAM(username);

        GET_PROTO_PARAM(password);

        return true;
    }

    return false;
}
