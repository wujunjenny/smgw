// MessageQueueEx.cpp: implementation of the CMessageQueueEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "process.h"
#include "MessageQueueEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageQueueEx::CMessageQueueEx()
{
    m_strQueueName = _T("");
	m_strMutexName = _T("");
	m_strEventName = _T("");

	m_dwQueueSize = 0;

	m_hEvent    = NULL;
	m_hMutex    = NULL;
	m_hShMem    = NULL;
	m_pMsgQueue =  NULL;
}

CMessageQueueEx::CMessageQueueEx(LPCTSTR pszName, 
					DWORD dwQueueSize
					)
{
	ASSERT(pszName);
	ASSERT(pszName[0] != '\0');
	ASSERT(strlen(pszName) < MAX_NAME_LEN_EX);

	CString strTmpName = pszName;
	m_strQueueName = strTmpName + "Queue";
	m_strMutexName = strTmpName + "Mutex";
	m_strEventName = strTmpName + "Event";

	ASSERT(dwQueueSize > sizeof(DWORD)); 

	m_dwQueueSize = dwQueueSize;

	m_hEvent = NULL;
	m_hMutex = NULL;
	m_hShMem = NULL;
	m_pMsgQueue =  NULL;

}

CMessageQueueEx::~CMessageQueueEx()
{
	if (m_pMsgQueue)
        UnmapViewOfFile(m_pMsgQueue);

    if (m_hShMem)
        CloseHandle(m_hShMem);

    if (m_hMutex)
        CloseHandle(m_hMutex);

    if (m_hEvent)
        CloseHandle(m_hEvent);
}

/* 应首先被调用在调用消息队列其他成员函数.
   创建成功返回TRUE */
BOOL CMessageQueueEx::Create()
{
    m_hEvent = CreateEvent(NULL, FALSE, FALSE, m_strEventName);
	if (m_hEvent == FALSE)
		return FALSE;

	m_hMutex = CreateMutex(NULL, FALSE, m_strMutexName);
	if (m_hMutex == FALSE)
		return FALSE;
	
	DWORD dwSize = sizeof(tagQueueInShMem) - 1 + m_dwQueueSize;
	m_hShMem = CreateFileMapping(
						(HANDLE)0xFFFFFFFF, 
						NULL,
						PAGE_READWRITE,
						0,
						dwSize ,
						m_strQueueName
						);
   if (m_hShMem == FALSE)
		return FALSE;
	
   BOOL bShMemAlreadyExist = FALSE;
   if (GetLastError() == ERROR_ALREADY_EXISTS)
		bShMemAlreadyExist = TRUE;


	
   m_pMsgQueue =  (PtrLoopQueue)MapViewOfFile(m_hShMem, FILE_MAP_WRITE, 0, 0, 0);
   if (m_pMsgQueue == FALSE)
		return FALSE;

   
   if (bShMemAlreadyExist == FALSE)
   {
		m_pMsgQueue->dwQueueHead = 0;
		m_pMsgQueue->dwQueueTail = 0;
		m_pMsgQueue->dwQueueSize = m_dwQueueSize;
   }
   else 
   {
   } 

   return TRUE;
}

BOOL CMessageQueueEx::Create(LPCTSTR pszName, DWORD dwQueueSize)
{
    ASSERT(pszName);
	ASSERT(pszName[0] != '\0');
	ASSERT(strlen(pszName) < MAX_NAME_LEN_EX);

	CString strTmpName = pszName;
	m_strQueueName = strTmpName + "Queue";
	m_strMutexName = strTmpName + "Mutex";
	m_strEventName = strTmpName + "Event";

	ASSERT(dwQueueSize > sizeof(DWORD)); 

	m_dwQueueSize = dwQueueSize;

    return Create();
}

void CMessageQueueEx::Empty()
{
	WaitForSingleObject(m_hMutex, INFINITE);
	
	m_pMsgQueue->dwQueueHead = 0;
	m_pMsgQueue->dwQueueTail = 0;
	
	ResetEvent(m_hEvent);
	ReleaseMutex(m_hMutex);
}

