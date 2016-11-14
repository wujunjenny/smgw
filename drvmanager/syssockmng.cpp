// SmsDrvManager.cpp: implementation of the CSysSockMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "SmsRecv.h"
#include "syssockmng.h"
#include "servicemng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSysSockMng::CSysSockMng()
{
	//::InitializeCriticalSection(&m_Lock);
	m_pIFMng = NULL;

}

CSysSockMng::~CSysSockMng()
{
	Destroy();
}

void CSysSockMng::SetIFManager(CServiceMng* pIFMng)
{
	m_pIFMng = pIFMng;
}

/*
BOOL CSysSockMng::Create()
{
	CreateTcpServer(1000, CODE_INNER);
	CreateTcpServer(7000, CODE_INNER);
	CreateTcpServer(5016, CODE_SMEI);	
	return TRUE;
}
*/

int CSysSockMng::OnAccept( HANDLE hDrvObject, LPCTSTR pRemoteAddress, int AddrSize, 
		DWORD dwCodeType)
{
	if (m_pIFMng)
		return m_pIFMng->OnAccept(hDrvObject, pRemoteAddress, dwCodeType);
	else
		return 1;	//表示不成功
}

int CSysSockMng::OnRead( HANDLE hIFHandle, LPVOID pBuff, int size)
{
	if (m_pIFMng)
		return m_pIFMng->OnRead(hIFHandle, (LPCTSTR)pBuff, size);
	else
		return 0;
}

int CSysSockMng::OnConnect( HANDLE hIFHandle )
{
	if (m_pIFMng)
		return m_pIFMng->OnConnect(hIFHandle);
	else
		return 0;
}

int CSysSockMng::OnError( HANDLE hIFHandle , int ErrorCode, BOOL bRecover)
{
	if (m_pIFMng)
		return m_pIFMng->OnError(hIFHandle, ErrorCode, bRecover);
	else
		return 0;
}

int CSysSockMng::OnTrace(HANDLE hIFHandle, BOOL bIn, int size, LPVOID pBuff )
{
    /*
	if (m_pIFMng)
		return m_pIFMng->OnTrace(hIFHandle, bIn, size, pBuff);
	else
		return 0;
    */
    //ASSERT(0);
    return 0;
}