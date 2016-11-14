// ICDComDrv.cpp: implementation of the CICDComDrv class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#ifndef _NO_ICDCOMM_
#include "ICDComDrv.h"
#include "fep.h"
#include "sockmng.h"

#ifndef __public_drvmanager__
#include "GatewayIF.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CICDComDrv::CICDComDrv(CSockMng* pParent, UL ProcID)
{
	::InitializeCriticalSection(&m_Critical);
	m_ProcID = ProcID;
	m_pParent = pParent;
	TcpInit(ProcID);
	Start();
	
}

CICDComDrv::~CICDComDrv()
{
	Stop();

	TcpExit();
	::DeleteCriticalSection(&m_Critical);
}

int CICDComDrv::Run(DWORD param)
{
	DWORD size=MAX_ICD_MSG_SIZE;
	DWORD ip;
	DWORD pid;

	while(!IsExit())
	{
		if(TcpRecvExt(&ip, &pid, m_ProcID, m_rcvbuff, &size) == NULL)
		{

#ifdef _debug_lock_
			TRACE("ICDCOM recv ENTER lock.\n");
#endif

			::EnterCriticalSection(&m_Critical);
			m_pParent->OnICDRead(ip, pid, m_ProcID, m_rcvbuff, size);
			::LeaveCriticalSection(&m_Critical);

#ifdef _debug_lock_
			TRACE("ICDCOM recv LEAVE lock.\n");
#endif
		}
	}

	return 0;
}

int CICDComDrv::Send(DWORD dwRemoteIP, DWORD dwPID, LPVOID pBuff, int size)
{
	int iRet;
	iRet = TcpSendExt(m_ProcID, dwRemoteIP, dwPID, (char*)pBuff, size,0);
	return TRUE;
}

void CICDComDrv::Stop()
{
	SetEvent(m_hEvent);

	Sleep(10);
	FEP_LOGIN_ACK ack;
	ack.SendID=1;
	ack.RcvMID=0;
	ack.RcvPID=0;
	ack.MsgLen=3;
	ack.MsgType=MSG_FEP_LOGIN_ACK;
	char name[100];
	gethostname(name,100);
	HOSTENT* phost=gethostbyname(name);

    DWORD * pIPAddr = (DWORD*)(phost->h_addr_list[0]);
    int i = 0;
    DWORD ip;
    while(pIPAddr)
    {
        ip = *pIPAddr;
        pIPAddr = (DWORD*)(phost->h_addr_list[++i]);
        TcpSendExt(m_ProcID, ip, m_ProcID, (char*)&ack, sizeof(ack), 0);
    }
	
	if (WaitForSingleObject(m_hThread, 10000) != WAIT_OBJECT_0)
    {
        ASSERT(0);
        TerminateThread(m_hThread, 1);
    }
}

#endif