#include "stdafx.h"
#include "msgqueue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PTRMSG WINAPI NewMessage(const char* pData, int nLen)
{
	//ASSERT(nLen);
    if (nLen <= 0)
    {
        ASSERT(0);
        return NULL;
    }
	if( pData )
	{
		char* pRet = new char[nLen];
		if(!pRet)
			return NULL;
 		memcpy(pRet, pData, nLen);
		return pRet;
	}

	return new char[nLen];
}

void WINAPI DeleteMessage(PTRMSG pMsg)
{
	if( pMsg )
	{
		delete pMsg;
	}
}

///////////////////////////////////
//	class CMsgQueue
//	CRITICAL_SECTION m_lock;
//	HANDLE m_hEvent;
CMsgQueue::CMsgQueue()
{
	InitializeCriticalSection(&m_lock);
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CMsgQueue::~CMsgQueue()
{
	DeleteCriticalSection(&m_lock); 
	CloseHandle(m_hEvent);
    m_hEvent = NULL;
	POSITION pos = m_listMsg.GetHeadPosition();
	while( pos )
	{
		DeleteMessage((PTRMSG)m_listMsg.GetNext(pos));
	}
	m_listMsg.RemoveAll();
}

// operators:
PTRMSG CMsgQueue::WaitForNormalMessage(int dwMilliseconds)
{
	DWORD dw = WaitForSingleObject(m_hEvent, dwMilliseconds);
	if( dw == WAIT_OBJECT_0 )
		return PopupMessage();
	return NULL;
}

PTRMSG CMsgQueue::PopupMessage()
{
	PTRMSG pRet = NULL;
	EnterCriticalSection(&m_lock);
	POSITION pos = m_listMsg.GetHeadPosition();
	if( pos )
	{
		pRet = (PTRMSG)m_listMsg.GetAt(pos);
		m_listMsg.RemoveAt(pos);
		if( m_listMsg.GetCount() )
			SetEvent(m_hEvent);
		else ResetEvent(m_hEvent);
	}
	LeaveCriticalSection(&m_lock);
	ASSERT(pRet);
	return pRet;
}

BOOL CMsgQueue::PutInMessage(PTRMSG pMsg)
{
	BOOL bRet = TRUE;
	ASSERT(pMsg);
    if(NULL == pMsg)
    {
        return FALSE;
    }
	EnterCriticalSection(&m_lock);
	if( m_listMsg.GetCount() >= MAX_STOREMSG_COUNT )
	{
		TRACE("Too many message-----------------------------\n");
		//ASSERT(0);
		bRet = FALSE;
		POSITION pos = m_listMsg.GetHeadPosition();
		while( pos )
		{
			DeleteMessage((PTRMSG)m_listMsg.GetNext(pos));
		}
		m_listMsg.RemoveAll();
	}
	m_listMsg.AddTail(pMsg);
	SetEvent(m_hEvent);
	LeaveCriticalSection(&m_lock);
	return bRet;
}

//new message queue
	CNewMsgQueue::CNewMsgQueue()
	{
		InitializeCriticalSection(&m_lock);
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_pBuffer = new char[8000000+100];

		m_dwQueueHead = 0; 
		m_dwQueueTail = 0; 
		m_dwQueueSize = 8000000;

		
		m_pTmpMsg = new char[40000];
		m_dwTmpMsgSize = 40000;
	}

	CNewMsgQueue::~CNewMsgQueue()
	{
		DeleteCriticalSection(&m_lock); 
		CloseHandle(m_hEvent);
		delete [] m_pBuffer;
		delete [] m_pTmpMsg;
	}
	
	PTRMSG CNewMsgQueue::WaitForNormalMessage(int dwMilliseconds)
	{
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(m_hEvent, dwMilliseconds);	
		if (WAIT_TIMEOUT == dwWaitResult || WAIT_FAILED == dwWaitResult)
		{
			return NULL;
		}
		EnterCriticalSection(&m_lock);

		if (m_dwQueueHead == m_dwQueueTail)
		{
			LeaveCriticalSection(&m_lock);
			return NULL;
		}
	
		DWORD dwMsgLen;
		DWORD dwTailToTurn = m_dwQueueSize - m_dwQueueTail;
	
		if (m_dwQueueTail < m_dwQueueHead)
		{
			dwMsgLen = *(DWORD *)&m_pBuffer[m_dwQueueTail];
			//if (dwBufSize < dwMsgLen)
			//{
 			//	dwBufSize = dwMsgLen;
			//	return FALSE;
			//}

			m_dwQueueTail += sizeof(DWORD);
			memcpy(m_pTmpMsg, &m_pBuffer[m_dwQueueTail], dwMsgLen);
			m_dwQueueTail += dwMsgLen;
		}
		else if (dwTailToTurn >= sizeof(DWORD))
		{
			dwMsgLen = *(DWORD *)&m_pBuffer[m_dwQueueTail];
			//if (dwBufSize < dwMsgLen)
			//{
			//	m_dwTmpMsgSize = dwMsgLen;
			//	return FALSE;
			//}

			m_dwQueueTail = (m_dwQueueTail + sizeof(DWORD)) % m_dwQueueSize;
			dwTailToTurn -= sizeof(DWORD);

			if (dwTailToTurn >= dwMsgLen)
			{
				memcpy(m_pTmpMsg, &m_pBuffer[m_dwQueueTail], dwMsgLen);
				m_dwQueueTail = (m_dwQueueTail + dwMsgLen) % m_dwQueueSize;
			}
			else
			{
				if (dwTailToTurn)
					memcpy(m_pTmpMsg, &m_pBuffer[m_dwQueueTail], dwTailToTurn);
				memcpy((char *)m_pTmpMsg + dwTailToTurn, m_pBuffer, dwMsgLen - dwTailToTurn); 
				m_dwQueueTail = dwMsgLen - dwTailToTurn;
			}
		}
		else
		{

			memcpy(&dwMsgLen, &m_pBuffer[m_dwQueueTail], dwTailToTurn);
			memcpy((char *)&dwMsgLen + dwTailToTurn, m_pBuffer, sizeof(DWORD) - dwTailToTurn);
			//if (dwBufSize < dwMsgLen)
			//{
			//	return FALSE;
			//}

			m_dwQueueTail = sizeof(DWORD) - dwTailToTurn;

			memcpy(m_pTmpMsg, &m_pBuffer[m_dwQueueTail], dwMsgLen);
			m_dwQueueTail += dwMsgLen;
		}
	
		LeaveCriticalSection(&m_lock);

	if (m_dwQueueTail != m_dwQueueHead)
		SetEvent(m_hEvent);
	else
		ResetEvent(m_hEvent);
	
	//ReleaseMutex(m_hMutex);

	//	dwBufSize = dwMsgLen;
		return m_pTmpMsg;
	}

	
	BOOL CNewMsgQueue::PutInMessage(PTRMSG pMsg ,DWORD nSize)
	{
		if(pMsg == NULL || nSize == 0 || nSize >= m_dwQueueSize)
			return FALSE;
		DWORD dwSaveBagLen = (sizeof(DWORD) + nSize);
		EnterCriticalSection(&m_lock);
		DWORD dwFreeSize = (m_dwQueueSize - 1 + m_dwQueueTail - m_dwQueueHead) % m_dwQueueSize;
		if (dwFreeSize < dwSaveBagLen)
		{
			LeaveCriticalSection(&m_lock);
			return FALSE;
		}
		DWORD dwHeadToTurn = m_dwQueueSize - m_dwQueueHead;

		if ((dwHeadToTurn >= dwSaveBagLen) || (m_dwQueueTail > m_dwQueueHead))
		{
			*(DWORD *)&m_pBuffer[m_dwQueueHead] = nSize;
			m_dwQueueHead += sizeof(DWORD);
		
			memcpy(&m_pBuffer[m_dwQueueHead], pMsg, nSize);
			m_dwQueueHead = (m_dwQueueHead + nSize) % m_dwQueueSize;
		}
		else if (dwHeadToTurn >= sizeof(DWORD))
		{
			*(DWORD *)&m_pBuffer[m_dwQueueHead] = nSize;
			m_dwQueueHead = (m_dwQueueHead + sizeof(DWORD)) % m_dwQueueSize;
			dwHeadToTurn -=  sizeof(DWORD);
			if ( dwHeadToTurn )
			{
				memcpy(&m_pBuffer[m_dwQueueHead], pMsg, dwHeadToTurn);
				pMsg = (char *)pMsg + dwHeadToTurn;
				nSize -= dwHeadToTurn;
			}
		
			memcpy(m_pBuffer, pMsg, nSize);
			m_dwQueueHead = nSize;
		}
		else
		{
			/*set dwSize while write head point turns*/
			memcpy(&m_pBuffer[m_dwQueueHead], &nSize, dwHeadToTurn);
			m_dwQueueHead = sizeof(DWORD) - dwHeadToTurn;
			memcpy(m_pBuffer, (char *)&nSize + dwHeadToTurn, m_dwQueueHead);
			memcpy(&m_pBuffer[m_dwQueueHead], pMsg, nSize);
			m_dwQueueHead += nSize;
		}

		LeaveCriticalSection(&m_lock);
		SetEvent(m_hEvent);
		return TRUE;

}
	


