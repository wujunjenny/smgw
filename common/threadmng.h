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
		// 消息进入函数, 不需要new !!

	virtual void Destroy();
		// 删除CThreadMng类型的实例前，必须显式调用此函数

protected:
	// 需要重载的函数
	virtual BOOL OnMessage(PTRMSG pMsg) = NULL;
		// 消息处理基本函数, 实现的时候不需要删除消息包 !!

protected:
	// 消息调度过程
	virtual void Run();

protected:
	static DWORD WINAPI ThreadProc(LPVOID lpParam);
	HANDLE m_hThread;
	HANDLE m_hKillEvent;
	//CMsgQueue m_msgQueue;
	CNewMsgQueue m_msgQueue;
};

#endif	//_THREADMNG_H_INCLUDED_
