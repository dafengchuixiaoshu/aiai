#ifdef WIN32
#include <afx.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>
#endif

#include "xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XML_TOKEN_START    1  // <
#define XML_TOKEN_END      2  // >
#define XML_TOKEN_START2   3  // </
#define XML_TOKEN_END2     4  // />
#define XML_TOKEN_PI_START 5  // <?
#define XML_TOKEN_PI_END   6  // ?>
#define XML_TOKEN_TEXT     7
#define XML_TOKEN_NAME     8
#define XML_TOKEN_STRING   9  // "xxx" or 'xxx'
#define XML_TOKEN_EQUAL    10 // =
#define XML_TOKEN_EOF      11
#define XML_TOKEN_CDATA    12
#define XML_TOKEN_INVALID  13



#ifndef WIN32
#define ASSERT    assert
#define strnicmp  strncasecmp
#define stricmp   strcasecmp
#endif


char *tokenTypeStrings[] = 
{
   "", //base 0
   "<",
   ">",
   "</",
   "/>",
   "<?",
   "?>",
   "text",
   "tag/attrname",
   "string",
   "=",
   "EOF",
   "CDATA",
   "invalid",
};


void ASSERT_TOKEN(int x); 

void strncpyx(char *dest, const char *src, int len)
{
    if(src==NULL)
    {
        dest[0]=0;
        return;
    } 
    strncpy(dest, src, len-1);
    dest[len-1] = 0;
}

TXMLAttr::TXMLAttr()
{
   m_strName[0]=0;
   m_strValue=NULL;
   brother = NULL;
   lineNo = 0;
}

TXMLAttr::~TXMLAttr()
{
    if(m_strValue)
        delete m_strValue;
}

void TXMLAttr::setName(const char* src)
{
   if(strlen(src) >ATTRNAME_LENGTH)
   {
      strncpy(m_strName,src,ATTRNAME_LENGTH);
      m_strName[ATTRNAME_LENGTH]=0;
   }
   else
   {
       strcpy(m_strName,src);
	   m_strName[strlen(m_strName)]='\0';
   }
}

char* TXMLAttr::getName()
{
   return m_strName;
}

void TXMLAttr::setValue(const char* src)
{
   if(m_strValue)
       delete m_strValue;
   m_strValue=new char[strlen(src)+1];
   strcpy(m_strValue,src);
}

char* TXMLAttr::getValue()
{
   return m_strValue;
}

TXMLAttr* TXMLAttr::dup()
{
   TXMLAttr* attr=new TXMLAttr;
   attr->setName(m_strName);
   attr->setValue(m_strValue);
         
   if(brother!=NULL)
      attr->brother=brother->dup();
   attr->lineNo=lineNo;
   return attr;
}
char* TXMLAttr::toStr()
{
   char* s=new char[strlen(m_strName) + strlen(m_strValue) + 4];
   sprintf(s,"%s=\"%s\"",m_strName,m_strValue);
   return s;
}
TXMLAttr*  TXMLAttr::getBrother()
{
    return brother;
}

TXMLElem::TXMLElem(TXMLParse* pParse)
{
	m_pParse = pParse;

   child=NULL;
   brother=NULL;
   parent=NULL;
   attr = NULL;
   lineNo = 0;

   m_strTag[0]=0;
   m_strContent=NULL;
}

TXMLElem::~TXMLElem()
{
   TXMLElem* p=child;
   while(p!=NULL)
   {
      TXMLElem *pNext = p->brother;
      delete p;
      p = pNext;
   }

   TXMLAttr* pAddr = attr;
   while(pAddr!=NULL)
   {
	   TXMLAttr *pNext = pAddr->brother;
	   delete pAddr;
	   pAddr = pNext;
   }

   if(m_strContent)
   {
       delete m_strContent;
       m_strContent=NULL;
   }

}

