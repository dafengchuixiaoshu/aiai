#ifndef __FRAMEWORK_PROTOBYTE_H_
#define __FRAMEWORK_PROTOBYTE_H_

#include "header.h"

enum
{
    PROTO_NULL = 0,
    PROTO_XML,
    PROTO_BYTE
};

enum
{
    BYTE_SIGN_ROOT = 0,
    BYTE_SIGN_NODE,
    BYTE_SIGN_ATTR
};

class CProtoByte
{
    CProtoByte(const CProtoByte &r);

    CProtoByte& operator= (const CProtoByte& r);

    int SetByte(string& inbuf, const char* byte, int bytelen, bool istrim);

public:

    CProtoByte(int prototype = PROTO_BYTE);

    CProtoByte(int sign, CProtoByte* parent);

    ~CProtoByte();    
    
    int ToBuffer();

    int ToBuffer(int prototype);

    int ToBlock(const char* inbuf, int inlen);

    int ToBlock(const char* inbuf, int inlen, int prototype);

    /***********************************************/
    int& GetSign(){ return m_bytesign; }

    const string& GetBuffer(){ return m_bytebuffer; }

    CProtoByte* GetBlock(int sign, string key, int index = 0);

    CProtoByte* GetParent(){ return (NULL == m_parent) ? this : m_parent; }

    /***********************************************/
    const string& GetKey(){ return m_bytekey; }

    int SetKey(const string& key, bool istrim = true);

    int SetKey(const char* key, int keylen = 0, bool istrim = true);

    /***********************************************/
    const string& GetValue(){ return m_bytevalue; }

    int SetValue(const string& value, bool istrim = true);

    int SetValue(const char* value, int valuelen = 0, bool istrim = true);

private:

    int m_bytesign;

    int m_prototype;

    string m_bytekey;

    string m_bytevalue;

    string m_bytebuffer;

    CProtoByte* m_parent;

    vector<CProtoByte*> m_children;
};

bool set_proto_block(CProtoByte* protobyte, string key, string value);

int get_proto_block(CProtoByte* protobyte, const char* key, const char* &value, int sign, int index = 0);

#endif //__FRAMEWORK_PROTOBYTE_H_
