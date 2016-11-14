// WaitAck.cpp: implementation of the CWaitResp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmsApp.h"
#include "waitresp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaitResp::CWaitResp()
{
	m_pMsg = NULL;
	m_nTimerCount = MAX_TIMER_WAIT;
    m_byWaitAck = WAIT_ACK_YES;
}

CWaitResp::~CWaitResp()
{
	if(m_pMsg != NULL)
		delete m_pMsg;
    m_pMsg = NULL;
}

BOOL CWaitResp::SaveMsg(PTRMSG pMsg)
{
	tagSmsHead* pHead = (tagSmsHead*)pMsg;
	m_pMsg = new char[pHead->nLen];
	if (m_pMsg == NULL)
	{ 
		ASSERT(0);
		return FALSE;
	};
	memcpy(m_pMsg, pMsg, pHead->nLen);
	return TRUE;
}

PTRMSG CWaitResp::GetMsg()
{
	return m_pMsg;
}

void CWaitResp::SetAck(UC byWaitAck)
{
    m_byWaitAck = byWaitAck;
}


UC CWaitResp::GetAck()
{
    return m_byWaitAck;
}

CWaitResp::CWaitResp(int nMaxWaitCount)
{
    m_pMsg = NULL;
	m_nTimerCount = nMaxWaitCount;
    m_byWaitAck = WAIT_ACK_YES;
}