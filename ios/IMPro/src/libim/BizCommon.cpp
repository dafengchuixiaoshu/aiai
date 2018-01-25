#include "BizCommon.h"
#include "CWorkContact.h"
#include "CWorkGroup.h"
#include "iXCapInterface.h"

CXCapImpl* CXCapImpl::g_pXCapImpl = NULL;

///< 对itoa函数重写
#define itoa(n,buf,radix)	my_itoa(n, buf, radix)
inline char* my_itoa(int val, char* buf, unsigned int radix) {
	char* p;
	unsigned int a; //every digit
//	int len;
	char* b; //start of the digit char
	char temp;
	unsigned int u;
	p = buf;
	if (val < 0)  {
		*p++ = '-';
		val = 0 - val;
	}
	u = (unsigned int)val;
	b = p;
	do {
		a = u % radix;
		u /= radix;
		*p++ = a + '0';
	} while (u > 0);

//	len = (int)(p - buf);
	*p-- = 0;
	//swap
	do {
		temp = *p;
		*p = *b;
		*b = temp;
		--p;
		++b;
	} while (b < p);
	return buf;
}



///< 对itoa函数重写
#define itow(n,buf,radix)	my_itow(n, buf, radix)
wchar_t* my_itow(int val, wchar_t* buf, unsigned int radix);

int my_wtoi (wchar_t* num) {
	int n = 0;
	int i = 0;
	while (num [i] != '\0') {
		if (num [i] < '0' || num [i] > '9') {
			return n;
		}

		n *= 10;
		n += (num [i] - '0');
		i++;
	}

	return n;
}

wchar_t* my_itow(int val, wchar_t* buf, unsigned int radix) {
	wchar_t* p;
	unsigned int a; //every digit
//	int len;
	wchar_t* b; //start of the digit char
	wchar_t temp;
	unsigned int u;

	p = buf;
	if (val < 0)  {
		*p++ = '-';
		val = 0 - val;
	}
	u = (unsigned int)val;
	b = p;
	do {
		a = u % radix;
		u /= radix;
		*p++ = a + '0';
	} while (u > 0);

//	len = (int)(p - buf);
	*p-- = 0;
	//swap
	do {
		temp = *p;
		*p = *b;
		*b = temp;
		--p;
		++b;
	} while (b < p);
	return buf;
}