TXMLElem* TXMLElem::dup(TXMLElem* aParent)
{
   TXMLElem* elem=new TXMLElem(m_pParse);
   elem->setTag(m_strTag);
   elem->setContent(m_strContent);
  
   TXMLElem* pChild=child;
   while(pChild!=NULL)
   {
      TXMLElem* pDupChild=pChild->dup(elem);
      elem->addChild(pDupChild);
      pChild=pChild->brother;
   }
   elem->parent=aParent;

   if(attr!=NULL)
      elem->attr=attr->dup();
   elem->lineNo=lineNo;
   return elem;
}

void TXMLElem::setTag(const char* src)
{
   if(strlen(src) >TAG_LENGTH)
   {
      strncpy(m_strTag,src,TAG_LENGTH);
      m_strTag[TAG_LENGTH]=0;
   }
   else
   {
       strcpy(m_strTag,src);
   }
}

void TXMLElem::setContent(const int src)
{
	delete( m_strContent );

   char temp[32];
   sprintf(temp,"%u",src);
   m_strContent = new char[strlen(temp)+1];
   strcpy(m_strContent,temp);
}
void TXMLElem::setContent(const char* src)
{
   if(m_strContent)
       delete m_strContent;

   m_strContent = new char[strlen(src)+1];
   strcpy(m_strContent,src);
}

char* TXMLElem::getTag()
{
   return m_strTag;
}
char* TXMLElem::getContent()
{
	if (m_strContent)
        return m_strContent;
	return "";
}

void TXMLElem::GetLocation(char *pCurr)
{
   char *p = m_pParse->pLast;
   while(p != pCurr)
   {
      if(*p == '\n')
      {
         m_pParse->lineNumber ++;
         m_pParse->colNumber = 1;
      }
      m_pParse->colNumber ++;
      p++;
   }
   m_pParse->pLast = pCurr;
}

#define TOKEN_HEAD 10

