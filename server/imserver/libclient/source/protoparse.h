#ifndef _LIBCLIENT_PROTOPARSE_H_
#define _LIBCLIENT_PROTOPARSE_H_

#include "protoxml.h"

typedef int proto_parsemsg(CProtoByte* &protobyte, const char* inbuf, int inlen);

template<typename ProtoByte> int protobuffer_toblock(CProtoByte* &protobyte, const char* inbuf, int inlen)
{
    int packetlen = 0;

    protobyte = new ProtoByte();

    if(NULL != protobyte)
    {
        if((packetlen = protobyte->ToBlock(inbuf, inlen)) <= 0)
        {
            delete protobyte;
        }
    }

    return packetlen;
}

inline proto_parsemsg* get_proto_parse_method(const string& proto)
{
    if("BYTE" == proto)
    {
        return protobuffer_toblock<CProtoByte>;
    }
    else if("XML" == proto)
    {
        return protobuffer_toblock<CProtoXml>;
    }
    else
    {
        return NULL;
    }
}

inline int get_proto_value(CProtoByte* protobyte,
                           const char* key, const char* &value, int sign, int index = 0)
{
    if(NULL != protobyte)
    {
        protobyte = protobyte->GetChild(sign, key, index);

        if(NULL != protobyte)
        {
            CByte& bytevalue = protobyte->GetValue();

            value = bytevalue.c_str();

            return bytevalue.size();
        }
    }

    return -1;
}

inline CProtoByte* get_proto_block(int byteproto)
{
    if(PROTO_BYTE == byteproto)
    {
        return new CProtoByte();
    }
    else if(PROTO_XML == byteproto)
    {
        return new CProtoXml();
    }
    else
    {
        return NULL;
    }
}

inline CProtoByte* app_proto_block(int sign, CProtoByte* protobyte)
{
    if(0 == sign || NULL == protobyte)
    {
        return NULL;
    }
    else if(PROTO_BYTE == protobyte->GetProto())
    {
        return app_block<CProtoByte>(sign, protobyte);
    }
    else if(PROTO_XML == protobyte->GetProto())
    {
        return app_block<CProtoXml>(sign, protobyte);
    }
    else
    {
        return NULL;
    }
}

inline bool set_proto_value(CProtoByte* protobyte, string key, string value)
{
    if(NULL != protobyte)
    {
        if(protobyte->SetKey(key) > 0)
        {
            protobyte->SetValue(value);

            return true;
        }
    }

    return false;
}

#endif //_LIBCLIENT_PROTOPARSE_H_
