// Pool.cpp: implementation of the CPool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPool::CPool(int size)
{
	m_pBuff=new char[size];
	m_DataSize=0;
	m_BuffSize=size;
}

CPool::~CPool()
{
	delete m_pBuff;
}

LPVOID CPool::GetData(int &size)
{
	size=m_DataSize;
	return m_pBuff;
}


BOOL CPool::OutPool(int size)
{
	if(size>m_DataSize||size<0)
    {   
        ASSERT(0);
		return FALSE;
    }
	memmove(m_pBuff,((char*)m_pBuff)+size,m_DataSize-size);
	m_DataSize-=size;
	return TRUE;
}

BOOL CPool::PutPool(LPVOID pBuff,int size)
{
	if(m_DataSize+size > m_BuffSize||size<0)
    {
 //       ASSERT(0);//不让跳出异常用返回值判断
        TRACE("Too many message to pool \n");
		return FALSE;
    }
 	memcpy(((char*)m_pBuff)+m_DataSize,pBuff,size);
	m_DataSize+=size;
	return TRUE;
}

int CPool::GetDataSize()
{
	return m_DataSize;
}

int CPool::GetFreeSize()
{
	return m_BuffSize-m_DataSize;
}


void CPool::ClearBuffer()
{
	m_DataSize=0;
}