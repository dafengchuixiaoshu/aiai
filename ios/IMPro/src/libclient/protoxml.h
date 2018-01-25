#ifndef _LIBCLIENT_PROTOXML_H_
#define _LIBCLIENT_PROTOXML_H_

#include "protobyte.h"

#define XML_STATIC_HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"

class CProtoXml : public CProtoByte
{
    CProtoXml(int sign, CProtoByte* parent);

    template<typename Class> friend CProtoByte* app_block(int sign, CProtoByte* parent);

public:

    CProtoXml(const char* xmlheader = NULL);

    virtual ~CProtoXml(){}

    virtual int ToBuffer();

    virtual int ToBlock(const char* inbuf, int inlen);

private:

    bool FindLabel(const char* &pbegin, const char* &plabel, int restlen);

    int ParseLabel(CProtoByte* block, const char* plabel, const char* pend);

private:

    string m_xmlheader;
};

#endif //_LIBCLIENT_PROTOXML_H_
