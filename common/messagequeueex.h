// MessageQueueEx.h: interface for the CMessageQueueEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MESSAGEQUEUEEX_H_INCLUDED_)
#define _MESSAGEQUEUEEX_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_NAME_LEN_EX 128
struct   tagQueueInShMem
{
	DWORD   dwQueueHead; // write data index in queue
	DWORD   dwQueueTail; // read  data index in queue
	DWORD   dwQueueSize;
	char    buf[1];
} ;
typedef struct tagQueueInShMem * PtrLoopQueue;

class CMessageQueueEx  
{
public:
    CMessageQueueEx();
	CMessageQueueEx(LPCTSTR pszName, DWORD dwQueueSize);
	virtual ~CMessageQueueEx();
	BOOL   Create();
    BOOL   Create(LPCTSTR pszName, DWORD dwQueueSize);

	void    Empty();
	BOOL    WaitForNormalMessage(void *buf, DWORD& dwSize, DWORD dwMilliseconds=INFINITE);
	BOOL    PutInMessage(void *pMsg, DWORD dwSize);

private:
	PtrLoopQueue m_pMsgQueue;
	
	DWORD   m_dwQueueSize;
	CString m_strQueueName;
	CString m_strMutexName;
	CString m_strEventName;

	HANDLE  m_hMutex;
	HANDLE  m_hEvent;
	HANDLE  m_hShMem;
};

#endif // !defined(_MESSAGEQUEUEEX_H_INCLUDED_)
