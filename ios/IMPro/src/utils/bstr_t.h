#include"BizCommon.h"
#include "StdString.h"


#ifndef _BSTR_T 
#define _BSTR_T

#define _com_issue_error(a)
#define _COM_ASSERT(a)

int unicode_to_utf8(wchar_t *in, int insize, uint8_t **out);
int utf8_to_unicode(uint8_t *in, wchar_t **out, int *outsize);


inline void _COM_MEMCPY_S (void* dest, int destsize, const void* src, int count) {
	int n = count;
	if (n > destsize) {
		n = destsize;
	}
	memcpy (dest, src, n);
}

class _bstr_t {
public:
    // Constructors
    //
    _bstr_t() throw();
    _bstr_t(const _bstr_t& s) throw();
    _bstr_t(const char* s) ;
    _bstr_t(const wchar_t* s) ;
    _bstr_t(BSTR bstr, bool fCopy) ;

    // Destructor
    //
    ~_bstr_t() throw();

    // Assignment operators
    //
    _bstr_t& operator=(const _bstr_t& s) throw();
    _bstr_t& operator=(const char* s) ;
    _bstr_t& operator=(const wchar_t* s) ;

    // Operators
    //
    _bstr_t& operator+=(const _bstr_t& s) ;
    _bstr_t operator+(const _bstr_t& s) const ;

    // Friend operators
    //
    friend _bstr_t operator+(const char* s1, const _bstr_t& s2) ;
    friend _bstr_t operator+(const wchar_t* s1, const _bstr_t& s2) ;

    // Extractors
    //
    operator const wchar_t*() const throw();
    operator wchar_t*() const throw();
    operator const char*() const ;
    operator char*() const ;

    // Comparison operators
    //
    bool operator!() const throw();
    bool operator==(const _bstr_t& str) const throw();
    bool operator!=(const _bstr_t& str) const throw();
    bool operator<(const _bstr_t& str) const throw();
    bool operator>(const _bstr_t& str) const throw();
    bool operator<=(const _bstr_t& str) const throw();
    bool operator>=(const _bstr_t& str) const throw();

    // Low-level helper functions
    //
    BSTR copy(bool fCopy = true) const ;
    unsigned int length() const throw();

    // Binary string assign
    //
    void Assign(BSTR s) ;

    // Get the physical BSTR
    //
    BSTR& GetBSTR() ;
    BSTR* GetAddress() ;

    // Attach to the internal BSTR w/o copying
    //
    void Attach(BSTR s) ;

    // Detach the internal BSTR
    //
    BSTR Detach();

private:
    // Referenced counted wrapper
    //
    class Data_t {
    public:
        // Constructors
        //
        Data_t(const char* s) ;
        Data_t(const wchar_t* s) ;
        Data_t(BSTR bstr, bool fCopy) ;
        Data_t(const _bstr_t& s1, const _bstr_t& s2) ;

        // Reference counting routines
        //
        unsigned long AddRef() throw();
        unsigned long Release() throw();
        unsigned long RefCount() const throw();

        // Extractors
        //
        operator const wchar_t*() const throw();
        operator const char*() const ;

        // Low-level helper functions
        //
        const wchar_t* GetWString() const throw();
        wchar_t*& GetWString() throw();
        const char* GetString() const ;

        BSTR Copy() const ;
        void Assign(BSTR s) ;
        void Attach(BSTR s) throw();
        unsigned int Length() const throw();
        int Compare(const Data_t& str) const throw();

        // Exception agnostic wrapper for new
        //
        void* operator new(size_t sz);          

    private:
        BSTR	        m_wstr;
        mutable char*   m_str;
        unsigned long   m_RefCount;

        // Never allow default construction
        //
        Data_t() throw();

        // Never allow copy
        //
        Data_t(const Data_t& s) throw();

        // Prevent deletes from outside. Release() must be used.
        //
        ~Data_t() throw();

        void _Free() throw();

		char* ConvertBSTRToString (BSTR pSrc) const {
			int len = wcslen (pSrc);
			char* ret = NULL;
			unicode_to_utf8 (pSrc, len * sizeof (wchar_t), (uint8_t** ) &ret);
			return ret;
		}

