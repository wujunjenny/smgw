// Pool.cpp: implementation of the CSockMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sockmsgqueue.h"
//Temp by hyh 2012-6-12
#include "smsapp.h"
#include <stdexcept>
using namespace std;
//End Temp by hyh 2012-6-12
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockMsgQueue::CSockMsgQueue(int size)
{
	//updated by hyh begin  2012-6-12
	try
	{
		m_pBuff=new char[size];
	}
	catch (CMemoryException* e)
	{
		CString log;
		log.Format("CSockMsgQueue::CSockMsgQueue CMemoryException* e 异常 ");
		GetSmsApp()->WriteTestRecord(log, 0);
		throw;
	}
	catch (CFileException* e)
	{
		CString log;
		log.Format("CSockMsgQueue::CSockMsgQueue CFileException* e 异常 ");
		GetSmsApp()->WriteTestRecord(log, 0);
		throw;
	}
	catch (CException* e)
	{
		CString log;
		log.Format("CSockMsgQueue::CSockMsgQueue CException* e 异常 ");
		GetSmsApp()->WriteTestRecord(log, 0);
		throw;
	}
	catch(...)
	{
		CString log;
		log.Format("CSockMsgQueue::CSockMsgQueue 其它异常 ");
		GetSmsApp()->WriteTestRecord(log, 0);
		throw;
	}
	//end updated by hyh 2012-6-12
	//m_pBuff=new char[size];
	m_DataSize=0;
	m_BuffSize=size;
}

CSockMsgQueue::~CSockMsgQueue()
{
	delete m_pBuff;
}

LPVOID CSockMsgQueue::GetData(int &size)
{
	size=m_DataSize;
	return m_pBuff;
}


BOOL CSockMsgQueue::OutPool(int size)
{
	if(size>m_DataSize||size<0)
    {   
        //ASSERT(0);
		return FALSE;
    }
	memmove(m_pBuff,((char*)m_pBuff)+size,m_DataSize-size);
	m_DataSize-=size;
	return TRUE;
}

//***SMGW40-04, 2004-12-19, jdz, modi begin***//
BOOL CSockMsgQueue::PutPool(LPVOID pBuff,int size, int PriFlag)
{
	if(0 == PriFlag)
	{
		if(m_DataSize+size > m_BuffSize || size<0)
		{
			//ASSERT(0);
			TRACE("Too many message to pool \n");
			return FALSE;
		}
	}
	else
	{//预留空间
		if(m_DataSize+size > ((m_BuffSize * 8)/ 10) || size<0)
		{
			//ASSERT(0);
			TRACE("Too many message to pool \n");
			return FALSE;
		}
	}
	//***SMGW40-04, 2004-12-19, jdz, modi begin***//

	//if(m_DataSize+size > m_BuffSize||size<0)
    //{
        //ASSERT(0);
    //    TRACE("Too many message to pool \n");
	//	return FALSE;
    //}

 	memcpy(((char*)m_pBuff)+m_DataSize,pBuff,size);
	m_DataSize+=size;

	return TRUE;
}

int CSockMsgQueue::GetDataSize()
{
	return m_DataSize;
}

int CSockMsgQueue::GetFreeSize()
{
	return m_BuffSize-m_DataSize;
}


void CSockMsgQueue::ClearBuffer()
{
	m_DataSize=0;
}