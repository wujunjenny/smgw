#ifndef _THREADMNG_H_INCLUDED_
#define _THREADMNG_H_INCLUDED_

#include "msgqueue.h"

class CThreadMng 
{
public:
	CThreadMng();
	virtual ~CThreadMng();

public:
	virtual void SendMessage(PTRMSG pMsg, DWORD nMsgLen);
		// ��Ϣ���뺯��, ����Ҫnew !!

	virtual void Destroy();
		// ɾ��CThreadMng���͵�ʵ��ǰ��������ʽ���ô˺���

protected:
	// ��Ҫ���صĺ���
	virtual BOOL OnMessage(PTRMSG pMsg) = NULL;
		// ��Ϣ�����������, ʵ�ֵ�ʱ����Ҫɾ����Ϣ�� !!

protected:
	// ��Ϣ���ȹ���
	virtual void Run();

protected:
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hThread;
	HANDLE m_hKillEvent;
	//CMsgQueue m_msgQueue;
	CNewMsgQueue m_msgQueue;
};

#endif	//_THREADMNG_H_INCLUDED_
