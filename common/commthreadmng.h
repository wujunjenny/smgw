#ifndef _COMMTHREADMNG_H_INCLUDED_
#define _COMMTHREADMNG_H_INCLUDED_

#include "threadandtimer.h"

class CCommThreadMng : public CThreadAndTimer
{
public:
	CCommThreadMng(UINT nTimerCount = 2000);
	~CCommThreadMng();

public:
	void SendMessageToQueue(PTRMSG pMsg); // ��Ϣ���뺯��, ����Ҫnew !!

};

#endif	//_COMMTHREADMNG_H_INCLUDED_