void TXMLElem::GetToken()
{
   m_pParse->tokenValue[0] = 0;
//   m_pParse->textValue[0] = 0;
   char *p = m_pParse->pCurrent;
   if(m_pParse->bReadTag)
   {
      //skip blank
      while(*p==' ' || *p=='\t' || *p=='\n' || *p=='\r')
      {
         p++;
      }
      //only handle NAME, STRING, EQUAL, END, END2
      GetLocation(p);
      if(*p==0)
      {
         m_pParse->tokenType = XML_TOKEN_EOF;
      }
      else if(strncmp(p, "/>", 2) == 0)
      {
         m_pParse->tokenType = XML_TOKEN_END2;
         strcpy(m_pParse->tokenValue, "/>");
         p+=2;
         m_pParse->bReadTag = 0;
      }
      else if(*p=='>')
      {
         m_pParse->tokenType = XML_TOKEN_END;
         strcpy(m_pParse->tokenValue, ">");
         p++;
         m_pParse->bReadTag = 0;
      }
      else if(*p=='=')
      {
         m_pParse->tokenType = XML_TOKEN_EQUAL;
         strcpy(m_pParse->tokenValue, "=");
         p++;
      }
      else if(*p=='"' || *p=='\'')
      {
         char beginChar=*p;
         p++;
         char *pBegin = p;
//         while(*p !=0  && *p !='\n' && *p != beginChar)
         while(*p !=0  && *p != beginChar)
            p++;
         if( *p != beginChar || p - pBegin+1 > MAX_TOKEN_LEN )
         {
            //too long string
            strncpyx(m_pParse->tokenValue, pBegin, TOKEN_HEAD);
            strcat(m_pParse->tokenValue, "(Too long)");
            m_pParse->tokenType = XML_TOKEN_INVALID;
         }
         else
         {
            m_pParse->tokenType = XML_TOKEN_STRING;
            strncpyx(m_pParse->tokenValue, pBegin, p - pBegin+1);
            xmlTextDecode(m_pParse->tokenValue);
            p++;
         }
      }
      else if(isalpha(*p))
      {
         char *pBegin = p;
         p++;
         while(isalpha(*p) || *p>='0' && *p<='9' || *p == '_' || *p == ':'||*p == '-')
         {
            p++;
         }
         if(p - pBegin+1 > MAX_TOKEN_LEN )
         {
            //too long name
            strncpyx(m_pParse->tokenValue, pBegin, TOKEN_HEAD);
            strcat(m_pParse->tokenValue, "(Too long)");
            m_pParse->tokenType = XML_TOKEN_INVALID;
         }
         else
         {
            m_pParse->tokenType = XML_TOKEN_NAME;
            strncpyx(m_pParse->tokenValue, pBegin, p - pBegin+1);
            
            //luojianmin add on 2004/05/13            
            char* tok=strchr(m_pParse->tokenValue,':');
            if(tok!=NULL && strchr(tok+1,':')!=NULL)
               m_pParse->tokenType = XML_TOKEN_INVALID;
         }
      }
      else
      {
            //unknown token
         strncpyx(m_pParse->tokenValue, p, TOKEN_HEAD);
         m_pParse->tokenType = XML_TOKEN_INVALID;
      }
   }
   else 
   {
restart:
      //skip blank
      while(*p==' ' || *p=='\t' || *p=='\n' || *p=='\r')
      {
         p++;
      }
      //only handle BEGIN, COMMENT, CDATA, PI, TEXT
      GetLocation(p);
      if(*p==0)
      {
         m_pParse->tokenType = XML_TOKEN_EOF;
      }
      else if(strncmp(p, "<!--", 4) == 0)
      {
         char *pTail = strstr(p, "-->");
         if(!pTail)
         {
            //invalid comment
            strcpy(m_pParse->tokenValue, "<!-- ... (NEED -->)");
            m_pParse->tokenType = XML_TOKEN_INVALID;
         }
         else 
         {
            p = pTail + 3;
            goto restart;
         }
      }
      else if(strncmp(p, "<?", 2) == 0)
      {
         char *pTail = strstr(p, "?>");
         if(!pTail)
         {
            //invalid Process Instrution
            strcpy(m_pParse->tokenValue, "<? ... (NEED ?>)");
            m_pParse->tokenType = XML_TOKEN_INVALID;
         }
         else 
         {
            p = pTail + 2;
            goto restart;
         }
      }
      else if(strncmp(p, "<![CDATA[", 9) == 0)
      {
         char *pTail = strstr(p, "]]>");
         if(!pTail)
         {
            //invalid Process Instrution
            strcpy(m_pParse->tokenValue, "<![CDATA ... (NEED ]]>)");
            m_pParse->tokenType = XML_TOKEN_INVALID;
         }
         else 
         {
            //don't check it's length?
//             if(pTail - (p+9) >MAX_TEXT_LEN)
//             {
//                strncpyx(m_pParse->textValue, p, TOKEN_HEAD);
//                strcat(m_pParse->textValue, "(Too long)");
//                m_pParse->tokenType = XML_TOKEN_INVALID;
//             }
//             else
//             {
//                m_pParse->tokenType = XML_TOKEN_CDATA;
//                strncpyx(m_pParse->textValue, p+9, pTail-(p+9)+1);
//                p = pTail + 3;
//             }
			 
			if (m_pParse->m_pTextValue)
			{
				delete m_pParse->m_pTextValue;
				m_pParse->m_pTextValue = NULL;
			}
            
			m_pParse->m_pTextValue = new char[pTail-(p+9)+1];
			if (!m_pParse->m_pTextValue)
				m_pParse->tokenType = XML_TOKEN_INVALID;
			else
			{
				m_pParse->tokenType = XML_TOKEN_CDATA;
				strncpyx(m_pParse->m_pTextValue, p+9, pTail-(p+9)+1);
				p = pTail + 3;
			}
         }
      }
      else if(strncmp(p, "</", 2) == 0)
      {
         m_pParse->tokenType = XML_TOKEN_START2;
         strcpy(m_pParse->tokenValue, "</");
         p+=2;
         m_pParse->bReadTag = 1;
      }
      else if(*p=='<')
      {
         m_pParse->tokenType = XML_TOKEN_START;
         strcpy(m_pParse->tokenValue, "<");
         p++;
         m_pParse->bReadTag = 1;
      }
      else
      {
         //we think this is a text
         char *pBegin = p;
         while(*p !=0  && *p !='<')
            p++;
//          if(p - pBegin+1 > MAX_TEXT_LEN )
//          {
//             //too long text
//             strncpyx(m_pParse->textValue, pBegin, TOKEN_HEAD);
//             strcat(m_pParse->textValue, "(Too long)");
//             m_pParse->tokenType = XML_TOKEN_INVALID;
//          }
//          else
//          {
//             m_pParse->tokenType = XML_TOKEN_TEXT;
//             strncpyx(m_pParse->textValue, pBegin, p - pBegin+1);
//             xmlTextDecode(m_pParse->textValue);
//          }

		    if (m_pParse->m_pTextValue)
			{
				delete m_pParse->m_pTextValue;
				m_pParse->m_pTextValue = NULL;
			}
            
			m_pParse->m_pTextValue = new char[p - pBegin+1];
			if (!m_pParse->m_pTextValue)
				m_pParse->tokenType = XML_TOKEN_INVALID;
			else
			{
				m_pParse->tokenType = XML_TOKEN_TEXT;
				strncpyx(m_pParse->m_pTextValue, pBegin, p - pBegin+1);
				xmlTextDecode(m_pParse->m_pTextValue);
			}
      }
   }
   m_pParse->pCurrent = p;
}

