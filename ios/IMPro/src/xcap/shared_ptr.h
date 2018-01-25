#pragma once
#include "BizCommon.h"

//////////////////////////////////////////////////////////////////////////
///
///	@file		shared_ptr.h
///	
///	@brief		�򻯵����ü���������ָ��
///	
///	@author		yangchuanchuan 
///		
///	@version    version:	
///	
///	@data		2010/11/10
///	
//////////////////////////////////////////////////////////////////////////
namespace util
{

//namespace detail_shared
//{
//
//class shared_count_base
//{
//public:
//	shared_count_base()
//	
//
//};

template<typename T>
class shared_count
{
public:
	shared_count():m_refCount(1),m_ptr( NULL )
	{
	}
	shared_count( T *ptr ):m_refCount(1),m_ptr( ptr )
	{
	}
	
protected:
	~shared_count()
	{
		if( m_ptr != NULL )
		{
			delete m_ptr;
		}
	}

public:
	void release()
	{
	    ATLASSERT( InterlockedExchangeAdd( &m_refCount, 0) > 0 );
        if( (--m_refCount) == 0 )
        {
            delete this;
        }
	}

	void addref()
	{
		ATLASSERT( InterlockedExchangeAdd( &m_refCount, 0 ) >= 0 );
		++m_refCount;
	}
	
	long getref()
	{
		return m_refCount;
	}

private:
	volatile long m_refCount;
    shared_count( shared_count const & );
    shared_count & operator= ( shared_count const & );

public:
	T* m_ptr;
};


//}

template< typename T > class shared_ptr
{
public:
    typedef T& reference;
    typedef shared_ptr<T> this_type;

public:
	shared_ptr():m_ptr( NULL ) , m_pc( NULL )
	{
	
	}
	/*explicit */shared_ptr( T *ptr ):m_ptr( ptr ), m_pc( NULL )
	{
		//ATLASSERT( ptr != NULL );
		m_pc = new shared_count<T>( ptr );
		//ATLASSERT( m_pc != NULL );
	}
	
	/*template<class T>*/
    shared_ptr( shared_ptr const &r ): m_ptr( r.m_ptr ), m_pc( r.m_pc )
    {
		if( m_pc )
		{
			m_pc->m_ptr = m_ptr;
			m_pc->addref();
		}
    }

	template<class Other>
    shared_ptr( shared_ptr< Other > /*const*/ &r )//: m_ptr( r.m_ptr ), m_pc( r.m_pc )
    {
		m_ptr = static_cast< T* >(r.m_ptr);
		m_pc = reinterpret_cast< shared_count<T> * >(r.m_pc);
		if( m_pc )
		{
			m_pc->m_ptr = m_ptr;
			m_pc->addref();
		}
    }

	template<class Other>
    shared_ptr& operator=( shared_ptr<Other> const &r )
    {
		unsigned char* pThis = (unsigned char*)(this);
		unsigned char* pOther = (unsigned char*)(&r);
		if( pThis != pOther )
		{
			if( m_pc ) m_pc->release();
			m_ptr = static_cast< T* >(r.m_ptr);
			m_pc = reinterpret_cast< shared_count<T> * >(r.m_pc);
			if( m_pc )
			{
				m_pc->m_ptr = m_ptr;
				m_pc->addref();
			}
		}
		return *this;
    }


    shared_ptr& operator=( shared_ptr const &r )
    {
		if( this != &r )
		{
			if( m_pc ) m_pc->release();
			m_ptr = r.m_ptr ;
			m_pc = r.m_pc; 
			if( m_pc )
			{
				m_pc->m_ptr = m_ptr;
				m_pc->addref();
			}
		}
		return *this;
    }


	~shared_ptr()
	{
		if( m_pc )
		{
			m_pc->release();
		}
	}
public:
	long use_count()
	{
		long ret = 0;
		if( m_pc )
		{
			ret = m_pc->getref();
		}
		return ret;
	}

public:
	void reset()
	{
		this_type().swap( *this );
	}

    template<class Y> void reset(Y * p)		
    {
        ATLASSERT( p == 0 || p != m_ptr );	
        this_type(p).swap(*this);
    }

public:
	T * get() const
	{
//		ATLASSERT( m_ptr != NULL );
		return m_ptr;
	}
	T * operator-> () const
	{
		ATLASSERT( m_ptr != NULL );
		return m_ptr;
	}

	T& operator* () const
	{
		ATLASSERT( m_ptr != NULL );
		return *m_ptr;
	}

private:
    void swap( shared_ptr<T> & other )
    {
        std::swap( m_ptr, other.m_ptr );
		std::swap( m_pc, other.m_pc );
    }


public:
	T*	m_ptr;	
	shared_count<T> *m_pc;
};

template<class T, class U> inline bool operator==(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() != b.get();
}

template<class T, class U> inline bool operator<(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() < b.get();
}

void TestSharePtr();

}