BOOL CMessageQueueEx::PutInMessage(void *pMsg, DWORD dwSize)
{
//	ASSERT(pMsg != NULL && dwSize > 0 && dwSize < m_pMsgQueue->dwQueueSize);

	if(pMsg == NULL || dwSize == 0 || dwSize >= m_pMsgQueue->dwQueueSize)
		return FALSE;

	WaitForSingleObject(m_hMutex, INFINITE);
	
	DWORD dwSaveBagLen = (sizeof(DWORD) + dwSize);
	DWORD dwFreeSize = (m_pMsgQueue->dwQueueSize - 1 + m_pMsgQueue->dwQueueTail - m_pMsgQueue->dwQueueHead) % m_pMsgQueue->dwQueueSize;
	
	if (dwFreeSize < dwSaveBagLen)
	{
		ReleaseMutex(m_hMutex);
		return FALSE;
	}

	/*the write head point may be turn*/
	DWORD dwHeadToTurn = m_pMsgQueue->dwQueueSize - m_pMsgQueue->dwQueueHead;

	if ((dwHeadToTurn >= dwSaveBagLen) || (m_pMsgQueue->dwQueueTail > m_pMsgQueue->dwQueueHead))
	{
		*(DWORD *)&m_pMsgQueue->buf[m_pMsgQueue->dwQueueHead] = dwSize;
		m_pMsgQueue->dwQueueHead += sizeof(DWORD);
		
		memcpy(&m_pMsgQueue->buf[m_pMsgQueue->dwQueueHead], pMsg, dwSize);
		m_pMsgQueue->dwQueueHead = (m_pMsgQueue->dwQueueHead + dwSize) % m_pMsgQueue->dwQueueSize;
	}
	else if (dwHeadToTurn >= sizeof(DWORD))
	{
		*(DWORD *)&m_pMsgQueue->buf[m_pMsgQueue->dwQueueHead] = dwSize;
		m_pMsgQueue->dwQueueHead = (m_pMsgQueue->dwQueueHead + sizeof(DWORD)) % m_pMsgQueue->dwQueueSize;

		dwHeadToTurn -=  sizeof(DWORD);
		if ( dwHeadToTurn )
		{
			memcpy(&m_pMsgQueue->buf[m_pMsgQueue->dwQueueHead], pMsg, dwHeadToTurn);
			pMsg = (char *)pMsg + dwHeadToTurn;
			dwSize -= dwHeadToTurn;
		}
		
		memcpy(m_pMsgQueue->buf, pMsg, dwSize);
		m_pMsgQueue->dwQueueHead = dwSize;
	}
	else
	{
		/*set dwSize while write head point turns*/
		memcpy(&m_pMsgQueue->buf[m_pMsgQueue->dwQueueHead], &dwSize, dwHeadToTurn);
		m_pMsgQueue->dwQueueHead = sizeof(DWORD) - dwHeadToTurn;
		memcpy(m_pMsgQueue->buf, (char *)&dwSize + dwHeadToTurn, m_pMsgQueue->dwQueueHead);

		memcpy(&m_pMsgQueue->buf[m_pMsgQueue->dwQueueHead], pMsg, dwSize);
		m_pMsgQueue->dwQueueHead += dwSize;
	}
	
	SetEvent(m_hEvent);
	ReleaseMutex(m_hMutex);
	return TRUE;
}

