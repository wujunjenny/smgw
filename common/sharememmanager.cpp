#include "stdafx.h"
#include "ShareMemManager.h"
//Temp by hyh 2012-6-21
#include "smsapp.h"
//End Temp by hyh 2012-6-21
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShareMemManager::CShareMemManager(LPCTSTR lpszShareMemName,
                    DWORD dwAccess, DWORD dwMemSize)
{
    m_ShareName = lpszShareMemName;
    m_dwMemSize = dwMemSize;

    if (dwAccess != SMS_WRITE)
        m_dwAccess  = SMS_READ;
    else
        m_dwAccess  = SMS_WRITE;

    m_hKillEvent    = NULL;
    m_hThread       = NULL;

}

CShareMemManager::~CShareMemManager()
{

}

BOOL CShareMemManager::Create()
{
    if (!m_MsgQueue.Create(m_ShareName, m_dwMemSize))
        return FALSE;

    if (SMS_READ == m_dwAccess)
    {
        ASSERT(!m_hKillEvent);
        m_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        ASSERT(m_hKillEvent);

	    DWORD dwThreadID;

        ASSERT(!m_hThread);
	    m_hThread = CreateThread(
					    NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID);
        ASSERT(m_hThread);
    }

    m_MsgQueue.Empty();

    return TRUE;
}

void CShareMemManager::Destroy()
{
    if (SMS_READ != m_dwAccess)
        return;

    ASSERT(m_hKillEvent);
    ASSERT(m_hThread);

    if (m_hKillEvent)
        SetEvent(m_hKillEvent);

    if (m_hThread)
	    if( WaitForSingleObject(m_hThread, 100000) != WAIT_OBJECT_0 )
	    {
		    ASSERT(0);
		    TerminateThread(m_hThread, 4);
	    };

    if (m_hKillEvent)
        CloseHandle(m_hKillEvent);
    m_hKillEvent = NULL;

    if (m_hThread)
        CloseHandle(m_hThread);
    m_hThread = NULL;
}

BOOL CShareMemManager::SendMessageToQueue(void* pMsg, DWORD dwSize)
{
    if (SMS_WRITE != m_dwAccess)
        return FALSE;

    if (dwSize >= MAX_SHAREMEMMSG_SIZE)
        return FALSE;

    if (!m_MsgQueue.PutInMessage(pMsg, dwSize))
        m_MsgQueue.Empty();
    else
        return TRUE;

    return m_MsgQueue.PutInMessage(pMsg, dwSize);

}

BOOL CShareMemManager::OnMessage(void* pMsg, DWORD dwSize)
{
    return TRUE;
}

DWORD WINAPI CShareMemManager::ThreadProc(LPVOID lpParam)
{
	//Temp by hyh 2012-6-21
	CString log;	
	log.Format("CShareMemManager::ThreadProc 线程 Start [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

    CShareMemManager* pMng = (CShareMemManager*)lpParam;
    ASSERT(pMng);
    DWORD dwSize ;
    while( WaitForSingleObject(pMng->m_hKillEvent, 0) != WAIT_OBJECT_0 )
	{
        dwSize = MAX_SHAREMEMMSG_SIZE;
		if (pMng->m_MsgQueue.WaitForNormalMessage(pMng->m_MsgBuff, dwSize, 1000))
		    pMng->OnMessage(pMng->m_MsgBuff, dwSize);
        else
            if ((0XFFFFFFFF == dwSize) || (0 == dwSize))
                continue;
            else
            {
                ASSERT(0);
                pMng->m_MsgQueue.Empty();
            }
	}

	//Temp by hyh 2012-6-21
	log.Format("CShareMemManager::ThreadProc 线程 End [%d]", GetCurrentThreadId());
	GetSmsApp()->WriteTestRecord(log, 0);
	//End Temp by hyh 2012-6-21

	return 0L;

}