		BSTR ConvertStringToBSTR (const char* src) const {
			BSTR bstr = NULL;
			int out_size;
			utf8_to_unicode ( (uint8_t* ) src, &bstr, &out_size);
			return bstr;
		}
    };

private:
    // Reference counted representation
    //
    Data_t* m_Data;

private:
    // Low-level utilities
    //
    void _AddRef() throw();
    void _Free() throw();
    int _Compare(const _bstr_t& str) const throw();
};

//////////////////////////////////////////////////////////////////////////////
//
// Constructors
//
//////////////////////////////////////////////////////////////////////////////

// Default constructor
//
inline _bstr_t::_bstr_t() throw()
    : m_Data(NULL)
{
}

// Copy constructor
//
inline _bstr_t::_bstr_t(const _bstr_t& s) throw()
    : m_Data(s.m_Data)
{
    _AddRef();
}

// Construct a _bstr_t from a const char*
//
inline _bstr_t::_bstr_t(const char* s) 
    : m_Data(new Data_t(s))
{
    if (m_Data == NULL) {
        _com_issue_error(E_OUTOFMEMORY);
    }
}

// Construct a _bstr_t from a const whar_t*
//
inline _bstr_t::_bstr_t(const wchar_t* s) 
    : m_Data(new Data_t(s))
{
    if (m_Data == NULL) {
        _com_issue_error(E_OUTOFMEMORY);
    }
}

// Construct a _bstr_t from a BSTR.  If fCopy is FALSE, give control of
// data to the _bstr_t without making a new copy.
//
inline _bstr_t::_bstr_t(BSTR bstr, bool fCopy) 
    : m_Data(new Data_t(bstr, fCopy))
{
    if (m_Data == NULL) {
        _com_issue_error(E_OUTOFMEMORY);
    }
}

// Destructor
//
inline _bstr_t::~_bstr_t() throw()
{
    _Free();
}

//////////////////////////////////////////////////////////////////////////////
//
// Assignment operators
//
//////////////////////////////////////////////////////////////////////////////

// Default assign operator
//
inline _bstr_t& _bstr_t::operator=(const _bstr_t& s) throw()
{
    if (this != &s) {
        _Free();

        m_Data = s.m_Data;
        _AddRef();
    }

    return *this;
}

// Assign a const char* to a _bstr_t
//
inline _bstr_t& _bstr_t::operator=(const char* s) 
{
    _COM_ASSERT(s == NULL || static_cast<const char*>(*this) != s);

    if (s == NULL || static_cast<const char*>(*this) != s)
    {
        _Free();

        m_Data = new Data_t(s);
        if (m_Data == NULL) {
            _com_issue_error(E_OUTOFMEMORY);
        }
    }

    return *this;
}

// Assign a const wchar_t* to a _bstr_t
//
inline _bstr_t& _bstr_t::operator=(const wchar_t* s) 
{
    _COM_ASSERT(s == NULL || static_cast<const wchar_t*>(*this) != s);

    if (s == NULL || static_cast<const wchar_t*>(*this) != s)
    {
        _Free();

        m_Data = new Data_t(s);
        if (m_Data == NULL) {
            _com_issue_error(E_OUTOFMEMORY);
        }
    }

    return *this;
}

//////////////////////////////////////////////////////////////////////////////
//
// Operators
//
//////////////////////////////////////////////////////////////////////////////

// Concatenate a _bstr_t onto this _bstr_t
//
inline _bstr_t& _bstr_t::operator+=(const _bstr_t& s) 
{
    Data_t* newData = new Data_t(*this, s);
    if (newData == NULL) {
        _com_issue_error(E_OUTOFMEMORY);
    }
    else {
        _Free();
        m_Data = newData;
    }

    return *this;
}

// Return the concatenation of this _bstr_t with another _bstr_t
//
inline _bstr_t _bstr_t::operator+(const _bstr_t& s) const 
{
    _bstr_t b = *this;
    b += s;

    return b;
}

