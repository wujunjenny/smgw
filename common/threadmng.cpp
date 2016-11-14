///////////////////////////////////////////////////////////////////////////////////
//	此文件实现一个虚拟的任务管理员，主要提供消息驱动机制
#include "stdafx.h"
#include "threadmng.h"
//Temp by hyh 2012-6-21
#include "smsapp.h"
//End Temp by hyh 2012-6-21
//////////////////////////////////////////////////////////////////////////////
//	class CThreadMng 
//		HANDLE m_hThread;
//		HANDLE m_hKillEvent;
//		CMsgQueue m_msgQueue;
CThreadMng::CThreadMng()
{
	m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThreadID;
	m_hThread = CreateThread(
					NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
}

CThreadMng::~CThreadMng()
{
}

void CThreadMng::Destroy()
{
	SetEvent(m_hKillEvent);
	if( WaitForSingleObject(m_hThread, 100000) != WAIT_OBJECT_0 )
	{
		ASSERT(0);
		TerminateThread(m_hThread, 4);
	}
	CloseHandle(m_hKillEvent);
	CloseHandle(m_hThread);

    m_hKillEvent = NULL;
    m_hThread = NULL;
}

void CThreadMng::SendMessage(PTRMSG pMsg, DWORD nMsgLen)
{
 	m_msgQueue.PutInMessage(pMsg,nMsgLen);
	//DeleteMessage(pNewMsg);
}

void CThreadMng::Run()
{
	while( WaitForSingleObject(m_hKillEvent, 0) != WAIT_OBJECT_0 )
	{
		PTRMSG pMsg = m_msgQueue.WaitForNormalMessage(1000);
		if( pMsg )
		{
			OnMessage(pMsg);
			//DeleteMessage(pMsg);
		}
	}
}

DWORD WINAPI CThreadMng::ThreadProc(LPVOID lpParam)
{
	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CThreadMng::ThreadProc 线程 Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	CThreadMng* pVM = (CThreadMng*)lpParam;
	ASSERT(pVM);
	pVM->Run();

	//Temp by hyh 2012-6-21
	log.Format("CThreadMng::ThreadProc 线程 End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	return 0L;
}

