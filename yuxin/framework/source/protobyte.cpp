#include "protobyte.h"

typedef vector<CProtoByte*>::const_iterator BlockIterator;

#define XML_HEAD "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"

CProtoByte::CProtoByte(int prototype)
: m_bytesign(0)
, m_prototype(prototype)
, m_parent(NULL)
{
}

CProtoByte::CProtoByte(int sign, CProtoByte* parent)
: m_bytesign(sign)
, m_prototype(parent->m_prototype)
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

int CProtoByte::SetByte(string& inbuf, const char* byte, int bytelen, bool istrim)
{
    const char* start = byte;

    if(0 == bytelen)
    {
        bytelen = strlen(byte);
    }

    if(istrim)
    {
        bytelen = sys_trim(start, byte, bytelen);
    }

    if(bytelen > 0)
    {
        inbuf.append(start, bytelen);
    }

    return bytelen;
}

CProtoByte* CProtoByte::GetBlock(int sign, string key, int index)
{
    BlockIterator it = m_children.begin();

    while(m_children.end() != it)
    {
        if(0 == sign)
        {
            return (*it);
        }
        else if((*it)->m_bytesign == sign && (key.empty() || (*it)->m_bytekey == key))
        {
            if(0 == index)
            {
                return (*it);
            }
            else if(index > 0)
            {
                --index;
            }
        }

        ++it;
    }

    return NULL;
}

int CProtoByte::SetKey(const string& key, bool istrim)
{
    return SetKey(key.c_str(), key.size(), istrim);
}

int CProtoByte::SetKey(const char* key, int keylen, bool istrim)
{
    if(NULL == key)
    {
        return 0;
    }

    return SetByte(m_bytekey, key, keylen, istrim);
}

int CProtoByte::SetValue(const string& value, bool istrim)
{
    return SetValue(value.c_str(), value.size());
}

int CProtoByte::SetValue(const char* value, int valuelen, bool istrim)
{
    if(NULL == value)
    {
        return 0;
    }
    
    return SetByte(m_bytevalue, value, valuelen, istrim);
}

static int xml_buffer_attr(string& bytebuffer, const string& key, const string& value)
{
    bytebuffer.resize(64, '\0');

    bytebuffer.append(" ");

    bytebuffer.append(key);

    bytebuffer.append("=\"");

    bytebuffer.append(value);

    bytebuffer.append("\"");

    return bytebuffer.size();
}

static int xml_buffer_node(string& bytebuffer, const string& key, const string& value, const string& attr, const string& node)
{
    bytebuffer.resize(256, '\0');

    bytebuffer.append("<");

    bytebuffer.append(key);

    bytebuffer.append(attr);

    if(node.empty() && value.empty())
    {
        bytebuffer.append("/>");
    }
    else
    {
        bytebuffer.append(">");

        bytebuffer.append(value);

        bytebuffer.append(node);

        bytebuffer.append("</");

        bytebuffer.append(key);

        bytebuffer.append(">");
    }

    return bytebuffer.size();
}

static int xml_buffer_root(string& attr, string& node, const vector<CProtoByte*>& children)
{
    attr.resize(256, '\0');

    node.resize(256, '\0');

    for(BlockIterator it = children.begin(); it != children.end(); ++it)
    {
        if((*it)->ToBuffer() > 0)
        {
            if(BYTE_SIGN_ATTR == (*it)->GetSign())
            {
                attr.append((*it)->GetBuffer());

                continue;
            }
            else if(BYTE_SIGN_NODE == (*it)->GetSign())
            {
                node.append((*it)->GetBuffer());

                continue;
            }
        }

        return 0;
    }

    return 1;
}

int CProtoByte::ToBuffer()
{
    return ToBuffer(m_prototype);
}

int CProtoByte::ToBuffer(int prototype)
{
    m_bytebuffer.clear();

    if(m_bytekey.empty())
    {
        return 0;
    }

    if(PROTO_XML == prototype)
    {
        switch(m_bytesign)
        {
        case BYTE_SIGN_ATTR:
            {
                return xml_buffer_attr(m_bytebuffer, m_bytekey, m_bytevalue);
            }

        case BYTE_SIGN_ROOT:
            {
                m_bytebuffer += XML_HEAD;
            }

        case BYTE_SIGN_NODE:
            {
                int outlen;
                
                string attr, node;

                if(1 == (outlen = xml_buffer_root(attr, node, m_children)))
                {
                    return xml_buffer_node(m_bytebuffer, m_bytekey, m_bytevalue, attr, node);
                }
    
                return outlen;
            }

        default:
            return -1;
        }
    }
    else if(PROTO_BYTE == prototype)
    {
        string temp;

        int outlen = temp.size();

        m_bytebuffer.resize(outlen + 10, 0);
        
        m_bytebuffer.append(to_string<int>(outlen) + "&");

        m_bytebuffer.append(temp);

        return m_bytebuffer.size();
    }
    else
    {
        return -1;
    }
}

int xml_block_find(const char* &pbegin, const char* &plabel)
{
    if(NULL != (plabel = strchr(pbegin, '<')))
    {
        ++plabel;

        if('>' == *plabel)
        {
            return -1;
        }
        else if(NULL != (pbegin = strchr(plabel, '>')))
        {
            if('?' == *plabel)
            {
                if('?' != *(pbegin - 1))
                {
                    return -1;
                }
            }
            else if(0 == strncmp(plabel, "!--", 3))
            {
                if(0 != strncmp(pbegin - 3, "!--", 3))
                {
                    return -1;
                }
            }

            ++pbegin;

            return 1;
        }
    }

    return 0;
}

