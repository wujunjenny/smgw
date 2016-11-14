 /********************************************************
 *      msgqueue.h	...stolen from Vmail				*
********************************************************/

#ifndef _MSGQUEUE_H_INCLUDED_
#define _MSGQUEUE_H_INCLUDED_

#define MAX_STOREMSG_COUNT	800000

typedef char* PTRMSG;

class CMsgQueue
{
public:
	CMsgQueue();
	~CMsgQueue();
	// operators:
	PTRMSG WaitForNormalMessage(int dwMilliseconds=INFINITE);
	BOOL PutInMessage(PTRMSG pMsg);
		// 如果返回FALSE，表示消息队列过载
	PTRMSG GetSpecialMsg(int dwMilliseconds);

protected:
	// implementations:
	PTRMSG PopupMessage();
	CRITICAL_SECTION m_lock;
	HANDLE m_hEvent;
	CPtrList m_listMsg;
};

//消息基本处理函数
PTRMSG WINAPI NewMessage(const char* pData=NULL, int nLen=0);
void WINAPI DeleteMessage(PTRMSG pMsg);


class CNewMsgQueue
{
public:
	CNewMsgQueue();
	~CNewMsgQueue();
	// operators:
	PTRMSG WaitForNormalMessage(int dwMilliseconds=INFINITE);
	BOOL PutInMessage( PTRMSG pMsg, DWORD nSize );
	// 如果返回FALSE，表示消息队列过载

protected:
	// implementations:
	CRITICAL_SECTION m_lock;
	HANDLE m_hEvent;
	
	char*   m_pBuffer;
	char*   m_pTmpMsg;
	DWORD   m_dwTmpMsgSize;

	DWORD   m_dwQueueHead; 
	DWORD   m_dwQueueTail; 
	DWORD   m_dwQueueSize;
};

#endif	//_MSGQUEUE_H_INCLUDED_