void TXMLElem::addAttr(TXMLAttr* pAttr)
{
   if(attr==NULL)
   {
      attr=pAttr;
      return;
   }
   TXMLAttr* p=attr;
   while(p->brother!=NULL)
      p=p->brother;
   p->brother=pAttr;
}

char* TXMLElem::GetAttrValue(const char *name)
{
   TXMLAttr* p=GetAttr(name);
   if(p)
   {
       return p->getValue();
   }
   else
      return NULL;
}

TXMLAttr *TXMLElem::GetAttr(const char *name)
{
   for(TXMLAttr* p=attr; p!=NULL; p=p->brother)
   {
      if(stricmp(name, p->getName()) == 0)
         return p;
   }
   return NULL;
}

int TXMLElem::GetAttrNum()
{
   int i=0;
   for(TXMLAttr* p=attr; p!=NULL; p=p->brother)
   {
      i++;
   }
   return i;
}


TXMLAttr *TXMLElem::GetAttr(int index)
{
   int i=0;
   for(TXMLAttr* p=attr; p!=NULL; p=p->brother)
   {
      if(i == index)
         return p;
      i++;
   }
   return NULL;
}

TXMLElem *TXMLElem::GetSubElem(const char *tag1)
{
   for(TXMLElem* p=child; p!=NULL; p=p->brother)
   {
      if(stricmp(tag1, p->getTag()) == 0)
         return p;
   }
   return NULL;
}

int TXMLElem::GetSubElemNum()
{
   int i=0;
   for(TXMLElem* p=child; p!=NULL; p=p->brother)
   {
      i++;
   }
   return i;
}

int TXMLElem::GetSubElemNumByTag(char* strTag)
{
   if (strTag==NULL) return 0;
   int i=0;
   for(TXMLElem* p=child; p!=NULL; p=p->brother)
   {
      if(p->getTag()!=NULL && stricmp(p->getTag(),strTag)==0)
         i++;
   }
   return i;
}

int TXMLElem::GetSubElemName(int index,char* name)
{
   TXMLElem* p=GetSubElem(index);
   if(p!=NULL)
      strcpy(name,p->getTag());
   else
      name[0]=0;
   return 1;
}

TXMLElem *TXMLElem::GetSubElem(int index)
{
   int i=0;
   for(TXMLElem* p=child; p!=NULL; p=p->brother)
   {
      if(i == index)
         return p;
      i++;
   }
   return NULL;
}