int xml_block_parse(CProtoByte* protobyte, const char* plabel, const char* pend)
{
    const char* pcur = plabel;

    while(*pcur > 0x20 && ' ' != *pcur && '/' != *pcur && pcur < pend)
    {
        ++pcur;
    }

    if(0 == protobyte->SetKey(plabel, pcur - plabel))
    {
        return -1;
    }

    const char* pattr;

    CProtoByte* attrblock;

    while(pcur < pend)//attributes
    {
        while(*pcur <= 0x20)
        {
            ++pcur;
        }

        if('/' == *pcur || pcur >= pend)
        {
            return pend - plabel;
        }

        pattr = pcur;

        while('=' != *pcur)
        {
            ++pcur;

            if(pend == pcur)
            {
                return -1;
            }
        }

        if(NULL == (attrblock = new CProtoByte(BYTE_SIGN_ATTR, protobyte->GetParent())))
        {
            return 0;
        }

        //m_children.push_back(attrblock);

        if(0 == attrblock->SetKey(pattr, pcur - pattr))
        {
            return -1;
        }

        ++pcur;

        while(*pcur <= 0x20)
        {
            ++pcur;

            if(pend == pcur)
            {
                return -1;
            }
        }

        if('\"' != *pcur && '\'' != *pcur)
        {
            return -1;
        }

        pattr = ++pcur;

        while('\"' != *pcur && '\'' != *pcur)
        {
            ++pcur;

            if(pend == pcur)
            {
                return -1;
            }
        }

        attrblock->SetValue(pattr, pcur - pattr);

        ++pcur;
    }

    return pend - plabel;
}

int CProtoByte::ToBlock(const char* inbuf, int inlen)
{
    if('<' == inbuf[0])
    {
        m_prototype = PROTO_XML;

        return ToBlock(inbuf, inlen, m_prototype);
    }
    else
    {
        int result = 0;

        m_prototype = PROTO_BYTE;

        const char* temp = strchr(inbuf, '&');

        if(NULL != temp)
        {
            int packlen = atoi(inbuf);
      
            int pos = temp - inbuf + 1 + packlen;

            if(pos <= inlen)
            {
                if((result = ToBlock(temp + 1, packlen, m_prototype)) > 0)
                {
                    return pos;
                }
            }
        }
        
        return result;
    }
}

int CProtoByte::ToBlock(const char* inbuf, int inlen, int prototype)
{
    if(PROTO_XML == prototype)
    {
        const char* pcur = inbuf;

        if(sys_trim(pcur, inbuf, inlen) > 0)
        {
            int depth = 0;

            CProtoByte* curnode = NULL;

            CProtoByte* protobyte = NULL;

            const char* pend = inbuf + inlen;

            while(pcur < pend)
            {
                int find;

                const char* plast = pcur;

                const char* plabel = NULL;

                if((find = xml_block_find(pcur, plabel)) > 0)
                {
                    switch(*plabel)
                    {
                    case '?':
                    case '!':
                        break;

                    case '/':
                        {
                            const char* start;

                            int templen = sys_trim(start, plabel + 1, pcur - 1 - plabel - 1);

                            if(0 == strncmp(curnode->GetKey().c_str(), start, templen))
                            {
                                curnode->SetValue(plast, plabel - plast - 1);

                                --depth;

                                if(depth > 0)
                                {
                                    curnode = curnode->GetParent();

                                    break;
                                }
                                else if(0 == depth && this != curnode)
                                {
                                    m_bytebuffer.append(inbuf, pcur - inbuf);

                                    return pcur - inbuf;
                                }

                            }

                            return -1;
                        }

                    default:
                        {
                            if(NULL == curnode)
                            {
                                curnode = protobyte = this;
                            }
                            else if(NULL == (protobyte = new CProtoByte(BYTE_SIGN_ATTR, this)))
                            {
                                return 0;
                            }
                            else
                            {
                                m_children.push_back(protobyte);
                            }

                            int parse;

                            curnode->SetValue(plast, plabel - plast - 1);

                            if((parse = xml_block_parse(protobyte, plabel, pcur - 1)) > 0)
                            {
                                const char* pend = strchr(plabel, '/');

                                if(NULL == pend || pend >= pcur - 1)
                                {
                                    ++depth;

                                    curnode = protobyte;
                                }
                                else if(0 == depth)
                                {
                                    m_bytebuffer.append(inbuf, pcur - inbuf);

                                    return pcur - inbuf;
                                }

                                break;
                            }

                            return parse;
                        }
                    }

                    continue;
                }
                
                return find;
            }
        }

        return 0;
    }
    else if(PROTO_BYTE == prototype)
    {
    }

    return -1;
}

bool set_proto_block(CProtoByte* protobyte, string key, string value)
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

int get_proto_block(CProtoByte* protobyte, const char* key, const char* &value, int sign, int index)
{
    if(NULL != protobyte)
    {
        protobyte = protobyte->GetBlock(sign, key, index);

        if(NULL != protobyte)
        {
            const string& bytevalue = protobyte->GetValue();

            value = bytevalue.c_str();

            return bytevalue.size();
        }
    }

    value = NULL;

    return -1;
}
