#ifndef _XML_H
#define _XML_H

#define TAG_LENGTH      128
#define ATTRNAME_LENGTH 128
#define MAX_TOKEN_LEN   640
//#define MAX_TEXT_LEN    2048 //2048
#include <string.h>

class TXMLExcept 
{
public:
	char pStr[512];
	TXMLExcept() { pStr[0]=0; }
	TXMLExcept(char *str) { strcpy(pStr,str); }
};

class TXMLAttr
{
public:
   TXMLAttr *brother;
   int lineNo;

   TXMLAttr();
   ~TXMLAttr();
   
   void       setName(const char* src);
   char*      getName();
   void       setValue(const char* src);
   char*      getValue();
   TXMLAttr*  dup();
   char*      toStr();
   TXMLAttr*  getBrother();

private:
   char    m_strName[ATTRNAME_LENGTH+1];
   char*   m_strValue;
};

class TXMLParse      
{                    
public:              
	int  bReadTag;     
	int  tokenType;    
	char tokenValue[MAX_TOKEN_LEN+1];  
//	char textValue[MAX_TOKEN_LEN+1];  
	int  lineNumber;   
	int  colNumber;    
	char *pCurrent;    
	char *pBuffer;     
	char *pLast;       

	char* m_pTextValue;
                     
	TXMLParse(char* s);

	~TXMLParse();
};                   
                     
class TXMLElem
{
    friend TXMLElem* ParseXML(char* s, int& packlen);

public:
   TXMLElem* child;
   TXMLElem* brother;
   TXMLElem* parent;
   TXMLAttr *attr;

   int lineNo;

   TXMLElem(TXMLParse* pParse=NULL);
   ~TXMLElem();
   
   bool Read();
   int  ReadBeginTag(); //1 include content, 0 no include
   bool ReadAttribute();
   bool ReadContent();
   bool ReadEndTag();
   int  deleteChild(TXMLElem* child);
   void addChild(TXMLElem* child,int mode=1);
   void addChild(const char* tag,const char* content);
   void addChild(const char* tag, int content);
   void addBrother(TXMLElem* elem,int mode);
   void addBrother(TXMLElem* elem);
   void addAttr(TXMLAttr* attr);
   void addAttr(const char *name,const char *value);
   void setAttr(const char *name,const char *value);

   char* GetAttrValue(const char *name);
   int GetAttrNum();
   TXMLAttr *GetAttr(const char *name);
   TXMLAttr *GetAttr(int i);
   int GetSubElemNum();
   int GetSubElemNumByTag(char* strTag);
   TXMLElem *GetSubElem(const char *tag);
   TXMLElem *GetSubElem(int i);
   int      GetSubElemName(int i,char* value);
   TXMLElem *GetSubElem(const char *tag, int i);
   TXMLElem *GetSubElem(const char *tag, const char *attrName, const char *attrValue);
   TXMLElem* GetChild();
   TXMLElem* GetBrother();
   TXMLAttr* GetAttr();

   TXMLElem* dup(TXMLElem* parent);

   void  setTag(const char*);
   char* getTag();
   void setContent(const char*);
   void setContent(const int src);
   char* getContent();
   char* toStr();

   void GetLocation(char *pCurr);
   void GetToken();
   void ASSERT_TOKEN(int x) ;
private:
   TXMLParse* m_pParse;      
   char    m_strTag[TAG_LENGTH+1];
   char*   m_strContent;
};

void XMLERROR(const char* fmt, ...);
TXMLElem* ParseXML(char*);
TXMLElem* ParseXML(char* s, int& packlen);
TXMLElem* GetXMLElem(TXMLElem* handle, char* path);
void xmlTextDecode(char*);
char* xmlTextEncode(char* xmlText);
void TrimAll(char* sour);
#endif

