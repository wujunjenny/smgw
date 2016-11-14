#ifndef _THREADANDTIMER_H_INCLUDED_
#define _THREADANDTIMER_H_INCLUDED_

#include "threadmng.h"

class CTimerMng;

class CThreadAndTimer : public CThreadMng 
{
public:
	CThreadAndTimer(UINT nTimerCount = 2000);

	virtual ~CThreadAndTimer();

public:
	virtual void PreExit();
	virtual void SendMessage(PTRMSG pMsg, DWORD nMsgLen);

	DWORD SetTimer(DWORD dwMilliSeconds, WORD wTimes, DWORD dwPara);

	void KillTimer(DWORD nTimerID);	

	virtual void Destroy();

    void SetServiceType(DWORD ulServiceType)
    {
        m_ulServiceType = ulServiceType;
    }
protected:
    DWORD m_ulServiceType;

	virtual BOOL OnTimer(DWORD timeID, DWORD dwPara) = NULL;

protected:
	virtual void Run();

protected:
	static void OnTimmerTrigger(DWORD, DWORD, DWORD);
	CTimerMng* m_pTM;
	enum BASE_MESSAGE_TYPE
	{
		NORMAL_MESSAGE,
		TIMER_MESSAGE,
	};
	typedef struct tagINNER_TIMER_MSG
	{
		char msgType;
		DWORD para1;
		DWORD para2;
	}INNER_TIMER_MSG;
};

#endif	//_THREADANDTIMER_H_INCLUDED_