TXMLElem *TXMLElem::GetSubElem(const char *tag1, const char *attrName, const char *attrValue)
{
   for(TXMLElem* p=child; p!=NULL; p=p->brother)
   {
      if(!tag1 || stricmp(tag1, p->getTag()) == 0)
      {
         char* value=p->GetAttrValue(attrName);
         if(value && strcmp(value,attrValue)==0)
            return p;
      }
   }
   return NULL;
}

TXMLElem *TXMLElem::GetSubElem(const char *tag1, int index)
{
   int i=0;
   for(TXMLElem* p=child; p!=NULL; p=p->brother)
   {
      if(stricmp(tag1, p->getTag()) == 0)
      {
         if(i == index)
            return p;
         i++;
      }
   }
   return NULL;
}

int TXMLElem::deleteChild(TXMLElem* pElement)
{
   TXMLElem* p=child;
   if(p==NULL)
      return -2;
  
   if(p==pElement)
   {
      child=pElement->brother;
      delete pElement;
      return 0;
   }

   int i=1;
   while(p!=NULL && p->brother!=pElement)
   {
      p=p->brother;
      i++;
   }
   if(p==NULL)
      return -3;

   p->brother=pElement->brother;
   delete pElement;
   return i;
}

void TXMLElem::addChild(TXMLElem* pElement, int mode)
{
   pElement->parent=this;
   if(child==NULL)
   {
      child=pElement;
      return;
   }

   if(mode==1)
   {
      TXMLElem* p=child;
      while(p->brother!=NULL)
         p=p->brother;
      p->brother=pElement;
   }
   else
   {
      pElement->brother=child;
      child=pElement;
   }
}

void TXMLElem::addChild(const char* tag,const char* content)
{
	ASSERT(tag);

	TXMLElem* pChild=new TXMLElem(m_pParse);
	pChild->setTag(tag);
	if(content)
	{
		pChild->setContent(content);
	}
	addChild(pChild);
}

void TXMLElem::addChild(const char* tag, int content)
{
	ASSERT(tag);

	TXMLElem* pChild=new TXMLElem(m_pParse);
	pChild->setTag(tag);
	if(content)
	{
		pChild->setContent(content);
	}
	addChild(pChild);
}
TXMLElem* TXMLElem::GetChild()
{
    return child;
}

TXMLElem* TXMLElem::GetBrother()
{
   return brother;
}
TXMLAttr* TXMLElem::GetAttr()
{
    return attr;
}
void TXMLElem::setAttr(const char *name,const char *value)
{
   if(name==NULL || value==NULL) return ;

   if(attr==NULL)
   {
      attr=new TXMLAttr;
      attr->setName(name);
      attr->setValue(value);
      return;
   }

   if(strcmp(attr->getName(),name)==0)
   {
      attr->setValue(value);
      return;
   }

   TXMLAttr* p=attr;
   while(1)
   { 
      if(strcmp(p->getName(),name)==0)
      {
         p->setValue(value);
         return;
      }
      if(p->brother==NULL)
      {
         p->brother=new TXMLAttr;
         p->brother->setName(name);
         p->brother->setValue(value);
         return;
      }
      p=p->brother;
   }
};

void TXMLElem::addAttr(const char *name,const char *value)
{
   if(name==NULL || value==NULL) return ;
   TXMLAttr* p=new TXMLAttr;
   p->setName(name);
   p->setValue(value);
   addAttr(p);
};

// mode: 0, add elder brother;  1, add younger brother
void TXMLElem::addBrother(TXMLElem* pElement, int mode)
{
   pElement->parent=parent;   //if over write operator = ?

   if(mode==1)
   {
      pElement->brother=brother;
      brother=pElement;
   }
   else
   {
      TXMLElem* p=parent->child; 
      if(p==this)
      {
         pElement->brother=p;
         parent->child=pElement;
      }
      else
      {
         while(p!=NULL && p->brother!=this)
            p=p->brother;

         pElement->brother=p->brother;
         p->brother=pElement;
      }
   }
}

