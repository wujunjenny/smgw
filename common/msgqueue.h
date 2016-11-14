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
		// �������FALSE����ʾ��Ϣ���й���
	PTRMSG GetSpecialMsg(int dwMilliseconds);

protected:
	// implementations:
	PTRMSG PopupMessage();
	CRITICAL_SECTION m_lock;
	HANDLE m_hEvent;
	CPtrList m_listMsg;
};

//��Ϣ����������
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
	// �������FALSE����ʾ��Ϣ���й���

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