//////////////////////////////////////////////////////////////////////////////
//
// Friend Operators
//
//////////////////////////////////////////////////////////////////////////////

// Return the concatenation of a const char* with a _bstr_t
//
inline _bstr_t operator+(const char* s1, const _bstr_t& s2) 
{
    _bstr_t b = s1; 
    b += s2;

    return b;
}

// Return the concatenation of a const char* with a _bstr_t
//
inline _bstr_t operator+(const wchar_t* s1, const _bstr_t& s2) 
{
    _bstr_t b = s1; 
    b += s2;

    return b;
}

//////////////////////////////////////////////////////////////////////////////
//
// Extractors
//
//////////////////////////////////////////////////////////////////////////////

// Extract a const wchar_t*
//
inline _bstr_t::operator const wchar_t*() const throw()
{
    return (m_Data != NULL) ? m_Data->GetWString() : NULL;
}

// Extract a wchar_t*
//
inline _bstr_t::operator wchar_t*() const throw()
{
    return const_cast<wchar_t*>((m_Data != NULL) ? m_Data->GetWString() : NULL);
}

// Extract a const char_t*
//
inline _bstr_t::operator const char*() const 
{
    return (m_Data != NULL) ? m_Data->GetString() : NULL;
}

// Extract a char_t*
//
inline _bstr_t::operator char*() const 
{
    return const_cast<char*>((m_Data != NULL) ? m_Data->GetString() : NULL);
}

//////////////////////////////////////////////////////////////////////////////
//
// Comparison operators
//
//////////////////////////////////////////////////////////////////////////////

inline bool _bstr_t::operator!() const throw()
{
    return (m_Data != NULL) ? !m_Data->GetWString() : true;
}

inline bool _bstr_t::operator==(const _bstr_t& str) const throw()
{
    return _Compare(str) == 0;
}

inline bool _bstr_t::operator!=(const _bstr_t& str) const throw()
{
    return _Compare(str) != 0;
}

inline bool _bstr_t::operator<(const _bstr_t& str) const throw()
{
    return _Compare(str) < 0;
}

inline bool _bstr_t::operator>(const _bstr_t& str) const throw()
{
    return _Compare(str) > 0;
}

inline bool _bstr_t::operator<=(const _bstr_t& str) const throw()
{
    return _Compare(str) <= 0;
}

inline bool _bstr_t::operator>=(const _bstr_t& str) const throw()
{
    return _Compare(str) >= 0;
}

//////////////////////////////////////////////////////////////////////////////
//
// Low-level help functions
//
//////////////////////////////////////////////////////////////////////////////

// Extract a copy of the wrapped BSTR
//
inline BSTR _bstr_t::copy(bool fCopy) const 
{
    return (m_Data != NULL) ? (fCopy ? m_Data->Copy() : m_Data->GetWString()) : NULL;
}

// Return the length of the wrapped BSTR
//
inline unsigned int _bstr_t::length() const throw()
{
    return (m_Data != NULL) ? m_Data->Length() : 0;
}

// Binary string assign
//
inline void _bstr_t::Assign(BSTR s) 
{
    _COM_ASSERT(s == NULL || m_Data == NULL || m_Data->GetWString() != s);
    
    if (s == NULL || m_Data == NULL || m_Data->GetWString() != s)
    {
        _Free();

        m_Data = new Data_t(s, TRUE);
        if (m_Data == NULL) {
            _com_issue_error(E_OUTOFMEMORY);
        }
    }
}

// Get the physical BSTR
//
inline BSTR& _bstr_t::GetBSTR()  
{
    if (m_Data == NULL) {
        m_Data = new Data_t(0, FALSE);
        if (m_Data == NULL) {
            _com_issue_error(E_OUTOFMEMORY);
        }
    }
    return m_Data->GetWString();
}

// Get the address of the physical BSTR to pass as an 'out'-parameter
//
inline BSTR* _bstr_t::GetAddress()  
{
    Attach(0);
    return &m_Data->GetWString();
}