//字符串分割函数
vector<string> split(string str, string pattern)
 {
	string::size_type pos;
	vector<string> result;
	str += pattern; //扩展字符串以方便操作
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if ((int)pos < size)
		{
			string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

bool is_dir_exist (const char* dir_name)
{
	DIR* dir = opendir (dir_name);
	if (dir == NULL)
	{
		return false;
	}
	else
	{
		closedir (dir);
		return true;
	}
}

bool ensure_path_exist (const char* dir_name)
{
	if (!is_dir_exist (dir_name))
	{
		if (mkdir (dir_name, 0755) != 0)
		{
			LOGD ("create file path failed: %s\n", dir_name);
			return false;
		}
	}
	return true;
}

bool file_exist (const char* path)
{
	FILE* fp = fopen (path, "r");
	if (fp == NULL)
	{
		return false;
	}
	fclose (fp);
	return true;
}

string GetUserDir (string& strRootDir, string& id, bool bCreate)
{
	string root = strRootDir;
	ensure_path_exist (root.c_str());
	root += USER_DIR;
	ensure_path_exist (root.c_str());
	root += "/" + id;
	if (bCreate)
	{
		ensure_path_exist (root.c_str());
	}
	return root + "/";
}

string GetConfigDir (string& strRootDir)
{
	string root = strRootDir;
	ensure_path_exist (root.c_str());
	root += CONFIG_DIR;
	ensure_path_exist (root.c_str());
	return root + "/";
}

string GetCrashDir (string& strRootDir)
{
	string root = strRootDir;
	ensure_path_exist (root.c_str());
	root += CRASH_DIR;
	ensure_path_exist (root.c_str());
	return root + "/";
}

string GetLogDir (string& strRootDir)
{
	string root = strRootDir;
	ensure_path_exist (root.c_str());
	root += LOG_DIR;
	ensure_path_exist (root.c_str());
	return root + "/";
}

string GetDBDir (string& strRootDir, string& strUserId)
{
	string root = strRootDir;
	ensure_path_exist (root.c_str());
	root += DATABASE_DIR;
	ensure_path_exist (root.c_str());
	root += "/" + strUserId;
	ensure_path_exist (root.c_str());
	return root + "/";
}

string GetRootDir ()
{
	string root;
	char *buffer;
	if((buffer = getcwd(NULL, 0)) == NULL)
	{
		LOGD ("getcwd error");
		return "";
	}
	else
	{
//		LOGD ("%s\n", buffer);
		root = buffer;
		free(buffer);
	}
	return root + "/";
}

string FromInt (int n)
{
	char tc[21];
	return itoa (n, tc, 10);
}

string RandomString () {
	///< 用时间做种子并不安全，加一个静态值
	static DWORD dwInc = 0;

	string strFormat;
	const int SIZE_CHAR = 15; //生成16个字符的字符串
	const char CCH[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	srand((unsigned)time(NULL));
	srand (rand () + (dwInc++ ));
//	char ch[SIZE_CHAR + 1] = {0};
	for (int i = 0; i < SIZE_CHAR; i++)
	{
		//modify by ganenping -1 改为-2，因为CCH大小为63，最后一个字符为'\0'
		//int x = rand() / (RAND_MAX / (sizeof(CCH) - 1));
		int x = rand() / (RAND_MAX / (sizeof(CCH) - 2));
		strFormat = strFormat + CCH[x];
	}
	return strFormat;
}

string RandomDigitString ()
{
	struct timeval t;
	memset (&t, 0, sizeof (t));
	gettimeofday (&t, NULL);
//	unsigned long n = t.tv_usec;	///< 取毫秒
	srand(t.tv_usec);
	int n = rand ();
	string strFormat = StringFormat ("%lu%lu%u", t.tv_sec, t.tv_usec, n);
	return strFormat;
}

int wcslen2 (const wchar_t* s) {
	int n = 0;
	while (*s != 0) {
		n++;
		s++;
	}
	return n;
}

int wcscmp2 (const wchar_t* s1, const wchar_t* s2) {
	while (*s1 != 0
		&& *s2 != 0
		&& *s1 == *s2) {
			s1 ++;
			s2 ++;
	}

	return *s1 - *s2;
}

int unicode_to_utf8(wchar_t *in, int insize, uint8_t **out)
{
    int i = 0;
    int outsize = 0;
    int charscount = 0;
    uint8_t *result = NULL;
    uint8_t *tmp = NULL;

    charscount = insize / sizeof(wchar_t);
    result = (uint8_t *)malloc(charscount * 3 + 1);
    memset(result, 0, charscount * 3 + 1);
    tmp = result;

    for (i = 0; i < charscount; i++)
    {
        unsigned short unicode = in[i];

        if (unicode >= 0x0000 && unicode <= 0x007f)
        {
            *tmp = (uint8_t)unicode;
            tmp += 1;
            outsize += 1;
        }
        else if (unicode >= 0x0080 && unicode <= 0x07ff)
        {
            *tmp = 0xc0 | (unicode >> 6);
            tmp += 1;
            *tmp = 0x80 | (unicode & (0xff >> 2));
            tmp += 1;
            outsize += 2;
        }
        else if (unicode >= 0x0800 && unicode <= 0xffff)
        {
            *tmp = 0xe0 | (unicode >> 12);
            tmp += 1;
            *tmp = 0x80 | (unicode >> 6 & 0x003f);
            tmp += 1;
            *tmp = 0x80 | (unicode & (0xff >> 2));
            tmp += 1;
            outsize += 3;
        }

    }

    *tmp = '\0';
    *out = result;
    return 0;
}

int utf8_to_unicode(uint8_t *in, wchar_t **out, int *outsize)
{
    uint8_t *p = in;
    wchar_t *result = NULL;
    int resultsize = 0;
    uint8_t *tmp = NULL;

    result = (wchar_t *)malloc(strlen( (const char* ) in) * 4 + 4); /* should be enough */
    memset(result, 0, strlen( (const char* ) in) * 4 + 4);
    tmp = (uint8_t *)result;

    while(*p)
    {
        if (*p >= 0x00 && *p <= 0x7f)
        {
            *tmp = *p;
            tmp++;
			memset (tmp, 0, 3);		///< linux下的四字节宽字符，不适用用windows
            resultsize += 4;
			tmp += 3;
        }
        else if ((*p & (0xff << 5))== 0xc0)
        {
//            wchar_t t = 0;
            uint8_t t1 = 0;
            uint8_t t2 = 0;

            t1 = *p & (0xff >> 3);
            p++;
            t2 = *p & (0xff >> 2);

            *tmp = t2 | ((t1 & (0xff >> 6)) << 6);//t1 >> 2;
            tmp++;

            *tmp = t1 >> 2;//t2 | ((t1 & (0xff >> 6)) << 6);
            tmp++;

			memset (tmp, 0, 2);
            resultsize += 4;
			tmp += 2;
        }
        else if ((*p & (0xff << 4))== 0xe0)
        {
//            wchar_t t = 0;
            uint8_t t1 = 0;
            uint8_t t2 = 0;
            uint8_t t3 = 0;

            t1 = *p & (0xff >> 3);
            p++;
            t2 = *p & (0xff >> 2);
            p++;
            t3 = *p & (0xff >> 2);

            //Little Endian
            *tmp = ((t2 & (0xff >> 6)) << 6) | t3;//(t1 << 4) | (t2 >> 2);
            tmp++;

            *tmp = (t1 << 4) | (t2 >> 2);//((t2 & (0xff >> 6)) << 6) | t3;
            tmp++;
			memset (tmp, 0, 2);
            resultsize += 4;
			tmp += 2;
        }

        p++;
    }

	memset (tmp, 0, 4);
    resultsize += 4;

    *out = result;
    *outsize = resultsize;
    return 0;
}

void wcscpy2 (wchar_t* dst,const wchar_t* src) {
	while (*src != 0) {
		*dst = *src;
		dst ++;
		src ++;
	}
	*dst = 0;
}

// 产生格式 2011-03-18T12:16:49-08:00
string MkIntDateTime()
{
	string strDateTime;
	time_t timep;
	struct tm pt;
	time (&timep);
	struct tm* p = localtime_r(&timep,&pt);

	// 获取时区
	int time_zone_diff_hours = p->tm_gmtoff / 3600;

	char pBuf[100] = { '\0' };
	strftime(pBuf, 99, "%Y-%m-%dT%H:%M:%S", p);
	if (time_zone_diff_hours < 0)
	{
		strDateTime = StringFormat("%s-%02d:00", pBuf, abs(time_zone_diff_hours));
	}
	else
	{
		strDateTime = StringFormat("%s+%02d:00", pBuf, time_zone_diff_hours);
	}
	return strDateTime;
}

// 产生格式 2011-03-18T12-16-49-08-00
string MkIntDateTime2()
{
	string strDateTime;
	time_t timep;
	struct tm* p;
	time (&timep);
	p = localtime (&timep);

	// 获取时区
	int time_zone_diff_hours = p->tm_gmtoff / 3600;

	char pBuf[100] = { '\0' };
	strftime(pBuf, 99, _T("%Y-%m-%dT%H-%M-%S"), p);
	if (time_zone_diff_hours < 0)
	{
		strDateTime = StringFormat("%s-%02d-00", pBuf, abs(time_zone_diff_hours));
	}
	else
	{
		strDateTime = StringFormat("%s+%02d-00", pBuf, time_zone_diff_hours);
	}
	return strDateTime;
}

// 产生格式 140723168679
string MkIntDateTimeLong()
{
	struct timeval t = {0};
	gettimeofday (&t, NULL);
	unsigned long long sec = t.tv_sec;
	unsigned long long usec = t.tv_usec;
	unsigned long long n = sec * 1000 + usec / 1000;	///< 取毫秒

	char s[64];
	snprintf (s, 63, "%llu", n);
	string str = s;
	return str;
}

string DatetimeStrToLong(const string& strTime)
{
	if (strTime.empty())
	{
		return "";
	}
	tm tm_;
	int year = 0, month = 0, day = 0, hour = 0, minute = 0,second = 0;
	sscanf (strTime.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	tm_.tm_year  = year - 1900;
	tm_.tm_mon   = month - 1;
	tm_.tm_mday  = day;
	tm_.tm_hour  = hour;
	tm_.tm_min   = minute;
	tm_.tm_sec   = second;
	tm_.tm_isdst = 0;

	time_t t_ = mktime(&tm_); //已经减了8个时区

	unsigned long long sec = t_;
	unsigned long long usec = 0;
	unsigned long long n = sec * 1000 + usec / 1000;	///< 取毫秒

	char s[64];
	snprintf (s, 63, "%llu", n);
	string str = s;
	return str;
}

void replace_separator2(string& instr, string srcdelim, string destdelim)
{
    size_t srcdelimsize = srcdelim.size();

    size_t destdelimsize = destdelim.size();

    string::size_type pos = instr.find(srcdelim);

    while(string::npos != pos)
    {
        string substr = instr.substr(pos + srcdelimsize);

        instr.erase(pos);

        instr += destdelim;

        instr += substr;

        pos = instr.find(srcdelim, pos + destdelimsize);
    }
}


extern int base64_encode(char* &outbuf, const char* inbuf, int inlen);
extern int base64_decode(char* &outbuf, const char* inbuf, int inlen);

string Base64Encode(const string& source)
{
	char * outbuf = NULL;
	if (base64_encode(outbuf, source.c_str(), source.length()) > 0 && outbuf != NULL)
	{
		string outstr = outbuf;
		//free(outbuf);
		return outstr;
	}
	//free(outbuf);
	return "";
}

string Base64Decode(const string& source)
{
	char * outbuf = NULL;
	if (base64_decode(outbuf, source.c_str(), source.length()) > 0 && outbuf != NULL)
	{
		string outstr = outbuf;
		//free(outbuf);
		return outstr;
	}
	//free(outbuf);
	return "";
}

MessageType String2Type (const string& type)
{
	enum MessageType eMessageType = MT_PERSONAL;
	if (type == "chat")
	{
		eMessageType = MT_PERSONAL;
	}
	else if (type == "groupchat")
	{
		eMessageType = MT_GROUP;
	}
//	else if (type == "customchat")
//	{
//		eMessageType = MT_CUSTOM;
//	}
//	else if (type == "maishouchat")
//	{
//		eMessageType = MT_MAISHOU;
//	}

	return eMessageType;
}

CategoryType String2Category (const string& category)
{
	enum CategoryType eCategoryType = CT_PLAIN_TEXT;
	if (category == "plain-text")
	{
		eCategoryType = CT_PLAIN_TEXT;
	}
	else if (category == "rich-text")
	{
		eCategoryType = CT_RICH_TEXT;
	}
	else if (category == "image")
	{
		eCategoryType = CT_IMAGE;
	}
	else if (category == "audio")
	{
		eCategoryType = CT_AUDIO;
	}
	else if (category == "video")
	{
		eCategoryType = CT_VIDEO;
	}

	return eCategoryType;
}

OnlineStatus String2OnlineStatus (const string& offline)
{
	enum OnlineStatus eOnlineStatus = OS_ONLINE;

	if (offline.empty() == FALSE && offline == "1")
	{
		eOnlineStatus = OS_OFFLINE;
	}

	return eOnlineStatus;
}

string Type2String (MessageType eType)
{
	string type       = "chat";
	switch (eType)
	{
	case MT_PERSONAL:
		type = "chat";
		break;

	case MT_GROUP:
		type = "groupchat";
		break;

//	case MT_CUSTOM:
//		type = "customchat";
//		break;
//
//	case MT_MAISHOU:
//		type = "maishouchat";
//		break;
	}

	return type;
}

string Category2String (CategoryType eCategory)
{
	string category   = "plain-text";
	switch (eCategory)
	{
	case CT_PLAIN_TEXT:
		category = "plain-text";
		break;

	case CT_RICH_TEXT:
		category = "rich-text";
		break;

	case CT_IMAGE:
		category = "image";
		break;

	case CT_AUDIO:
		category = "audio";
		break;

	case CT_VIDEO:
		category = "video";
		break;
	}

	return category;
}

string StringFormat (const string fmt, ...)
{
	int size = ((int) fmt.size()) * 2 + 50; // use a rubric appropriate for your code
	string str;
	va_list ap;
	while (1)
	{
		// maximum 2 passes on a POSIX system...
		str.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char *) str.data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size)
		{
			// everything worked
			str.resize(n);
			return str;
		}
		if (n > -1)  // needed size returned
			size = n + 1;   // for null char
		else
			size *= 2;      // guess at a larger size (o/s specific)
	}
	return str;
}

BOOL IfContactOrGroupExist(enum MessageType eMessageType, const string& pUserIdOrGroupId)
{
	switch (eMessageType)
	{
	case MT_PERSONAL:
	//case MT_CUSTOM:
		// 发送时：
		// Sender=小号
		// Receiver=商品ID
		// 接收时：
		// to=小号
		// from=商品ID
	//case MT_MAISHOU:
		{
			// judge receiver is in the contact list or not
			CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
			const string pUserId = pUserIdOrGroupId;
			BOOL bResult = pWorkContact->DB_IsContactExist(pUserId);
			if (bResult == FALSE)
			{
				LOGD ("UserId %s is not in the contact list\r\n", pUserId.c_str());
				pWorkContact->LoadContact(pUserId);
			}
			return TRUE;
		}
		break;

	case MT_GROUP:
		{
			// judge receiver is in the group list or not
			CWorkGroup*      pWorkGroup      = CWorkGroup::GetInstance();
			const string pGroupId = pUserIdOrGroupId;
			BOOL bResult = pWorkGroup->IsGroupExist(pGroupId);
			if (bResult == FALSE)
			{
				LOGD ("GroupId %s is not in the group list\r\n", pGroupId.c_str());
				pWorkGroup->LoadGroupList(TRUE);
			}
			return bResult;
		}
		break;
	}
}

map<string, string> RetrieveContactOrGroup (enum MessageType eMessageType, const string& pUserIdOrGroupId)
{
	map<string, string> mapData;

	// session name and portrait key need to be set data by message type
	switch (eMessageType)
	{
	case MT_PERSONAL:
	//case MT_CUSTOM:
	//case MT_MAISHOU:
		{
			CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
			const string pUserId = pUserIdOrGroupId;
			BuddyParams pBuddyParams = pWorkContact->GetContact(pUserId, FALSE);
			mapData["SessionName"] = pBuddyParams.pUserName;
			mapData["PortraitKey"] = pBuddyParams.pPortraitKey;
		}
		break;

	case MT_GROUP:
		{
			CWorkGroup*      pWorkGroup      = CWorkGroup::GetInstance();
			const string pGroupId = pUserIdOrGroupId;
			GroupParams groupParams = pWorkGroup->GetGroupInfo(pGroupId);
			mapData["SessionName"] = groupParams.GroupName;
			mapData["PortraitKey"] = groupParams.GroupPortraitKey;
		}
		break;
	}

	return mapData;
}

map<string, string> RetrieveContactOrMember (enum MessageType eMessageType,
		const string& pUserId, const string& pGroupId)
{
	map<string, string> mapData;

	// session name and portrait key need to be set data by message type
	switch (eMessageType)
	{
	case MT_PERSONAL:
	//case MT_CUSTOM:
	//case MT_MAISHOU:
		{
			CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
			BuddyParams pBuddyParams = pWorkContact->GetContact(pUserId, FALSE);
			mapData["SenderName"] = pBuddyParams.pUserName;
			mapData["SenderKey"]  = pBuddyParams.pPortraitKey;
		}
		break;

	case MT_GROUP:
		{
			CWorkContact*      pWorkContact      = CWorkContact::GetInstance();
//			HRESULT hResult = pWorkContact->DB_IsMemberExist(pGroupId, pUserId);
//			if (hResult == FALSE)
//			{
//				LOGD ("UserId %s is not in the member list\r\n", pUserId.c_str());
//			}
			MemberParams pMemberParams = pWorkContact->GetMember(pGroupId, pUserId, FALSE);
			mapData["SenderName"] = pMemberParams.MemberCardName;
			mapData["SenderKey"]  = pMemberParams.PortraitKey;
		}
		break;
	}

	return mapData;
}