/*SUCEESS READ消息返回TRUE， */
BOOL  CMessageQueueEx::WaitForNormalMessage(void *pMsg, DWORD& dwBufSize, DWORD dwMilliseconds)
{
	DWORD dwWaitResult;
	HANDLE handles[2];

	handles[0] = m_hEvent;
	handles[1] = m_hMutex;
	dwWaitResult = WaitForMultipleObjects(2, handles, TRUE, dwMilliseconds);	
	if (WAIT_TIMEOUT == dwWaitResult || WAIT_FAILED == dwWaitResult)
	{
		dwBufSize = 0XFFFFFFFF;
		return FALSE;
	}

	if (m_pMsgQueue->dwQueueHead == m_pMsgQueue->dwQueueTail)
	{
		ReleaseMutex(m_hMutex);
		dwBufSize = 0;
		return FALSE;
	}
	
	DWORD dwMsgLen;
	DWORD dwTailToTurn = m_pMsgQueue->dwQueueSize - m_pMsgQueue->dwQueueTail;
	
	if (m_pMsgQueue->dwQueueTail < m_pMsgQueue->dwQueueHead)
	{
		dwMsgLen = *(DWORD *)&m_pMsgQueue->buf[m_pMsgQueue->dwQueueTail];
		if (dwBufSize < dwMsgLen)
		{
            ReleaseMutex(m_hMutex);
			dwBufSize = dwMsgLen;
			return FALSE;
		}

		m_pMsgQueue->dwQueueTail += sizeof(DWORD);
	
		ASSERT(dwMsgLen != 0);
		ASSERT(dwMsgLen < m_pMsgQueue->dwQueueSize);
		ASSERT((dwMsgLen + m_pMsgQueue->dwQueueTail) <= m_pMsgQueue->dwQueueHead);

		memcpy(pMsg, &m_pMsgQueue->buf[m_pMsgQueue->dwQueueTail], dwMsgLen);
		m_pMsgQueue->dwQueueTail += dwMsgLen;
	}
	else if (dwTailToTurn >= sizeof(DWORD))
	{
		dwMsgLen = *(DWORD *)&m_pMsgQueue->buf[m_pMsgQueue->dwQueueTail];
		if (dwBufSize < dwMsgLen)
		{
            ReleaseMutex(m_hMutex);
			dwBufSize = dwMsgLen;
			return FALSE;
		}

		m_pMsgQueue->dwQueueTail = (m_pMsgQueue->dwQueueTail + sizeof(DWORD)) % m_pMsgQueue->dwQueueSize;
		dwTailToTurn -= sizeof(DWORD);
	
		ASSERT(dwMsgLen != 0);
		ASSERT(dwMsgLen < m_pMsgQueue->dwQueueSize);

		if (dwTailToTurn >= dwMsgLen)
		{
			memcpy(pMsg, &m_pMsgQueue->buf[m_pMsgQueue->dwQueueTail], dwMsgLen);
			m_pMsgQueue->dwQueueTail = (m_pMsgQueue->dwQueueTail + dwMsgLen) % m_pMsgQueue->dwQueueSize;
		}
		else
		{
			if (dwTailToTurn)
				memcpy(pMsg, &m_pMsgQueue->buf[m_pMsgQueue->dwQueueTail], dwTailToTurn);
			memcpy((char *)pMsg + dwTailToTurn, m_pMsgQueue->buf, dwMsgLen - dwTailToTurn); 
			m_pMsgQueue->dwQueueTail = dwMsgLen - dwTailToTurn;
		}
	}
	else
	{
		ASSERT(dwTailToTurn != 0);

		memcpy(&dwMsgLen, &m_pMsgQueue->buf[m_pMsgQueue->dwQueueTail], dwTailToTurn);
		memcpy((char *)&dwMsgLen + dwTailToTurn, m_pMsgQueue->buf, sizeof(DWORD) - dwTailToTurn);
		if (dwBufSize < dwMsgLen)
		{
            ReleaseMutex(m_hMutex);
			dwBufSize = dwMsgLen;
			return FALSE;
		}

		m_pMsgQueue->dwQueueTail = sizeof(DWORD) - dwTailToTurn;

		ASSERT(dwMsgLen != 0);
		ASSERT(dwMsgLen < m_pMsgQueue->dwQueueHead);
		ASSERT(dwMsgLen < m_pMsgQueue->dwQueueSize);

		memcpy(pMsg, &m_pMsgQueue->buf[m_pMsgQueue->dwQueueTail], dwMsgLen);
		m_pMsgQueue->dwQueueTail += dwMsgLen;
	}
	
	if (m_pMsgQueue->dwQueueTail != m_pMsgQueue->dwQueueHead)
		SetEvent(m_hEvent);
	
	ReleaseMutex(m_hMutex);

	dwBufSize = dwMsgLen;
	return TRUE;
}

