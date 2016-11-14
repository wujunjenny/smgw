// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Thread.h"

#include <process.h>    /* _beginthread, _endthread */

//Temp by hyh 2012-5-23
#include "smsapp.h"
//End Temp by hyh 2012-5-23

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThread::CThread()
{
    m_hThread = NULL;
    m_hEvent = NULL;
	m_hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
}

CThread::~CThread()
{
	//Stop();
//	TRACE("Thread Close Event %d\n",m_hEvent);
    if (NULL != m_hEvent)
	    CloseHandle(m_hEvent);

    if (NULL != m_hThread)
    {
        CloseHandle(m_hThread);
    }
}

int CThread::Run(DWORD param)
{
	return TRUE;
}


BOOL CThread::Start()
{
    //unsigned int iThreadID;
	//updated by hyh begin  2012-6-21
	DWORD iThreadID;
	ResetEvent( m_hEvent);
	//end updated by hyh 2012-6-21
	//Temp by hyh 2012-5-23
	try
	{
		//m_hThread=(HANDLE)(::_beginthreadex(NULL, NULL, stRun, this, NULL, &iThreadID)); 
		//updated by hyh begin  2012-7-24
		m_hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &iThreadID);
		//end updated by hyh 2012-7-24
	}
	catch (...)
	{
		CString log;
		log.Format("CThread::Start 异常");
		GetSmsApp()->WriteTestRecord(log, 0);
		throw;
	}		
	//End Temp by hyh 2012-5-23
	if(m_hThread==NULL)
    {
        ASSERT(0);
		CString log;
		log.Format("CThread::Start 失败 代码[%d]", GetLastError());
		GetSmsApp()->WriteTestRecord(log, 0);
		throw;
		return FALSE;
    }
	return TRUE;
}

//unsigned int WINAPI CThread::stRun(LPVOID lpParam)
DWORD WINAPI CThread::ThreadProc(LPVOID lpParam)
{
	CThread* pTh=(CThread*)lpParam;
	if(pTh==NULL)
		return -1;
	DWORD rt=pTh->Run(NULL);
	return rt;
}

BOOL CThread::IsExit()
{
	return WaitForSingleObject(m_hEvent,0)==WAIT_OBJECT_0;
}

BOOL CThread::Stop(DWORD dwWaitTime, BOOL bTerminate)
{
    if( !IsExit())
    {
        SetEvent(m_hEvent);
    }
    DWORD res;

	//updated by hyh begin  2012-7-24
	if ( NULL == m_hThread)
	{//当start失败时，stop应该回TRUE以便socketconnect对象能资源能被sockmng回收
		return TRUE;
	}
	//end updated by hyh 2012-7-24

    //如果无限等待
    if (0xffffffff == dwWaitTime)
    {
        //如果立即退出
        if (bTerminate)
        {
			ASSERT(0);
			TerminateThread(m_hThread, -2);     //强制退出
			CloseHandle(m_hThread);
			m_hThread = NULL;
			return TRUE;
        }

        res = WaitForSingleObject(m_hThread, INFINITE);
    }
    else
        res = WaitForSingleObject(m_hThread, dwWaitTime);

    if (res == WAIT_OBJECT_0)
	{
//		ASSERT(0);
		CloseHandle(m_hThread);
		m_hThread = NULL;
        return TRUE;
	}
    else
    {
        if (bTerminate)
        {
            ASSERT(0);
            TerminateThread(m_hThread, -2);     //强制退出
			CloseHandle(m_hThread);
			m_hThread = NULL;
            return TRUE;
        }
        else
            return FALSE;
    }
}

BOOL CThread::IsActive()
{
	if(WaitForSingleObject(m_hThread,0)==WAIT_TIMEOUT)
		return TRUE;
	else
		return FALSE;
}