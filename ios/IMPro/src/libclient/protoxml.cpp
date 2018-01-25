#include "protoxml.h"

int sys_trim(const char* &start, const char* inbuf, uint inlen);

CProtoXml::CProtoXml(const char* xmlheader)
{
    m_byteproto = PROTO_XML;

    if(NULL != xmlheader)
    {
        m_xmlheader = xmlheader;
    }
}

CProtoXml::CProtoXml(int sign, CProtoByte* parent)
: CProtoByte(sign, parent)
{
    m_byteproto = PROTO_XML;
}

int CProtoXml::ToBuffer()
{
    m_bytebuffer.clear();

    switch(m_bytesign)
    {
    case BYTE_SIGN_ATTR:
        if(m_bytekey.empty())
        {
            return TOBUFFER_INVALID;
        }
        else
        {
            m_bytebuffer.append(" ", 1, 128);

            m_bytebuffer.append(m_bytekey);

            m_bytebuffer.append("=\"");

            m_bytebuffer.append(m_bytevalue);

            m_bytebuffer.append("\"");
        }
        break;

    case BYTE_SIGN_ROOT:
        if(m_bytekey.empty())
        {
            return TOBUFFER_EMPTY;
        }
        else
        {
            m_bytebuffer.append(m_xmlheader);
        }

    case BYTE_SIGN_NODE:
        if(m_bytekey.empty())
        {
            return TOBUFFER_INVALID;
        }
        else
        {
            CByte attr; CByte node;

            for(BlockIterator it = m_children.begin(); it != m_children.end(); ++it)
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

                return TOBUFFER_INVALID;
            }

            m_bytebuffer.append("<", 1, 128);

            m_bytebuffer.append(m_bytekey);

            m_bytebuffer.append(attr);

            if(node.empty() && m_bytevalue.empty())
            {
                m_bytebuffer.append("/>");
            }
            else
            {
                m_bytebuffer.append(">");

                m_bytebuffer.append(m_bytevalue);

                m_bytebuffer.append(node);

                m_bytebuffer.append("</");

                m_bytebuffer.append(m_bytekey);

                m_bytebuffer.append(">");
            }
        }
        break;

    default:
        return TOBUFFER_INVALID;
    }

    return m_bytebuffer.size();
}

int CProtoXml::ToBlock(const char* inbuf, int inlen)
{
    if(!m_children.empty())
    {
        return TOBLOCK_INVALID;
    }

    int depth = 0;

    const char* pcur = inbuf;

    const char* pend = inbuf + inlen;

    if(0 == sys_trim(pcur, inbuf, inlen))
    {
        return TOBLOCK_CONTINUE;
    }
    else if('<' != *pcur)
    {
        return TOBLOCK_INVALID;
    }

    CProtoByte* block = NULL;

    CProtoByte* curnode = NULL;

    while(pcur < pend)
    {
        const char* plast = pcur;

        const char* plabel = NULL;

        if(!FindLabel(pcur, plabel, pend - pcur))
        {
            return TOBLOCK_CONTINUE;
        }

        switch(*plabel)
        {
        case '?':
        case '!':
            break;

        case '>':
            return TOBLOCK_INVALID;

        case '/':
            {
                const char* start;

                int templen = sys_trim(start, plabel + 1, pcur - 1 - plabel - 1);

                if(curnode->compare(start, templen))
                {
                    curnode->SetValue(plast, plabel - plast - 1);

                    --depth;

                    if(depth < 0)
                    {
                        return TOBLOCK_INVALID;
                    }
                    else if(0 == depth)
                    {
                        if(this != curnode)
                        {
                            return TOBLOCK_INVALID;
                        }
                        else
                        {
                            m_bytebuffer.append(inbuf, pcur - inbuf);

                            return pcur - inbuf;
                        }
                    }

                    curnode = curnode->GetParent();
                }
                else
                {
                    return TOBLOCK_INVALID;
                }
            }
            break;

        default:
            {
                if(NULL == curnode)
                {
                    curnode = block = this;
                }
                else if(NULL == (block = app_block<CProtoXml>(BYTE_SIGN_NODE, curnode)))
                {
                    return TOBLOCK_CONTINUE;
                }

                int result;

                curnode->SetValue(plast, plabel - plast - 1);

                if((result = ParseLabel(block, plabel, pcur - 1)) > 0)
                {
                    const char* pend = strchr(plabel, '/');

                    if(NULL == pend || pend >= pcur - 1)
                    {
                        ++depth;

                        curnode = block;
                    }
                    else
                    {
                        if(0 == depth)
                        {
                            m_bytebuffer.append(inbuf, pcur - inbuf);

                            return pcur - inbuf;
                        }
                    }
                }
                else
                {
                    return result;
                }
            }
            break;
        }
    }

    return TOBLOCK_CONTINUE;
}

bool CProtoXml::FindLabel(const char* &pbegin, const char* &plabel, int restlen)
{
    if(NULL != (plabel = strchr(pbegin, '<')))
    {
        if(NULL != (pbegin = strchr(++plabel, '>')))
        {
            if('?' == *plabel && NULL != (pbegin = strstr(plabel + 1, "?>")))
            {
                pbegin += 2;
            }
            else if(0 == strncmp(plabel, "!--", 3) && NULL != (pbegin = strstr(plabel + 3, "-->")))
            {
                pbegin += 3;
            }
            else 
            {
                ++pbegin;
            }

            return true;
        }
    }

    return false;
}

int CProtoXml::ParseLabel(CProtoByte* block, const char* plabel, const char* pend)
{
    const char* pcur = plabel;

    while(*pcur > 0x20 && ' ' != *pcur && '/' != *pcur && pcur < pend)
    {
        ++pcur;
    }

    if(0 == block->SetKey(plabel, pcur - plabel))
    {
        return TOBLOCK_INVALID;
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
                return TOBLOCK_INVALID;
            }
        }

        if(NULL == (attrblock = app_block<CProtoXml>(BYTE_SIGN_ATTR, block)))
        {
            return 0;
        }

        if(0 == attrblock->SetKey(pattr, pcur - pattr))
        {
            return TOBLOCK_INVALID;
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
                return TOBLOCK_INVALID;
            }
        }

        attrblock->SetValue(pattr, pcur - pattr);

        ++pcur;
    }

    return pend - plabel;
}
