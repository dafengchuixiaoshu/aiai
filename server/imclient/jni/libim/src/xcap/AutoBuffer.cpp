//#include "StdAfx.h"
#include "AutoBuffer.h"
#include "BizCommon.h"
namespace util
{

CAutoBuffer::CAutoBuffer()
:m_nSize(0),
m_pData( NULL )
{

}

CAutoBuffer::CAutoBuffer( unsigned int size )
:m_nSize(0),
 m_pData( NULL )
{
	AllocateBuffer( size );
}

CAutoBuffer::CAutoBuffer( const CAutoBuffer &src )
:m_nSize(0),
m_pData(NULL)
{
	*this = src;
}


CAutoBuffer::~CAutoBuffer()
{
	FreeBuffer();
}

CAutoBuffer& CAutoBuffer::operator=( const CAutoBuffer &src )
{
	if( this != &src )
	{
		AllocateBuffer( src.m_nSize );
		if( m_nSize > 0 && m_pData != NULL )
			memcpy( m_pData , src.m_pData , m_nSize );
	}
	return *this;
}

bool CAutoBuffer::CopyBuffer( const char *pDataSrc , unsigned int nSize )
{
	if( pDataSrc == NULL ) 
	{
		FreeBuffer();
		return false;
	}
	AllocateBuffer( nSize );
	if( m_nSize > 0 && m_pData != NULL )
	{
		memcpy( m_pData , pDataSrc , m_nSize );
		return true;
	}
	return false;
}

void CAutoBuffer::AppendBuffer( const char *pDataSrc , unsigned int nSize )
{
	if( pDataSrc == NULL || nSize <= 0 )
	{
		ATLASSERT( FALSE );
		return ;
	}
	CAutoBuffer tmp = *this;
	AllocateBuffer( tmp.GetBufferSize() + nSize );
	
	memcpy( GetBuffer() , tmp.GetBuffer() , tmp.GetBufferSize() );
	memcpy( GetBuffer() + tmp.GetBufferSize(), pDataSrc ,nSize );	
}

bool CAutoBuffer::AttachBuffer( char *&pDataSrc , unsigned int nSize )
{
	FreeBuffer();
	m_pData = pDataSrc;
	m_nSize = nSize ;
	pDataSrc = NULL;
	return true;
}

std::string CAutoBuffer::ToSTLString()
{
	std::string strRet;
	if( m_nSize > 0 && m_pData != NULL )
		strRet.assign( m_pData , m_nSize );
	return strRet;
}



bool CAutoBuffer::AllocateBuffer( unsigned int nSize )
{
	if( nSize == 0 ) 
	{
		FreeBuffer();
		return false;
	}
	if( nSize == m_nSize && m_pData != NULL )
	{
		memset( m_pData , '\0' , m_nSize );
	}
	else
	{
		m_nSize = nSize;
		if( m_pData != NULL )
		{ 
			delete []m_pData;
			m_pData = NULL;
		} 
		m_pData = new char[m_nSize];
		if( m_pData == NULL )
		{	
			m_nSize = 0;
			return false;
		}
		else
		{
			memset( m_pData , '\0' , m_nSize );
		}
	}
	return true;
}

void CAutoBuffer::FreeBuffer()
{
	m_nSize = 0;
	if( m_pData != NULL )
	{
		delete []m_pData;
	}
	m_pData = NULL;
}

unsigned int CAutoBuffer::GetBufferSize() const
{ 
	return m_nSize ;
}

char* CAutoBuffer::GetBuffer() const
{
	return m_pData ;
}

const char& CAutoBuffer::operator[]( unsigned int i ) const
{
    if( m_pData == NULL )
	{
		//throw "empty pointer";
	}
    else if(  i >= m_nSize )
	{
		//throw "out of index";
	}
    return m_pData[i];
}

char& CAutoBuffer::operator[]( unsigned int i ) 
{
	const CAutoBuffer &tmp = static_cast<const CAutoBuffer&>(*this);
	return const_cast<char&>(tmp[i]);
}

void CAutoBuffer::SwapBuffer( CAutoBuffer& other )
{
	unsigned int tempSize = m_nSize;
	char* tempData = m_pData;
	m_nSize = other.m_nSize;
	m_pData = other.m_pData;
	other.m_nSize = tempSize;
	other.m_pData = tempData;
}

}
