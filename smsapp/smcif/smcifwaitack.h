// SmcIFWaitAck.h: interface for the CSmcIFWaitAck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SMCWAITACK_H_INCLUDED_)
#define _SMCWAITACK_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\IFMNG\waitresp.h"

//每单位为握手时间
#define SMCIF_MAX_TIMER_WAIT    3
//到短消息中心去的消息的等待序列
//因为现在短消息中心不支持对它的DELIVERY 消息，所以如果短消息中心
//所询问的一个业务的回答，只能以SUBMIT的形式提交，它所产生的SUBMIT ACK
//如果回到业务调度中心，将对对话造成不良影响。

class CSmcIFWaitAck : public CWaitResp  
{
public:
	CSmcIFWaitAck();
    CSmcIFWaitAck(int nMaxWaitCount):CWaitResp(nMaxWaitCount)
    {
        //CWaitResp::CWaitResp(nMaxWaitCount);
    };
	virtual ~CSmcIFWaitAck();
private:
    UL m_ulSrcSeq;      //源提交消息的序号
public:
    UL GetSourceSeq()
    {
        return m_ulSrcSeq;
    }
	virtual BOOL SaveMsg(PTRMSG pMsg)
    {
        m_ulSrcSeq = ((tagSmsHead *)pMsg)->nSequenceID;
        return TRUE;
    }
};

#endif // !defined(_SMCWAITACK_H_INCLUDED_)