void TXMLElem::addBrother(TXMLElem* elem)
{
	addBrother(elem,1);
}
bool TXMLElem::Read()
{
   int bIncludeContent = ReadBeginTag();
   
   if(bIncludeContent)
   {
      if (!ReadContent())
      {
		  return false;
      }
      
      if (!ReadEndTag())
      {
		  return false;
      }      
   }
  
   return true;
}

int TXMLElem::ReadBeginTag()
{
   lineNo = m_pParse->lineNumber;
   ASSERT_TOKEN(XML_TOKEN_START);

   GetToken(); //name
   ASSERT_TOKEN(XML_TOKEN_NAME);
   if(strlen(m_pParse->tokenValue) > TAG_LENGTH)
   {
       XMLERROR("line %d, col %d, too long tag,the max length is %d",m_pParse->lineNumber, m_pParse->colNumber,TAG_LENGTH); 
	   return 0;
   }
   strcpy(m_strTag,m_pParse->tokenValue);
   
   GetToken();
   while(m_pParse->tokenType != XML_TOKEN_END && m_pParse->tokenType != XML_TOKEN_END2)
   {
      ReadAttribute();
   }
   int bIncludeContent = (m_pParse->tokenType == XML_TOKEN_END);
   GetToken(); 
   return bIncludeContent;
}

bool TXMLElem::ReadAttribute()
{
   ASSERT_TOKEN(XML_TOKEN_NAME);
   TXMLAttr* pAttr=new TXMLAttr;
   ASSERT(pAttr);

   if(strlen(m_pParse->tokenValue) > ATTRNAME_LENGTH)
   {
      XMLERROR("line %d, col %d, too long attribute name,the max length is %d", m_pParse->lineNumber, m_pParse->colNumber,ATTRNAME_LENGTH); 
	  return false;
   }
   pAttr->setName(m_pParse->tokenValue);

   pAttr->lineNo = m_pParse->lineNumber;
   addAttr(pAttr);

   GetToken();
   ASSERT_TOKEN(XML_TOKEN_EQUAL);

   GetToken();
   ASSERT_TOKEN(XML_TOKEN_STRING);
   pAttr->setValue(m_pParse->tokenValue);

   GetToken();

   return true;
}

bool TXMLElem::ReadEndTag()
{
   ASSERT_TOKEN(XML_TOKEN_START2);

   GetToken(); //name
   ASSERT_TOKEN(XML_TOKEN_NAME);
   if(strcmp(getTag(), m_pParse->tokenValue) != 0)
   {
      XMLERROR("line %d, col %d, tag %s not match %s", m_pParse->lineNumber, m_pParse->colNumber, m_pParse->tokenValue, getTag()); 
	  return false;
   }

   GetToken(); 
   ASSERT_TOKEN(XML_TOKEN_END);
   
   GetToken();

   return true;
}

bool TXMLElem::ReadContent()
{
//    char textBuf[MAX_TEXT_LEN+1];
//    textBuf[0] = 0;

	if (XML_TOKEN_START2 == m_pParse->tokenType)
	{
        setContent('\0');
		return true;
	}

   while(m_pParse->tokenType != XML_TOKEN_START2)
   {
      if(m_pParse->tokenType == XML_TOKEN_TEXT)
      {
//          if(strlen(textBuf) + strlen(m_pParse->textValue) > MAX_TEXT_LEN)
// 		 {
//             XMLERROR("line %d, col %d, too long text", m_pParse->lineNumber, m_pParse->colNumber); 
// 			return false;
// 		 }
//          strcat(textBuf, m_pParse->textValue);
         GetToken();
      }
      else if(m_pParse->tokenType == XML_TOKEN_START)
      {
         TXMLElem* pChild = new TXMLElem(m_pParse);
         ASSERT(pChild);
         addChild(pChild);
         if (!pChild->Read())
         {
			 return false;
         }         
      }
      else if(m_pParse->tokenType == XML_TOKEN_CDATA)
      {
         TXMLElem* pCData= new TXMLElem(m_pParse);
         pCData->setTag("CDATA");
         pCData->setContent(m_pParse->m_pTextValue);
         addChild(pCData);
         GetToken();
      }
      else
      {
         XMLERROR("line %d, col %d, invalid token", m_pParse->lineNumber, m_pParse->colNumber); 
		 return false;
      }
   }

//    TrimAll(textBuf);
//    setContent(textBuf);

   if (m_pParse->m_pTextValue)
   {	   
	   TrimAll(m_pParse->m_pTextValue);
	   setContent(m_pParse->m_pTextValue);
   }

   return true;
}

