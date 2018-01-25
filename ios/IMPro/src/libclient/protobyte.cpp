#include "protobyte.h"

int sys_trim(const char* &start, const char* inbuf, uint inlen)
{
    start = inbuf;

    const char* end = inbuf + inlen;

    while(start < end)
    {
        if(0 == *start)
        {
            return 0;
        }
        else if((unsigned char)*start > 0x20)
        {
            break;
        }
        else
        {
            ++start;
        }
    }

    while(start < end)
    {
        --end;

        if((unsigned char)*end > 0x20)
        {
            ++end;

            break;
        }
    }

    return end - start;
}

TString::TString(const char* inbuf, int inlen)
: m_rpos(0)
{
    m_wpos = inlen;

    m_buffer.resize(inlen);

    memcpy(&m_buffer[0], inbuf, inlen);
}

int TString::Read(char* outbuf, int outlen)
{
    if(outlen > 0 && NULL != outbuf && outlen + m_rpos <= m_buffer.size())
    {
        memcpy(outbuf, &m_buffer[m_rpos], outlen);

        m_rpos += outlen;

        return outlen;
    }

    return -1;
}

bool TString::Write(const char* inbuf, int inlen)
{
    if(inlen > 0 && NULL != inbuf)
    {
        if(m_buffer.size() < m_wpos + inlen)
        {
            m_buffer.resize(m_wpos + inlen + 128);
        }

        memcpy(&m_buffer[m_wpos], inbuf, inlen);

        m_wpos += inlen;

        return true;
    }

    return false;
}

CByte::CByte()
: m_buflen(0)
{
}

CByte::CByte(const CByte &r)
: m_buflen(0)
{
    append(r.m_buffer.c_str(), r.m_buflen);
}

CByte::CByte(const char* inbuf)
: m_buflen(0)
{
    append(inbuf);
}

CByte& CByte::operator= (const CByte& r)
{
    if(this != &r)
    {
        clear();

        append(r.m_buffer.c_str(), r.m_buflen);
    }

    return *this;
}

bool CByte::erase(size_t outlen)
{
    if(outlen <= 0 || m_buflen < outlen)
    {
        return false;
    }

    m_buflen -= outlen;

    m_buffer.erase(0, outlen);

    return true;
}

bool CByte::compare(const char* src)
{
    return src == m_buffer;
}

bool CByte::reduce(int& value)
{
    if(reduce((char*)&value, sizeof(int)))
    {
        value = ntohl(value);

        return true;
    }

    return false;
}

bool CByte::reduce(char* outbuf, uint outlen)
{
    if(outlen <= 0 || NULL == outbuf || m_buflen < outlen)
    {
        return false;
    }
    
    memcpy(outbuf, c_str(), outlen);

    return erase(outlen);
}

bool CByte::append(int value)
{
    value = htonl(value);

    return append((const char*)&value, sizeof(int));
}

bool CByte::append(const char* inbuf)
{
    if(NULL == inbuf)
    {
        return false;
    }

    return append(inbuf, strlen(inbuf), 0);
}

bool CByte::append(string& instr, uint exlen)
{
    return append(instr.c_str(), instr.size(), exlen);
}

bool CByte::append(CByte& instr, uint exlen)
{
    return append(instr.c_str(), instr.size(), exlen);
}

bool CByte::append(const char* inbuf, uint inlen, uint exlen)
{
    if(inlen <= 0 || NULL == inbuf)
    {
        return false;
    }

    if(m_buffer.size() < m_buflen + inlen)
    {
        m_buffer.resize(m_buflen + inlen + exlen, '\0');
    }

    memcpy(&m_buffer[m_buflen], inbuf, inlen);

    m_buflen += inlen;

    return true;
}

CProtoByte::CProtoByte()
: m_bytesign(0)
, m_byteproto(PROTO_BYTE)
, m_parent(NULL)
{
}

CProtoByte::CProtoByte(int sign, CProtoByte* parent)
: m_bytesign(sign)
, m_byteproto(PROTO_BYTE)
, m_parent(parent)
{
}

CProtoByte::~CProtoByte()
{
    for(BlockIterator it = m_children.begin(); m_children.end() != it; ++it)
    {
        delete (*it);
    }

    m_children.clear();
}

int CProtoByte::SetByte(CByte& instr, const char* byte, uint bytelen, bool istrim)
{
    const char* start = byte;

    if(istrim)
    {
        bytelen = sys_trim(start, byte, bytelen);
    }

    if(bytelen > 0)
    {
        instr.append(start, bytelen);
    }

    return bytelen;
}

bool CProtoByte::compare(const char* inbuf, uint inlen)
{
    if(m_bytekey.size() == inlen && 0 == strncmp(inbuf, m_bytekey.c_str(), inlen))
    {
        return true;
    }

    return false;
}

bool CProtoByte::append(string& inbuf, uint exlen)
{
    return append(inbuf.c_str(), inbuf.size(), exlen);
}

bool CProtoByte::append(const char* inbuf, uint inlen, uint exlen)
{
    return m_bytebuffer.append(inbuf, inlen, exlen);
}

int CProtoByte::SetKey(string key, bool istrim)
{
    return SetKey(key.c_str(), key.size(), istrim);
}

int CProtoByte::SetValue(string value, bool istrim)
{
    return SetValue(value.c_str(), value.size());
}

int CProtoByte::SetKey(const char* key, uint keylen, bool istrim)
{
    return SetByte(m_bytekey, key, keylen, istrim);
}

int CProtoByte::SetValue(const char* value, uint valuelen, bool istrim)
{
    return SetByte(m_bytevalue, value, valuelen, istrim);
}

CProtoByte* CProtoByte::GetChild(const int sign, const char* key, int index)
{
    BlockIterator it = m_children.begin();

    while(m_children.end() != it)
    {
        bool isok = false;

        if(0 == sign)
        {
            isok = true;
        }
        else if((*it)->m_bytesign == sign)
        {
            if(NULL == key)
            {
                isok = true;
            }
            else if((*it)->compare(key, strlen(key)))
            {
                isok = true;
            }
        }

        if(isok)
        {
            if(0 == index)
            {
                return (*it);
            }
            else
            {
                --index;
            }
        }

        ++it;
    }

    return NULL;
}