// Attach to the internal BSTR w/o copying
//
inline void _bstr_t::Attach(BSTR s) 
{
    _Free();

    m_Data = new Data_t(s, FALSE);
    if (m_Data == NULL) {
        _com_issue_error(E_OUTOFMEMORY);
    }
}

// Detach the internal BSTR
//
inline BSTR _bstr_t::Detach()
{
    _COM_ASSERT(m_Data != NULL && m_Data->RefCount() == 1);

    if (m_Data != NULL && m_Data->RefCount() == 1) {
        BSTR b = m_Data->GetWString();
        m_Data->GetWString() = NULL;
        _Free();
        return b;
    } 
    else {
        _com_issue_error(E_POINTER);

		return NULL;
    }
}

// AddRef the BSTR
//
inline void _bstr_t::_AddRef() throw()
{
    if (m_Data != NULL) {
        m_Data->AddRef();
    }
}

// Free the BSTR
//
inline void _bstr_t::_Free() throw()
{
    if (m_Data != NULL) {
        m_Data->Release();
        m_Data = NULL;
    }
}

// Compare two _bstr_t objects
//
inline int _bstr_t::_Compare(const _bstr_t& str) const throw()
{
    if (m_Data == str.m_Data) {
        return 0;
    }

    if (m_Data == NULL) {
        return -1;
    }

    if (str.m_Data == NULL) {
        return 1;
    }

    return m_Data->Compare(*str.m_Data);
}

//////////////////////////////////////////////////////////////////////////////
//
// Reference counted wrapper - Constructors
//
//////////////////////////////////////////////////////////////////////////////

// Construct a Data_t from a const char*
//
inline _bstr_t::Data_t::Data_t(const char* s) 
    : m_str(NULL), m_RefCount(1)
{
    m_wstr = ConvertStringToBSTR(s);
}

// Construct a Data_t from a const wchar_t*
//
inline _bstr_t::Data_t::Data_t(const wchar_t* s) 
    : m_str(NULL), m_RefCount(1)
{
    m_wstr = SysAllocString(s);

    if (m_wstr == NULL && s != NULL) {
        _com_issue_error(E_OUTOFMEMORY);
    }
}

// Construct a Data_t from a BSTR.  If fCopy is FALSE, give control of
// data to the Data_t without doing a SysAllocStringByteLen.
//
inline _bstr_t::Data_t::Data_t(BSTR bstr, bool fCopy) 
    : m_str(NULL), m_RefCount(1)
{
    if (fCopy && bstr != NULL) {
        m_wstr = ::SysAllocString(bstr);

        if (m_wstr == NULL) {
            _com_issue_error(E_OUTOFMEMORY);
        }
    }
    else {
        m_wstr = bstr;
    }
}

