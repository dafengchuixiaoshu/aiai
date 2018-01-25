#ifndef _MZIM_LOGIN_H_
#define _MZIM_LOGIN_H_

#include "mzim.h"

class CMZIMLogin : public CMZIM
{
public:

    CMZIMLogin();

    CMZIMLogin(int status);

    CMZIMLogin(const string& username, const string& password);

    virtual ~CMZIMLogin(){}

    virtual int ToBuffer(string& outbuf);

    virtual bool ToBlock(const MZ::IM& inbuf);

public:

    const int& Status(){ return m_status; }

    const string& Username(){ return m_username; }

    const string& Password(){ return m_password; }

private:

    int m_status;

    string m_username;

    string m_password;
};

#endif //_MZIM_LOGIN_H_