char* TXMLElem::toStr()
{
	int attrNum=GetAttrNum();
	int childNum=GetSubElemNum();
	
	typedef char* PCHAR;
	PCHAR* attrStr=new PCHAR[attrNum];
	PCHAR* childStr=new PCHAR[childNum];
	
	int totalLen=0;
	int i=0;
	TXMLAttr* pAttr=attr;
	while(pAttr!=NULL)
	{
		attrStr[i]=pAttr->toStr();
		totalLen+=strlen(attrStr[i])+1;
		pAttr=pAttr->brother;
		i++;
	}
	
	i=0;
	TXMLElem* pChild=child;
	while(pChild!=NULL)
	{
		childStr[i]=pChild->toStr();
		totalLen+=strlen(childStr[i]);
		pChild=pChild->brother;
		i++;
	}
	if(m_strContent)
		totalLen+=strlen(m_strTag)*2+2+3+1+(strlen(m_strContent)*6);
	else
		totalLen+=strlen(m_strTag)*2+2+3+1;
    
	TXMLElem *p =parent;
	int k =1; //表示节点的级数
	while (p)
	{
		k++;
		p =p->parent;
	}
	char* s=new char[totalLen+4+4 +childNum *(4*k) ];
	
	s[0]=0;
	strcat(s,"<");
	
	strcat(s,m_strTag);
	for(i=0;i<attrNum;i++)
	{
		strcat(s," ");
		strcat(s,attrStr[i]);
		delete attrStr[i];
	}
	if(m_strContent ||childNum>0)
	{
		strcat(s,">");
		if(m_strContent)
		{
			char* encodeResult=xmlTextEncode(m_strContent);
			strcat(s,encodeResult);
			delete encodeResult;
			
		}
		
		for(i=0;i<childNum;i++)
		{
			if( i==0)
				strcat(s,"\r\n");
			for(int j=0; j<k ;j++)
				strcat(s, "  ");
			strcat(s,childStr[i]);
			delete childStr[i];
		}
		
		if ( childNum >0)
		{
			for(int j=0; j<k-1 ;j++)
				strcat(s, "  ");
		}
		strcat(s,"</");
		strcat(s,m_strTag);
		strcat(s,">\r\n");
	}
	else
	{
		strcat(s,"/>\r\n");
	}
	delete attrStr;
	delete childStr;
	
	return s;
}

TXMLParse::TXMLParse(char* s)
{
   bReadTag = 0;
   tokenType = XML_TOKEN_EOF;
   tokenValue[0] = 0;
   m_pTextValue = NULL;
   pCurrent = s;
   pBuffer = s;
   pLast = s;
   lineNumber = 1;
   colNumber = 1; 
}

TXMLParse::~TXMLParse()
{
	if (m_pTextValue)
	{
		delete m_pTextValue;
		m_pTextValue = NULL;
	}
}

TXMLElem* ParseXML(char* s, int& packlen)
{
    TXMLElem* xmlelem = ParseXML(s);

    if(NULL != xmlelem)
    {
        packlen = xmlelem->m_pParse->pCurrent - s - 1;
    }

    return xmlelem;
}

