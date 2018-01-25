#ifndef __FRAMEWORK_OPNAME_H_
#define __FRAMEWORK_OPNAME_H_

#include "mutex.h"

class COpname
{
    COpname(){}

    friend class CSingleton<COpname>;

    typedef map<int, string> key_map;

    typedef map<int, key_map> name_map;

public:

    string Get(int enumname, int enumkey)
    {
        const name_map::const_iterator itmap = m_namemap.find(enumname);

        if(m_namemap.end() != itmap)
        {
            const key_map::const_iterator itenum = itmap->second.find(enumkey);

            if(itmap->second.end() != itenum)
            {
                return itenum->second;
            }
        }

        return to_string<int>(enumname) + to_string<int>(enumkey);
    }

    void Push(int enumname, int enumkey, string enumvalue)
    {
        m_namemap[enumname][enumkey] = enumvalue;
    }

private:

    name_map m_namemap;
};

#define GET_OPNAME(enumname, enumkey) CSingleton<COpname>::Instance().Get(enumname, enumkey).c_str()

#define PUSH_OPNAME(enumname, enumkey, enumvalue) CSingleton<COpname>::Instance().Push(enumname, enumkey, enumvalue)

#endif //__FRAMEWORK_OPNAME_H_
