#ifndef _COMMTHREADMNG_H_INCLUDED_
#define _COMMTHREADMNG_H_INCLUDED_

#include "threadandtimer.h"

class CCommThreadMng : public CThreadAndTimer
{
public:
	CCommThreadMng(UINT nTimerCount = 2000);
	~CCommThreadMng();

public:
	void SendMessageToQueue(PTRMSG pMsg); // 消息进入函数, 不需要new !!

};

#endif	//_COMMTHREADMNG_H_INCLUDED_
