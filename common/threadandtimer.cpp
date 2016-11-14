///////////////////////////////////////////////////////////////////////////////////
//	此文件实现一个增强型的虚拟任务管理员，除了支持普通消息外，另外还支持定时器
#include "stdafx.h"
#include "threadandtimer.h"
#include "timermng.h"

//////////////////////////////////////////////////////////////////////////////
//	class CThreadAndTimer 
CThreadAndTimer::CThreadAndTimer(UINT nTimerCount)
{
	m_pTM = new CTimerMng(nTimerCount);
}

CThreadAndTimer::~CThreadAndTimer()
{
}

void CThreadAndTimer::Destroy()
{
    delete m_pTM;
	m_pTM = NULL;

	CThreadMng::Destroy();
}

DWORD CThreadAndTimer::SetTimer(DWORD dwMilliSeconds, WORD wTimes, DWORD dwPara)
	// 返回值为时钟的IDentify, 每一个时钟都有一个唯一值
{
	ASSERT(m_pTM);
	if(m_pTM)
		return m_pTM->SetTimer(dwMilliSeconds, (DWORD)this, wTimes, dwPara, OnTimmerTrigger);
	return 0xffffffff;
}

void CThreadAndTimer::KillTimer(DWORD nTimerID)
{
	ASSERT(m_pTM);
	if(m_pTM)
		m_pTM->KillTimer(nTimerID);
}

void CThreadAndTimer::SendMessage(PTRMSG pMsg, DWORD nMsgLen)
{
	PTRMSG pNewMsg = NULL;
    pNewMsg = NewMessage(NULL, nMsgLen+1);
    ASSERT(pNewMsg);
    if(NULL == pNewMsg)
        return;
	*((char*)pNewMsg) = NORMAL_MESSAGE;
	memcpy(pNewMsg+1, pMsg, nMsgLen);
 	//m_msgQueue.PutInMessage(pNewMsg);
	m_msgQueue.PutInMessage(pNewMsg,nMsgLen+1);
	DeleteMessage(pNewMsg);
}

void CThreadAndTimer::OnTimmerTrigger(DWORD dwPara1, DWORD dwTimerID, DWORD dwPara2)
{
	ASSERT(dwPara1);
	CThreadAndTimer* pManager = (CThreadAndTimer*)dwPara1;
	PTRMSG pNewMsg = NewMessage(NULL, sizeof(DWORD) * 2 + 1);
	INNER_TIMER_MSG* pMsg = (INNER_TIMER_MSG*)pNewMsg;
	pMsg->msgType = TIMER_MESSAGE;
	pMsg->para1 = dwTimerID;
	pMsg->para2 = dwPara2;
 	//pManager->m_msgQueue.PutInMessage(pNewMsg);
	pManager->m_msgQueue.PutInMessage(pNewMsg,sizeof(DWORD) * 2 + 1);
	DeleteMessage(pNewMsg);

}

void CThreadAndTimer::Run()
{
	while( WaitForSingleObject(m_hKillEvent, 0) != WAIT_OBJECT_0 )
	{
		PTRMSG pMsg = m_msgQueue.WaitForNormalMessage(1000);
		if( pMsg )
		{
            
			// SMGW431-06, 2005-10-27, wzy modify begin //
			//INNER_TIMER_MSG *pTest = (INNER_TIMER_MSG*)pMsg;
			// SMGW431-06, 2005-10-27, wzy modify end //
            

			if( TIMER_MESSAGE == *pMsg )
			{
				OnTimer(((INNER_TIMER_MSG*)pMsg)->para1, ((INNER_TIMER_MSG*)pMsg)->para2);
			}
			else 
				OnMessage(pMsg+1);
			//DeleteMessage(pMsg);
		}
	}
}


void CThreadAndTimer::PreExit()
{
    TRACE("PreExit do nothing \n");
}

