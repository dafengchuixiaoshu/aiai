#ifndef _MZIM_H_
#define _MZIM_H_

#include <string>
using std::string;

#include "mzim.pb.h"

enum
{
    MZIM_TOBUFFER_NULL = -1,
    MZIM_TOBUFFER_FAILED = -2,
};

#define SET_PROTO_PARAM(param) object->set_##param(m_##param)

#define GET_PROTO_PARAM(param) if(object.has_##param()) m_##param = object.param();

class CMZIM
{
public:

    CMZIM():m_cmdno(MZ::IM_CMD_CMD_ROOT){}

    virtual ~CMZIM(){}

    virtual int ToBuffer(string& outbuf) = 0;

    virtual bool ToBlock(const MZ::IM& inbuf) = 0;

protected:

    MZ::IM_CMD  m_cmdno;
};

#endif //_MZIM_H_