// Construct a Data_t from the concatenation of two _bstr_t objects
//
inline _bstr_t::Data_t::Data_t(const _bstr_t& s1, const _bstr_t& s2) 
    : m_str(NULL), m_RefCount(1)
{
    const unsigned int l1 = s1.length();
    const unsigned int l2 = s2.length();

    m_wstr = ::SysAllocStringByteLen(NULL, (l1 + l2) * sizeof(wchar_t));
    if (m_wstr == NULL) {
        _com_issue_error(E_OUTOFMEMORY);
        return;
    }

    const wchar_t* wstr1 = static_cast<const wchar_t*>(s1);

    if (wstr1 != NULL) {
        _COM_MEMCPY_S(m_wstr, (l1 + l2 + 1) * sizeof(wchar_t), wstr1, (l1 + 1) * sizeof(wchar_t));
    }

    const wchar_t* wstr2 = static_cast<const wchar_t*>(s2);

    if (wstr2 != NULL) {
        _COM_MEMCPY_S(m_wstr + l1, (l2 + 1) * sizeof(wchar_t), wstr2, (l2 + 1) * sizeof(wchar_t));
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Reference counted wrapper - reference counting routines
//
//////////////////////////////////////////////////////////////////////////////

inline unsigned long _bstr_t::Data_t::AddRef() throw()
{
    ++m_RefCount;
    return m_RefCount;
}

inline unsigned long _bstr_t::Data_t::Release() throw()
{
    unsigned long cRef = --m_RefCount;
    if (cRef == 0) {
        delete this;
    }

    return cRef;
}

inline unsigned long _bstr_t::Data_t::RefCount() const throw()
{
    return m_RefCount;
}

//////////////////////////////////////////////////////////////////////////////
//
// Reference counted wrapper - extractors
//
//////////////////////////////////////////////////////////////////////////////

// Extract a const wchar_t*
//
inline _bstr_t::Data_t::operator const wchar_t*() const throw()
{
    return m_wstr;
}

// Extract a const char_t*
//
inline _bstr_t::Data_t::operator const char*() const 
{
    return GetString();
}

//////////////////////////////////////////////////////////////////////////////
//
// Reference counted wrapper - helper functions
//
//////////////////////////////////////////////////////////////////////////////

inline const wchar_t* _bstr_t::Data_t::GetWString() const throw()
{
    return m_wstr;
}

inline wchar_t*& _bstr_t::Data_t::GetWString() throw()
{
    return m_wstr;
}

inline const char* _bstr_t::Data_t::GetString() const 
{
    if (m_str == NULL) {
        m_str = ConvertBSTRToString(m_wstr);
    }

    return m_str;
}

// Return a copy of the wrapped BSTR
//
inline BSTR _bstr_t::Data_t::Copy() const 
{
    if (m_wstr != NULL) {
        BSTR bstr = SysAllocStringByteLen(m_wstr, ::SysStringByteLen(m_wstr));

        if (bstr == NULL) {
            _com_issue_error(E_OUTOFMEMORY);
        }

        return bstr;
    }

    return NULL;
}

inline void _bstr_t::Data_t::Assign(BSTR s) 
{
    _Free();

    if (s != NULL) {
        m_wstr = ::SysAllocStringByteLen(s, ::SysStringByteLen(s));
        m_str = 0;
    }
}

inline void _bstr_t::Data_t::Attach(BSTR s) throw()
{
    _Free();

    m_wstr = s;
    m_str = 0;
    m_RefCount = 1;
}

// Return the length of the wrapper BSTR
//
inline unsigned int _bstr_t::Data_t::Length() const throw()
{
    return m_wstr ? ::SysStringLen(m_wstr) : 0;
}

// Compare two wrapped BSTRs
//
inline int _bstr_t::Data_t::Compare(const _bstr_t::Data_t& str) const throw()
{
    if (m_wstr == NULL) {
        return str.m_wstr ? -1 : 0;
    }

    if (str.m_wstr == NULL) {
        return 1;
    }

    const unsigned int l1 = ::SysStringLen(m_wstr);
    const unsigned int l2 = ::SysStringLen(str.m_wstr);

    unsigned int len = l1;
    if (len > l2) {
        len = l2;
    }

    BSTR bstr1 = m_wstr;
    BSTR bstr2 = str.m_wstr;

    while (len-- > 0) {
        if (*bstr1++ != *bstr2++) {
            return bstr1[-1] - bstr2[-1];
        }
    }

    return (l1 < l2) ? -1 : (l1 == l2) ? 0 : 1;
}

// Exception agnostic wrapper for new
//
#ifdef _COM_OPERATOR_NEW_THROWS
inline void* _bstr_t::Data_t::operator new(size_t sz) 
{
    //try {
        return ::operator new(sz);
    //}
    //catch (...) {
    //    return NULL;
    //}
}
#else // _COM_OPERATOR_NEW_THROWS
inline void* _bstr_t::Data_t::operator new(size_t sz) 
{
    return ::operator new(sz);
}
#endif // _COM_OPERATOR_NEW_THROWS

// Destruct this object
//
inline _bstr_t::Data_t::~Data_t() throw()
{
    _Free();
}

// Free up this object
//
inline void _bstr_t::Data_t::_Free() throw()
{
    if (m_wstr != NULL) {
        ::SysFreeString(m_wstr);
    }

    if (m_str != NULL) {
        delete [] m_str;
    }
}

#endif