TXMLElem* ParseXML(char* s)
{
   if(s == NULL || *s == 0)
       return NULL;

   TXMLParse parse(s);

   TXMLElem *pRoot = new TXMLElem(&parse);
   pRoot->GetToken();
   if (parse.tokenType == XML_TOKEN_INVALID || parse.tokenType == XML_TOKEN_TEXT)
   {
        delete pRoot;
        return NULL;
   }

   try 
   {
      if (pRoot->Read())
      {
          return pRoot;
      }
      else
      {
          delete( pRoot );
          return NULL;
      }
   } 
   catch(TXMLExcept &e)
   {
       delete( pRoot );
       return NULL;
   }
   return NULL;
}

void TXMLElem::ASSERT_TOKEN(int x) 
{ 
   if (m_pParse->tokenType != x)
   {
      if(m_pParse->tokenType == XML_TOKEN_INVALID)
         XMLERROR("line %d, col %d, \"%s\", Syntax Error", 
            m_pParse->lineNumber, m_pParse->colNumber, m_pParse->tokenValue);
      else
         XMLERROR("line %d, col %d, \"%s\" should be token type \"%s\"", 
            m_pParse->lineNumber, m_pParse->colNumber, m_pParse->tokenValue, tokenTypeStrings[x]);
   }
}

void XMLERROR(const char* fmt, ...)
{
   char buffer[4096];
   va_list args;
   va_start(args, fmt);
   vsprintf(buffer, fmt, args);
   va_end(args);

   throw TXMLExcept(buffer); 
}

void xmlTextDecode(char* xmlText)
{
   char* r=xmlText;
   char* w=xmlText;
   while(*r!=0)
   {
      if(*r=='&')
      {
         if(strncmp(r+1,"amp;",4)==0)
         {
            r+=5;
            *w='&';
            w++;
            continue;
         }
         if(strncmp(r+1,"lt;",3)==0)
         {
            r+=4;
            *w='<';
            w++;
            continue;
         }
         if(strncmp(r+1,"gt;",3)==0)
         {
            r+=4;
            *w='>';
            w++;
            continue;
         }
         if(strncmp(r+1,"apos;",5)==0)
         {
            r+=6;
            *w='\'';
            w++;
            continue;
         }
         if(strncmp(r+1,"quot;",5)==0)
         {
            r+=6;
            *w='"';
            w++;
            continue;
         }
      }
      *w=*r;
      w++;
      r++;
   }
   *w=0;
}

char* xmlTextEncode(char* xmlText)
{
   char* xmlTextResult=new char[strlen(xmlText)*6+1];
   char* r=xmlText;
   char* w=xmlTextResult;
   while(*r!=0)
   {
      if(*r=='&')
      {
         memcpy(w,"&amp;",5);
         w+=5;
         r++;
         continue;
      }
      if(*r=='<')
      {
         memcpy(w,"&lt;",4);
         w+=4;
         r++;
         continue;
      }
      if(*r=='>')
      {
         memcpy(w,"&gt;",4);
         w+=4;
         r++;
         continue;
      }
      if(*r=='\'')
      {
         memcpy(w,"&apos;",6);
         w+=6;
         r++;
         continue;
      }
      if(*r=='"')
      {
         memcpy(w,"&quot;",6);
         w+=6;
         r++;
         continue;
      }
      *w=*r;
      w++;
      r++;
   }
   *w=0;
   return xmlTextResult;
}

void TrimAll(char* sour)
{
   char* p=sour;
   if(*p==0)
      return;
   
   for(;;)
   {
      if((unsigned)*p >0x20 || *p ==0)
         break;
      p++;
   }

   {
       size_t i; size_t n = strlen(p);

       for (i = 0 ; i < n && p[i] != '\0' ; i++)
           sour[i] = p[i];
       //for ( ; i < n ; i++)
           sour[i] = '\0';
   }

   //strcpy(sour,p);
   
   if(*p ==0)
      return;
   
   p=sour+strlen(sour);
   for(;;)
   {
      if((unsigned)*p >0x20)
      {
         *(p+1)=0;
         return;
      }
      p--;
   }
}
