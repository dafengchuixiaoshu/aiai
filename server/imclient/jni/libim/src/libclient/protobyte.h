#ifndef _LIBCLIENT_PROTOBYTE_H_
#define _LIBCLIENT_PROTOBYTE_H_

#include "header.h"

enum
{
    TOBLOCK_INVALID = -1,
    TOBLOCK_CONTINUE = 0,

    TOBUFFER_EMPTY = 0,
    TOBUFFER_INVALID = -1,
};

class TString
{
public:

    TString():m_rpos(0),m_wpos(0){}

    TString(const char* inbuf, int inlen);

    int GetSize(){ return m_wpos; }

    int Read(char* outbuf, int outlen);

    bool Write(const char* inbuf, int inlen);

    char* GetBuffer(){ return &m_buffer[0]; }

    TString& operator<<(int value){ _Write<int>(htonl(value)); return *this; }

    TString& operator>>(int &value){ if(_Read<int>(value)) value = ntohl(value); return *this; }

private:

    template <typename T> bool _Read(T &value){ return sizeof(T) == Read((char*)&value, sizeof(T)); }

    template <typename T> bool _Write(T value){ return Write((const char*)&value, sizeof(T)); }

private:

    size_t m_rpos;

    size_t m_wpos;

    vector<char> m_buffer;
};

class CByte
{
public:

	size_t m_buflen;

    string m_buffer;

    CByte();

    CByte(const CByte &r);

    CByte(const char* inbuf);

    CByte& operator= (const CByte& r);

    bool erase(size_t outlen);
    
    bool compare(const char* src);

    size_t size(){ return m_buflen; }

    bool empty(){ return 0 == m_buflen; }

    bool clear(){ return erase(m_buflen); }

    const char* c_str(){ return m_buffer.c_str(); }

    bool reduce(int& value);

    bool reduce(char* outbuf, uint outlen);

    bool append(int value);

    bool append(const char* inbuf);

    bool append(string& instr, uint exlen = 0);

    bool append(CByte& instr, uint exlen = 0);

    bool append(const char* inbuf, uint inlen, uint exlen = 0);
};

enum
{
    PROTO_XML = 0,
    PROTO_BYTE,
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

    int SetByte(CByte& instr, const char* byte, uint bytelen, bool istrim);

    template<typename Class> friend CProtoByte* app_block(int sign, CProtoByte* parent);

public:

    CProtoByte();

    CProtoByte(int sign, CProtoByte* parent);

    virtual ~CProtoByte();

    virtual int ToBuffer(){ return TOBUFFER_EMPTY; }

    virtual int ToBlock(const char* inbuf, uint inlen){ return TOBLOCK_CONTINUE; }

public:

    int& GetSign(){ return m_bytesign; }

    int& GetProto(){ return m_byteproto; }

    CByte& GetKey(){ return m_bytekey; }

    CByte& GetValue(){ return m_bytevalue; }

    CByte& GetBuffer(){ return m_bytebuffer; }

    CProtoByte* GetParent(){ return m_parent; }

    bool compare(const char* inbuf, uint inlen);

    bool append(string& inbuf, uint exlen = 0);

    bool append(const char* inbuf, uint inlen, uint exlen = 0);

public:

    int SetKey(string key, bool istrim = true);

    int SetValue(string value, bool istrim = true);

    int SetKey(const char* key, uint keylen, bool istrim = true);

    int SetValue(const char* value, uint valuelen, bool istrim = true);

    CProtoByte* GetChild(const int sign = 0, const char* key = NULL, int index = 0);

protected:

    int m_bytesign;

    int m_byteproto;

    CByte m_bytekey;

    CByte m_bytevalue;

    CByte m_bytebuffer;

    CProtoByte* m_parent;

    vector<CProtoByte*> m_children;
};

typedef vector<CProtoByte*>::const_iterator BlockIterator;

template<typename Class> CProtoByte* app_block(int sign, CProtoByte* parent)
{
    if(0 == sign || NULL == parent)
    {
        return NULL;
    }

    CProtoByte* child = new Class(sign, parent);

    if(NULL != child)
    {
        parent->m_children.push_back(child);
    }

    return child;
}

#endif //_LIBCLIENT_PROTOBYTE_H_
