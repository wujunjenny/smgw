// waitresp.h: interface for the CWaitResp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_WAITRESP_H_INCLUDED_)
#define _WAITRESP_H_INCLUDED_

#include "..\COMMON\msgqueue.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_TIMER_WAIT	30	//ȱʡ��Ϣ�ȴ�ACK��ʱ��(��)��
#define WAIT_ACK_YES    0
#define WAIT_ACK_NO     1

class CWaitResp  
{
public:
	CWaitResp();
    CWaitResp(int nMaxWaitCount);
	virtual ~CWaitResp();
	PTRMSG	m_pMsg;	//ĳҵ���ύ����Ϣ
	int		m_nTimerCount;	//ʱ�Ӽ��������ڳ�ʱ
public:
	virtual UC GetAck();
    virtual void SetAck(UC byAlreadyAck);
	int DecTimerCount()
	{
		return --m_nTimerCount;
	}
	PTRMSG GetMsg();
	BOOL SaveMsg(PTRMSG pMsg);
protected:
	UC m_byWaitAck;
};

#endif // !defined(_WAITRESP_H_INCLUDED_)
